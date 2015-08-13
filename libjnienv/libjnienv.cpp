#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <map>

#include "cutils/log.h"

#define __get_tls() \
        ({ register unsigned int __val; \
         asm ("mrc p15, 0, %0, c13, c0, 3" : "=r"(__val)); \
         (volatile void*) __val; })

#define __set_tls(arg) \
        asm volatile( "push {r0, r7}\n" \
                "mov r0, %[tls]\n" \
                "ldr r7, =0x000f0005\n" \
                "svc #0\n" \
                "pop {r0, r7}\n" \
                : : [tls] "r" (arg));

#define __do_log 0

#define jump_out() \
    ({ if(__do_log) ALOGE("%s ----> at %d", __FUNCTION__, __LINE__); \
    void** new_tls = reinterpret_cast<void**>(const_cast<void*>(__get_tls())); \
    void* old_tls = *(void**)((unsigned long)new_tls-sizeof(void*)); \
    __set_tls(old_tls); \
    void** tmp = reinterpret_cast<void**>(const_cast<void*>(__get_tls())); \
    asm volatile( \
            "mov ip, r7\n" \
            "ldr r7, =0x17d\n" \
            "svc #0\n" \
            "mov r7, ip\n" \
            : : ); \
    new_tls; })

#define jump_in(tls) \
    asm volatile( \
            "mov ip, r7\n" \
            "ldr r7, =0x17c\n" \
            "svc #0\n" \
            "mov r7, ip\n" \
            : : ); \
    if(__do_log) ALOGE("%s ----< at %d", __FUNCTION__, __LINE__); \
    __set_tls(tls);

/*
 * TLS to JNIEnv*
 */
std::map<void*, JNIEnv*> env_map;
#define gEnv \
    env_map[const_cast<void*>(__get_tls())]

/*
 * native ptr to java ptr
 */
std::map<void*, void*> fake_ptr;
#define set_fake_ptr(a, b) \
    fake_ptr[(void*)(a)] = (void*)(b)
#define release_fake_ptr(type, a, b) \
    type a = (type)fake_ptr[(void*)(b)]; \
    fake_ptr.erase((void*)(b))

/*
 * ===========================================================================
 *      JNI implementation
 * ===========================================================================
 */

static jint UT_GetVersion(JNIEnv* env) {
    void** tls = jump_out();
    jint ret = gEnv->GetVersion();
    jump_in(tls);
    return ret;
}

static jclass UT_DefineClass(JNIEnv* env, const char *name, jobject loader,
    const jbyte* buf, jsize bufLen)
{
    void** tls = jump_out();
    jclass ret = gEnv->DefineClass(name, loader, buf, bufLen);
    jump_in(tls);
    return ret;
}

static jclass UT_FindClass(JNIEnv* env, const char* name) {
    void** tls = jump_out();
    jclass ret = gEnv->FindClass(name);
    jump_in(tls);
    return ret;
}

static jclass UT_GetSuperclass(JNIEnv* env, jclass jclazz) {
    void** tls = jump_out();
    jclass ret = gEnv->GetSuperclass(jclazz);
    jump_in(tls);
    return ret;
}

static jboolean UT_IsAssignableFrom(JNIEnv* env, jclass jclazz1, jclass jclazz2) {
    void** tls = jump_out();
    jboolean ret = gEnv->IsAssignableFrom(jclazz1, jclazz2);
    jump_in(tls);
    return ret;
}

static jmethodID UT_FromReflectedMethod(JNIEnv* env, jobject jmethod) {
    void** tls = jump_out();
    jmethodID ret = gEnv->FromReflectedMethod(jmethod);
    jump_in(tls);
    return ret;
}

static jfieldID UT_FromReflectedField(JNIEnv* env, jobject jfield) {
    void** tls = jump_out();
    jfieldID ret = gEnv->FromReflectedField(jfield);
    jump_in(tls);
    return ret;
}

static jobject UT_ToReflectedMethod(JNIEnv* env, jclass jcls, jmethodID methodID, jboolean isStatic) {
    void** tls = jump_out();
    jobject ret = gEnv->ToReflectedMethod(jcls, methodID, isStatic);
    jump_in(tls);
    return ret;
}

static jobject UT_ToReflectedField(JNIEnv* env, jclass jcls, jfieldID fieldID, jboolean isStatic) {
    void** tls = jump_out();
    jobject ret = gEnv->ToReflectedField(jcls, fieldID, isStatic);
    jump_in(tls);
    return ret;
}

