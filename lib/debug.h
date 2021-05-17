#ifndef LIBFLEX_DEBUG_H
#define LIBFLEX_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define IS_ERR __glibc_unlikely

#define ERROR(fmt, ...) \
	do { \
		fprintf(stderr, "[%s:%d:%s] " fmt ": %s\n", \
				__FILE__, __LINE__, \
				__func__, ##__VA_ARGS__, \
				strerror(errno)); \
		exit(EXIT_FAILURE); \
	} while (0)

#ifdef DEBUG
#define PRINT(fmt, ...) \
	do { \
		fprintf(stderr, "%ld [%s:%d:%s] " fmt "\n", \
				(long)pthread_self(), \
				__FILE__, __LINE__, \
				__func__, ##__VA_ARGS__); \
	} while (0)
#else /* DEBUG */
#define PRINT(fmt, ...) \
	do { \
	} while (0)
#endif /* DEBUG */

#endif /* LIBFLEX_DEBUG_H */
