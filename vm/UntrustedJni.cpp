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
#include <dlfcn.h>

#if defined(__arm__)
#include <sys/mman.h>

#include "../libjnienv/structhelpfunc.h"

#define SECTION_SIZE (1<<20)
#define HEAP_SIZE (256*SECTION_SIZE)
#define READ_HEAP_SECTIONS 255
#define MODULAR(ptr, size) ((unsigned long)(ptr) % size)
#define ROUND_UP(ptr, size) \
    (MODULAR(ptr, size) ? \
     (unsigned long)(ptr) - MODULAR(ptr, size) + size : (unsigned long)(ptr))

static void* utm_handle = NULL;
static void (*ut_init_malloc) ( void* , size_t ) = NULL;
static void* (*ut_malloc) ( size_t ) = NULL;
static void (*ut_free) ( void* ) = NULL;

static void* heap = NULL;

static void* env_cpy(const char** envp) {
    const char** p = envp;
    size_t envs = 0;
    while (*p++) ++envs;
    char* env = (char*)&((char**)heap)[envs+1];
    p = envp;
    envs = 0;
    while (*p) {
        ((char**)heap)[envs] = env;
        const char* str = *p;
        while(*str) *env++ = *str++;
        *env++ = '\0';
        ++p;
        ++envs;
    }
    ((char**)heap)[envs] = NULL;
    return (void*)env;
}

struct aux {
    unsigned long type;
    unsigned long value;
};

static void setup_sys_env(void* f1, void* f2) {
    void* handle = dlopen("libc.so\0", RTLD_LAZY);
    const char** (*ut_get_envp)(void) =
        (const char** (*)(void)) dlsym(handle, "ut_get_envp\0");
    const aux* (*ut_get_auxv)(void) =
        (const aux* (*)(void)) dlsym(handle, "ut_get_auxv\0");

    const char** envp = ut_get_envp();
    aux* heap_ = (aux*)env_cpy(envp);
    const aux* auxv = ut_get_auxv();

    void (*ut_set_envp)(char**) = (void (*)(char**))f1;
    ut_set_envp((char**)heap);

    void (*ut_set_auxv)(void*) = (void (*)(void*))f2;
    ut_set_auxv((void*)heap_);

    // copy auxv
    for (const aux* v = auxv; v->type != 0; ++v) {
        heap_->type = v->type;
        heap_->value = v->value;
        ++heap_;
    }
    heap = (void*)ROUND_UP(heap_, PAGESIZE);
}

static void __utm_init(void) {
    if (utm_handle) return;

    void* addr = NULL;
    utm_handle = dlopen_in_sandbox("_libc.so\0", RTLD_LAZY, &addr);
    ut_init_malloc = (void (*) ( void* , size_t )) dlsym_in_sandbox(utm_handle,
            "init_malloc\0");
    ut_malloc = (void* (*) ( size_t )) dlsym_in_sandbox(utm_handle, "malloc\0");
    ut_free = (void (*) ( void* )) dlsym_in_sandbox(utm_handle, "free\0");

    void* base = mmap(NULL, HEAP_SIZE,
            PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    heap = (void*)ROUND_UP(base, SECTION_SIZE);
    if (base < heap)
        munmap(base, (unsigned long)heap - (unsigned long)base);
    for (unsigned long i = 0; i < READ_HEAP_SECTIONS; ++i) {
        *(int *)((unsigned long)heap + i*SECTION_SIZE) = 3;
    }

    asm volatile(
            "push {r0, r1, r7}\n"
            "mov r0, %[base]\n"
            "mov r1, 255\n"
            "ldr r7, =0x17e\n"
            "svc #0\n"
            "pop {r0, r1, r7}\n"
            : : [base] "r" (heap));

    setup_sys_env(dlsym(utm_handle, "ut_set_envp\0"),
            dlsym(utm_handle, "ut_set_auxv\0"));

    ut_init_malloc(heap, READ_HEAP_SECTIONS*SECTION_SIZE);
}

void dvmUntrustedInit(void) {
    static bool once = true;
    if (once) {
        __utm_init();
        once = false;
    }
}

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
    new_tls; })

/* sys_jump_in */
#define jump_in(tls, ret) \
    if(__do_log) ALOGE("%s ----< at %d", __FUNCTION__, __LINE__); \
    __set_tls(tls); \
    asm volatile( \
            "mov r0, %0\n" \
            "ldr r7, =384\n" \
            "svc #0\n" \
            : : "r" (ret))

