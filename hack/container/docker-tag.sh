#!/bin/bash

TAG="${1:-devel}"

docker build -t fromanirh/kvm-info-nfd-plugin:$TAG .
