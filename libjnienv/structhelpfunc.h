#ifndef __STRUCTHELPFUNC_H
#define __STRUCTHELPFUNC_H

#include <jni.h>

struct structhelpfunc_t {
    const char* (*GetObjectName)(JNIEnv *, jobject);
};

#endif