#define jump_in2(tls) \
    if(__do_log) ALOGE("%s ----< at %d", __FUNCTION__, __LINE__); \
    __set_tls(tls); \
    asm volatile( \
            "ldr r7, =384\n" \
            "svc #0\n" \
            : : )

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
std::map<void*, size_t> fake_ptr_size;
#define set_fake_ptr(a, b, size) \
    fake_ptr[(void*)(a)] = (void*)(b); \
    fake_ptr_size[(void*)(a)] = size; \
    memcpy((void*)(a), (void*)(b), size)
#define release_fake_ptr(type, a, b) \
    type a = (type)fake_ptr[(void*)(b)]; \
    fake_ptr.erase((void*)(b)); \
    memcpy((void*)(a), (void*)(b), fake_ptr_size[(void*)(b)]); \
    fake_ptr_size.erase((void*)(b)); \
    ut_free((void*)b)

/*
 * ===========================================================================
 *      JNI implementation
 * ===========================================================================
 */

static jint UT_GetVersion(JNIEnv* env) {
    void** tls = jump_out();
    jint ret = gEnv->GetVersion();
    jump_in(tls, ret);
    /* not reached */
    return ret;
}

static jclass UT_DefineClass(JNIEnv* env, const char *name, jobject loader,
    const jbyte* buf, jsize bufLen)
{
    void** tls = jump_out();
    jclass ret = gEnv->DefineClass(name, loader, buf, bufLen);
    jump_in(tls, ret);
    return ret;
}

static jclass UT_FindClass(JNIEnv* env, const char* name) {
    void** tls = jump_out();
    jclass ret = gEnv->FindClass(name);
    jump_in(tls, ret);
    return ret;
}

static jclass UT_GetSuperclass(JNIEnv* env, jclass jclazz) {
    void** tls = jump_out();
    jclass ret = gEnv->GetSuperclass(jclazz);
    jump_in(tls, ret);
    return ret;
}

static jboolean UT_IsAssignableFrom(JNIEnv* env, jclass jclazz1, jclass jclazz2) {
    void** tls = jump_out();
    jboolean ret = gEnv->IsAssignableFrom(jclazz1, jclazz2);
    jump_in(tls, ret);
    return ret;
}

static jmethodID UT_FromReflectedMethod(JNIEnv* env, jobject jmethod) {
    void** tls = jump_out();
    jmethodID ret = gEnv->FromReflectedMethod(jmethod);
    jump_in(tls, ret);
    return ret;
}

