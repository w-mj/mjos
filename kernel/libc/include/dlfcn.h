//
// Created by wmj on 3/23/20.
//

#ifndef OS_DLFCN_H
#define OS_DLFCN_H

typedef struct {
    const char *dli_fname;  /* Pathname of shared object that
                                          contains address */
    void       *dli_fbase;  /* Base address at which shared
                                          object is loaded */
    const char *dli_sname;  /* Name of symbol whose definition
                                          overlaps addr */
    void       *dli_saddr;  /* Exact address of symbol named
                                          in dli_sname */
} Dl_info;


static inline int dladdr(void*, Dl_info*) {
    return 0;
}

#endif //OS_DLFCN_H
