#!/bin/sh
cd cmd/kvm-caps-info-nfd-plugin && gcc -Wall -g -I../../lib -o kvm-caps-info-nfd-plugin main.c ../../lib/libkvminfocaps.a kvm_ioctl.c