static jfieldID UT_FromReflectedField(JNIEnv* env, jobject jfield) {
    void** tls = jump_out();
    jfieldID ret = gEnv->FromReflectedField(jfield);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_ToReflectedMethod(JNIEnv* env, jclass jcls, jmethodID methodID, jboolean isStatic) {
    void** tls = jump_out();
    jobject ret = gEnv->ToReflectedMethod(jcls, methodID, isStatic);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_ToReflectedField(JNIEnv* env, jclass jcls, jfieldID fieldID, jboolean isStatic) {
    void** tls = jump_out();
    jobject ret = gEnv->ToReflectedField(jcls, fieldID, isStatic);
    jump_in(tls, ret);
    return ret;
}

static jint UT_Throw(JNIEnv* env, jthrowable jobj) {
    void** tls = jump_out();
    jint ret = gEnv->Throw(jobj);
    jump_in(tls, ret);
    return ret;
}

static jint UT_ThrowNew(JNIEnv* env, jclass jcls, const char* message) {
    void** tls = jump_out();
    jint ret = gEnv->ThrowNew(jcls, message);
    jump_in(tls, ret);
    return ret;
}

static jthrowable UT_ExceptionOccurred(JNIEnv* env) {
    void** tls = jump_out();
    jthrowable ret = gEnv->ExceptionOccurred();
    jump_in(tls, ret);
    return ret;
}

static void UT_ExceptionDescribe(JNIEnv* env) {
    void** tls = jump_out();
    gEnv->ExceptionDescribe();
    jump_in2(tls);
}

static void UT_ExceptionClear(JNIEnv* env) {
    void** tls = jump_out();
    gEnv->ExceptionClear();
    jump_in2(tls);
}

static void UT_FatalError(JNIEnv* env, const char* msg) {
    void** tls = jump_out();
    gEnv->FatalError(msg);
    jump_in2(tls);
}

static jint UT_PushLocalFrame(JNIEnv* env, jint capacity) {
    void** tls = jump_out();
    jint ret = gEnv->PushLocalFrame(capacity);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_PopLocalFrame(JNIEnv* env, jobject jresult) {
    void** tls = jump_out();
    jobject ret = gEnv->PopLocalFrame(jresult);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_NewGlobalRef(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jobject ret = gEnv->NewGlobalRef(jobj);
    jump_in(tls, ret);
    return ret;
}

static void UT_DeleteGlobalRef(JNIEnv* env, jobject jglobalRef) {
    void** tls = jump_out();
    gEnv->DeleteGlobalRef(jglobalRef);
    jump_in2(tls);
}

static jobject UT_NewLocalRef(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jobject ret = gEnv->NewLocalRef(jobj);
    jump_in(tls, ret);
    return ret;
}

static void UT_DeleteLocalRef(JNIEnv* env, jobject jlocalRef) {
    void** tls = jump_out();
    gEnv->DeleteLocalRef(jlocalRef);
    jump_in2(tls);
}

static jint UT_EnsureLocalCapacity(JNIEnv* env, jint capacity) {
    void** tls = jump_out();
    jint ret = gEnv->EnsureLocalCapacity(capacity);
    jump_in(tls, ret);
    return ret;
}

static jboolean UT_IsSameObject(JNIEnv* env, jobject jref1, jobject jref2) {
    void** tls = jump_out();
    jboolean ret = gEnv->IsSameObject(jref1, jref2);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_AllocObject(JNIEnv* env, jclass jcls) {
    void** tls = jump_out();
    jobject ret = gEnv->AllocObject(jcls);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_NewObject(JNIEnv* env, jclass jclazz, jmethodID methodID, ...) {
    void** tls = jump_out();
    va_list args;
    va_start(args, methodID);
    jobject ret = gEnv->NewObjectV(jclazz, methodID, args);
    va_end(args);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_NewObjectV(JNIEnv* env, jclass jclazz, jmethodID methodID, va_list args) {
    void** tls = jump_out();
    jobject ret = gEnv->NewObjectV(jclazz, methodID, args);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_NewObjectA(JNIEnv* env, jclass jclazz, jmethodID methodID, jvalue* args) {
    void** tls = jump_out();
    jobject ret = gEnv->NewObjectA(jclazz, methodID, args);
    jump_in(tls, ret);
    return ret;
}

static jclass UT_GetObjectClass(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jclass ret = gEnv->GetObjectClass(jobj);
    jump_in(tls, ret);
    return ret;
}

static jboolean UT_IsInstanceOf(JNIEnv* env, jobject jobj, jclass jclazz) {
    void** tls = jump_out();
    jboolean ret = gEnv->IsInstanceOf(jobj, jclazz);
    jump_in(tls, ret);
    return ret;
}

static jmethodID UT_GetMethodID(JNIEnv* env, jclass jclazz, const char* name, const char* sig) {
    void** tls = jump_out();
    jmethodID ret = gEnv->GetMethodID(jclazz, name, sig);
    jump_in(tls, ret);
    return ret;
}

static jfieldID UT_GetFieldID(JNIEnv* env, jclass jclazz, const char* name, const char* sig) {
    void** tls = jump_out();
    jfieldID ret = gEnv->GetFieldID(jclazz, name, sig);
    jump_in(tls, ret);
    return ret;
}

static jmethodID UT_GetStaticMethodID(JNIEnv* env, jclass jclazz, const char* name, const char* sig) {
    void** tls = jump_out();
    jmethodID ret = gEnv->GetStaticMethodID(jclazz, name, sig);
    jump_in(tls, ret);
    return ret;
}

static jfieldID UT_GetStaticFieldID(JNIEnv* env, jclass jclazz, const char* name, const char* sig) {
    void** tls = jump_out();
    jfieldID ret = gEnv->GetStaticFieldID(jclazz, name, sig);
    jump_in(tls, ret);
    return ret;
}

#define GET_STATIC_TYPE_FIELD(_ctype, _jname, _isref)                       \
    static _ctype UT_GetStatic##_jname##Field(JNIEnv* env, jclass jclazz,   \
        jfieldID fieldID)                                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype value = gEnv->GetStatic##_jname##Field(jclazz, fieldID); \
        jump_in(tls, value);                                                          \
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
        jump_in2(tls);                                                          \
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
        jump_in(tls, value);                                                          \
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
        jump_in2(tls);                                                          \
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
        jump_in(tls, ret);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_Call##_jname##MethodV(JNIEnv* env, jobject jobj,          \
        jmethodID methodID, va_list args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->Call##_jname##MethodV(jobj, methodID, args); \
        jump_in(tls, ret);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_Call##_jname##MethodA(JNIEnv* env, jobject jobj,          \
        jmethodID methodID, jvalue* args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->Call##_jname##MethodA(jobj, methodID, args); \
        jump_in(tls, ret);                                                          \
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
        jump_in2(tls);                                                          \
    }                                                                       \
static     void UT_CallVoidMethodV(JNIEnv* env, jobject jobj,          \
        jmethodID methodID, va_list args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallVoidMethodV(jobj, methodID, args); \
        jump_in2(tls);                                                          \
    }                                                                       \
static     void UT_CallVoidMethodA(JNIEnv* env, jobject jobj,          \
        jmethodID methodID, jvalue* args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallVoidMethodA(jobj, methodID, args); \
        jump_in2(tls);                                                          \
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
        jump_in(tls, ret);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_CallNonvirtual##_jname##MethodV(JNIEnv* env, jobject jobj,\
        jclass jclazz, jmethodID methodID, va_list args)                    \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->CallNonvirtual##_jname##MethodV(jobj,      \
                jclazz, methodID, args);                                    \
        jump_in(tls, ret);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_CallNonvirtual##_jname##MethodA(JNIEnv* env, jobject jobj,\
        jclass jclazz, jmethodID methodID, jvalue* args)                    \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->CallNonvirtual##_jname##MethodA(jobj,      \
                jclazz, methodID, args);                                    \
        jump_in(tls, ret);                                                          \
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
        jump_in2(tls);                                                          \
    }                                                                       \
static     void UT_CallNonvirtualVoidMethodV(JNIEnv* env, jobject jobj,\
        jclass jclazz, jmethodID methodID, va_list args)                    \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallNonvirtualVoidMethodV(jobj,      \
                jclazz, methodID, args);                                    \
        jump_in2(tls);                                                          \
    }                                                                       \
static     void UT_CallNonvirtualVoidMethodA(JNIEnv* env, jobject jobj,\
        jclass jclazz, jmethodID methodID, jvalue* args)                    \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallNonvirtualVoidMethodA(jobj,      \
                jclazz, methodID, args);                                    \
        jump_in2(tls);                                                          \
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
        jump_in(tls, ret);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_CallStatic##_jname##MethodV(JNIEnv* env, jclass jclazz,   \
        jmethodID methodID, va_list args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->CallStatic##_jname##MethodV(jclazz, methodID, args); \
        jump_in(tls, ret);                                                          \
        return ret;                                                         \
    }                                                                       \
