#include "file.h"

#include <sys/stat.h>

#include "fops.h"
#include "mmf.h"
#include "debug.h"

extern fops_t posix;

static file_t *file_array[MAX_FILES];

void init_file_array(void)
{
	for (int i=0; i < MAX_FILES; i++)
		file_array[i] = NULL;
}

inline bool check_flex(int fd)
{
	if (file_array[fd] != NULL)
		return true;
	return false;
}

int open_file(const char *pathname, int flags, int mode)
{
	struct stat statbuf;
	file_t *filp;
	int fd, s;

	if (IS_ERR(posix.open64 == NULL))
		init_fops();

	fd = posix.open64(pathname, flags, mode);
	if (IS_ERR(fd < 0))
		ERROR("posix.open64");

	s = posix.__fxstat64(_STAT_VER, fd, &statbuf);
	if (IS_ERR(s != 0))
		ERROR("fstat");

	filp = (file_t *)malloc(sizeof(file_t));
	if (IS_ERR(filp == NULL))
		ERROR("malloc");

	filp->pos = 0;
	filp->ino = statbuf.st_ino;
	filp->mmf = get_mmf(filp->ino, fd, statbuf.st_size);

	s = pthread_mutex_init(&filp->mutex, NULL);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_init");

	file_array[fd] = filp;
	return fd;
}

ULONG read_file(int fd, void *buf, ULONG len)
{
	file_t *filp;
	ULONG result;
	int s;

	result = 0;

	filp = file_array[fd];

	s = pthread_mutex_lock(&filp->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	result = read_mmf(filp->mmf, filp->pos, buf, len);
	filp->pos += result;

	s = pthread_mutex_unlock(&filp->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_unlock");

	return result;
}

ULONG write_file(int fd, const void *buf, ULONG len)
{
	file_t *filp;
	ULONG result = 0;
	int s;

	filp = file_array[fd];

	s = pthread_mutex_lock(&filp->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	result = write_mmf(fd, filp->mmf, filp->pos, buf, len);
	filp->pos += result;

	s = pthread_mutex_unlock(&filp->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_unlock");

	return result;
}

ULONG lseek_file(int fd, ULONG offset, int whence)
{
	file_t *filp;
	ULONG result;
	int s;

	filp = file_array[fd];

	s = pthread_mutex_lock(&filp->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	switch (whence) {
		case SEEK_SET:
			filp->pos = offset;
			break;
		case SEEK_CUR:
			filp->pos += offset;
			break;
		case SEEK_END:
			filp->pos = filp->mmf->filesize + offset;
			break;
		default:
			ERROR("wrong whence");
	}

	result = filp->pos;

	s = pthread_mutex_unlock(&filp->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_unlock");

	return result;
}

int close_file(int fd)
{
	file_t *filp;
	int s;

	filp = file_array[fd];

	s = pthread_mutex_lock(&filp->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	file_array[fd] = NULL;

	release_mmf(filp->mmf, fd);

	free(filp);

	if (IS_ERR(posix.close == NULL))
		init_fops();

	return posix.close(fd);
}
