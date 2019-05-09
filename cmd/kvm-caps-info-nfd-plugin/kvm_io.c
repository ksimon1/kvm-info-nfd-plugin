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
#include <stdlib.h>

#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/kvm.h>


#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif


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

struct kvm_msr_list *kvm_get_msr_list(int fd)
{
    int ret = -1;
    size_t msr_size = 0;
    struct kvm_msr_list msr_list, *kvm_msr_list;

    /* Obtain MSR list from KVM.  These are the MSRs that we must
     * save/restore */
    msr_list.nmsrs = 0;
    ret = kvm_ioctl(fd, KVM_GET_MSR_INDEX_LIST, &msr_list);
    if (ret < 0 && ret != -E2BIG) {
        return NULL;
    }
    /* Old kernel modules had a bug and could write beyond the provided
       memory. Allocate at least a safe amount of 1K. */
    msr_size = MAX(1024, sizeof(msr_list) + msr_list.nmsrs * sizeof(msr_list.indices[0]));
    kvm_msr_list = calloc(1, msr_size);
    if (kvm_msr_list == NULL) {
        return NULL;
    }

    kvm_msr_list->nmsrs = msr_list.nmsrs;

    ret = kvm_ioctl(fd, KVM_GET_MSR_INDEX_LIST, kvm_msr_list);
    if (ret < 0) {
        free(kvm_msr_list);
        kvm_msr_list = NULL;
    }
    return kvm_msr_list;
}
