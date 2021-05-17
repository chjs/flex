#define _GNU_SOURCE

#include "mmf.h"

#include <fcntl.h>
#include <sys/mman.h>

#include "fops.h"
#include "pmem.h"
#include "debug.h"

extern fops_t posix;

typedef struct head_struct {
	list_t head;
	pthread_mutex_t mutex;
} head_t;

static head_t mmf_table[MAX_MMFS];

void init_mmf_table(void)
{
	int s;

	for (int i = 0; i < MAX_MMFS; i++) {
		INIT_LIST_HEAD(&mmf_table[i].head);

		s = pthread_mutex_init(&mmf_table[i].mutex, NULL);
		if (IS_ERR(s != 0))
			ERROR("pthread_mutex_init");
	}
}

static inline int get_hash_index(ULONG ino)
{
	return ino % MAX_MMFS;
}

static inline void expand_underlying(int fd, ULONG len)
{
	int s;
	s = posix_fallocate(fd, 0, len);
	if (IS_ERR(s != 0))
		ERROR("posix_fallocate");
}

static mmf_t *create_mmf(int fd, ULONG len, ULONG ino)
{
	mmf_t *mmf;
	int prot, flags, s;

	mmf = (mmf_t *)malloc(sizeof(mmf_t));
	if (IS_ERR(mmf == NULL))
		ERROR("malloc");

	prot = PROT_READ | PROT_WRITE;
	flags = MAP_SHARED | MAP_POPULATE;

	if (len < DEFAULT_MMAP_SIZE) {
		len = DEFAULT_MMAP_SIZE;
		expand_underlying(fd, len);
	}

	mmf->start = mmap(NULL, len, prot, flags, fd, 0);
	if (IS_ERR(mmf->start == MAP_FAILED))
		ERROR("mmap");

	mmf->end = mmf->start + len;
	mmf->ino = ino;
	mmf->ref = 0;
	mmf->filesize = len;

	s = pthread_mutex_init(&mmf->mutex, NULL);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_init");

	return mmf;
}

mmf_t *find_mmf(ULONG ino)
{
	mmf_t *tmp, *mmf = NULL;
	int index, s;

	index = get_hash_index(ino);
	
	s = pthread_mutex_lock(&mmf_table[index].mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	if (!list_empty(&mmf_table[index].head)) {
		LIST_FOR_EACH_ENTRY(tmp, &mmf_table[index].head, list) {
			if (ino == tmp->ino) {
				mmf = tmp;
				break;
			}
		}
	}

	s = pthread_mutex_unlock(&mmf_table[index].mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_unlock");

	return mmf;
}

mmf_t *get_mmf(ULONG ino, int fd, ULONG size)
{
	mmf_t *mmf;
	int s;

	mmf = find_mmf(ino);

	if (mmf == NULL) {
		int index;

		mmf = create_mmf(fd, size, ino);
		index = get_hash_index(ino);

		s = pthread_mutex_lock(&mmf_table[index].mutex);
		if (IS_ERR(s != 0))
			ERROR("pthread_mutex_lock");

		list_add(&mmf->list, &mmf_table[index].head);

		s = pthread_mutex_unlock(&mmf_table[index].mutex);
		if (IS_ERR(s != 0))
			ERROR("pthread_mutex_unlock");
	}
	s = pthread_mutex_lock(&mmf->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	mmf->ref++;

	s = pthread_mutex_unlock(&mmf->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_unlock");

	return mmf;
}

static inline bool is_exceed(mmf_t *mmf, ULONG pos, ULONG len)
{
	if (mmf->end < (mmf->start + pos + len))
		return true;
	return false;
}

static void expand_mmf(mmf_t *mmf, ULONG pos, ULONG len, int fd)
{
	ULONG current_len, new_len;
	int s;

	while (is_exceed(mmf, pos, len)) {
		current_len = mmf->end - mmf->start;
		new_len = current_len << 1;

		s = posix_fallocate(fd, 0, new_len);
		if (IS_ERR(s != 0))
			ERROR("posix_fallocate");

		mmf->start = mremap(mmf->start, current_len, new_len,
				MREMAP_MAYMOVE);
		if (IS_ERR(mmf->start == MAP_FAILED))
			ERROR("mremap");

		mmf->end = mmf->start + new_len;
	}
}

ULONG read_mmf(mmf_t *mmf, ULONG pos, void *buf, ULONG len)
{
	void *src;
	int s;

	s = pthread_mutex_lock(&mmf->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	if (pos + len > mmf->filesize)
		len = mmf->filesize - pos;

	src = mmf->start + pos;
	memcpy(buf, src, len);

	s = pthread_mutex_unlock(&mmf->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_unlock");

	return len;
}

ULONG write_mmf(int fd, mmf_t *mmf, ULONG pos, const void *buf, ULONG len)
{
	void *dst;
	int s;

	s = pthread_mutex_lock(&mmf->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	if (is_exceed(mmf, pos, len))
		expand_mmf(mmf, pos, len, fd);

	dst = mmf->start + pos;
	NTSTORE(dst, buf, len);

	if (pos + len > mmf->filesize)
		mmf->filesize = pos + len;

	s = pthread_mutex_unlock(&mmf->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_unlock");

	return len;
}

void release_mmf(mmf_t *mmf, int fd)
{
	ULONG len = 0;
	int s;

	s = pthread_mutex_lock(&mmf->mutex);
	if (IS_ERR(s != 0))
		ERROR("pthread_mutex_lock");

	if (mmf->ref > 1) {
		mmf->ref -= 1;

		s = pthread_mutex_unlock(&mmf->mutex);
		if (IS_ERR(s != 0))
			ERROR("pthread_mutex_unlock");
	}
	list_del(&mmf->list);

	len = mmf->end - mmf->start;

	s = munmap(mmf->start, len);
	if (IS_ERR(s != 0))
		ERROR("munmap");

	if (mmf->filesize < len) {
		if (IS_ERR(posix.ftruncate == NULL))
			init_fops();

		s = posix.ftruncate(fd, mmf->filesize);
		if (IS_ERR(s != 0))
			ERROR("ftruncate");
	}

	free(mmf);
}
