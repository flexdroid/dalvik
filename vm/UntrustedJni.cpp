/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Support for -Xcheck:jni (the "careful" version of the JNI interfaces).
 *
 * We want to verify types, make sure class and field IDs are valid, and
 * ensure that JNI's semantic expectations are being met.  JNI seems to
 * be relatively lax when it comes to requirements for permission checks,
 * e.g. access to private methods is generally allowed from anywhere.
 */

#include "Dalvik.h"
#include "JniInternal.h"

#include <sys/mman.h>
#include <zlib.h>

/*
 * Abort if we are configured to bail out on JNI warnings.
 */
static void abortMaybe() {
    if (!gDvmJni.warnOnly) {
        dvmDumpThread(dvmThreadSelf(), false);
        dvmAbort();
    }
}

/*
 * ===========================================================================
 *      JNI call bridge wrapper
 * ===========================================================================
 */

/*
 * Check the result of a native method call that returns an object reference.
 *
 * The primary goal here is to verify that native code is returning the
 * correct type of object.  If it's declared to return a String but actually
 * returns a byte array, things will fail in strange ways later on.
 *
 * This can be a fairly expensive operation, since we have to look up the
 * return type class by name in method->clazz' class loader.  We take a
 * shortcut here and allow the call to succeed if the descriptor strings
 * match.  This will allow some false-positives when a class is redefined
 * by a class loader, but that's rare enough that it doesn't seem worth
 * testing for.
 *
 * At this point, pResult->l has already been converted to an object pointer.
 */
static void checkCallResultCommon(const u4* args, const JValue* pResult,
        const Method* method, Thread* self)
{
    assert(pResult->l != NULL);
    const Object* resultObj = (const Object*) pResult->l;

    if (resultObj == kInvalidIndirectRefObject) {
        ALOGW("JNI WARNING: invalid reference returned from native code");
        const Method* method = dvmGetCurrentJNIMethod();
        char* desc = dexProtoCopyMethodDescriptor(&method->prototype);
        ALOGW("             in %s.%s:%s", method->clazz->descriptor, method->name, desc);
        free(desc);
        abortMaybe();
        return;
    }

    ClassObject* objClazz = resultObj->clazz;

    /*
     * Make sure that pResult->l is an instance of the type this
     * method was expected to return.
     */
    const char* declType = dexProtoGetReturnType(&method->prototype);
    const char* objType = objClazz->descriptor;
    if (strcmp(declType, objType) == 0) {
        /* names match; ignore class loader issues and allow it */
        ALOGV("Check %s.%s: %s io %s (FAST-OK)",
            method->clazz->descriptor, method->name, objType, declType);
    } else {
        /*
         * Names didn't match.  We need to resolve declType in the context
         * of method->clazz->classLoader, and compare the class objects
         * for equality.
         *
         * Since we're returning an instance of declType, it's safe to
         * assume that it has been loaded and initialized (or, for the case
         * of an array, generated).  However, the current class loader may
         * not be listed as an initiating loader, so we can't just look for
         * it in the loaded-classes list.
         */
        ClassObject* declClazz = dvmFindClassNoInit(declType, method->clazz->classLoader);
        if (declClazz == NULL) {
            ALOGW("JNI WARNING: method declared to return '%s' returned '%s'",
                declType, objType);
            ALOGW("             failed in %s.%s ('%s' not found)",
                method->clazz->descriptor, method->name, declType);
            abortMaybe();
            return;
        }
        if (!dvmInstanceof(objClazz, declClazz)) {
            ALOGW("JNI WARNING: method declared to return '%s' returned '%s'",
                declType, objType);
            ALOGW("             failed in %s.%s",
                method->clazz->descriptor, method->name);
            abortMaybe();
            return;
        } else {
            ALOGV("Check %s.%s: %s io %s (SLOW-OK)",
                method->clazz->descriptor, method->name, objType, declType);
        }
    }
}

/*
 * Determine if we need to check the return type coming out of the call.
 *
 * (We don't simply do this at the top of checkCallResultCommon() because
 * this is on the critical path for native method calls.)
 */
static inline bool callNeedsCheck(const u4* args, JValue* pResult,
    const Method* method, Thread* self)
{
    return (method->shorty[0] == 'L' && !dvmCheckException(self) && pResult->l != NULL);
}

/*
 * ===========================================================================
 *      JNI function helpers
 * ===========================================================================
 */

static inline const JNINativeInterface* baseEnv(JNIEnv* env) {
    return ((JNIEnvExt*) env)->baseFuncTable;
}

static inline const JNIInvokeInterface* baseVm(JavaVM* vm) {
    return ((JavaVMExt*) vm)->baseFuncTable;
}

/*
 * Flags passed into ScopedCheck.
 */
#define kFlag_Default       0x0000

#define kFlag_CritBad       0x0000      /* calling while in critical is bad */
#define kFlag_CritOkay      0x0001      /* ...okay */
#define kFlag_CritGet       0x0002      /* this is a critical "get" */
#define kFlag_CritRelease   0x0003      /* this is a critical "release" */
#define kFlag_CritMask      0x0003      /* bit mask to get "crit" value */

#define kFlag_ExcepBad      0x0000      /* raised exceptions are bad */
#define kFlag_ExcepOkay     0x0004      /* ...okay */

#define kFlag_Release       0x0010      /* are we in a non-critical release function? */
#define kFlag_NullableUtf   0x0020      /* are our UTF parameters nullable? */

#define kFlag_Invocation    0x8000      /* Part of the invocation interface (JavaVM*) */

static const char* indirectRefKindName(IndirectRef iref)
{
    return indirectRefKindToString(indirectRefKind(iref));
}

/*
 * ===========================================================================
 *      Guarded arrays
 * ===========================================================================
 */

#define kGuardLen       512         /* must be multiple of 2 */
#define kGuardPattern   0xd5e3      /* uncommon values; d5e3d5e3 invalid addr */
#define kGuardMagic     0xffd5aa96

/*
 * Return the width, in bytes, of a primitive type.
 */
static int dvmPrimitiveTypeWidth(PrimitiveType primType) {
    switch (primType) {
        case PRIM_BOOLEAN: return 1;
        case PRIM_BYTE:    return 1;
        case PRIM_SHORT:   return 2;
        case PRIM_CHAR:    return 2;
        case PRIM_INT:     return 4;
        case PRIM_LONG:    return 8;
        case PRIM_FLOAT:   return 4;
        case PRIM_DOUBLE:  return 8;
        case PRIM_VOID:
        default: {
            assert(false);
            return -1;
        }
    }
}

/*
 * Create a guarded copy of a primitive array.  Modifications to the copied
 * data are allowed.  Returns a pointer to the copied data.
 */
static void* createGuardedPACopy(JNIEnv* env, const jarray jarr, jboolean* isCopy) {
    ScopedCheckJniThreadState ts(env);

    ArrayObject* arrObj = (ArrayObject*) dvmDecodeIndirectRef(dvmThreadSelf(), jarr);
    PrimitiveType primType = arrObj->clazz->elementClass->primitiveType;
    int len = arrObj->length * dvmPrimitiveTypeWidth(primType);
    void* result = GuardedCopy::create(arrObj->contents, len, true);
    if (isCopy != NULL) {
        *isCopy = JNI_TRUE;
    }
    return result;
}