static     _ctype UT_CallStatic##_jname##MethodA(JNIEnv* env, jclass jclazz,   \
        jmethodID methodID, jvalue* args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        _ctype ret = gEnv->CallStatic##_jname##MethodA(jclazz, methodID, args); \
        jump_in(tls, ret);                                                          \
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
        jump_in2(tls);                                                          \
    }                                                                       \
static     void UT_CallStaticVoidMethodV(JNIEnv* env, jclass jclazz,   \
        jmethodID methodID, va_list args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallStaticVoidMethodV(jclazz, methodID, args); \
        jump_in2(tls);                                                          \
    }                                                                       \
static     void UT_CallStaticVoidMethodA(JNIEnv* env, jclass jclazz,   \
        jmethodID methodID, jvalue* args)                                   \
    {                                                                       \
        void** tls = jump_out();                                                         \
        gEnv->CallStaticVoidMethodA(jclazz, methodID, args); \
        jump_in2(tls);                                                          \
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
    jump_in(tls, ret);
    return ret;
}

static jsize UT_GetStringLength(JNIEnv* env, jstring jstr) {
    void** tls = jump_out();
    jsize ret = gEnv->GetStringLength(jstr);
    jump_in(tls, ret);
    return ret;
}

static const jchar* UT_GetStringChars(JNIEnv* env, jstring jstr, jboolean* isCopy) {
    void** tls = jump_out();
    JNIEnv* e = gEnv;
    jsize len = e->GetStringLength(jstr);
    if (!len) {
        jump_in(tls, NULL);
        return NULL;
    }
    jchar* buf = NULL;
    if (len) {
        buf = (jchar*)ut_malloc(sizeof(jchar) * (len+1));
        const jchar* str = e->GetStringChars(jstr, isCopy);
        set_fake_ptr(buf, str, sizeof(jchar) * len);
        buf[len] = '\0';
    }
    jump_in(tls, buf);
    return (const jchar*)buf;
}

