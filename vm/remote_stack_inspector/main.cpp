#include "main.h"
#include "../Dalvik.h"
#include "../Ddm.h"

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

void *do_stack_inspection(void *arg)
{
    // for IPC
    int fd = open("/dev/stack_inspection_channel", O_RDWR);
    pid_t target_tid;
    char buf[4*BUFF_SIZE];
    int key[BUFF_SIZE];
    int size;

    // for stack trace
    int* traceBuf;
    size_t stackDepth = 0, i, numSB;

    // for cache
    std::map<Method*, int> sandboxCache;
    std::map<Method*, int>::iterator it;

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

        // main loop to response stack inspection
        while(1)
        {
            read(fd, &target_tid, sizeof(pid_t));
            stackDepth = dvmDdmGetStackTrace(target_tid, &traceBuf);
            /*
             * convert stack trace to sandbox index.
             */
            numSB = 0;
            for (i = 0; i < stackDepth; i++) {
                Method* meth = (Method*) *traceBuf++;
                it = sandboxCache.find(meth);
                if (it == sandboxCache.end()) {
                    std::string methName(meth->name);
                    methName = dvmHumanReadableDescriptor(meth->clazz->descriptor)
                        + "." + methName;
                    sandboxCache[meth] = query_sandbox_key(methName);
                } else {
                    if (it->second != -1) {
                        key[numSB++] = it->second;
                    }
                }
            }
            free(traceBuf);
            write(fd, key, numSB*sizeof(int));
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
        write(fd, target, 2*sizeof(int));
        size = read(fd, buf, BUFF_SIZE) / sizeof(int);

        stackData = dvmAllocPrimitiveArray('I', size, ALLOC_DEFAULT);
        intPtr = (int*)(void*)stackData->contents;
        for (size_t i = 0; i < size; ++i) {
            *intPtr++=buf[i];
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