/*
 * Perform the array "release" operation, which may or may not copy data
 * back into the VM, and may or may not release the underlying storage.
 */
static void* releaseGuardedPACopy(JNIEnv* env, jarray jarr, void* dataBuf, int mode) {
    ScopedCheckJniThreadState ts(env);
    ArrayObject* arrObj = (ArrayObject*) dvmDecodeIndirectRef(dvmThreadSelf(), jarr);

    if (!GuardedCopy::check(dataBuf, true)) {
        ALOGE("JNI: failed guarded copy check in releaseGuardedPACopy");
        abortMaybe();
        return NULL;
    }

    if (mode != JNI_ABORT) {
        size_t len = GuardedCopy::fromData(dataBuf)->originalLen;
        memcpy(arrObj->contents, dataBuf, len);
    }

    u1* result = NULL;
    if (mode != JNI_COMMIT) {
        result = (u1*) GuardedCopy::destroy(dataBuf);
    } else {
        result = (u1*) (void*) GuardedCopy::fromData(dataBuf)->originalPtr;
    }

    /* pointer is to the array contents; back up to the array object */
    result -= OFFSETOF_MEMBER(ArrayObject, contents);
    return result;
}


/*
 * ===========================================================================
 *      JNI functions
 * ===========================================================================
 */

#define CHECK_JNI_ENTRY(flags, types, args...) \
    ScopedCheck sc(env, flags, __FUNCTION__); \
    sc.check(true, types, ##args)

#define CHECK_JNI_EXIT(type, exp) ({ \
    typeof (exp) _rc = (exp); \
    sc.check(false, type, _rc); \
    _rc; })
#define CHECK_JNI_EXIT_VOID() \
    sc.check(false, "V")

static jint UT_GetVersion(JNIEnv* env) {
    CHECK_JNI_ENTRY(kFlag_Default, "E", env);
    return CHECK_JNI_EXIT("I", baseEnv(env)->GetVersion(env));
}

static jclass UT_DefineClass(JNIEnv* env, const char* name, jobject loader,
    const jbyte* buf, jsize bufLen)
{
    CHECK_JNI_ENTRY(kFlag_Default, "EuLpz", env, name, loader, buf, bufLen);
    sc.checkClassName(name);
    return CHECK_JNI_EXIT("c", baseEnv(env)->DefineClass(env, name, loader, buf, bufLen));
}

static jclass UT_FindClass(JNIEnv* env, const char* name) {
    CHECK_JNI_ENTRY(kFlag_Default, "Eu", env, name);
    sc.checkClassName(name);
    return CHECK_JNI_EXIT("c", baseEnv(env)->FindClass(env, name));
}

static jclass UT_GetSuperclass(JNIEnv* env, jclass clazz) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ec", env, clazz);
    return CHECK_JNI_EXIT("c", baseEnv(env)->GetSuperclass(env, clazz));
}

static jboolean UT_IsAssignableFrom(JNIEnv* env, jclass clazz1, jclass clazz2) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ecc", env, clazz1, clazz2);
    return CHECK_JNI_EXIT("b", baseEnv(env)->IsAssignableFrom(env, clazz1, clazz2));
}

static jmethodID UT_FromReflectedMethod(JNIEnv* env, jobject method) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, method);
    // TODO: check that 'field' is a java.lang.reflect.Method.
    return CHECK_JNI_EXIT("m", baseEnv(env)->FromReflectedMethod(env, method));
}

static jfieldID UT_FromReflectedField(JNIEnv* env, jobject field) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, field);
    // TODO: check that 'field' is a java.lang.reflect.Field.
    return CHECK_JNI_EXIT("f", baseEnv(env)->FromReflectedField(env, field));
}

static jobject UT_ToReflectedMethod(JNIEnv* env, jclass cls,
        jmethodID methodID, jboolean isStatic)
{
    CHECK_JNI_ENTRY(kFlag_Default, "Ecmb", env, cls, methodID, isStatic);
    return CHECK_JNI_EXIT("L", baseEnv(env)->ToReflectedMethod(env, cls, methodID, isStatic));
}

static jobject UT_ToReflectedField(JNIEnv* env, jclass cls,
        jfieldID fieldID, jboolean isStatic)
{
    CHECK_JNI_ENTRY(kFlag_Default, "Ecfb", env, cls, fieldID, isStatic);
    return CHECK_JNI_EXIT("L", baseEnv(env)->ToReflectedField(env, cls, fieldID, isStatic));
}

static jint UT_Throw(JNIEnv* env, jthrowable obj) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, obj);
    // TODO: check that 'obj' is a java.lang.Throwable.
    return CHECK_JNI_EXIT("I", baseEnv(env)->Throw(env, obj));
}

static jint UT_ThrowNew(JNIEnv* env, jclass clazz, const char* message) {
    CHECK_JNI_ENTRY(kFlag_NullableUtf, "Ecu", env, clazz, message);
    return CHECK_JNI_EXIT("I", baseEnv(env)->ThrowNew(env, clazz, message));
}

static jthrowable UT_ExceptionOccurred(JNIEnv* env) {
    CHECK_JNI_ENTRY(kFlag_ExcepOkay, "E", env);
    return CHECK_JNI_EXIT("L", baseEnv(env)->ExceptionOccurred(env));
}

static void UT_ExceptionDescribe(JNIEnv* env) {
    CHECK_JNI_ENTRY(kFlag_ExcepOkay, "E", env);
    baseEnv(env)->ExceptionDescribe(env);
    CHECK_JNI_EXIT_VOID();
}

static void UT_ExceptionClear(JNIEnv* env) {
    CHECK_JNI_ENTRY(kFlag_ExcepOkay, "E", env);
    baseEnv(env)->ExceptionClear(env);
    CHECK_JNI_EXIT_VOID();
}

static void UT_FatalError(JNIEnv* env, const char* msg) {
    CHECK_JNI_ENTRY(kFlag_NullableUtf, "Eu", env, msg);
    baseEnv(env)->FatalError(env, msg);
    CHECK_JNI_EXIT_VOID();
}

static jint UT_PushLocalFrame(JNIEnv* env, jint capacity) {
    CHECK_JNI_ENTRY(kFlag_Default | kFlag_ExcepOkay, "EI", env, capacity);
    return CHECK_JNI_EXIT("I", baseEnv(env)->PushLocalFrame(env, capacity));
}

static jobject UT_PopLocalFrame(JNIEnv* env, jobject res) {
    CHECK_JNI_ENTRY(kFlag_Default | kFlag_ExcepOkay, "EL", env, res);
    return CHECK_JNI_EXIT("L", baseEnv(env)->PopLocalFrame(env, res));
}

static jobject UT_NewGlobalRef(JNIEnv* env, jobject obj) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, obj);
    return CHECK_JNI_EXIT("L", baseEnv(env)->NewGlobalRef(env, obj));
}

