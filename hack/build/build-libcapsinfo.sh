#!/bin/bash
set -e
cd lib
gcc -Wall -c -g -Ilinux-headers -o kvminfocaps.o kvminfocaps.c
ar crus libkvminfocaps.a kvminfocaps.o
