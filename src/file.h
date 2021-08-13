#ifndef LIBFLEX_FILE_H
#define LIBFLEX_FILE_H

#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>

#include "mmf.h"
#include "type.h"

typedef struct file_struct {
	ULONG pos;
	ULONG ino;
	mmf_t *mmf;
	pthread_mutex_t mutex;
} file_t;

#define MAX_FILES 1024

void init_file_array(void);
bool check_flex(int fd);
int open_file(const char *pathname, int flags, int mode);
ULONG read_file(int fd, void *buf, ULONG len);
ULONG write_file(int fd, const void *buf, ULONG len);
ULONG lseek_file(int fd, ULONG offset, int whence);
int close_file(int fd);

#endif /* LIBFLEX_FILE_H */