static void UT_DeleteGlobalRef(JNIEnv* env, jobject globalRef) {
    CHECK_JNI_ENTRY(kFlag_Default | kFlag_ExcepOkay, "EL", env, globalRef);
    if (globalRef != NULL && dvmGetJNIRefType(sc.self(), globalRef) != JNIGlobalRefType) {
        ALOGW("JNI WARNING: DeleteGlobalRef on non-global %p (type=%d)",
             globalRef, dvmGetJNIRefType(sc.self(), globalRef));
        abortMaybe();
    } else {
        baseEnv(env)->DeleteGlobalRef(env, globalRef);
        CHECK_JNI_EXIT_VOID();
    }
}

static jobject UT_NewLocalRef(JNIEnv* env, jobject ref) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, ref);
    return CHECK_JNI_EXIT("L", baseEnv(env)->NewLocalRef(env, ref));
}

static void UT_DeleteLocalRef(JNIEnv* env, jobject localRef) {
    CHECK_JNI_ENTRY(kFlag_Default | kFlag_ExcepOkay, "EL", env, localRef);
    if (localRef != NULL && dvmGetJNIRefType(sc.self(), localRef) != JNILocalRefType) {
        ALOGW("JNI WARNING: DeleteLocalRef on non-local %p (type=%d)",
             localRef, dvmGetJNIRefType(sc.self(), localRef));
        abortMaybe();
    } else {
        baseEnv(env)->DeleteLocalRef(env, localRef);
        CHECK_JNI_EXIT_VOID();
    }
}

static jint UT_EnsureLocalCapacity(JNIEnv *env, jint capacity) {
    CHECK_JNI_ENTRY(kFlag_Default, "EI", env, capacity);
    return CHECK_JNI_EXIT("I", baseEnv(env)->EnsureLocalCapacity(env, capacity));
}

static jboolean UT_IsSameObject(JNIEnv* env, jobject ref1, jobject ref2) {
    CHECK_JNI_ENTRY(kFlag_Default, "ELL", env, ref1, ref2);
    return CHECK_JNI_EXIT("b", baseEnv(env)->IsSameObject(env, ref1, ref2));
}

static jobject UT_AllocObject(JNIEnv* env, jclass clazz) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ec", env, clazz);
    return CHECK_JNI_EXIT("L", baseEnv(env)->AllocObject(env, clazz));
}

static jobject UT_NewObject(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ecm.", env, clazz, methodID);
    va_list args;
    va_start(args, methodID);
    jobject result = baseEnv(env)->NewObjectV(env, clazz, methodID, args);
    va_end(args);
    return CHECK_JNI_EXIT("L", result);
}

static jobject UT_NewObjectV(JNIEnv* env, jclass clazz, jmethodID methodID, va_list args) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ecm.", env, clazz, methodID);
    return CHECK_JNI_EXIT("L", baseEnv(env)->NewObjectV(env, clazz, methodID, args));
}

static jobject UT_NewObjectA(JNIEnv* env, jclass clazz, jmethodID methodID, jvalue* args) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ecm.", env, clazz, methodID);
    return CHECK_JNI_EXIT("L", baseEnv(env)->NewObjectA(env, clazz, methodID, args));
}

static jclass UT_GetObjectClass(JNIEnv* env, jobject obj) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, obj);
    return CHECK_JNI_EXIT("c", baseEnv(env)->GetObjectClass(env, obj));
}

static jboolean UT_IsInstanceOf(JNIEnv* env, jobject obj, jclass clazz) {
    CHECK_JNI_ENTRY(kFlag_Default, "ELc", env, obj, clazz);
    return CHECK_JNI_EXIT("b", baseEnv(env)->IsInstanceOf(env, obj, clazz));
}

static jmethodID UT_GetMethodID(JNIEnv* env, jclass clazz, const char* name, const char* sig) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ecuu", env, clazz, name, sig);
    return CHECK_JNI_EXIT("m", baseEnv(env)->GetMethodID(env, clazz, name, sig));
}

static jfieldID UT_GetFieldID(JNIEnv* env, jclass clazz, const char* name, const char* sig) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ecuu", env, clazz, name, sig);
    return CHECK_JNI_EXIT("f", baseEnv(env)->GetFieldID(env, clazz, name, sig));
}

static jmethodID UT_GetStaticMethodID(JNIEnv* env, jclass clazz,
        const char* name, const char* sig)
{
    CHECK_JNI_ENTRY(kFlag_Default, "Ecuu", env, clazz, name, sig);
    return CHECK_JNI_EXIT("m", baseEnv(env)->GetStaticMethodID(env, clazz, name, sig));
}

static jfieldID UT_GetStaticFieldID(JNIEnv* env, jclass clazz,
        const char* name, const char* sig)
{
    CHECK_JNI_ENTRY(kFlag_Default, "Ecuu", env, clazz, name, sig);
    return CHECK_JNI_EXIT("f", baseEnv(env)->GetStaticFieldID(env, clazz, name, sig));
}

#define FIELD_ACCESSORS(_ctype, _jname, _ftype, _type) \
    static _ctype UT_GetStatic##_jname##Field(JNIEnv* env, jclass clazz, jfieldID fieldID) { \
        CHECK_JNI_ENTRY(kFlag_Default, "Ecf", env, clazz, fieldID); \
        sc.checkStaticFieldID(clazz, fieldID); \
        sc.checkFieldTypeForGet(fieldID, _type, true); \
        return CHECK_JNI_EXIT(_type, baseEnv(env)->GetStatic##_jname##Field(env, clazz, fieldID)); \
    } \
    static _ctype UT_Get##_jname##Field(JNIEnv* env, jobject obj, jfieldID fieldID) { \
        CHECK_JNI_ENTRY(kFlag_Default, "ELf", env, obj, fieldID); \
        sc.checkInstanceFieldID(obj, fieldID); \
        sc.checkFieldTypeForGet(fieldID, _type, false); \
        return CHECK_JNI_EXIT(_type, baseEnv(env)->Get##_jname##Field(env, obj, fieldID)); \
    } \
    static void UT_SetStatic##_jname##Field(JNIEnv* env, jclass clazz, jfieldID fieldID, _ctype value) { \
        CHECK_JNI_ENTRY(kFlag_Default, "Ecf" _type, env, clazz, fieldID, value); \
        sc.checkStaticFieldID(clazz, fieldID); \
        /* "value" arg only used when type == ref */ \
        sc.checkFieldTypeForSet((jobject)(u4)value, fieldID, _ftype, true); \
        baseEnv(env)->SetStatic##_jname##Field(env, clazz, fieldID, value); \
        CHECK_JNI_EXIT_VOID(); \
    } \
    static void UT_Set##_jname##Field(JNIEnv* env, jobject obj, jfieldID fieldID, _ctype value) { \
        CHECK_JNI_ENTRY(kFlag_Default, "ELf" _type, env, obj, fieldID, value); \
        sc.checkInstanceFieldID(obj, fieldID); \
        /* "value" arg only used when type == ref */ \
        sc.checkFieldTypeForSet((jobject)(u4) value, fieldID, _ftype, false); \
        baseEnv(env)->Set##_jname##Field(env, obj, fieldID, value); \
        CHECK_JNI_EXIT_VOID(); \
    }