static void UT_ReleaseStringChars(JNIEnv* env, jstring jstr, const jchar* chars) {
    if (!chars) return;
    void** tls = jump_out();
    release_fake_ptr(jchar*, str, chars);
    gEnv->ReleaseStringChars(jstr, str);
    jump_in2(tls);
}

static jstring UT_NewStringUTF(JNIEnv* env, const char* bytes) {
    void** tls = jump_out();
    jstring ret = gEnv->NewStringUTF(bytes);
    jump_in(tls, ret);
    return ret;
}

static jsize UT_GetStringUTFLength(JNIEnv* env, jstring jstr) {
    void** tls = jump_out();
    jsize ret = gEnv->GetStringUTFLength(jstr);
    jump_in(tls, ret);
    return ret;
}

static const char* UT_GetStringUTFChars(JNIEnv* env, jstring jstr, jboolean* isCopy) {
    void** tls = jump_out();
    JNIEnv* e = gEnv;
    jsize len = e->GetStringUTFLength(jstr);
    if (!len) {
        jump_in(tls, NULL);
        return NULL;
    }
    char* buf = NULL;
    if (len) {
        buf = (char*)ut_malloc(len+1);
        const char* str = e->GetStringUTFChars(jstr, isCopy);
        set_fake_ptr(buf, str, len);
        buf[len] = '\0';
    }
    jump_in(tls, buf);
    return (const char*)buf;
}

static void UT_ReleaseStringUTFChars(JNIEnv* env, jstring jstr, const char* utf) {
    if (!utf) return;
    void** tls = jump_out();
    release_fake_ptr(const char*, str, utf);
    gEnv->ReleaseStringUTFChars(jstr, str);
    jump_in2(tls);
}

static jsize UT_GetArrayLength(JNIEnv* env, jarray jarr) {
    void** tls = jump_out();
    jsize ret = gEnv->GetArrayLength(jarr);
    jump_in(tls, ret);
    return ret;
}

static jobjectArray UT_NewObjectArray(JNIEnv* env, jsize length,
    jclass jelementClass, jobject jinitialElement)
{
    void** tls = jump_out();
    jobjectArray ret = gEnv->NewObjectArray(length, jelementClass, jinitialElement);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_GetObjectArrayElement(JNIEnv* env, jobjectArray jarr, jsize index) {
    void** tls = jump_out();
    jobject ret = gEnv->GetObjectArrayElement(jarr, index);
    jump_in(tls, ret);
    return ret;
}

static void UT_SetObjectArrayElement(JNIEnv* env, jobjectArray jarr, jsize index, jobject jobj) {
    void** tls = jump_out();
    gEnv->SetObjectArrayElement(jarr, index, jobj);
    jump_in2(tls);
}

#define NEW_PRIMITIVE_ARRAY(_artype, _jname) \
static     _artype UT_New##_jname##Array(JNIEnv* env, jsize length) { \
        void** tls = jump_out(); \
        _artype result = gEnv->New##_jname##Array(length); \
        jump_in(tls, result); \
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
            jump_in(tls, NULL); \
            return NULL; \
        } \
        _ctype* data = NULL; \
        if (len) { \
            data = (_ctype*)ut_malloc(sizeof(_ctype)*(len+1)); \
            _ctype* val = gEnv->Get##_jname##ArrayElements(jarr, isCopy); \
            set_fake_ptr(data, val, sizeof(_ctype)*len); \
            data[len] = (_ctype)0; \
        } \
        jump_in(tls, data); \
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
        jump_in2(tls); \
    }

#define GET_PRIMITIVE_ARRAY_REGION(_ctype, _jname) \
static     void UT_Get##_jname##ArrayRegion(JNIEnv* env, \
        _ctype##Array jarr, jsize start, jsize len, _ctype* buf) \
    { \
        void** tls = jump_out(); \
        gEnv->Get##_jname##ArrayRegion(jarr, start, len, buf); \
        jump_in2(tls); \
    }

