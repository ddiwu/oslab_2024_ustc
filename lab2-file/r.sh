#!/bin/bash
cd /home/wd/oslab/lab2-file
gcc -static -o get_ps_info get_ps_info.c
mv get_ps_info /home/wd/oslab/busybox-1.32.1/_install
cd /home/wd/oslab/busybox-1.32.1/_install
find . -print0 | cpio --null -ov --format=newc | gzip -9 > /home/wd/oslab/initramfs-busybox-x64.cpio.gz
cd /home/wd/oslab/linux-4.9.263
make -j $((`nproc`-2))