FIELD_ACCESSORS(jobject, Object, PRIM_NOT, "L");
FIELD_ACCESSORS(jboolean, Boolean, PRIM_BOOLEAN, "Z");
FIELD_ACCESSORS(jbyte, Byte, PRIM_BYTE, "B");
FIELD_ACCESSORS(jchar, Char, PRIM_CHAR, "C");
FIELD_ACCESSORS(jshort, Short, PRIM_SHORT, "S");
FIELD_ACCESSORS(jint, Int, PRIM_INT, "I");
FIELD_ACCESSORS(jlong, Long, PRIM_LONG, "J");
FIELD_ACCESSORS(jfloat, Float, PRIM_FLOAT, "F");
FIELD_ACCESSORS(jdouble, Double, PRIM_DOUBLE, "D");

#define CALL(_ctype, _jname, _retdecl, _retasgn, _retok, _retsig) \
    /* Virtual... */ \
    static _ctype UT_Call##_jname##Method(JNIEnv* env, jobject obj, \
        jmethodID methodID, ...) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "ELm.", env, obj, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, false); \
        sc.checkVirtualMethod(obj, methodID); \
        _retdecl; \
        va_list args; \
        va_start(args, methodID); \
        _retasgn baseEnv(env)->Call##_jname##MethodV(env, obj, methodID, args); \
        va_end(args); \
        _retok; \
    } \
    static _ctype UT_Call##_jname##MethodV(JNIEnv* env, jobject obj, \
        jmethodID methodID, va_list args) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "ELm.", env, obj, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, false); \
        sc.checkVirtualMethod(obj, methodID); \
        _retdecl; \
        _retasgn baseEnv(env)->Call##_jname##MethodV(env, obj, methodID, args); \
        _retok; \
    } \
    static _ctype UT_Call##_jname##MethodA(JNIEnv* env, jobject obj, \
        jmethodID methodID, jvalue* args) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "ELm.", env, obj, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, false); \
        sc.checkVirtualMethod(obj, methodID); \
        _retdecl; \
        _retasgn baseEnv(env)->Call##_jname##MethodA(env, obj, methodID, args); \
        _retok; \
    } \
    /* Non-virtual... */ \
    static _ctype UT_CallNonvirtual##_jname##Method(JNIEnv* env, \
        jobject obj, jclass clazz, jmethodID methodID, ...) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "ELcm.", env, obj, clazz, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, false); \
        sc.checkVirtualMethod(obj, methodID); \
        _retdecl; \
        va_list args; \
        va_start(args, methodID); \
        _retasgn baseEnv(env)->CallNonvirtual##_jname##MethodV(env, obj, clazz, methodID, args); \
        va_end(args); \
        _retok; \
    } \
    static _ctype UT_CallNonvirtual##_jname##MethodV(JNIEnv* env, \
        jobject obj, jclass clazz, jmethodID methodID, va_list args) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "ELcm.", env, obj, clazz, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, false); \
        sc.checkVirtualMethod(obj, methodID); \
        _retdecl; \
        _retasgn baseEnv(env)->CallNonvirtual##_jname##MethodV(env, obj, clazz, methodID, args); \
        _retok; \
    } \
    static _ctype UT_CallNonvirtual##_jname##MethodA(JNIEnv* env, \
        jobject obj, jclass clazz, jmethodID methodID, jvalue* args) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "ELcm.", env, obj, clazz, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, false); \
        sc.checkVirtualMethod(obj, methodID); \
        _retdecl; \
        _retasgn baseEnv(env)->CallNonvirtual##_jname##MethodA(env, obj, clazz, methodID, args); \
        _retok; \
    } \
    /* Static... */ \
    static _ctype UT_CallStatic##_jname##Method(JNIEnv* env, \
        jclass clazz, jmethodID methodID, ...) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "Ecm.", env, clazz, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, true); \
        sc.checkStaticMethod(clazz, methodID); \
        _retdecl; \
        va_list args; \
        va_start(args, methodID); \
        _retasgn baseEnv(env)->CallStatic##_jname##MethodV(env, clazz, methodID, args); \
        va_end(args); \
        _retok; \
    } \
    static _ctype UT_CallStatic##_jname##MethodV(JNIEnv* env, \
        jclass clazz, jmethodID methodID, va_list args) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "Ecm.", env, clazz, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, true); \
        sc.checkStaticMethod(clazz, methodID); \
        _retdecl; \
        _retasgn baseEnv(env)->CallStatic##_jname##MethodV(env, clazz, methodID, args); \
        _retok; \
    } \
    static _ctype UT_CallStatic##_jname##MethodA(JNIEnv* env, \
        jclass clazz, jmethodID methodID, jvalue* args) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "Ecm.", env, clazz, methodID); /* TODO: args! */ \
        sc.checkSig(methodID, _retsig, true); \
        sc.checkStaticMethod(clazz, methodID); \
        _retdecl; \
        _retasgn baseEnv(env)->CallStatic##_jname##MethodA(env, clazz, methodID, args); \
        _retok; \
    }

#define NON_VOID_RETURN(_retsig, _ctype) return CHECK_JNI_EXIT(_retsig, (_ctype) result)
#define VOID_RETURN CHECK_JNI_EXIT_VOID()

CALL(jobject, Object, Object* result, result=(Object*), NON_VOID_RETURN("L", jobject), "L");
CALL(jboolean, Boolean, jboolean result, result=, NON_VOID_RETURN("Z", jboolean), "Z");
CALL(jbyte, Byte, jbyte result, result=, NON_VOID_RETURN("B", jbyte), "B");
CALL(jchar, Char, jchar result, result=, NON_VOID_RETURN("C", jchar), "C");
CALL(jshort, Short, jshort result, result=, NON_VOID_RETURN("S", jshort), "S");
CALL(jint, Int, jint result, result=, NON_VOID_RETURN("I", jint), "I");
CALL(jlong, Long, jlong result, result=, NON_VOID_RETURN("J", jlong), "J");
CALL(jfloat, Float, jfloat result, result=, NON_VOID_RETURN("F", jfloat), "F");
CALL(jdouble, Double, jdouble result, result=, NON_VOID_RETURN("D", jdouble), "D");
CALL(void, Void, , , VOID_RETURN, "V");

static jstring UT_NewString(JNIEnv* env, const jchar* unicodeChars, jsize len) {
    CHECK_JNI_ENTRY(kFlag_Default, "Epz", env, unicodeChars, len);
    return CHECK_JNI_EXIT("s", baseEnv(env)->NewString(env, unicodeChars, len));
}

static jsize UT_GetStringLength(JNIEnv* env, jstring string) {
    CHECK_JNI_ENTRY(kFlag_CritOkay, "Es", env, string);
    return CHECK_JNI_EXIT("I", baseEnv(env)->GetStringLength(env, string));
}

