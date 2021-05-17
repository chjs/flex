#include "../include/flex.h"

#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdarg.h>

#include "fops.h"
#include "file.h"
#include "mmf.h"
#include "atomic.h"
#include "debug.h"

extern fops_t posix;

static bool initialized = false;

void init_libflex(void)
{
	init_fops();
	init_file_array();
	init_mmf_table();
	PRINT("flex was initialized");
}

void __attribute__((constructor)) load_libflex(void)
{
	if (CAS(&initialized, false, true)) {
		init_libflex();
	}
}

int open64(const char *pathname, int flags, ...)
{
	int mode = 0;

	if (flags & O_CREAT) {
		va_list arg;
		va_start(arg, flags);
		mode = va_arg(arg, int);
		va_end(arg);
	}

	if (flags & O_FLEX) {
		PRINT("path=%s, flags=%d, mode=%d", pathname, flags, mode);
		return open_file(pathname, flags, mode);
	}

	if (IS_ERR(posix.open64 == NULL))
		init_fops();

	return posix.open64(pathname, flags, mode);
}

ssize_t read(int fd, void *buf, size_t len)
{
	if (check_flex(fd)) {
		PRINT("fd=%d, buf=%p, len=%lu", fd, buf, len);
		return (ssize_t)read_file(fd, buf, len);
	}

	if (IS_ERR(posix.read == NULL))
		init_fops();

	return (ssize_t)posix.read(fd, buf, len);
}

ssize_t write(int fd, const void *buf, size_t len)
{
	if (check_flex(fd)) {
		PRINT("fd=%d, buf=%p, len=%lu", fd, buf, len);
		return (ssize_t)write_file(fd, buf, len);
	}

	if (IS_ERR(posix.write == NULL))
		init_fops();

	return (ssize_t)posix.write(fd, buf, len);
}

off_t lseek64(int fd, off_t offset, int whence)
{
	if (check_flex(fd)) {
		PRINT("fd=%d, offset=%ld whence=%d", fd, offset, whence);
		return (off_t)lseek_file(fd, offset, whence);
	}

	if (IS_ERR(posix.lseek64 == NULL))
		init_fops();

	return (off_t)posix.lseek64(fd, offset, whence);
}

int __xstat64(int ver, const char *pathname, struct stat *statbuf)
{
	mmf_t *mmf;
	int s;

	PRINT("path=%s", pathname);

	if (IS_ERR(posix.__xstat64 == NULL))
		init_fops();

	s = posix.__xstat64(ver, pathname, statbuf);
	if (IS_ERR(s != 0))
		return s;

	mmf = find_mmf(statbuf->st_ino);

	if (mmf)
		statbuf->st_size = mmf->filesize;

	return s;
}

int __fxstat64(int ver, int fd, struct stat *statbuf)
{
	mmf_t *mmf;
	int s;

	PRINT("fd=%d", fd);

	if (IS_ERR(posix.__fxstat64 == NULL))
		init_fops();

	s = posix.__fxstat64(ver, fd, statbuf);
	if (IS_ERR(s != 0))
		return s;

	mmf = find_mmf(statbuf->st_ino);

	if (mmf)
		statbuf->st_size = mmf->filesize;

	return s;
}

int fsync(int fd)
{
	if (check_flex(fd)) {
		PRINT("fd=%d", fd);
		return 0;
	}

	return posix.fsync(fd);
}

int close(int fd)
{
	if (check_flex(fd)) {
		PRINT("fd=%d", fd);
		return close_file(fd);
	}

	if (IS_ERR(posix.close == NULL))
		init_fops();

	return posix.close(fd);
}