#define SET_PRIMITIVE_ARRAY_REGION(_ctype, _jname) \
static     void UT_Set##_jname##ArrayRegion(JNIEnv* env, \
        _ctype##Array jarr, jsize start, jsize len, const _ctype* buf) \
    { \
        void** tls = jump_out(); \
        gEnv->Set##_jname##ArrayRegion(jarr, start, len, buf); \
        jump_in2(tls); \
    }

static void UT_SetDoubleArrayRegion2(JNIEnv* env,
        jdoubleArray jarr, jsize start, jsize len,
        unsigned long dummy0, unsigned long dummy1, const jdouble* buf)
    {
        void** tls = jump_out();
        gEnv->SetDoubleArrayRegion(jarr, start, len, buf);
        jump_in2(tls);
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
    jump_in(tls, ret);
    return ret;
}

static jint UT_UnregisterNatives(JNIEnv* env, jclass jclazz) {
    void** tls = jump_out();
    jint ret = gEnv->UnregisterNatives(jclazz);
    jump_in(tls, ret);
    return ret;
}

static jint UT_MonitorEnter(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jint ret = gEnv->MonitorEnter(jobj);
    jump_in(tls, ret);
    return ret;
}

static jint UT_MonitorExit(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jint ret = gEnv->MonitorExit(jobj);
    jump_in(tls, ret);
    return ret;
}

static jint UT_GetJavaVM(JNIEnv* env, JavaVM** vm) {
    void** tls = jump_out();
    jint ret = gEnv->GetJavaVM(vm);
    jump_in(tls, ret);
    return ret;
}

static void UT_GetStringRegion(JNIEnv* env, jstring jstr, jsize start, jsize len, jchar* buf) {
    void** tls = jump_out();
    gEnv->GetStringRegion(jstr, start, len, buf);
    jump_in2(tls);
}

static void UT_GetStringUTFRegion(JNIEnv* env, jstring jstr, jsize start, jsize len, char* buf) {
    void** tls = jump_out();
    gEnv->GetStringUTFRegion(jstr, start, len, buf);
    jump_in2(tls);
}

static void* UT_GetPrimitiveArrayCritical(JNIEnv* env, jarray jarr, jboolean* isCopy) {
    void** tls = jump_out();
    JNIEnv* e = gEnv;
    jsize len = e->GetArrayLength(jarr);
    if (!len) {
        jump_in(tls, NULL);
        return NULL;
    }
    size_t width = 0;
    char type = (dvmGetObjectName(e, jarr))[1];
    switch (type) {
        case 'I': width = 4; break;
        case 'C': width = 2; break;
        case 'B': width = 1; break;
        case 'Z': width = 1; /* special-case this? */ break;
        case 'F': width = 4; break;
        case 'D': width = 8; break;
        case 'S': width = 2; break;
        case 'J': width = 8; break;
        default: width = 8; break;
    }
    void* buf = NULL;
    if (len) {
        buf = ut_malloc(width*(len+1));
        if (buf == NULL) {
            ALOGE("buf is null");
            jump_in(tls, NULL);
            return NULL;
        }
        void* ret = e->GetPrimitiveArrayCritical(jarr, isCopy);
        if (ret == NULL) {
            ALOGE("ret is null");
            ut_free(buf);
            jump_in(tls, NULL);
            return NULL;
        }
        set_fake_ptr(buf, ret, width*len);
        memset((void*)((unsigned long)buf+(unsigned long)(width*len)), 0, width);
    }
    jump_in(tls, buf);
    return buf;
}

static void UT_ReleasePrimitiveArrayCritical(JNIEnv* env, jarray jarr, void* carray, jint mode) {
    if (!carray) return;
    void** tls = jump_out();
    release_fake_ptr(void*, __arr, carray);
    gEnv->ReleasePrimitiveArrayCritical(jarr, __arr, mode);
    jump_in2(tls);
}

