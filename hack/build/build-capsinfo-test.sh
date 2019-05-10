#!/bin/sh
gcc -Wall -g -Ilib -Icontrib/tomlc99 -o tests/caps-info/kvm-caps-info-test \
	cmd/kvm-caps-info-nfd-plugin/main.c \
	lib/libkvminfocaps.a \
	tests/caps-info/mock_kvm_io.c \
	contrib/tomlc99/toml.c
