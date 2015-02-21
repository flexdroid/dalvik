#include "main.h"
#include "Ddm.h"

#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>

#define  BUFF_SIZE      1024

enum {
    CHANNEL_REGISTER_PM = 0,
    CHANNEL_REGISTER_INSPECTOR,
    CHANNEL_UNREGISTER,
    CHANNEL_PM_WAIT,
    CHANNEL_REQUEST_PM,
    CHANNEL_PM_RESPONSE,
};

void set_prio_max()
{
    pthread_attr_t thAttr;
    int policy = 0;
    int max_prio_for_policy = 0;

    pthread_attr_init(&thAttr);
    pthread_attr_getschedpolicy(&thAttr, &policy);
    max_prio_for_policy = sched_get_priority_max(policy);

    if (setpriority(PRIO_PROCESS, dvmGetSysThreadId(), max_prio_for_policy) != 0) {
        ALOGI("jaebaek priority %d set failed!", max_prio_for_policy);
    }
    pthread_attr_destroy(&thAttr);
}

struct read_data {
    pid_t target_tid;
    int is_target_thd_suspended;
};

sem_t stack_tracer_init_sema;
int is_stack_tracer_created = 0;

// class loader privilege sets
static std::map<Method*, std::vector<int> > clsLdPriv;
static std::map<Method*, int> sandboxCache;

void *do_stack_inspection(void *arg)
{
    // for IPC
    int fd = open("/dev/stack_inspection_channel", O_RDWR);
    read_data __read_data;
    char buf[4*BUFF_SIZE];
    int keys[BUFF_SIZE];
    int size;

    // for stack trace
    int* traceBuf;
    int* traceBufMock;
    size_t stackDepth = 0, i, numSB;

    // for cache
    std::map<Method*, int>::iterator it;
    //std::set<std::string> cacheChecker;
    int cacheHit = 0, cacheMiss = 0;
    int key;

    // for class load check
    std::map<Method*, std::vector<int> >::iterator clsLdPrivIt;

    if (fd > 0)
    {
        // set prio as highest
        set_prio_max();

        // register as stack inspector
        ioctl(fd, CHANNEL_REGISTER_INSPECTOR, 0);

        // construct method name to sandbox key map
        size = ioctl(fd, CHANNEL_REQUEST_PM, buf);
        buf[size] = '\0';
        create_sandbox_tree(buf);

        /* init done -> sema up */
        is_stack_tracer_created = size;
        sem_post(&stack_tracer_init_sema);

        // main loop to response stack inspection
        while(1)
        {
            __read_data.target_tid = 0;
            __read_data.is_target_thd_suspended = 0;
            size = read(fd, &__read_data, sizeof(read_data));
            if (size != sizeof(read_data) ||
                    (__read_data.is_target_thd_suspended != 0
                     && __read_data.is_target_thd_suspended != 1)) {
                write(fd, keys, 0);
                continue;
            }
            traceBuf = NULL;
            stackDepth = dvmDdmGetStackTrace(
                    __read_data.target_tid,
                    &traceBuf,
                    __read_data.is_target_thd_suspended);
            if (!stackDepth || !traceBuf) {
                write(fd, keys, 0);
                continue;
            }
            traceBufMock = traceBuf;
            /*
             * convert stack trace to sandbox index.
             */
            numSB = 0;
            for (i = 0; i < stackDepth; i++) {
                Method* meth = (Method*) *traceBufMock++;

                /* class load check */
                clsLdPrivIt = clsLdPriv.find(meth);
                if (clsLdPrivIt != clsLdPriv.end()) {
                    const std::vector<int>& tmp = clsLdPrivIt->second;
                    for (unsigned int j = 0; j < tmp.size(); ++j) {
                        keys[numSB++] = tmp[j];
                    }
                    traceBufMock++;
                    continue;
                }

                it = sandboxCache.find(meth);
                if (it == sandboxCache.end()) {
                    std::string methName(meth->name);
                    std::string dotName(dvmHumanReadableDescriptor(meth->clazz->descriptor));
                    methName = dotName + "." + methName;

                    key = query_sandbox_key(methName);
                    sandboxCache[meth] = key;
                    if (key != -1) {
                        keys[numSB++] = key;
                    }

                    /* cache miss check */
                    ++cacheMiss;
                    if (cacheMiss + cacheHit >= 2000) {
                        ALOGI("jaebaek cacheMiss rate = %d / %d",
                                cacheMiss, (cacheMiss + cacheHit));
                        cacheMiss = 0;
                        cacheHit = 0;
                    }
                } else {
                    if (it->second != -1) {
                        keys[numSB++] = it->second;
                    }

                    /* cache miss check */
                    ++cacheHit;
                    if (cacheMiss + cacheHit >= 2000) {
                        ALOGI("jaebaek cacheMiss rate = %d / %d",
                                cacheMiss, (cacheMiss + cacheHit));
                        cacheMiss = 0;
                        cacheHit = 0;
                    }
                }
                traceBufMock++;
            }
            free(traceBuf);
            write(fd, keys, numSB*sizeof(int));
        }
        close(fd);
    }
    return (void *)0;
}

