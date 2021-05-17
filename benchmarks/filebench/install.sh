#!/bin/bash
git clone https://github.com/filebench/filebench.git src
cd src
git checkout 1.4.9.1
patch -p1 < ../filebench-flex.patch
libtoolize
aclocal
autoheader
automake --add-missing
autoconf
./configure
make