static jint UT_Throw(JNIEnv* env, jthrowable jobj) {
    void** tls = jump_out();
    jint ret = gEnv->Throw(jobj);
    jump_in(tls);
    return ret;
}

static jint UT_ThrowNew(JNIEnv* env, jclass jcls, const char* message) {
    void** tls = jump_out();
    jint ret = gEnv->ThrowNew(jcls, message);
    jump_in(tls);
    return ret;
}

static jthrowable UT_ExceptionOccurred(JNIEnv* env) {
    void** tls = jump_out();
    jthrowable ret = gEnv->ExceptionOccurred();
    jump_in(tls);
    return ret;
}

static void UT_ExceptionDescribe(JNIEnv* env) {
    void** tls = jump_out();
    gEnv->ExceptionDescribe();
    jump_in(tls);
}

static void UT_ExceptionClear(JNIEnv* env) {
    void** tls = jump_out();
    gEnv->ExceptionClear();
    jump_in(tls);
}

static void UT_FatalError(JNIEnv* env, const char* msg) {
    void** tls = jump_out();
    gEnv->FatalError(msg);
    jump_in(tls);
}

static jint UT_PushLocalFrame(JNIEnv* env, jint capacity) {
    void** tls = jump_out();
    jint ret = gEnv->PushLocalFrame(capacity);
    jump_in(tls);
    return ret;
}

static jobject UT_PopLocalFrame(JNIEnv* env, jobject jresult) {
    void** tls = jump_out();
    jobject ret = gEnv->PopLocalFrame(jresult);
    jump_in(tls);
    return ret;
}

static jobject UT_NewGlobalRef(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jobject ret = gEnv->NewGlobalRef(jobj);
    jump_in(tls);
    return ret;
}

static void UT_DeleteGlobalRef(JNIEnv* env, jobject jglobalRef) {
    void** tls = jump_out();
    gEnv->DeleteGlobalRef(jglobalRef);
    jump_in(tls);
}

static jobject UT_NewLocalRef(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jobject ret = gEnv->NewLocalRef(jobj);
    jump_in(tls);
    return ret;
}

static void UT_DeleteLocalRef(JNIEnv* env, jobject jlocalRef) {
    void** tls = jump_out();
    gEnv->DeleteLocalRef(jlocalRef);
    jump_in(tls);
}

static jint UT_EnsureLocalCapacity(JNIEnv* env, jint capacity) {
    void** tls = jump_out();
    jint ret = gEnv->EnsureLocalCapacity(capacity);
    jump_in(tls);
    return ret;
}

static jboolean UT_IsSameObject(JNIEnv* env, jobject jref1, jobject jref2) {
    void** tls = jump_out();
    jboolean ret = gEnv->IsSameObject(jref1, jref2);
    jump_in(tls);
    return ret;
}

static jobject UT_AllocObject(JNIEnv* env, jclass jcls) {
    void** tls = jump_out();
    jobject ret = gEnv->AllocObject(jcls);
    jump_in(tls);
    return ret;
}

static jobject UT_NewObject(JNIEnv* env, jclass jclazz, jmethodID methodID, ...) {
    void** tls = jump_out();
    va_list args;
    va_start(args, methodID);
    jobject ret = gEnv->NewObjectV(jclazz, methodID, args);
    va_end(args);
    jump_in(tls);
    return ret;
}

static jobject UT_NewObjectV(JNIEnv* env, jclass jclazz, jmethodID methodID, va_list args) {
    void** tls = jump_out();
    jobject ret = gEnv->NewObjectV(jclazz, methodID, args);
    jump_in(tls);
    return ret;
}

static jobject UT_NewObjectA(JNIEnv* env, jclass jclazz, jmethodID methodID, jvalue* args) {
    void** tls = jump_out();
    jobject ret = gEnv->NewObjectA(jclazz, methodID, args);
    jump_in(tls);
    return ret;
}

static jclass UT_GetObjectClass(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jclass ret = gEnv->GetObjectClass(jobj);
    jump_in(tls);
    return ret;
}

static jboolean UT_IsInstanceOf(JNIEnv* env, jobject jobj, jclass jclazz) {
    void** tls = jump_out();
    jboolean ret = gEnv->IsInstanceOf(jobj, jclazz);
    jump_in(tls);
    return ret;
}

