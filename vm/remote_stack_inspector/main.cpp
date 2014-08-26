#include "main.h"
#include "../Dalvik.h"
#include "../Ddm.h"

#define  BUFF_SIZE      (16 * 1024)

size_t copy_to_buf(char* dst, const std::vector<std::string>& src)
{
    size_t size = 0;
    for (size_t i = 0; i < src.size(); ++i) {
        strncpy(&dst[size], src[i].c_str(), src[i].size());
        size += src[i].size();
        dst[size++] = ' ';
    }
    /*
     * jaebaek: insert NULL at the end of char array
     *          Should I really do this?
     */
    if (size)
        dst[size - 1] = '\0';
    return size;
}

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
    int fd = open("/dev/stack_inspection_channel", O_RDWR);
    pid_t target_tid;
    int buf[BUFF_SIZE];
    int* traceBuf;
    size_t stackDepth = 0, i, numSB;
    std::map<Method*, int> sandboxCache;
    std::map<Method*, int>::iterator it;

    if (fd > 0)
    {
        set_prio_max();
        ioctl(fd, 1, 0);
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
                    // Add meth to sandboxCache
                    // if meth is sandboxed, register sandbox index
                    // otherwise, register -1
                    std::string methName(meth->name);
                    methName = dvmHumanReadableDescriptor(meth->clazz->descriptor)
                        + "." + methName;
                } else {
                    if (it->second != -1) {
                        buf[numSB++] = 0;//it->second;
                    }
                }
            }
            free(traceBuf);
            write(fd, buf, numSB*sizeof(int));
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
        ioctl(fd, 0, 0);
        close(fd);
    }
}
