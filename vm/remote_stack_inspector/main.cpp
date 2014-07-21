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
    std::vector<std::string> stack_info;
    char buf[BUFF_SIZE];
    size_t ret;

    if (fd > 0)
    {
        ioctl(fd, 1, 0);
        while(1)
        {
            ret = read(fd, &target_tid, sizeof(pid_t));
            ALOGI("do_stack_inspection: tid=%d", target_tid);
            dvmDdmGetStackTrace(target_tid, stack_info);
            ret = copy_to_buf(buf, stack_info);
            ALOGI("do_stack_inspection: buf=%s", buf);
            ret = write(fd, buf, ret);
        }
        close(fd);
    }
    return (void *)0;
}

std::string request_stack_inspection(const pid_t pid, const pid_t tid)
{
    int target[2] = {pid, tid};
    char buf[BUFF_SIZE] = {0};
    int fd = open("/dev/stack_inspection_channel", O_RDWR);
    std::string trace;
    size_t ret;
    if (fd > 0)
    {
        ret = write(fd, target, 2*sizeof(int));
        ALOGI("request_stack_inspection: pid=%d, tid=%d", pid, tid);
        // assert(ret == sizeof(int));
        ret = read(fd, buf, BUFF_SIZE);
        ALOGI("request_stack_inspection: buf=%s", buf);
        // assert(ret != -1);
        trace.append(buf);
        ALOGI("request_stack_inspection: trace=%s", trace.c_str());
        close(fd);
    }
    return trace;
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
