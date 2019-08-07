#!/bin/sh
export GO111MODULE=on
export GOPROXY=off
export GOFLAGS=-mod=vendor
cd cmd/kvm-version-info-nfd-plugin && go build -v .