static const jchar* UT_GetStringChars(JNIEnv* env, jstring string, jboolean* isCopy) {
    CHECK_JNI_ENTRY(kFlag_CritOkay, "Esp", env, string, isCopy);
    const jchar* result = baseEnv(env)->GetStringChars(env, string, isCopy);
    if (gDvmJni.forceCopy && result != NULL) {
        ScopedCheckJniThreadState ts(env);
        StringObject* strObj = (StringObject*) dvmDecodeIndirectRef(dvmThreadSelf(), string);
        int byteCount = strObj->length() * 2;
        result = (const jchar*) GuardedCopy::create(result, byteCount, false);
        if (isCopy != NULL) {
            *isCopy = JNI_TRUE;
        }
    }
    return CHECK_JNI_EXIT("p", result);
}

static void UT_ReleaseStringChars(JNIEnv* env, jstring string, const jchar* chars) {
    CHECK_JNI_ENTRY(kFlag_Default | kFlag_ExcepOkay, "Esp", env, string, chars);
    sc.checkNonNull(chars);
    if (gDvmJni.forceCopy) {
        if (!GuardedCopy::check(chars, false)) {
            ALOGE("JNI: failed guarded copy check in ReleaseStringChars");
            abortMaybe();
            return;
        }
        chars = (const jchar*) GuardedCopy::destroy((jchar*)chars);
    }
    baseEnv(env)->ReleaseStringChars(env, string, chars);
    CHECK_JNI_EXIT_VOID();
}

static jstring UT_NewStringUTF(JNIEnv* env, const char* bytes) {
    CHECK_JNI_ENTRY(kFlag_NullableUtf, "Eu", env, bytes); // TODO: show pointer and truncate string.
    return CHECK_JNI_EXIT("s", baseEnv(env)->NewStringUTF(env, bytes));
}

static jsize UT_GetStringUTFLength(JNIEnv* env, jstring string) {
    CHECK_JNI_ENTRY(kFlag_CritOkay, "Es", env, string);
    return CHECK_JNI_EXIT("I", baseEnv(env)->GetStringUTFLength(env, string));
}

static const char* UT_GetStringUTFChars(JNIEnv* env, jstring string, jboolean* isCopy) {
    CHECK_JNI_ENTRY(kFlag_CritOkay, "Esp", env, string, isCopy);
    const char* result = baseEnv(env)->GetStringUTFChars(env, string, isCopy);
    if (gDvmJni.forceCopy && result != NULL) {
        result = (const char*) GuardedCopy::create(result, strlen(result) + 1, false);
        if (isCopy != NULL) {
            *isCopy = JNI_TRUE;
        }
    }
    return CHECK_JNI_EXIT("u", result); // TODO: show pointer and truncate string.
}

static void UT_ReleaseStringUTFChars(JNIEnv* env, jstring string, const char* utf) {
    CHECK_JNI_ENTRY(kFlag_ExcepOkay | kFlag_Release, "Esu", env, string, utf); // TODO: show pointer and truncate string.
    if (gDvmJni.forceCopy) {
        if (!GuardedCopy::check(utf, false)) {
            ALOGE("JNI: failed guarded copy check in ReleaseStringUTFChars");
            abortMaybe();
            return;
        }
        utf = (const char*) GuardedCopy::destroy((char*)utf);
    }
    baseEnv(env)->ReleaseStringUTFChars(env, string, utf);
    CHECK_JNI_EXIT_VOID();
}

static jsize UT_GetArrayLength(JNIEnv* env, jarray array) {
    CHECK_JNI_ENTRY(kFlag_CritOkay, "Ea", env, array);
    return CHECK_JNI_EXIT("I", baseEnv(env)->GetArrayLength(env, array));
}

static jobjectArray UT_NewObjectArray(JNIEnv* env, jsize length,
        jclass elementClass, jobject initialElement)
{
    CHECK_JNI_ENTRY(kFlag_Default, "EzcL", env, length, elementClass, initialElement);
    return CHECK_JNI_EXIT("a", baseEnv(env)->NewObjectArray(env, length, elementClass, initialElement));
}

static jobject UT_GetObjectArrayElement(JNIEnv* env, jobjectArray array, jsize index) {
    CHECK_JNI_ENTRY(kFlag_Default, "EaI", env, array, index);
    return CHECK_JNI_EXIT("L", baseEnv(env)->GetObjectArrayElement(env, array, index));
}

static void UT_SetObjectArrayElement(JNIEnv* env, jobjectArray array, jsize index, jobject value)
{
    CHECK_JNI_ENTRY(kFlag_Default, "EaIL", env, array, index, value);
    baseEnv(env)->SetObjectArrayElement(env, array, index, value);
    CHECK_JNI_EXIT_VOID();
}

#define NEW_PRIMITIVE_ARRAY(_artype, _jname) \
    static _artype UT_New##_jname##Array(JNIEnv* env, jsize length) { \
        CHECK_JNI_ENTRY(kFlag_Default, "Ez", env, length); \
        return CHECK_JNI_EXIT("a", baseEnv(env)->New##_jname##Array(env, length)); \
    }
NEW_PRIMITIVE_ARRAY(jbooleanArray, Boolean);
NEW_PRIMITIVE_ARRAY(jbyteArray, Byte);
NEW_PRIMITIVE_ARRAY(jcharArray, Char);
NEW_PRIMITIVE_ARRAY(jshortArray, Short);
NEW_PRIMITIVE_ARRAY(jintArray, Int);
NEW_PRIMITIVE_ARRAY(jlongArray, Long);
NEW_PRIMITIVE_ARRAY(jfloatArray, Float);
NEW_PRIMITIVE_ARRAY(jdoubleArray, Double);


/*
 * Hack to allow forcecopy to work with jniGetNonMovableArrayElements.
 * The code deliberately uses an invalid sequence of operations, so we
 * need to pass it through unmodified.  Review that code before making
 * any changes here.
 */
#define kNoCopyMagic    0xd5aab57f

#define GET_PRIMITIVE_ARRAY_ELEMENTS(_ctype, _jname) \
    static _ctype* UT_Get##_jname##ArrayElements(JNIEnv* env, \
        _ctype##Array array, jboolean* isCopy) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default, "Eap", env, array, isCopy); \
        u4 noCopy = 0; \
        if (gDvmJni.forceCopy && isCopy != NULL) { \
            /* capture this before the base call tramples on it */ \
            noCopy = *(u4*) isCopy; \
        } \
        _ctype* result = baseEnv(env)->Get##_jname##ArrayElements(env, array, isCopy); \
        if (gDvmJni.forceCopy && result != NULL) { \
            if (noCopy == kNoCopyMagic) { \
                ALOGV("FC: not copying %p %x", array, noCopy); \
            } else { \
                result = (_ctype*) createGuardedPACopy(env, array, isCopy); \
            } \
        } \
        return CHECK_JNI_EXIT("p", result); \
    }

