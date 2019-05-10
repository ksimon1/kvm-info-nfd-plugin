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

#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/kvm.h>

#include "hyperv-proto.h"

#include "kvminfocaps.h"

enum {
    CAP_NAME_MAX = 1024
};

enum {
    KVMINFO_MSR_HV_DUMMY = 0,
    KVMINFO_MSR_HV_CRASH,
    KVMINFO_MSR_HV_RESET,
    KVMINFO_MSR_HV_VPINDEX,
    KVMINFO_MSR_HV_RUNTIME,
    KVMINFO_MSR_HV_SYNIC,
    KVMINFO_MSR_HV_STIMER,
    KVMINFO_MSR_HV_FREQUENCIES,
    KVMINFO_MSR_HV_REENLIGHTENMENT,
    // MUST be the last
    KVMINFO_MSR_HV_NUM
};

typedef struct KVMState KVMState;
struct KVMState {
    int fd;
    int msrs[KVMINFO_MSR_HV_NUM];
};

int KVMStateOpen(KVMState *s, const char *devkvm);
int KVMStateClose(KVMState *s);
static int KVMGetSupportedMSRs(KVMState *s);

int kvm_open(const char *dev);
int kvm_close(int fd);
int kvm_has_extension(int fd, unsigned int extension);
struct kvm_msr_list *kvm_get_msr_list(int fd);

int KVMStateOpen(KVMState *s, const char *devkvm)
{
    if (s == NULL) {
        return -1;
    }
    s->fd = kvm_open(devkvm);
    if (s->fd < 0) {
        return -1;
    }
    return KVMGetSupportedMSRs(s);
}

int KVMStateClose(KVMState *s)
{
    int err = 0;
    if (s == NULL) {
        return -1;
    }
    if (s->fd != -1) {
        err = kvm_close(s->fd);
        if (err == 0) {
            s->fd = -1;
        }
    }
    return err;
}

int KVMStateHasEnabledMSR(KVMState *s, unsigned int msr)
{
    if (s == NULL || msr > KVMINFO_MSR_HV_NUM) {
        return 0;
    }
    return s->msrs[msr];
}

static int KVMGetSupportedMSRs(KVMState *s)
{
    int ret = -1;
    struct kvm_msr_list *kvm_msr_list = kvm_get_msr_list(s->fd);
    if (kvm_msr_list != NULL) {
        int i;

        for (i = 0; i < kvm_msr_list->nmsrs; i++) {
            switch (kvm_msr_list->indices[i]) {
            case HV_X64_MSR_CRASH_CTL:
                s->msrs[KVMINFO_MSR_HV_CRASH] = 1;
                break;
            case HV_X64_MSR_RESET:
                s->msrs[KVMINFO_MSR_HV_RESET] = 1;
                break;
            case HV_X64_MSR_VP_INDEX:
                s->msrs[KVMINFO_MSR_HV_VPINDEX] = 1;
                break;
            case HV_X64_MSR_VP_RUNTIME:
                s->msrs[KVMINFO_MSR_HV_RUNTIME] = 1;
                break;
            case HV_X64_MSR_SCONTROL:
                s->msrs[KVMINFO_MSR_HV_SYNIC] = 1;
                break;
            case HV_X64_MSR_STIMER0_CONFIG:
                s->msrs[KVMINFO_MSR_HV_STIMER] = 1;
                break;
            case HV_X64_MSR_TSC_FREQUENCY:
                s->msrs[KVMINFO_MSR_HV_FREQUENCIES] = 1;
                break;
            case HV_X64_MSR_REENLIGHTENMENT_CONTROL:
                s->msrs[KVMINFO_MSR_HV_REENLIGHTENMENT] = 1;
                break;
            }
        }
    
        free(kvm_msr_list);
        ret = 0;
    }
    return ret;
}

typedef struct capdesc capdesc;
struct capdesc {
    int need_extension;
    unsigned int extension;
    int need_msr;
    unsigned int msr;
    const char *name;
};

#define KVMINFO_CAPS_MAX 16

