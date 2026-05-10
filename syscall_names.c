#include "syscall_names.h"

#include <stdio.h>

const char* get_syscall_name(unsigned long id) {
    switch(id) {
        case 0: return "read";
        case 1: return "write";
        case 2: return "open";
        case 3: return "close";
        case 4: return "stat";
        case 5: return "fstat";
        case 8: return "lseek";
        case 9: return "mmap";
        case 10: return "mprotect";
        case 11: return "munmap";
        case 12: return "brk";
        case 13: return "rt_sigaction";
        case 14: return "rt_sigprocmask";
        case 16: return "ioctl";
        case 21: return "access";
        case 35: return "nanosleep";
        case 39: return "getpid";
        case 59: return "execve";
        case 218: return "set_tid_address";
        case 231: return "exit_group";
        case 257: return "openat";
        case 318: return "getrandom";
        default: return "unknown"; 
    }
}