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

void *do_stack_inspection(void *arg)
{
    int fd = open("/dev/stack_inspection_channel", O_RDWR);
    pid_t target_tid;
    int test = 0;

    if (fd > 0)
    {
        ioctl(fd, 1, 0);
        while(1)
        {
            read(fd, &target_tid, sizeof(pid_t));
            //dvmDdmGetStackTrace(target_tid, stack_info);
            write(fd, &test, sizeof(int));
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
