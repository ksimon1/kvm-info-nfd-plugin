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
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/kvm.h>


static int kvm_ioctl(int fd, int type, ...)
{
    int ret;
    void *arg;
    va_list ap;

    va_start(ap, type);
    arg = va_arg(ap, void *);
    va_end(ap);

    ret = ioctl(fd, type, arg);
    if (ret == -1) {
        ret = -errno;
    }
    return ret;
}

int kvm_open(const char *dev)
{
    return open(dev, O_RDWR);
}

int kvm_close(int fd)
{
    return close(fd);
}

int kvm_has_extension(int fd, unsigned int extension)
{
    int ret = -1;

    ret = kvm_ioctl(fd, KVM_CHECK_EXTENSION, extension);
    if (ret < 0) {
        ret = 0;
    }

    return ret;
}

extern struct kvm_msr_list *kvm_alloc_msr_list(uint32_t nmsrs);

struct kvm_msr_list *kvm_get_msr_list(int fd)
{
    int ret = -1;
    struct kvm_msr_list msr_list, *kvm_msr_list;

    /* Obtain MSR list from KVM.  These are the MSRs that we must
     * save/restore */
    msr_list.nmsrs = 0;
    ret = kvm_ioctl(fd, KVM_GET_MSR_INDEX_LIST, &msr_list);
    if (ret < 0 && ret != -E2BIG) {
        return NULL;
    }

    kvm_msr_list = kvm_alloc_msr_list(msr_list.nmsrs);
    if (kvm_msr_list == NULL) {
        return NULL;
    }

    ret = kvm_ioctl(fd, KVM_GET_MSR_INDEX_LIST, kvm_msr_list);
    if (ret < 0) {
        free(kvm_msr_list);
        kvm_msr_list = NULL;
    }
    return kvm_msr_list;
}
