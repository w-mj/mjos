//
// Created by wmj on 4/20/20.
//

#ifndef OS_DIRENT_H
#define OS_DIRENT_H
#ifdef __cplusplus
extern "C" {
#endif

#define NAME_MAX 128

struct dirent{
//    long d_ino;              /* inode number */
//    long d_off;              /* offset to this old_linux_dirent */
//    unsigned short d_reclen;  /* length of this d_name */
    char  d_name[NAME_MAX+1]; /* filename (null-terminated) */
};

#ifdef __cplusplus
}
#endif
#endif //OS_DIRENT_H
