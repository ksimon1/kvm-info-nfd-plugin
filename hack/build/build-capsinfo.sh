#!/bin/sh
gcc -Wall -g -Ilib -o cmd/kvm-caps-info-nfd-plugin/kvm-caps-info-nfd-plugin \
	cmd/kvm-caps-info-nfd-plugin/main.c \
	cmd/kvm-caps-info-nfd-plugin/kvm_ioctl.c \
	lib/libkvminfocaps.a
