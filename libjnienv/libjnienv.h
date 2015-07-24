#ifndef _LIBJNIENV_H_
#define _LIBJNIENV_H_

#include <jni.h>
#include <string.h>

JNIEnv* init_libjnienv(JNIEnv* env, void* (*f) ( size_t ));

#endif
