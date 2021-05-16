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