static jmethodID UT_GetMethodID(JNIEnv* env, jclass jclazz, const char* name, const char* sig) {
    void** tls = jump_out();
    jmethodID ret = gEnv->GetMethodID(jclazz, name, sig);
    jump_in(tls);
    return ret;
}

static jfieldID UT_GetFieldID(JNIEnv* env, jclass jclazz, const char* name, const char* sig) {
    void** tls = jump_out();
    jfieldID ret = gEnv->GetFieldID(jclazz, name, sig);
    jump_in(tls);
    return ret;
}

static jmethodID UT_GetStaticMethodID(JNIEnv* env, jclass jclazz, const char* name, const char* sig) {
    void** tls = jump_out();
    jmethodID ret = gEnv->GetStaticMethodID(jclazz, name, sig);
    jump_in(tls);
    return ret;
}

static jfieldID UT_GetStaticFieldID(JNIEnv* env, jclass jclazz, const char* name, const char* sig) {
    void** tls = jump_out();
    jfieldID ret = gEnv->GetStaticFieldID(jclazz, name, sig);
    jump_in(tls);
    return ret;
}

#define GET_STATIC_TYPE_FIELD(_ctype, _jname, _isref)                       \
    static _ctype UT_GetStatic##_jname##Field(JNIEnv* env, jclass jclazz,   \
        jfieldID fieldID)                                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype value = gEnv->GetStatic##_jname##Field(jclazz, fieldID); \
        jump_in(tls);                                                          \
        return value;                                                       \
    }
GET_STATIC_TYPE_FIELD(jobject, Object, true);
GET_STATIC_TYPE_FIELD(jboolean, Boolean, false);
GET_STATIC_TYPE_FIELD(jbyte, Byte, false);
GET_STATIC_TYPE_FIELD(jchar, Char, false);
GET_STATIC_TYPE_FIELD(jshort, Short, false);
GET_STATIC_TYPE_FIELD(jint, Int, false);
GET_STATIC_TYPE_FIELD(jlong, Long, false);
GET_STATIC_TYPE_FIELD(jfloat, Float, false);
GET_STATIC_TYPE_FIELD(jdouble, Double, false);

#define SET_STATIC_TYPE_FIELD(_ctype, _ctype2, _jname, _isref)              \
static     void UT_SetStatic##_jname##Field(JNIEnv* env, jclass jclazz,        \
        jfieldID fieldID, _ctype value)                                     \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->SetStatic##_jname##Field(jclazz, fieldID, value);       \
        jump_in(tls);                                                          \
    }
SET_STATIC_TYPE_FIELD(jobject, Object*, Object, true);
SET_STATIC_TYPE_FIELD(jboolean, bool, Boolean, false);
SET_STATIC_TYPE_FIELD(jbyte, s1, Byte, false);
SET_STATIC_TYPE_FIELD(jchar, u2, Char, false);
SET_STATIC_TYPE_FIELD(jshort, s2, Short, false);
SET_STATIC_TYPE_FIELD(jint, s4, Int, false);
SET_STATIC_TYPE_FIELD(jlong, s8, Long, false);
SET_STATIC_TYPE_FIELD(jfloat, float, Float, false);
SET_STATIC_TYPE_FIELD(jdouble, double, Double, false);

#define GET_TYPE_FIELD(_ctype, _jname, _isref)                              \
static     _ctype UT_Get##_jname##Field(JNIEnv* env, jobject jobj,             \
        jfieldID fieldID)                                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype value = gEnv->Get##_jname##Field(jobj, fieldID);       \
        jump_in(tls);                                                          \
        return value;                                                       \
    }
GET_TYPE_FIELD(jobject, Object, true);
GET_TYPE_FIELD(jboolean, Boolean, false);
GET_TYPE_FIELD(jbyte, Byte, false);
GET_TYPE_FIELD(jchar, Char, false);
GET_TYPE_FIELD(jshort, Short, false);
GET_TYPE_FIELD(jint, Int, false);
GET_TYPE_FIELD(jlong, Long, false);
GET_TYPE_FIELD(jfloat, Float, false);
GET_TYPE_FIELD(jdouble, Double, false);

#define SET_TYPE_FIELD(_ctype, _ctype2, _jname, _isref)                     \
static     void UT_Set##_jname##Field(JNIEnv* env, jobject jobj,               \
        jfieldID fieldID, _ctype value)                                     \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->Set##_jname##Field(jobj, fieldID, value);         \
        jump_in(tls);                                                          \
    }