#define RELEASE_PRIMITIVE_ARRAY_ELEMENTS(_ctype, _jname) \
    static void UT_Release##_jname##ArrayElements(JNIEnv* env, \
        _ctype##Array array, _ctype* elems, jint mode) \
    { \
        CHECK_JNI_ENTRY(kFlag_Default | kFlag_ExcepOkay, "Eapr", env, array, elems, mode); \
        sc.checkNonNull(elems); \
        if (gDvmJni.forceCopy) { \
            if ((uintptr_t)elems == kNoCopyMagic) { \
                ALOGV("FC: not freeing %p", array); \
                elems = NULL;   /* base JNI call doesn't currently need */ \
            } else { \
                elems = (_ctype*) releaseGuardedPACopy(env, array, elems, mode); \
            } \
        } \
        baseEnv(env)->Release##_jname##ArrayElements(env, array, elems, mode); \
        CHECK_JNI_EXIT_VOID(); \
    }

#define GET_PRIMITIVE_ARRAY_REGION(_ctype, _jname) \
    static void UT_Get##_jname##ArrayRegion(JNIEnv* env, \
            _ctype##Array array, jsize start, jsize len, _ctype* buf) { \
        CHECK_JNI_ENTRY(kFlag_Default, "EaIIp", env, array, start, len, buf); \
        baseEnv(env)->Get##_jname##ArrayRegion(env, array, start, len, buf); \
        CHECK_JNI_EXIT_VOID(); \
    }

#define SET_PRIMITIVE_ARRAY_REGION(_ctype, _jname) \
    static void UT_Set##_jname##ArrayRegion(JNIEnv* env, \
            _ctype##Array array, jsize start, jsize len, const _ctype* buf) { \
        CHECK_JNI_ENTRY(kFlag_Default, "EaIIp", env, array, start, len, buf); \
        baseEnv(env)->Set##_jname##ArrayRegion(env, array, start, len, buf); \
        CHECK_JNI_EXIT_VOID(); \
    }

#define PRIMITIVE_ARRAY_FUNCTIONS(_ctype, _jname, _typechar) \
    GET_PRIMITIVE_ARRAY_ELEMENTS(_ctype, _jname); \
    RELEASE_PRIMITIVE_ARRAY_ELEMENTS(_ctype, _jname); \
    GET_PRIMITIVE_ARRAY_REGION(_ctype, _jname); \
    SET_PRIMITIVE_ARRAY_REGION(_ctype, _jname);

/* TODO: verify primitive array type matches call type */
PRIMITIVE_ARRAY_FUNCTIONS(jboolean, Boolean, 'Z');
PRIMITIVE_ARRAY_FUNCTIONS(jbyte, Byte, 'B');
PRIMITIVE_ARRAY_FUNCTIONS(jchar, Char, 'C');
PRIMITIVE_ARRAY_FUNCTIONS(jshort, Short, 'S');
PRIMITIVE_ARRAY_FUNCTIONS(jint, Int, 'I');
PRIMITIVE_ARRAY_FUNCTIONS(jlong, Long, 'J');
PRIMITIVE_ARRAY_FUNCTIONS(jfloat, Float, 'F');
PRIMITIVE_ARRAY_FUNCTIONS(jdouble, Double, 'D');

static jint UT_RegisterNatives(JNIEnv* env, jclass clazz, const JNINativeMethod* methods,
        jint nMethods)
{
    CHECK_JNI_ENTRY(kFlag_Default, "EcpI", env, clazz, methods, nMethods);
    return CHECK_JNI_EXIT("I", baseEnv(env)->RegisterNatives(env, clazz, methods, nMethods));
}

static jint UT_UnregisterNatives(JNIEnv* env, jclass clazz) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ec", env, clazz);
    return CHECK_JNI_EXIT("I", baseEnv(env)->UnregisterNatives(env, clazz));
}

static jint UT_MonitorEnter(JNIEnv* env, jobject obj) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, obj);
    return CHECK_JNI_EXIT("I", baseEnv(env)->MonitorEnter(env, obj));
}

static jint UT_MonitorExit(JNIEnv* env, jobject obj) {
    CHECK_JNI_ENTRY(kFlag_Default | kFlag_ExcepOkay, "EL", env, obj);
    return CHECK_JNI_EXIT("I", baseEnv(env)->MonitorExit(env, obj));
}

static jint UT_GetJavaVM(JNIEnv *env, JavaVM **vm) {
    CHECK_JNI_ENTRY(kFlag_Default, "Ep", env, vm);
    return CHECK_JNI_EXIT("I", baseEnv(env)->GetJavaVM(env, vm));
}

static void UT_GetStringRegion(JNIEnv* env, jstring str, jsize start, jsize len, jchar* buf) {
    CHECK_JNI_ENTRY(kFlag_CritOkay, "EsIIp", env, str, start, len, buf);
    baseEnv(env)->GetStringRegion(env, str, start, len, buf);
    CHECK_JNI_EXIT_VOID();
}

static void UT_GetStringUTFRegion(JNIEnv* env, jstring str, jsize start, jsize len, char* buf) {
    CHECK_JNI_ENTRY(kFlag_CritOkay, "EsIIp", env, str, start, len, buf);
    baseEnv(env)->GetStringUTFRegion(env, str, start, len, buf);
    CHECK_JNI_EXIT_VOID();
}

static void* UT_GetPrimitiveArrayCritical(JNIEnv* env, jarray array, jboolean* isCopy) {
    CHECK_JNI_ENTRY(kFlag_CritGet, "Eap", env, array, isCopy);
    void* result = baseEnv(env)->GetPrimitiveArrayCritical(env, array, isCopy);
    if (gDvmJni.forceCopy && result != NULL) {
        result = createGuardedPACopy(env, array, isCopy);
    }
    return CHECK_JNI_EXIT("p", result);
}

static void UT_ReleasePrimitiveArrayCritical(JNIEnv* env, jarray array, void* carray, jint mode)
{
    CHECK_JNI_ENTRY(kFlag_CritRelease | kFlag_ExcepOkay, "Eapr", env, array, carray, mode);
    sc.checkNonNull(carray);
    if (gDvmJni.forceCopy) {
        carray = releaseGuardedPACopy(env, array, carray, mode);
    }
    baseEnv(env)->ReleasePrimitiveArrayCritical(env, array, carray, mode);
    CHECK_JNI_EXIT_VOID();
}

static const jchar* UT_GetStringCritical(JNIEnv* env, jstring string, jboolean* isCopy) {
    CHECK_JNI_ENTRY(kFlag_CritGet, "Esp", env, string, isCopy);
    const jchar* result = baseEnv(env)->GetStringCritical(env, string, isCopy);
    if (gDvmJni.forceCopy && result != NULL) {
        ScopedCheckJniThreadState ts(env);
        StringObject* strObj = (StringObject*) dvmDecodeIndirectRef(dvmThreadSelf(), string);
        int byteCount = strObj->length() * 2;
        result = (const jchar*) GuardedCopy::create(result, byteCount, false);
        if (isCopy != NULL) {
            *isCopy = JNI_TRUE;
        }
    }
    return CHECK_JNI_EXIT("p", result);
}

