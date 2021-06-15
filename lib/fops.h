#ifndef LIBFLEX_FOPS_H
#define LIBFLEX_FOPS_H

#include <sys/types.h>
#include <sys/stat.h>

typedef struct fops_struct {
	int (*open)(const char *pathname, int flags, ...);
	int (*open64)(const char *pathname, int flags, ...);
	ssize_t (*read)(int fd, void *buf, size_t count);
	ssize_t (*write)(int fd, const void *buf, size_t count);
	off_t (*lseek64)(int fd, off_t offset, int whence);
	int (*ftruncate)(int fd, off_t length);
	int (*truncate)(const char *path, off_t length);
	int (*__xstat64)(int ver, const char *pathname, struct stat *statbuf);
	int (*__fxstat64)(int ver, int fd, struct stat *statbuf);
	int (*fsync)(int fd);
	int (*close)(int fd);
	int (*unlink)(const char *pathname);
} fops_t;

void init_fops(void);

#endif /* LIBFLEX_FOPS_H */
