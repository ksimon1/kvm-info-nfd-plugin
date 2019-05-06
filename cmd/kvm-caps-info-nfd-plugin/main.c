/*
 * code extracted and minimally changed to be standalone the QEMU KVM support.
 *
 * uses code from: target/i386/kvm.c
 * Copyright (C) 2006-2008 Qumranet Technologies
 * Copyright IBM, Corp. 2008
 *
 * Authors:
 *  Anthony Liguori   <aliguori@us.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 * uses code from: accel/kvm/kvm-all.c
 * Copyright IBM, Corp. 2008
 *           Red Hat, Inc. 2008
 *
 * Authors:
 *  Anthony Liguori   <aliguori@us.ibm.com>
 *  Glauber Costa     <gcosta@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include <string.h>

#include <libgen.h>

#include "kvminfocaps.h"

#define MISSING_EXE "kvm-caps-missing-nfd-plugin"
int main(int argc, char **argv)
{
    const char *exe = basename(argv[0]);
    int mode = (!strcmp(exe, MISSING_EXE)) ?KVM_INFO_SHOW_MISSING :KVM_INFO_SHOW_FEATURES;
    return KVMStateScan(stdout, mode);
}
