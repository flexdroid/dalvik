#ifndef STACK_INSPECTION_H
#define STACK_INSPECTION_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string>
#include <map>
#include <set>

#include <sys/time.h>
#include <sys/resource.h>
#include <semaphore.h>

#include "Dalvik.h"

extern sem_t stack_tracer_init_sema;

// jaebaek: do stack inspection as a thread
void *do_stack_inspection(void *);
void *request_stack_inspection(const pid_t, const pid_t);
void register_pm(void);

void create_sandbox_tree(const char *);
int query_sandbox_key(const std::string&);

void mark_class_load(Method*);

void* get_pstack(void);

#endif
