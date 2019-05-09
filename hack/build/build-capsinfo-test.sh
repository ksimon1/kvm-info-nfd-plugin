#!/bin/sh
gcc -Wall -g -Ilib -Icontrib/tomlc99 -o tests/capsinfo/kvm-caps-info-test cmd/kvm-caps-info-nfd-plugin/main.c lib/libkvminfocaps.a tests/capsinfo/mock_kvm_ioctl.c contrib/tomlc99/toml.c
