#ifndef LIBFLEX_PMEM_H
#define LIBFLEX_PMEM_H

#include <libpmem.h>

#define NTSTORE(dst, src, n) pmem_memcpy_persist(dst, src, n)

#endif /* LIBFLEX_PMEM_H */
