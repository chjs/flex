#!/bin/bash
LD_PRELOAD=../../lib/libflex.so \
	   numactl --cpunodebind=0 --membind=0 \
	   src/fio \
	   workloads/seq-write.fio

