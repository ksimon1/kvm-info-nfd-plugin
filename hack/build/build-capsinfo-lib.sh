#!/bin/bash
set -e
gcc -Wall -c -g -Ilib/linux-headers -o lib/kvminfocaps.o lib/kvminfocaps.c
ar crus lib/libkvminfocaps.a lib/kvminfocaps.o