SET_TYPE_FIELD(jobject, Object*, Object, true);
SET_TYPE_FIELD(jboolean, bool, Boolean, false);
SET_TYPE_FIELD(jbyte, s1, Byte, false);
SET_TYPE_FIELD(jchar, u2, Char, false);
SET_TYPE_FIELD(jshort, s2, Short, false);
SET_TYPE_FIELD(jint, s4, Int, false);
SET_TYPE_FIELD(jlong, s8, Long, false);
SET_TYPE_FIELD(jfloat, float, Float, false);
SET_TYPE_FIELD(jdouble, double, Double, false);

#define CALL_VIRTUAL(_ctype, _jname)                                        \
static     _ctype UT_Call##_jname##Method(JNIEnv* env, jobject jobj,           \
        jmethodID methodID, ...)                                            \
    {                                                                       \
        void** tls = jump_out();                                                         \
        va_list args; \
        va_start(args, methodID);                                           \
        _ctype ret = gEnv->Call##_jname##MethodV(jobj, methodID, args); \
        va_end(args);                                                       \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_Call##_jname##MethodV(JNIEnv* env, jobject jobj,          \
        jmethodID methodID, va_list args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->Call##_jname##MethodV(jobj, methodID, args); \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_Call##_jname##MethodA(JNIEnv* env, jobject jobj,          \
        jmethodID methodID, jvalue* args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->Call##_jname##MethodA(jobj, methodID, args); \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }
#define CALL_VIRTUAL_VOID                                                   \
static     void UT_CallVoidMethod(JNIEnv* env, jobject jobj,                   \
        jmethodID methodID, ...)                                            \
    {                                                                       \
        void** tls = jump_out();                                                         \
        va_list args; \
        va_start(args, methodID);                                           \
        gEnv->CallVoidMethodV(jobj, methodID, args); \
        va_end(args);                                                       \
        jump_in(tls);                                                          \
    }                                                                       \
static     void UT_CallVoidMethodV(JNIEnv* env, jobject jobj,          \
        jmethodID methodID, va_list args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallVoidMethodV(jobj, methodID, args); \
        jump_in(tls);                                                          \
    }                                                                       \
static     void UT_CallVoidMethodA(JNIEnv* env, jobject jobj,          \
        jmethodID methodID, jvalue* args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallVoidMethodA(jobj, methodID, args); \
        jump_in(tls);                                                          \
    }
CALL_VIRTUAL(jobject, Object);
CALL_VIRTUAL(jboolean, Boolean);
CALL_VIRTUAL(jbyte, Byte);
CALL_VIRTUAL(jchar, Char);
CALL_VIRTUAL(jshort, Short);
CALL_VIRTUAL(jint, Int);
CALL_VIRTUAL(jlong, Long);
CALL_VIRTUAL(jfloat, Float);
CALL_VIRTUAL(jdouble, Double);
CALL_VIRTUAL_VOID;

#define CALL_NONVIRTUAL(_ctype, _jname)                                     \
static     _ctype UT_CallNonvirtual##_jname##Method(JNIEnv* env, jobject jobj, \
        jclass jclazz, jmethodID methodID, ...)                             \
    {                                                                       \
        void** tls = jump_out();                                                         \
        va_list args; \
        va_start(args, methodID);                                           \
        _ctype ret = gEnv->CallNonvirtual##_jname##MethodV(jobj,      \
                jclazz, methodID, args);                                    \
        va_end(args);                                                       \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_CallNonvirtual##_jname##MethodV(JNIEnv* env, jobject jobj,\
        jclass jclazz, jmethodID methodID, va_list args)                    \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->CallNonvirtual##_jname##MethodV(jobj,      \
                jclazz, methodID, args);                                    \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_CallNonvirtual##_jname##MethodA(JNIEnv* env, jobject jobj,\
        jclass jclazz, jmethodID methodID, jvalue* args)                    \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->CallNonvirtual##_jname##MethodA(jobj,      \
                jclazz, methodID, args);                                    \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }
#define CALL_NONVIRTUAL_VOID                                     \
static     void UT_CallNonvirtualVoidMethod(JNIEnv* env, jobject jobj, \
        jclass jclazz, jmethodID methodID, ...)                             \
    {                                                                       \
        void** tls = jump_out();                                                         \
        va_list args; \
        va_start(args, methodID);                                           \
        gEnv->CallNonvirtualVoidMethodV(jobj,      \
                jclazz, methodID, args);                                    \
        va_end(args);                                                       \
        jump_in(tls);                                                          \
    }                                                                       \
