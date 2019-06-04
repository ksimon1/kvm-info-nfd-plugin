#!/bin/bash

set -ex

SELF=$( realpath $0 )
BASEPATH=$( dirname $SELF )

# no need to check the version-info plugin

./cmd/kvm-caps-info-nfd-plugin/kvm-caps-info-nfd-plugin | ${BASEPATH}/verify-labels.py
