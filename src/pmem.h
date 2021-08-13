#ifndef LIBFLEX_PMEM_H
#define LIBFLEX_PMEM_H

#include <libpmem.h>

#if 0
#define memcpy2pmem(dst, src, n) pmem_memcpy(dst, src, n, PMEM_F_MEM_NODRAIN)
#define memcpy2pmem(dst, src, n) pmem_memcpy(dst, src, n, PMEM_F_MEM_NOFLUSH)
#define memcpy2pmem(dst, src, n) pmem_memcpy(dst, src, n, PMEM_F_MEM_NONTEMPORAL)
#define memcpy2pmem(dst, src, n) pmem_memcpy(dst, src, n, PMEM_F_MEM_TEMPORAL)
#define memcpy2pmem(dst, src, n) pmem_memcpy(dst, src, n, PMEM_F_MEM_WC)
#define memcpy2pmem(dst, src, n) pmem_memcpy(dst, src, n, PMEM_F_MEM_WB)
#define memcpy2pmem(dst, src, n) pmem_memcpy_persist(dst, src, n)
#define memcpy2pmem(dst, src, n) pmem_memcpy_nodrain(dst, src, n)

#define memcpy2pmem(dst, src, n) \
	do { \
		pmem_memcpy(dst, src, n, PMEM_F_MEM_NOFLUSH); \
		pmem_drain(); \
	} while (0)
#endif

#define memcpy2pmem(dst, src, n) pmem_memcpy_persist(dst, src, n)

#endif /* LIBFLEX_PMEM_H */
