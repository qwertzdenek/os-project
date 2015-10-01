#ifndef CALLS_H
#define CALLS_H

#include <unistd.h>
#include <sys/reboot.h>

// 1
#define do_exit(code) _exit(code)

// 2
#define do_fork() fork()

// 11
#define do_exec(name) execve(name)

// 20
#define do_getpid() getpid()

// 68
#define do_get_ppid() getppid()

// 88
#define do_reboot(cmd) reboot(cmd)

#endif