static const jchar* UT_GetStringCritical(JNIEnv* env, jstring jstr, jboolean* isCopy) {
    void** tls = jump_out();
    JNIEnv* e = gEnv;
    jsize len = e->GetStringLength(jstr);
    if (!len) {
        jump_in(tls, NULL);
        return NULL;
    }
    jchar* buf = NULL;
    if (len) {
        buf = (jchar*)ut_malloc(sizeof(jchar) * (len+1));
        const jchar* ret = e->GetStringCritical(jstr, isCopy);
        set_fake_ptr(buf, ret, sizeof(jchar) * len);
        buf[len] = '\0';
    }
    jump_in(tls, buf);
    return (const jchar*)buf;
}

static void UT_ReleaseStringCritical(JNIEnv* env, jstring jstr, const jchar* carray) {
    if (!carray) return;
    void** tls = jump_out();
    release_fake_ptr(const jchar*, str, carray);
    gEnv->ReleaseStringCritical(jstr, str);
    jump_in2(tls);
}

static jweak UT_NewWeakGlobalRef(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jweak ret = gEnv->NewWeakGlobalRef(jobj);
    jump_in(tls, ret);
    return ret;
}

static void UT_DeleteWeakGlobalRef(JNIEnv* env, jweak wref) {
    void** tls = jump_out();
    gEnv->DeleteWeakGlobalRef(wref);
    jump_in2(tls);
}

static jboolean UT_ExceptionCheck(JNIEnv* env) {
    void** tls = jump_out();
    jboolean ret = gEnv->ExceptionCheck();
    jump_in(tls, ret);
    return ret;
}

static jobjectRefType UT_GetObjectRefType(JNIEnv* env, jobject jobj) {
    void** tls = jump_out();
    jobjectRefType ret = gEnv->GetObjectRefType(jobj);
    jump_in(tls, ret);
    return ret;
}

static jobject UT_NewDirectByteBuffer(JNIEnv* env, void* address, jlong capacity) {
    void** tls = jump_out();
    jobject ret = gEnv->NewDirectByteBuffer(address, capacity);
    jump_in(tls, ret);
    return ret;
}

//TODO
static void* UT_GetDirectBufferAddress(JNIEnv* env, jobject jbuf) {
    void** tls = jump_out();
    void* ret = gEnv->GetDirectBufferAddress(jbuf);
    jump_in(tls, ret);
    return ret;
}

static jlong UT_GetDirectBufferCapacity(JNIEnv* env, jobject jbuf) {
    void** tls = jump_out();
    jlong ret = gEnv->GetDirectBufferCapacity(jbuf);
    jump_in(tls, ret);
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
    (void (*)(JNIEnv*,
        jdoubleArray, jsize, jsize,
        const jdouble*))UT_SetDoubleArrayRegion2,

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

static const structhelpfunc_t helper = {
    dvmGetObjectName,
};
static void* jnienv_handle = NULL;
static JNIEnv* (*init_libjnienv)(structhelpfunc_t*) = NULL;
static void (*set_jnienv)(JNIEnv*) = NULL;
static JNIEnv* __jnienv_init(void) {
    void* addr = NULL;
    jnienv_handle = dlopen_in_sandbox("libjnienv.so\0", RTLD_LAZY, &addr);
    if (!jnienv_handle)
        ALOGE("[sandbox] jnienv_handle is null at %d", __LINE__);
    init_libjnienv = (JNIEnv* (*)(structhelpfunc_t*))
        dlsym_in_sandbox(jnienv_handle, "init_libjnienv\0");
    if (!init_libjnienv)
        ALOGE("[sandbox] init_libjnienv is null at %d", __LINE__);
    set_jnienv = (void (*)(JNIEnv*)) dlsym_in_sandbox(jnienv_handle, "set_jnienv\0");
    if (!set_jnienv)
        ALOGE("[sandbox] set_jnienv is null at %d", __LINE__);

    asm volatile( "push {r0, r7}\n"
            "mov r0, %[funcv]\n"
            "ldr r7, =385\n"
            "svc #0\n"
            "pop {r0, r7}\n"
            : : [funcv] "r" (&gBridgeInterface));

    return init_libjnienv((structhelpfunc_t*)&helper);
}

static JNIEnv* gUntrustedEnv = NULL;
JNIEnv* dvmGetUntrustedEnv(JNIEnv* env) {
    if (!jnienv_handle) {
        gUntrustedEnv = __jnienv_init();
    }
    if (env) {
        set_jnienv(env);
        gEnv = env;
    }
    if (0) {
        UT_SetDoubleArrayRegion(env, 0, 0, 0, 0);
    }
    return gUntrustedEnv;
}
#endif
