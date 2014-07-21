#ifndef STACK_INSPECTION_H
#define STACK_INSPECTION_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string>

// jaebaek: do stack inspection as a thread
void *do_stack_inspection(void *);
std::string request_stack_inspection(const pid_t, const pid_t);
void register_pm(void);

#endif
