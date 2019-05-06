#!/bin/sh
cd cmd/kvm-caps-info-nfd-plugin && gcc -Wall -g -Ilinux-headers -o kvm-caps-info-nfd-plugin kvminfocaps.c main.c
