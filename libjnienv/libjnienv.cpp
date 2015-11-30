#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <map>

#include "structhelpfunc.h"
#include "cutils/log.h"

static jobject
UT_NewObjectV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jobject
UT_CallObjectMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jboolean
UT_CallBooleanMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jbyte
UT_CallByteMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jchar
UT_CallCharMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jshort
UT_CallShortMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jint
UT_CallIntMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jlong
UT_CallLongMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jfloat
UT_CallFloatMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jdouble
UT_CallDoubleMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static void
UT_CallVoidMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3);
static jobject
UT_CallNonvirtualObjectMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jboolean
UT_CallNonvirtualBooleanMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jbyte
UT_CallNonvirtualByteMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jchar
UT_CallNonvirtualCharMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jshort
UT_CallNonvirtualShortMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jint
UT_CallNonvirtualIntMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jlong
UT_CallNonvirtualLongMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jfloat
UT_CallNonvirtualFloatMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jdouble
UT_CallNonvirtualDoubleMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static void
UT_CallNonvirtualVoidMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4);
static jobject
UT_CallStaticObjectMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jboolean
UT_CallStaticBooleanMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jbyte
UT_CallStaticByteMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jchar
UT_CallStaticCharMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jshort
UT_CallStaticShortMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jint
UT_CallStaticIntMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jlong
UT_CallStaticLongMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jfloat
UT_CallStaticFloatMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static jdouble
UT_CallStaticDoubleMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);
static void
UT_CallStaticVoidMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3);