static     void UT_CallNonvirtualVoidMethodV(JNIEnv* env, jobject jobj,\
        jclass jclazz, jmethodID methodID, va_list args)                    \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallNonvirtualVoidMethodV(jobj,      \
                jclazz, methodID, args);                                    \
        jump_in(tls);                                                          \
    }                                                                       \
static     void UT_CallNonvirtualVoidMethodA(JNIEnv* env, jobject jobj,\
        jclass jclazz, jmethodID methodID, jvalue* args)                    \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallNonvirtualVoidMethodA(jobj,      \
                jclazz, methodID, args);                                    \
        jump_in(tls);                                                          \
    }
CALL_NONVIRTUAL(jobject, Object);
CALL_NONVIRTUAL(jboolean, Boolean);
CALL_NONVIRTUAL(jbyte, Byte);
CALL_NONVIRTUAL(jchar, Char);
CALL_NONVIRTUAL(jshort, Short);
CALL_NONVIRTUAL(jint, Int);
CALL_NONVIRTUAL(jlong, Long);
CALL_NONVIRTUAL(jfloat, Float);
CALL_NONVIRTUAL(jdouble, Double);
CALL_NONVIRTUAL_VOID;


#define CALL_STATIC(_ctype, _jname)                                         \
static     _ctype UT_CallStatic##_jname##Method(JNIEnv* env, jclass jclazz,    \
        jmethodID methodID, ...)                                            \
    {                                                                       \
        void** tls = jump_out();                                                         \
        va_list args; \
        va_start(args, methodID);                                           \
        _ctype ret = gEnv->CallStatic##_jname##MethodV(jclazz, methodID, args); \
        va_end(args);                                                       \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_CallStatic##_jname##MethodV(JNIEnv* env, jclass jclazz,   \
        jmethodID methodID, va_list args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->CallStatic##_jname##MethodV(jclazz, methodID, args); \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_CallStatic##_jname##MethodA(JNIEnv* env, jclass jclazz,   \
        jmethodID methodID, jvalue* args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->CallStatic##_jname##MethodA(jclazz, methodID, args); \
        jump_in(tls);                                                          \
        return ret;                                                         \
    }
#define CALL_STATIC_VOID                                         \
static     void UT_CallStaticVoidMethod(JNIEnv* env, jclass jclazz,    \
        jmethodID methodID, ...)                                            \
    {                                                                       \
        void** tls = jump_out();                                                         \
        va_list args; \
        va_start(args, methodID);                                           \
        gEnv->CallStaticVoidMethodV(jclazz, methodID, args); \
        va_end(args);                                                       \
        jump_in(tls);                                                          \
    }                                                                       \
static     void UT_CallStaticVoidMethodV(JNIEnv* env, jclass jclazz,   \
        jmethodID methodID, va_list args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallStaticVoidMethodV(jclazz, methodID, args); \
        jump_in(tls);                                                          \
    }                                                                       \
static     void UT_CallStaticVoidMethodA(JNIEnv* env, jclass jclazz,   \
        jmethodID methodID, jvalue* args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallStaticVoidMethodA(jclazz, methodID, args); \
        jump_in(tls);                                                          \
    }
CALL_STATIC(jobject, Object);
CALL_STATIC(jboolean, Boolean);
CALL_STATIC(jbyte, Byte);
CALL_STATIC(jchar, Char);
CALL_STATIC(jshort, Short);
CALL_STATIC(jint, Int);
CALL_STATIC(jlong, Long);
CALL_STATIC(jfloat, Float);
CALL_STATIC(jdouble, Double);
CALL_STATIC_VOID;

static jstring UT_NewString(JNIEnv* env, const jchar* unicodeChars, jsize len) {
    void** tls = jump_out();
    jstring ret = gEnv->NewString(unicodeChars, len);
    jump_in(tls);
    return ret;
}

static jsize UT_GetStringLength(JNIEnv* env, jstring jstr) {
    void** tls = jump_out();
    jsize ret = gEnv->GetStringLength(jstr);
    jump_in(tls);
    return ret;
}