static void UT_ReleaseStringCritical(JNIEnv* env, jstring string, const jchar* carray) {
    CHECK_JNI_ENTRY(kFlag_CritRelease | kFlag_ExcepOkay, "Esp", env, string, carray);
    sc.checkNonNull(carray);
    if (gDvmJni.forceCopy) {
        if (!GuardedCopy::check(carray, false)) {
            ALOGE("JNI: failed guarded copy check in ReleaseStringCritical");
            abortMaybe();
            return;
        }
        carray = (const jchar*) GuardedCopy::destroy((jchar*)carray);
    }
    baseEnv(env)->ReleaseStringCritical(env, string, carray);
    CHECK_JNI_EXIT_VOID();
}

static jweak UT_NewWeakGlobalRef(JNIEnv* env, jobject obj) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, obj);
    return CHECK_JNI_EXIT("L", baseEnv(env)->NewWeakGlobalRef(env, obj));
}

static void UT_DeleteWeakGlobalRef(JNIEnv* env, jweak obj) {
    CHECK_JNI_ENTRY(kFlag_Default | kFlag_ExcepOkay, "EL", env, obj);
    baseEnv(env)->DeleteWeakGlobalRef(env, obj);
    CHECK_JNI_EXIT_VOID();
}

static jboolean UT_ExceptionCheck(JNIEnv* env) {
    CHECK_JNI_ENTRY(kFlag_CritOkay | kFlag_ExcepOkay, "E", env);
    return CHECK_JNI_EXIT("b", baseEnv(env)->ExceptionCheck(env));
}

static jobjectRefType UT_GetObjectRefType(JNIEnv* env, jobject obj) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, obj);
    // TODO: proper decoding of jobjectRefType!
    return CHECK_JNI_EXIT("I", baseEnv(env)->GetObjectRefType(env, obj));
}

static jobject UT_NewDirectByteBuffer(JNIEnv* env, void* address, jlong capacity) {
    CHECK_JNI_ENTRY(kFlag_Default, "EpJ", env, address, capacity);
    return CHECK_JNI_EXIT("L", baseEnv(env)->NewDirectByteBuffer(env, address, capacity));
}

static void* UT_GetDirectBufferAddress(JNIEnv* env, jobject buf) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, buf);
    // TODO: check that 'buf' is a java.nio.Buffer.
    return CHECK_JNI_EXIT("p", baseEnv(env)->GetDirectBufferAddress(env, buf));
}

static jlong UT_GetDirectBufferCapacity(JNIEnv* env, jobject buf) {
    CHECK_JNI_ENTRY(kFlag_Default, "EL", env, buf);
    // TODO: check that 'buf' is a java.nio.Buffer.
    return CHECK_JNI_EXIT("J", baseEnv(env)->GetDirectBufferCapacity(env, buf));
}


/*
 * ===========================================================================
 *      JNI invocation functions
 * ===========================================================================
 */

static jint UT_DestroyJavaVM(JavaVM* vm) {
    ScopedCheck sc(false, __FUNCTION__);
    sc.check(true, "v", vm);
    return CHECK_JNI_EXIT("I", baseVm(vm)->DestroyJavaVM(vm));
}

static jint UT_AttachCurrentThread(JavaVM* vm, JNIEnv** p_env, void* thr_args) {
    ScopedCheck sc(false, __FUNCTION__);
    sc.check(true, "vpt", vm, p_env, thr_args);
    return CHECK_JNI_EXIT("I", baseVm(vm)->AttachCurrentThread(vm, p_env, thr_args));
}

static jint UT_AttachCurrentThreadAsDaemon(JavaVM* vm, JNIEnv** p_env, void* thr_args) {
    ScopedCheck sc(false, __FUNCTION__);
    sc.check(true, "vpt", vm, p_env, thr_args);
    return CHECK_JNI_EXIT("I", baseVm(vm)->AttachCurrentThreadAsDaemon(vm, p_env, thr_args));
}

static jint UT_DetachCurrentThread(JavaVM* vm) {
    ScopedCheck sc(true, __FUNCTION__);
    sc.check(true, "v", vm);
    return CHECK_JNI_EXIT("I", baseVm(vm)->DetachCurrentThread(vm));
}

static jint UT_GetEnv(JavaVM* vm, void** env, jint version) {
    ScopedCheck sc(true, __FUNCTION__);
    sc.check(true, "v", vm);
    return CHECK_JNI_EXIT("I", baseVm(vm)->GetEnv(vm, env, version));
}


/*
 * ===========================================================================
 *      Function tables
 * ===========================================================================
 */