static jint
UT_GetVersion
(JNIEnv * a0)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =0\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jclass
UT_DefineClass
(JNIEnv* a0, const char* a1, jobject a2, const jbyte* a3, jsize a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =1\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jclass
UT_FindClass
(JNIEnv* a0, const char* a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =2\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jmethodID
UT_FromReflectedMethod
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =3\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfieldID
UT_FromReflectedField
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =4\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_ToReflectedMethod
(JNIEnv* a0, jclass a1, jmethodID a2, jboolean a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =5\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jclass
UT_GetSuperclass
(JNIEnv* a0, jclass a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =6\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_IsAssignableFrom
(JNIEnv* a0, jclass a1, jclass a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =7\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_ToReflectedField
(JNIEnv* a0, jclass a1, jfieldID a2, jboolean a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =8\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_Throw
(JNIEnv* a0, jthrowable a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =9\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_ThrowNew
(JNIEnv * a0, jclass a1, const char * a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =10\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jthrowable
UT_ExceptionOccurred
(JNIEnv* a0)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =11\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_ExceptionDescribe
(JNIEnv* a0)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =12\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_ExceptionClear
(JNIEnv* a0)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =13\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_FatalError
(JNIEnv* a0, const char* a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =14\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jint
UT_PushLocalFrame
(JNIEnv* a0, jint a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =15\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_PopLocalFrame
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =16\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_NewGlobalRef
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =17\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_DeleteGlobalRef
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =18\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_DeleteLocalRef
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =19\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jboolean
UT_IsSameObject
(JNIEnv* a0, jobject a1, jobject a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =20\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_NewLocalRef
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =21\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_EnsureLocalCapacity
(JNIEnv* a0, jint a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =22\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_AllocObject
(JNIEnv* a0, jclass a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =23\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_NewObject
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jobject jobj = UT_NewObjectV(a0, a1, a2, args);
    va_end(args);
    return jobj;
}

static jobject
UT_NewObjectV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =25\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_NewObjectA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =26\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jclass
UT_GetObjectClass
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =27\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_IsInstanceOf
(JNIEnv* a0, jobject a1, jclass a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =28\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jmethodID
UT_GetMethodID
(JNIEnv* a0, jclass a1, const char* a2, const char* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =29\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_CallObjectMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jobject jobj = UT_CallObjectMethodV(a0, a1, a2, args);
    va_end(args);
    return jobj;
}

static jobject
UT_CallObjectMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =31\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_CallObjectMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =32\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_CallBooleanMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jboolean ret = UT_CallBooleanMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jboolean
UT_CallBooleanMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =34\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_CallBooleanMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =35\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte
UT_CallByteMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jbyte ret = UT_CallByteMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jbyte
UT_CallByteMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =37\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte
UT_CallByteMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =38\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar
UT_CallCharMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jchar ret = UT_CallCharMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jchar
UT_CallCharMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =40\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar
UT_CallCharMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =41\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort
UT_CallShortMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jshort ret = UT_CallShortMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jshort
UT_CallShortMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =43\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort
UT_CallShortMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =44\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_CallIntMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jint ret = UT_CallIntMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jint
UT_CallIntMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =46\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_CallIntMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =47\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_CallLongMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jlong ret = UT_CallLongMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jlong
UT_CallLongMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =49\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_CallLongMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =50\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat
UT_CallFloatMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jfloat ret = UT_CallFloatMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jfloat
UT_CallFloatMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =52\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat
UT_CallFloatMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =53\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble
UT_CallDoubleMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jdouble ret = UT_CallDoubleMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jdouble
UT_CallDoubleMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =55\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble
UT_CallDoubleMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =56\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_CallVoidMethod
(JNIEnv* a0, jobject a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    UT_CallVoidMethodV(a0, a1, a2, args);
    va_end(args);
}

static void
UT_CallVoidMethodV
(JNIEnv* a0, jobject a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =58\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_CallVoidMethodA
(JNIEnv* a0, jobject a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =59\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jobject
UT_CallNonvirtualObjectMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jobject ret = UT_CallNonvirtualObjectMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jobject
UT_CallNonvirtualObjectMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =61\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_CallNonvirtualObjectMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =62\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_CallNonvirtualBooleanMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jboolean ret = UT_CallNonvirtualBooleanMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jboolean
UT_CallNonvirtualBooleanMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =64\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_CallNonvirtualBooleanMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =65\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte
UT_CallNonvirtualByteMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jbyte ret = UT_CallNonvirtualByteMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jbyte
UT_CallNonvirtualByteMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =67\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte
UT_CallNonvirtualByteMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =68\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar
UT_CallNonvirtualCharMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jchar ret = UT_CallNonvirtualCharMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jchar
UT_CallNonvirtualCharMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =70\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar
UT_CallNonvirtualCharMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =71\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort
UT_CallNonvirtualShortMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jshort ret = UT_CallNonvirtualShortMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jshort
UT_CallNonvirtualShortMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =73\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort
UT_CallNonvirtualShortMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =74\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_CallNonvirtualIntMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jint ret = UT_CallNonvirtualIntMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jint
UT_CallNonvirtualIntMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =76\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_CallNonvirtualIntMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =77\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_CallNonvirtualLongMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jlong ret = UT_CallNonvirtualLongMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jlong
UT_CallNonvirtualLongMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =79\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_CallNonvirtualLongMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =80\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat
UT_CallNonvirtualFloatMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jfloat ret = UT_CallNonvirtualFloatMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jfloat
UT_CallNonvirtualFloatMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =82\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat
UT_CallNonvirtualFloatMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =83\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble
UT_CallNonvirtualDoubleMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    jdouble ret = UT_CallNonvirtualDoubleMethodV(a0, a1, a2, a3, args);
    va_end(args);
    return ret;
}

static jdouble
UT_CallNonvirtualDoubleMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =85\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble
UT_CallNonvirtualDoubleMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =86\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_CallNonvirtualVoidMethod
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, ...)
{
    va_list args;
    va_start(args, a3);
    UT_CallNonvirtualVoidMethodV(a0, a1, a2, a3, args);
    va_end(args);
}

static void
UT_CallNonvirtualVoidMethodV
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, va_list a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =88\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_CallNonvirtualVoidMethodA
(JNIEnv* a0, jobject a1, jclass a2, jmethodID a3, jvalue* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =89\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jfieldID
UT_GetFieldID
(JNIEnv* a0, jclass a1, const char* a2, const char* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =90\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_GetObjectField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =91\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_GetBooleanField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =92\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte
UT_GetByteField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =93\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar
UT_GetCharField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =94\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort
UT_GetShortField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =95\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_GetIntField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =96\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_GetLongField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =97\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat
UT_GetFloatField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =98\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble
UT_GetDoubleField
(JNIEnv* a0, jobject a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =99\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_SetObjectField
(JNIEnv* a0, jobject a1, jfieldID a2, jobject a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =100\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetBooleanField
(JNIEnv* a0, jobject a1, jfieldID a2, jboolean a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =101\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetByteField
(JNIEnv* a0, jobject a1, jfieldID a2, jbyte a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =102\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetCharField
(JNIEnv* a0, jobject a1, jfieldID a2, jchar a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =103\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetShortField
(JNIEnv* a0, jobject a1, jfieldID a2, jshort a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =104\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetIntField
(JNIEnv* a0, jobject a1, jfieldID a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =105\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetLongField
(JNIEnv* a0, jobject a1, jfieldID a2, jlong a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =106\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetFloatField
(JNIEnv* a0, jobject a1, jfieldID a2, jfloat a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =107\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetDoubleField
(JNIEnv* a0, jobject a1, jfieldID a2, jdouble a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =108\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jmethodID
UT_GetStaticMethodID
(JNIEnv* a0, jclass a1, const char* a2, const char* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =109\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_CallStaticObjectMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jobject ret = UT_CallStaticObjectMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jobject
UT_CallStaticObjectMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =111\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_CallStaticObjectMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =112\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_CallStaticBooleanMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jboolean ret = UT_CallStaticBooleanMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jboolean
UT_CallStaticBooleanMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =114\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_CallStaticBooleanMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =115\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte
UT_CallStaticByteMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jbyte ret = UT_CallStaticByteMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jbyte
UT_CallStaticByteMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =117\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte
UT_CallStaticByteMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =118\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar
UT_CallStaticCharMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jchar ret = UT_CallStaticCharMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jchar
UT_CallStaticCharMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =120\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar
UT_CallStaticCharMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =121\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort
UT_CallStaticShortMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jshort ret = UT_CallStaticShortMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jshort
UT_CallStaticShortMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =123\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort
UT_CallStaticShortMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =124\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_CallStaticIntMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jint ret = UT_CallStaticIntMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jint
UT_CallStaticIntMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =126\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_CallStaticIntMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =127\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_CallStaticLongMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jlong ret = UT_CallStaticLongMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jlong
UT_CallStaticLongMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =129\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_CallStaticLongMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =130\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat
UT_CallStaticFloatMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jfloat ret = UT_CallStaticFloatMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jfloat
UT_CallStaticFloatMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =132\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat
UT_CallStaticFloatMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =133\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble
UT_CallStaticDoubleMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    jdouble ret = UT_CallStaticDoubleMethodV(a0, a1, a2, args);
    va_end(args);
    return ret;
}

static jdouble
UT_CallStaticDoubleMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =135\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble
UT_CallStaticDoubleMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =136\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_CallStaticVoidMethod
(JNIEnv* a0, jclass a1, jmethodID a2, ...)
{
    va_list args;
    va_start(args, a2);
    UT_CallStaticVoidMethodV(a0, a1, a2, args);
    va_end(args);
}

static void
UT_CallStaticVoidMethodV
(JNIEnv* a0, jclass a1, jmethodID a2, va_list a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =138\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_CallStaticVoidMethodA
(JNIEnv* a0, jclass a1, jmethodID a2, jvalue* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =139\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jfieldID
UT_GetStaticFieldID
(JNIEnv* a0, jclass a1, const char* a2, const char* a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =140\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_GetStaticObjectField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =141\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean
UT_GetStaticBooleanField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =142\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte
UT_GetStaticByteField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =143\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar
UT_GetStaticCharField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =144\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort
UT_GetStaticShortField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =145\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_GetStaticIntField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =146\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_GetStaticLongField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =147\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat
UT_GetStaticFloatField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =148\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble
UT_GetStaticDoubleField
(JNIEnv* a0, jclass a1, jfieldID a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =149\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_SetStaticObjectField
(JNIEnv* a0, jclass a1, jfieldID a2, jobject a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =150\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetStaticBooleanField
(JNIEnv* a0, jclass a1, jfieldID a2, jboolean a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =151\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetStaticByteField
(JNIEnv* a0, jclass a1, jfieldID a2, jbyte a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =152\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetStaticCharField
(JNIEnv* a0, jclass a1, jfieldID a2, jchar a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =153\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetStaticShortField
(JNIEnv* a0, jclass a1, jfieldID a2, jshort a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =154\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetStaticIntField
(JNIEnv* a0, jclass a1, jfieldID a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =155\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetStaticLongField
(JNIEnv* a0, jclass a1, jfieldID a2, jlong a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =156\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetStaticFloatField
(JNIEnv* a0, jclass a1, jfieldID a2, jfloat a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =157\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetStaticDoubleField
(JNIEnv* a0, jclass a1, jfieldID a2, jdouble a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =158\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jstring
UT_NewString
(JNIEnv* a0, const jchar* a1, jsize a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =159\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jsize
UT_GetStringLength
(JNIEnv* a0, jstring a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =160\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static const jchar*
UT_GetStringChars
(JNIEnv* a0, jstring a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =161\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_ReleaseStringChars
(JNIEnv* a0, jstring a1, const jchar* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =162\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jstring
UT_NewStringUTF
(JNIEnv* a0, const char* a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =163\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jsize
UT_GetStringUTFLength
(JNIEnv* a0, jstring a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =164\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static const char*
UT_GetStringUTFChars
(JNIEnv* a0, jstring a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =165\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_ReleaseStringUTFChars
(JNIEnv* a0, jstring a1, const char* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =166\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jsize
UT_GetArrayLength
(JNIEnv* a0, jarray a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =167\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobjectArray
UT_NewObjectArray
(JNIEnv* a0, jsize a1, jclass a2, jobject a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =168\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_GetObjectArrayElement
(JNIEnv* a0, jobjectArray a1, jsize a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =169\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_SetObjectArrayElement
(JNIEnv* a0, jobjectArray a1, jsize a2, jobject a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =170\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jbooleanArray
UT_NewBooleanArray
(JNIEnv* a0, jsize a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =171\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyteArray
UT_NewByteArray
(JNIEnv* a0, jsize a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =172\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jcharArray
UT_NewCharArray
(JNIEnv* a0, jsize a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =173\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshortArray
UT_NewShortArray
(JNIEnv* a0, jsize a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =174\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jintArray
UT_NewIntArray
(JNIEnv* a0, jsize a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =175\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlongArray
UT_NewLongArray
(JNIEnv* a0, jsize a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =176\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloatArray
UT_NewFloatArray
(JNIEnv* a0, jsize a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =177\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdoubleArray
UT_NewDoubleArray
(JNIEnv* a0, jsize a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =178\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jboolean*
UT_GetBooleanArrayElements
(JNIEnv* a0, jbooleanArray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =179\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jbyte*
UT_GetByteArrayElements
(JNIEnv* a0, jbyteArray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =180\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jchar*
UT_GetCharArrayElements
(JNIEnv* a0, jcharArray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =181\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jshort*
UT_GetShortArrayElements
(JNIEnv* a0, jshortArray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =182\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint*
UT_GetIntArrayElements
(JNIEnv* a0, jintArray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =183\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong*
UT_GetLongArrayElements
(JNIEnv* a0, jlongArray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =184\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jfloat*
UT_GetFloatArrayElements
(JNIEnv* a0, jfloatArray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =185\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jdouble*
UT_GetDoubleArrayElements
(JNIEnv* a0, jdoubleArray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =186\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_ReleaseBooleanArrayElements
(JNIEnv* a0, jbooleanArray a1, jboolean* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =187\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_ReleaseByteArrayElements
(JNIEnv* a0, jbyteArray a1, jbyte* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =188\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_ReleaseCharArrayElements
(JNIEnv* a0, jcharArray a1, jchar* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =189\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_ReleaseShortArrayElements
(JNIEnv* a0, jshortArray a1, jshort* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =190\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_ReleaseIntArrayElements
(JNIEnv* a0, jintArray a1, jint* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =191\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_ReleaseLongArrayElements
(JNIEnv* a0, jlongArray a1, jlong* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =192\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_ReleaseFloatArrayElements
(JNIEnv* a0, jfloatArray a1, jfloat* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =193\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_ReleaseDoubleArrayElements
(JNIEnv* a0, jdoubleArray a1, jdouble* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =194\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetBooleanArrayRegion
(JNIEnv* a0, jbooleanArray a1, jsize a2, jsize a3, jboolean* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =195\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetByteArrayRegion
(JNIEnv* a0, jbyteArray a1, jsize a2, jsize a3, jbyte* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =196\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetCharArrayRegion
(JNIEnv* a0, jcharArray a1, jsize a2, jsize a3, jchar* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =197\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetShortArrayRegion
(JNIEnv* a0, jshortArray a1, jsize a2, jsize a3, jshort* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =198\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetIntArrayRegion
(JNIEnv* a0, jintArray a1, jsize a2, jsize a3, jint* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =199\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetLongArrayRegion
(JNIEnv* a0, jlongArray a1, jsize a2, jsize a3, jlong* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =200\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetFloatArrayRegion
(JNIEnv* a0, jfloatArray a1, jsize a2, jsize a3, jfloat* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =201\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetDoubleArrayRegion
(JNIEnv* a0, jdoubleArray a1, jsize a2, jsize a3, jdouble* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =202\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetBooleanArrayRegion
(JNIEnv* a0, jbooleanArray a1, jsize a2, jsize a3, const jboolean* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =203\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetByteArrayRegion
(JNIEnv* a0, jbyteArray a1, jsize a2, jsize a3, const jbyte* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =204\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetCharArrayRegion
(JNIEnv* a0, jcharArray a1, jsize a2, jsize a3, const jchar* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =205\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetShortArrayRegion
(JNIEnv* a0, jshortArray a1, jsize a2, jsize a3, const jshort* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =206\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetIntArrayRegion
(JNIEnv* a0, jintArray a1, jsize a2, jsize a3, const jint* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =207\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetLongArrayRegion
(JNIEnv* a0, jlongArray a1, jsize a2, jsize a3, const jlong* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =208\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetFloatArrayRegion
(JNIEnv* a0, jfloatArray a1, jsize a2, jsize a3, const jfloat* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =209\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_SetDoubleArrayRegion
(JNIEnv* a0, jdoubleArray a1, jsize a2, jsize a3, const jdouble* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =210\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jint
UT_RegisterNatives
(JNIEnv* a0, jclass a1, const JNINativeMethod* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =211\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_UnregisterNatives
(JNIEnv* a0, jclass a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =212\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_MonitorEnter
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =213\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_MonitorExit
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =214\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jint
UT_GetJavaVM
(JNIEnv* a0, JavaVM** a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =215\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_GetStringRegion
(JNIEnv* a0, jstring a1, jsize a2, jsize a3, jchar* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =216\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void
UT_GetStringUTFRegion
(JNIEnv* a0, jstring a1, jsize a2, jsize a3, char* a4)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =217\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static void*
UT_GetPrimitiveArrayCritical
(JNIEnv* a0, jarray a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =218\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_ReleasePrimitiveArrayCritical
(JNIEnv* a0, jarray a1, void* a2, jint a3)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =219\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static const jchar*
UT_GetStringCritical
(JNIEnv* a0, jstring a1, jboolean* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =220\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_ReleaseStringCritical
(JNIEnv* a0, jstring a1, const jchar* a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =221\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jweak
UT_NewWeakGlobalRef
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =222\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void
UT_DeleteWeakGlobalRef
(JNIEnv* a0, jweak a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =223\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return;
}

static jboolean
UT_ExceptionCheck
(JNIEnv* a0)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =224\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobject
UT_NewDirectByteBuffer
(JNIEnv* a0, void* a1, jlong a2)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =225\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static void*
UT_GetDirectBufferAddress
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =226\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jlong
UT_GetDirectBufferCapacity
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =227\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return 0;
}

static jobjectRefType
UT_GetObjectRefType
(JNIEnv* a0, jobject a1)
{
    asm volatile( "push {r7, r8}\n"
            "ldr r8, =228\n"
            "ldr r7, =383\n"
            "svc #0\n"
            "pop {r7, r8}\n"
            "bx lr\n"
            : : );
    /* not reached */
    return JNIInvalidRefType;
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
extern "C" JNIEnv* init_libjnienv(structhelpfunc_t *h) {
    gBridgeEnv.functions = &gBridgeInterface;
    return &gBridgeEnv;
}

extern "C" void set_jnienv(JNIEnv* env) {
}