static const capdesc allcaps[KVMINFO_CAPS_MAX] = {
    {
        .need_extension = 1,
        .extension = KVM_CAP_HYPERV,
        .name = "base",
    },
    {
        .need_extension = 1,
        .extension = KVM_CAP_HYPERV_TIME,
        .name = "time",
    },
    {
        .need_extension = 1,
        .extension = KVM_CAP_HYPERV_VP_INDEX,
        .need_msr = 1,
        .msr = KVMINFO_MSR_HV_VPINDEX,
        .name = "vpindex",
	},
    {
        .need_extension = 1,
        .extension = KVM_CAP_HYPERV_TLBFLUSH,
        .name = "tlbflush",
    },
    {
        .need_extension = 1,
        .extension = KVM_CAP_HYPERV_SEND_IPI,
        .name = "ipi",
	},
    {
        .need_extension = 1,
        .extension = KVM_CAP_HYPERV_SYNIC,
        .need_msr = 1,
        .msr = KVMINFO_MSR_HV_SYNIC,
        .name = "synic",
    },
    {
        .need_extension = 1,
        .extension = KVM_CAP_HYPERV_SYNIC2,
        .need_msr = 1,
        .msr = KVMINFO_MSR_HV_SYNIC,
        .name = "synic2",
    },
    {
        .need_msr = 1,
        .msr = KVMINFO_MSR_HV_FREQUENCIES,
        .name = "frequencies",
    },
    {
        .need_msr = 1,
        .msr = KVMINFO_MSR_HV_RESET,
        .name = "reset",
    },
    {
        .need_msr = 1,
        .msr = KVMINFO_MSR_HV_RUNTIME,
        .name = "runtime",
    },
    {
        .need_msr = 1,
        .msr = KVMINFO_MSR_HV_STIMER,
        .name = "synictimer",
    },
    {
        .need_msr = 1,
        .msr = KVMINFO_MSR_HV_REENLIGHTENMENT,
        .name = "reenlightenment",
    }
};

static int must_emit_label(int enabled, int mode)
{
    if (mode == KVM_INFO_SHOW_FEATURES) {
        return enabled;
    }
    if (mode == KVM_INFO_SHOW_MISSING) {
        return !enabled;
    }
    return 0; // fallback; never reached
}

int KVMStateScan(const char *dev, KVMEmitCap emit, void *ud, int mode)
{
    int ix, err;
    KVMState s = {
        .fd = -1,
    };

    err = KVMStateOpen(&s, dev);
    if (err) {
        return 0;
    }

    const char *suffix = (mode == KVM_INFO_SHOW_MISSING) ?"=false" :"";
    for (ix = 0; ix < KVMINFO_CAPS_MAX; ix++) {
        int msr_ok = 1, ext_ok = 1;
        const capdesc *cap = &allcaps[ix];
        if (!cap->need_msr && !cap->need_extension) {
            break;
        }

        if (cap->need_msr) {
            msr_ok = KVMStateHasEnabledMSR(&s, cap->msr);
        }
        if (cap->need_extension) {
            ext_ok = kvm_has_extension(s.fd, cap->extension);
        }
        if (must_emit_label(msr_ok && ext_ok, mode)) {
            char capbuf[CAP_NAME_MAX] = { '\0' };
            snprintf(capbuf, sizeof(capbuf), "/kvm-info-cap-hyperv-%s%s\n", cap->name, suffix);
            emit(ud, capbuf);
        }
    }

    KVMStateClose(&s); // who cares about failures now?
    return 0;
}


#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

struct kvm_msr_list *kvm_alloc_msr_list(uint32_t nmsrs)
{
    size_t msr_size = 0;
    struct kvm_msr_list msr_list, *kvm_msr_list;

    /* Old kernel modules had a bug and could write beyond the provided
       memory. Allocate at least a safe amount of 1K. */
    msr_size = MAX(1024, sizeof(msr_list) + nmsrs * sizeof(msr_list.indices[0]));
    kvm_msr_list = calloc(1, msr_size);
    if (kvm_msr_list == NULL) {
        return NULL;
    }
    kvm_msr_list->nmsrs = nmsrs;
    return kvm_msr_list;
}
