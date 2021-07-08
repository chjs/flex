#!/bin/bash
git clone https://github.com/axboe/fio.git src
cd src
git checkout tags/fio-3.27
patch -p1 < ../fio-flex.patch
./configure
make
