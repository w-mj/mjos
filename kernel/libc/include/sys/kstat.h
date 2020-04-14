//
// Created by wmj on 4/14/20.
//

#ifndef OS_KSTAT_H
#define OS_KSTAT_H

#ifdef __cplusplus
extern "C" {
#endif

#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

typedef struct __Stat {
    unsigned long dev;     /* ID of device containing file */
    unsigned long ino;     /* inode number */
    unsigned int  mode;    /* protection */
    unsigned long nlink;   /* number of hard links */
    unsigned int  uid;     /* user ID of owner */
    unsigned long gid;     /* group ID of owner */
    unsigned long rdev;    /* device ID (if special file) */
    unsigned long size;    /* total size, in bytes */
    unsigned long blksize; /* blocksize for file system I/O */
    unsigned long blocks;  /* number of 512B blocks allocated */
    unsigned long atime;   /* time of last access */
    unsigned long mtime;   /* time of last modification */
    unsigned long ctime;   /* time of last status change */
} kStat;

#ifdef __cplusplus
};
#endif

#endif //OS_KSTAT_H