static const jchar* UT_GetStringChars(JNIEnv* env, jstring jstr, jboolean* isCopy) {
    void** tls = jump_out();
    jsize len = gEnv->GetStringLength(jstr);
    if (!len) {
        jump_in(tls);
        return NULL;
    }
    jchar* buf = NULL;
    if (len) {
        buf = (jchar*)malloc(sizeof(jchar) * (len+1));
        const jchar* str = gEnv->GetStringChars(jstr, isCopy);
        set_fake_ptr(buf, str);
        memcpy(buf, str, sizeof(jchar) * len);
        buf[len] = '\0';
    }
    jump_in(tls);
    return (const jchar*)buf;
}

static void UT_ReleaseStringChars(JNIEnv* env, jstring jstr, const jchar* chars) {
    if (!chars) return;
    void** tls = jump_out();
    release_fake_ptr(const jchar*, str, chars);
    gEnv->ReleaseStringChars(jstr, str);
    jump_in(tls);
    free((void*) chars);
}

static jstring UT_NewStringUTF(JNIEnv* env, const char* bytes) {
    void** tls = jump_out();
    jstring ret = gEnv->NewStringUTF(bytes);
    jump_in(tls);
    return ret;
}

static jsize UT_GetStringUTFLength(JNIEnv* env, jstring jstr) {
    void** tls = jump_out();
    jsize ret = gEnv->GetStringUTFLength(jstr);
    jump_in(tls);
    return ret;
}

static const char* UT_GetStringUTFChars(JNIEnv* env, jstring jstr, jboolean* isCopy) {
    void** tls = jump_out();
    jsize len = gEnv->GetStringUTFLength(jstr);
    if (!len) {
        jump_in(tls);
        return NULL;
    }
    char* buf = NULL;
    if (len) {
        buf = (char*)malloc(len+1);
        const char* str = gEnv->GetStringUTFChars(jstr, isCopy);
        set_fake_ptr(buf, str);
        memcpy(buf, str, len);
        buf[len] = '\0';
    }
    jump_in(tls);
    return (const char*)buf;
}

static void UT_ReleaseStringUTFChars(JNIEnv* env, jstring jstr, const char* utf) {
    if (!utf) return;
    void** tls = jump_out();
    release_fake_ptr(const char*, str, utf);
    gEnv->ReleaseStringUTFChars(jstr, str);
    jump_in(tls);
    free((char*) utf);
}

static jsize UT_GetArrayLength(JNIEnv* env, jarray jarr) {
    void** tls = jump_out();
    jsize ret = gEnv->GetArrayLength(jarr);
    jump_in(tls);
    return ret;
}

static jobjectArray UT_NewObjectArray(JNIEnv* env, jsize length,
    jclass jelementClass, jobject jinitialElement)
{
    void** tls = jump_out();
    jobjectArray ret = gEnv->NewObjectArray(length, jelementClass, jinitialElement);
    jump_in(tls);
    return ret;
}

static jobject UT_GetObjectArrayElement(JNIEnv* env, jobjectArray jarr, jsize index) {
    void** tls = jump_out();
    jobject ret = gEnv->GetObjectArrayElement(jarr, index);
    jump_in(tls);
    return ret;
}

static void UT_SetObjectArrayElement(JNIEnv* env, jobjectArray jarr, jsize index, jobject jobj) {
    void** tls = jump_out();
    gEnv->SetObjectArrayElement(jarr, index, jobj);
    jump_in(tls);
}

#define NEW_PRIMITIVE_ARRAY(_artype, _jname) \
static     _artype UT_New##_jname##Array(JNIEnv* env, jsize length) { \
        void** tls = jump_out(); \
        _artype result = gEnv->New##_jname##Array(length); \
        jump_in(tls); \
        return result; \
    }
NEW_PRIMITIVE_ARRAY(jbooleanArray, Boolean);
NEW_PRIMITIVE_ARRAY(jbyteArray, Byte);
NEW_PRIMITIVE_ARRAY(jcharArray, Char);
NEW_PRIMITIVE_ARRAY(jshortArray, Short);
NEW_PRIMITIVE_ARRAY(jintArray, Int);
NEW_PRIMITIVE_ARRAY(jlongArray, Long);
NEW_PRIMITIVE_ARRAY(jfloatArray, Float);
NEW_PRIMITIVE_ARRAY(jdoubleArray, Double);

