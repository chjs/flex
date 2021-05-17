#define _GNU_SOURCE

#include "fops.h"

#include <dlfcn.h>

#include "debug.h"

fops_t posix;

void init_fops(void)
{
	posix.open64 = dlsym(RTLD_NEXT, "open64");
	if (IS_ERR(posix.open64 == NULL)) {
		ERROR("dlsym(open64)");
	}

	posix.read = dlsym(RTLD_NEXT, "read");
	if (IS_ERR(posix.read == NULL)) {
		ERROR("dlsym(read)");
	}

	posix.write = dlsym(RTLD_NEXT, "write");
	if (IS_ERR(posix.write == NULL)) {
		ERROR("dlsym(write)");
	}

	posix.lseek64 = dlsym(RTLD_NEXT, "lseek64");
	if (IS_ERR(posix.lseek64 == NULL)) {
		ERROR("dlsym(lseek64)");
	}

	posix.ftruncate = dlsym(RTLD_NEXT, "ftruncate");
	if (IS_ERR(posix.ftruncate == NULL)) {
		ERROR("dlsym(ftruncate)");
	}


	posix.__xstat64 = dlsym(RTLD_NEXT, "__xstat64");
	if (IS_ERR(posix.__xstat64 == NULL)) {
		ERROR("dlsym(__xstat64)");
	}

	posix.__fxstat64 = dlsym(RTLD_NEXT, "__fxstat64");
	if (IS_ERR(posix.__fxstat64 == NULL)) {
		ERROR("dlsym(__fxstat64)");
	}

	posix.fsync = dlsym(RTLD_NEXT, "fsync");
	if (IS_ERR(posix.fsync == NULL)) {
		ERROR("dlsym(fsync)");
	}

	posix.close = dlsym(RTLD_NEXT, "close");
	if (IS_ERR(posix.close == NULL)) {
		ERROR("dlsym(close)");
	}
}