static const struct JNINativeInterface gUTNativeInterface = {
    NULL,
    NULL,
    NULL,
    NULL,

    UT_GetVersion,

    UT_DefineClass,
    UT_FindClass,

    UT_FromReflectedMethod,
    UT_FromReflectedField,
    UT_ToReflectedMethod,

    UT_GetSuperclass,
    UT_IsAssignableFrom,

    UT_ToReflectedField,

    UT_Throw,
    UT_ThrowNew,
    UT_ExceptionOccurred,
    UT_ExceptionDescribe,
    UT_ExceptionClear,
    UT_FatalError,

    UT_PushLocalFrame,
    UT_PopLocalFrame,

    UT_NewGlobalRef,
    UT_DeleteGlobalRef,
    UT_DeleteLocalRef,
    UT_IsSameObject,
    UT_NewLocalRef,
    UT_EnsureLocalCapacity,

    UT_AllocObject,
    UT_NewObject,
    UT_NewObjectV,
    UT_NewObjectA,

    UT_GetObjectClass,
    UT_IsInstanceOf,

    UT_GetMethodID,

    UT_CallObjectMethod,
    UT_CallObjectMethodV,
    UT_CallObjectMethodA,
    UT_CallBooleanMethod,
    UT_CallBooleanMethodV,
    UT_CallBooleanMethodA,
    UT_CallByteMethod,
    UT_CallByteMethodV,
    UT_CallByteMethodA,
    UT_CallCharMethod,
    UT_CallCharMethodV,
    UT_CallCharMethodA,
    UT_CallShortMethod,
    UT_CallShortMethodV,
    UT_CallShortMethodA,
    UT_CallIntMethod,
    UT_CallIntMethodV,
    UT_CallIntMethodA,
    UT_CallLongMethod,
    UT_CallLongMethodV,
    UT_CallLongMethodA,
    UT_CallFloatMethod,
    UT_CallFloatMethodV,
    UT_CallFloatMethodA,
    UT_CallDoubleMethod,
    UT_CallDoubleMethodV,
    UT_CallDoubleMethodA,
    UT_CallVoidMethod,
    UT_CallVoidMethodV,
    UT_CallVoidMethodA,

    UT_CallNonvirtualObjectMethod,
    UT_CallNonvirtualObjectMethodV,
    UT_CallNonvirtualObjectMethodA,
    UT_CallNonvirtualBooleanMethod,
    UT_CallNonvirtualBooleanMethodV,
    UT_CallNonvirtualBooleanMethodA,
    UT_CallNonvirtualByteMethod,
    UT_CallNonvirtualByteMethodV,
    UT_CallNonvirtualByteMethodA,
    UT_CallNonvirtualCharMethod,
    UT_CallNonvirtualCharMethodV,
    UT_CallNonvirtualCharMethodA,
    UT_CallNonvirtualShortMethod,
    UT_CallNonvirtualShortMethodV,
    UT_CallNonvirtualShortMethodA,
    UT_CallNonvirtualIntMethod,
    UT_CallNonvirtualIntMethodV,
    UT_CallNonvirtualIntMethodA,
    UT_CallNonvirtualLongMethod,
    UT_CallNonvirtualLongMethodV,
    UT_CallNonvirtualLongMethodA,
    UT_CallNonvirtualFloatMethod,
    UT_CallNonvirtualFloatMethodV,
    UT_CallNonvirtualFloatMethodA,
    UT_CallNonvirtualDoubleMethod,
    UT_CallNonvirtualDoubleMethodV,
    UT_CallNonvirtualDoubleMethodA,
    UT_CallNonvirtualVoidMethod,
    UT_CallNonvirtualVoidMethodV,
    UT_CallNonvirtualVoidMethodA,

    UT_GetFieldID,

    UT_GetObjectField,
    UT_GetBooleanField,
    UT_GetByteField,
    UT_GetCharField,
    UT_GetShortField,
    UT_GetIntField,
    UT_GetLongField,
    UT_GetFloatField,
    UT_GetDoubleField,
    UT_SetObjectField,
    UT_SetBooleanField,
    UT_SetByteField,
    UT_SetCharField,
    UT_SetShortField,
    UT_SetIntField,
    UT_SetLongField,
    UT_SetFloatField,
    UT_SetDoubleField,

    UT_GetStaticMethodID,

    UT_CallStaticObjectMethod,
    UT_CallStaticObjectMethodV,
    UT_CallStaticObjectMethodA,
    UT_CallStaticBooleanMethod,
    UT_CallStaticBooleanMethodV,
    UT_CallStaticBooleanMethodA,
    UT_CallStaticByteMethod,
    UT_CallStaticByteMethodV,
    UT_CallStaticByteMethodA,
    UT_CallStaticCharMethod,
    UT_CallStaticCharMethodV,
    UT_CallStaticCharMethodA,
    UT_CallStaticShortMethod,
    UT_CallStaticShortMethodV,
    UT_CallStaticShortMethodA,
    UT_CallStaticIntMethod,
    UT_CallStaticIntMethodV,
    UT_CallStaticIntMethodA,
    UT_CallStaticLongMethod,
    UT_CallStaticLongMethodV,
    UT_CallStaticLongMethodA,
    UT_CallStaticFloatMethod,
    UT_CallStaticFloatMethodV,
    UT_CallStaticFloatMethodA,
    UT_CallStaticDoubleMethod,
    UT_CallStaticDoubleMethodV,
    UT_CallStaticDoubleMethodA,
    UT_CallStaticVoidMethod,
    UT_CallStaticVoidMethodV,
    UT_CallStaticVoidMethodA,

    UT_GetStaticFieldID,

    UT_GetStaticObjectField,
    UT_GetStaticBooleanField,
    UT_GetStaticByteField,
    UT_GetStaticCharField,
    UT_GetStaticShortField,
    UT_GetStaticIntField,
    UT_GetStaticLongField,
    UT_GetStaticFloatField,
    UT_GetStaticDoubleField,

    UT_SetStaticObjectField,
    UT_SetStaticBooleanField,
    UT_SetStaticByteField,
    UT_SetStaticCharField,
    UT_SetStaticShortField,
    UT_SetStaticIntField,
    UT_SetStaticLongField,
    UT_SetStaticFloatField,
    UT_SetStaticDoubleField,

    UT_NewString,

    UT_GetStringLength,
    UT_GetStringChars,
    UT_ReleaseStringChars,

    UT_NewStringUTF,
    UT_GetStringUTFLength,
    UT_GetStringUTFChars,
    UT_ReleaseStringUTFChars,

    UT_GetArrayLength,
    UT_NewObjectArray,
    UT_GetObjectArrayElement,
    UT_SetObjectArrayElement,

    UT_NewBooleanArray,
    UT_NewByteArray,
    UT_NewCharArray,
    UT_NewShortArray,
    UT_NewIntArray,
    UT_NewLongArray,
    UT_NewFloatArray,
    UT_NewDoubleArray,

    UT_GetBooleanArrayElements,
    UT_GetByteArrayElements,
    UT_GetCharArrayElements,
    UT_GetShortArrayElements,
    UT_GetIntArrayElements,
    UT_GetLongArrayElements,
    UT_GetFloatArrayElements,
    UT_GetDoubleArrayElements,

    UT_ReleaseBooleanArrayElements,
    UT_ReleaseByteArrayElements,
    UT_ReleaseCharArrayElements,
    UT_ReleaseShortArrayElements,
    UT_ReleaseIntArrayElements,
    UT_ReleaseLongArrayElements,
    UT_ReleaseFloatArrayElements,
    UT_ReleaseDoubleArrayElements,

    UT_GetBooleanArrayRegion,
    UT_GetByteArrayRegion,
    UT_GetCharArrayRegion,
    UT_GetShortArrayRegion,
    UT_GetIntArrayRegion,
    UT_GetLongArrayRegion,
    UT_GetFloatArrayRegion,
    UT_GetDoubleArrayRegion,
    UT_SetBooleanArrayRegion,
    UT_SetByteArrayRegion,
    UT_SetCharArrayRegion,
    UT_SetShortArrayRegion,
    UT_SetIntArrayRegion,
    UT_SetLongArrayRegion,
    UT_SetFloatArrayRegion,
    UT_SetDoubleArrayRegion,

    UT_RegisterNatives,
    UT_UnregisterNatives,

    UT_MonitorEnter,
    UT_MonitorExit,

    UT_GetJavaVM,

    UT_GetStringRegion,
    UT_GetStringUTFRegion,

    UT_GetPrimitiveArrayCritical,
    UT_ReleasePrimitiveArrayCritical,

    UT_GetStringCritical,
    UT_ReleaseStringCritical,

    UT_NewWeakGlobalRef,
    UT_DeleteWeakGlobalRef,

    UT_ExceptionCheck,

    UT_NewDirectByteBuffer,
    UT_GetDirectBufferAddress,
    UT_GetDirectBufferCapacity,

    UT_GetObjectRefType
};

static const struct JNIInvokeInterface gUTInvokeInterface = {
    NULL,
    NULL,
    NULL,

    UT_DestroyJavaVM,
    UT_AttachCurrentThread,
    UT_DetachCurrentThread,

    UT_GetEnv,

    UT_AttachCurrentThreadAsDaemon,
};

/*
 * Replace the normal table with the checked table.
 */
struct JNINativeInterface* dvmGetSandboxedFuncTable(void) {
    return &gUTNativeInterface;
}
