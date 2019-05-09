/*
 * code extracted from the
 * QEMU KVM support
 *
 * Copyright (C) 2006-2008 Qumranet Technologies
 * Copyright IBM, Corp. 2008
 *
 * Authors:
 *  Anthony Liguori   <aliguori@us.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#ifndef KVM_INFO_CAPS_H
#define KVM_INFO_CAPS_H

typedef struct KVMState KVMState;

int KVMStateOpen(KVMState *s, const char *devkvm);
int KVMStateClose(KVMState *s);
int KVMStateHasExtension(KVMState *s, unsigned int extension);

enum {
    KVM_INFO_SHOW_FEATURES = 0,
    KVM_INFO_SHOW_MISSING = 1,
};

typedef int (*KVMEmitCap)(void *ud, const char *cap);

int KVMStateScan(KVMEmitCap emit, void *ud, int mode);

#endif /* KVM_INFO_CAPS_H */
