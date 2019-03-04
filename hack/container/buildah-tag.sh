#!/bin/bash

TAG="${1:-devel}"
XTAGS="${@:2}"  # see https://stackoverflow.com/questions/9057387/process-all-arguments-except-the-first-one-in-a-bash-script

buildah bud -t fromani/kvm-info-nfd-plugin:$TAG .
for XTAG in $XTAGS; do
	buildah tag fromani/kvm-info-nfd-plugin:$TAG fromani/kvm-info-nfd-plugin:$XTAG
done
