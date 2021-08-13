#ifndef LIBFLEX_MMF_H
#define LIBFLEX_MMF_H

#include <pthread.h>
#include <sys/types.h>

#include "type.h"
#include "list.h"

typedef struct mmf_struct {
	void *start;
	void *end;
	ULONG ino;
	ULONG filesize;
	int ref;
	pthread_mutex_t mutex;
	list_t list;
} mmf_t;

void init_mmf_table(void);
mmf_t *find_mmf(ULONG ino);
mmf_t *get_mmf(ULONG ino, int fd, ULONG size);
ULONG read_mmf(mmf_t *mmf, ULONG pos, void *buf, ULONG len);
ULONG write_mmf(int fd, mmf_t *mmf, ULONG pos, const void *buf, ULONG len);
void release_mmf(mmf_t *mmf);
void remove_mmf(mmf_t *mmf, const char *path);

#define MAX_MMFS 1024
#define DEFAULT_MMAP_SIZE 1 << 20 // 1MB 

#endif /* LIBFLEX_MMF_H */
