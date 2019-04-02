#!/bin/sh
cd cmd/kvm-caps-info-nfd-plugin && gcc -Wall -g -Ilinux-headers -DKVMINFO_TOOL=1 -o kvm-caps-info-nfd-plugin kvminfocaps.c
