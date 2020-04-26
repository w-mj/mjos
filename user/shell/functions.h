//
// Created by wmj on 4/26/20.
//

#ifndef OS_FUNCTIONS_H
#define OS_FUNCTIONS_H


struct ShellFunction {
    const char *name;
    void (*func)(const char *);
};

extern ShellFunction functionList[];

#endif //OS_FUNCTIONS_H