void *request_stack_inspection(const pid_t pid, const pid_t tid)
{
    ArrayObject* stackData = NULL;
    int target[2] = {pid, tid};
    int buf[BUFF_SIZE];
    size_t size;
    int* intPtr;
    int fd = open("/dev/stack_inspection_channel", O_RDWR);
    if (fd > 0)
    {
        size = write(fd, target, 2*sizeof(int));
        if (size == 2*sizeof(int)) {
            size = read(fd, buf, BUFF_SIZE) / sizeof(int);

            stackData = dvmAllocPrimitiveArray('I', size, ALLOC_DEFAULT);
            intPtr = (int*)(void*)stackData->contents;
            for (size_t i = 0; i < size; ++i) {
                *intPtr++=buf[i];
            }
        }
        close(fd);
    }
    return stackData;
}

void register_pm(void)
{
    int fd = open("/dev/stack_inspection_channel", O_RDWR);
    if (fd > 0)
    {
        ioctl(fd, CHANNEL_REGISTER_PM, 0);
        close(fd);
    }
}

void mark_class_load(Method* m)
{
    if (!is_stack_tracer_created)
        return;

    Thread* self = dvmThreadSelf();
    size_t stackDepth = 0;
    int* traceBuf;

    dvmLockThreadList(self);
    traceBuf = dvmFillInStackTraceRaw(self, &stackDepth);
    dvmUnlockThreadList();

    int key;
    std::map<Method*, int>::iterator it;
    std::map<Method*, std::vector<int> >::iterator clsLdPrivIt;
    std::vector<int> v;
    for (size_t i = 0; i < stackDepth; ++i) {
        Method* meth = (Method*) *traceBuf++;

        /* class load check */
        clsLdPrivIt = clsLdPriv.find(meth);
        if (clsLdPrivIt != clsLdPriv.end()) {
            const std::vector<int>& tmp = clsLdPrivIt->second;
            for (unsigned int j = 0; j < tmp.size(); ++j) {
                v.push_back(tmp[j]);
            }
            traceBuf++;
            continue;
        }

        it = sandboxCache.find(meth);
        if (it == sandboxCache.end()) {
            std::string methName(meth->name);
            std::string dotName(dvmHumanReadableDescriptor(meth->clazz->descriptor));
            methName = dotName + "." + methName;

            key = query_sandbox_key(methName);
            sandboxCache[meth] = key;
            if (key != -1) {
                v.push_back(key);
            }
        } else {
            if (it->second != -1) {
                v.push_back(it->second);
            }
        }
        traceBuf++;
    }

    if (v.size() > 0)
        clsLdPriv[m] = v;
}

#define handle_error(msg) \
    do { perror(msg); return NULL; } while (0)

void* get_pstack(void) {
    size_t size;
    StackSaveArea **buffer;
    StackSaveArea **ret;
    void* fp;
    Thread* self = dvmThreadSelf();

    static bool tf = false;
    if (!tf) {
        ALOGI("[JAEBAEK] sizeof(StackSaveArea) = %d", sizeof(StackSaveArea));
        tf = true;
    }

    dvmLockThreadList(self);
    fp = self->interpSave.curFrame;
    size = 0;
    while (fp != NULL) {
        StackSaveArea* saveArea = SAVEAREA_FROM_FP(fp);
        fp = saveArea->prevFrame;
        ++size;
    }
    dvmUnlockThreadList();

    buffer = (StackSaveArea **)malloc(sizeof(StackSaveArea*)*size);
    if (buffer == NULL)
        handle_error("memalign");
    ret = buffer;

    dvmLockThreadList(self);
    fp = self->interpSave.curFrame;
    while (fp != NULL) {
        StackSaveArea* saveArea = SAVEAREA_FROM_FP(fp);
        fp = saveArea->prevFrame;
        *buffer++ = saveArea;
    }
    *buffer++ = NULL;
    dvmUnlockThreadList();

    return (void*) ret;
}
