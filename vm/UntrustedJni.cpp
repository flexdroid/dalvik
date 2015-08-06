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

    void* base = mmap(NULL, HEAP_SIZE,
            PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    heap = (void*)ROUND_UP(base, SECTION_SIZE);
    if (base < heap)
        munmap(base, (unsigned long)heap - (unsigned long)base);
    for (unsigned long i = 0; i < READ_HEAP_SECTIONS; ++i) {
        *(int *)((unsigned long)heap + i*SECTION_SIZE) = 3;
    }

    // ALOGE("[sandbox] ut_malloc=%p at %d", ut_malloc, __LINE__);

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

static void* jnienv_handle = NULL;
static JNIEnv* (*init_libjnienv)(void) = NULL;
static void (*set_jnienv)(JNIEnv*) = NULL;
static JNIEnv* __jnienv_init(void) {
    void* addr = NULL;
    jnienv_handle = dlopen_in_sandbox("libjnienv.so\0", RTLD_LAZY, &addr);
    if (!jnienv_handle)
        ALOGE("[sandbox] jnienv_handle is null at %d", __LINE__);
    init_libjnienv = (JNIEnv* (*)(void)) dlsym_in_sandbox(jnienv_handle, "init_libjnienv\0");
    if (!init_libjnienv)
        ALOGE("[sandbox] init_libjnienv is null at %d", __LINE__);
    set_jnienv = (void (*)(JNIEnv*)) dlsym_in_sandbox(jnienv_handle, "set_jnienv\0");
    if (!set_jnienv)
        ALOGE("[sandbox] set_jnienv is null at %d", __LINE__);
    return init_libjnienv();
}

static JNIEnv* gUntrustedEnv = NULL;
JNIEnv* dvmGetUntrustedEnv(JNIEnv* env) {
    if (!jnienv_handle) {
        gUntrustedEnv = __jnienv_init();
    }
    set_jnienv(env);
    return gUntrustedEnv;
}
#endif