#define GET_PRIMITIVE_ARRAY_ELEMENTS(_ctype, _jname) \
static     _ctype* UT_Get##_jname##ArrayElements(JNIEnv* env, \
        _ctype##Array jarr, jboolean* isCopy) \
    { \
        void** tls = jump_out(); \
        jsize len = gEnv->GetArrayLength(jarr); \
        if (!len) { \
            jump_in(tls); \
            return NULL; \
        } \
        _ctype* data = NULL; \
        if (len) { \
            data = (_ctype*)malloc(sizeof(_ctype)*(len+1)); \
            _ctype* val = gEnv->Get##_jname##ArrayElements(jarr, isCopy); \
            set_fake_ptr(data, val); \
            memcpy(data, val, sizeof(_ctype)*len); \
            data[len] = (_ctype)0; \
        } \
        jump_in(tls); \
        return data; \
    }

#define RELEASE_PRIMITIVE_ARRAY_ELEMENTS(_ctype, _jname)                    \
static     void UT_Release##_jname##ArrayElements(JNIEnv* env,                 \
        _ctype##Array jarr, _ctype* elems, jint mode)                       \
    {                                                                       \
        if (!elems) return; \
        void** tls = jump_out(); \
        release_fake_ptr(_ctype *, data, elems); \
        gEnv->Release##_jname##ArrayElements(jarr, data, mode); \
        jump_in(tls); \
        free((void*) elems); \
    }

#define GET_PRIMITIVE_ARRAY_REGION(_ctype, _jname) \
static     void UT_Get##_jname##ArrayRegion(JNIEnv* env, \
        _ctype##Array jarr, jsize start, jsize len, _ctype* buf) \
    { \
        void** tls = jump_out(); \
        gEnv->Get##_jname##ArrayRegion(jarr, start, len, buf); \
        jump_in(tls); \
    }

#define SET_PRIMITIVE_ARRAY_REGION(_ctype, _jname) \
static     void UT_Set##_jname##ArrayRegion(JNIEnv* env, \
        _ctype##Array jarr, jsize start, jsize len, const _ctype* buf) \
    { \
        void** tls = jump_out(); \
        gEnv->Set##_jname##ArrayRegion(jarr, start, len, buf); \
        jump_in(tls); \
    }

/*
 * 4-in-1:
 *  Get<Type>ArrayElements
 *  Release<Type>ArrayElements
 *  Get<Type>ArrayRegion
 *  Set<Type>ArrayRegion
 */
#define PRIMITIVE_ARRAY_FUNCTIONS(_ctype, _jname)                           \
    GET_PRIMITIVE_ARRAY_ELEMENTS(_ctype, _jname);                           \
    RELEASE_PRIMITIVE_ARRAY_ELEMENTS(_ctype, _jname);                       \
    GET_PRIMITIVE_ARRAY_REGION(_ctype, _jname);                             \
    SET_PRIMITIVE_ARRAY_REGION(_ctype, _jname);

PRIMITIVE_ARRAY_FUNCTIONS(jboolean, Boolean);
PRIMITIVE_ARRAY_FUNCTIONS(jbyte, Byte);
PRIMITIVE_ARRAY_FUNCTIONS(jchar, Char);
PRIMITIVE_ARRAY_FUNCTIONS(jshort, Short);
PRIMITIVE_ARRAY_FUNCTIONS(jint, Int);
PRIMITIVE_ARRAY_FUNCTIONS(jlong, Long);
PRIMITIVE_ARRAY_FUNCTIONS(jfloat, Float);
PRIMITIVE_ARRAY_FUNCTIONS(jdouble, Double);

static jint UT_RegisterNatives(JNIEnv* env, jclass jclazz,
    const JNINativeMethod* methods, jint nMethods)
{
    void** tls = jump_out();
    jint ret = gEnv->RegisterNatives(jclazz, methods, nMethods);
    jump_in(tls);
    return ret;
}

static jint UT_UnregisterNatives(JNIEnv* env, jclass jclazz) {
    void** tls = jump_out();
    jint ret = gEnv->UnregisterNatives(jclazz);
    jump_in(tls);
    return ret;
}

static jint UT_MonitorEnter(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jint ret = gEnv->MonitorEnter(jobj);
    jump_in(tls);
    return ret;
}

static jint UT_MonitorExit(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jint ret = gEnv->MonitorExit(jobj);
    jump_in(tls);
    return ret;
}

static jint UT_GetJavaVM(JNIEnv* env, JavaVM** vm) {
    void** tls = jump_out();
    jint ret = gEnv->GetJavaVM(vm);
    jump_in(tls);
    return ret;
}

