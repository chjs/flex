# FLEX
I am implementing FLEX,
which was introduced
in [Jian Xu's paper](https://dl.acm.org/doi/10.1145/3297858.3304077).
* Finding and Fixing Performance Pathologies
in Persistent Memory Software Stacks,
_ASPLOS '19_.

## Introduction
FLEX is a useful tool that allows legacy applications
that perform conventional file IO (_e.g._, ```read```/```write``` system calls)
to perform memory-mapped IO (_e.g._, ```load```/```store``` instructions).
FLEX transparently intercepts the traditional file IO requests
and then perform memory-mapped IO.

In low-latency and byte-addressable non-volatile main memory (NVMM) systems,
the complex IO stack of the kernel appears as a significant overhead.
With FLEX, applications can minimize the software overhead
since they bypass the kernel's IO stack and
perform user-level IO by leveraging memory-mapped IO.

## Difference from Original FLEX
My FLEX is very similar to the original FLEX, but with some differences.
* For the convenience of evaluation, my FLEX provides a new open flag, ```O_FLEX```.
My FLEX handles only files opened with the ```O_FLEX``` flag,
and the underlying file system processes other files.
It requires source code modification to use the ```O_FLEX``` flag,
but it is the amount negligible.
For example, [FIO](https://github.com/axboe/fio)
and [Filebench](https://github.com/filebench/filebench) need to modify just two lines of source code.

* The original FLEX uses [non-temporal store instructions](https://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-1-manual.html)
to perform writes,
and when the application calls fsync,
it uses [sfence instructions](https://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-1-manual.html)
to ensure that all previous writes are permanent.
However, I think that the sfence instructions in fsync
cannot guarantee the durability of the updated data
since the fsync could be performed on other CPUs
that have not performed the writes.
Thus, my FLEX ensures that the updated data become durable
before returning the write call.
In FLEX that I implemented,
it uses [pmem_memcpy_persist](https://manpages.debian.org/testing/libpmem-dev/pmem_memcpy_persist.3.en.html)
to perform writes, and fsync does nothing.
