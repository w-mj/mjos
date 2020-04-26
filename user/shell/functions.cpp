//
// Created by wmj on 4/26/20.
//

#include "functions.h"
#include <syscall.h>


void cd(const char *arg) {
    if (arg == nullptr)
        return;
    sys_chdir(arg);
}

ShellFunction functionList[] = {
        {"cd", cd},
        {nullptr, nullptr}
};