static void UT_GetStringRegion(JNIEnv* env, jstring jstr, jsize start, jsize len, jchar* buf) {
    void** tls = jump_out();
    gEnv->GetStringRegion(jstr, start, len, buf);
    jump_in(tls);
}

static void UT_GetStringUTFRegion(JNIEnv* env, jstring jstr, jsize start, jsize len, char* buf) {
    void** tls = jump_out();
    gEnv->GetStringUTFRegion(jstr, start, len, buf);
    jump_in(tls);
}

//TODO
static void* UT_GetPrimitiveArrayCritical(JNIEnv* env, jarray jarr, jboolean* isCopy) {
    void** tls = jump_out();
    jsize len = gEnv->GetArrayLength(jarr);
    if (!len) {
        jump_in(tls);
        return NULL;
    }
    void* buf = NULL;
    if (len) {
        buf = malloc(8*len);
        void* ret = gEnv->GetPrimitiveArrayCritical(jarr, isCopy);
        set_fake_ptr(buf, ret);
        memcpy(buf, ret, 8*len);
    }
    jump_in(tls);
    return buf;
}

static void UT_ReleasePrimitiveArrayCritical(JNIEnv* env, jarray jarr, void* carray, jint mode) {
    if (!carray) return;
    void** tls = jump_out();
    release_fake_ptr(void*, __arr, carray);
    gEnv->ReleasePrimitiveArrayCritical(jarr, __arr, mode);
    jump_in(tls);
    free(carray);
}

static const jchar* UT_GetStringCritical(JNIEnv* env, jstring jstr, jboolean* isCopy) {
    void** tls = jump_out();
    jsize len = gEnv->GetStringLength(jstr);
    if (!len) {
        jump_in(tls);
        return NULL;
    }
    jchar* buf = NULL;
    if (len) {
        buf = (jchar*)malloc(sizeof(jchar) * (len+1));
        const jchar* ret = gEnv->GetStringCritical(jstr, isCopy);
        set_fake_ptr(buf, ret);
        memcpy(buf, ret, sizeof(jchar) * len);
        buf[len] = '\0';
    }
    jump_in(tls);
    return (const jchar*)buf;
}

static void UT_ReleaseStringCritical(JNIEnv* env, jstring jstr, const jchar* carray) {
    if (!carray) return;
    void** tls = jump_out();
    release_fake_ptr(const jchar*, str, carray);
    gEnv->ReleaseStringCritical(jstr, str);
    jump_in(tls);
    free((void*) carray);
}

static jweak UT_NewWeakGlobalRef(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jweak ret = gEnv->NewWeakGlobalRef(jobj);
    jump_in(tls);
    return ret;
}

static void UT_DeleteWeakGlobalRef(JNIEnv* env, jweak wref) {
    void** tls = jump_out();
    gEnv->DeleteWeakGlobalRef(wref);
    jump_in(tls);
}

static jboolean UT_ExceptionCheck(JNIEnv* env) {
    void** tls = jump_out();
    jboolean ret = gEnv->ExceptionCheck();
    jump_in(tls);
    return ret;
}

static jobjectRefType UT_GetObjectRefType(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jobjectRefType ret = gEnv->GetObjectRefType(jobj);
    jump_in(tls);
    return ret;
}

static jobject UT_NewDirectByteBuffer(JNIEnv* env, void* address, jlong capacity) {
    void** tls = jump_out();
    jobject ret = gEnv->NewDirectByteBuffer(address, capacity);
    jump_in(tls);
    return ret;
}

//TODO
static void* UT_GetDirectBufferAddress(JNIEnv* env, jobject jbuf) {
    void** tls = jump_out();
    void* ret = gEnv->GetDirectBufferAddress(jbuf);
    jump_in(tls);
    return ret;
}

static jlong UT_GetDirectBufferCapacity(JNIEnv* env, jobject jbuf) {
    void** tls = jump_out();
    jlong ret = gEnv->GetDirectBufferCapacity(jbuf);
    jump_in(tls);
    return ret;
}


/*
 * ===========================================================================
 *      Function tables
 * ===========================================================================
 */

static const struct JNINativeInterface gBridgeInterface = {
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

static JNIEnv gBridgeEnv;
extern "C" JNIEnv* init_libjnienv(void) {
    gBridgeEnv.functions = &gBridgeInterface;
    return &gBridgeEnv;
}

extern "C" void set_jnienv(JNIEnv* env) {
    gEnv = env;
}
