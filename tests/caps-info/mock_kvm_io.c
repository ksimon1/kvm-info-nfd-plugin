/*
 */

#include <errno.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <linux/kvm.h>

#include "toml.h"


// we should never need more than an handful of files, so this is overshoot by
// large
enum {
    ERR_BUF_SIZE = 256,
    MOCK_FILE_MAX = 256
};
static toml_table_t *files[MOCK_FILE_MAX];

static int find_first_free()
{
    int j;
    for (j = 0; j < MOCK_FILE_MAX; j++) {
        if (files[j] == NULL) {
            return j;
        }
    }
    return -1;
}

int kvm_open(const char *dev)
{
    char errbuf[ERR_BUF_SIZE] = { '\0' };
    FILE *src = NULL;
    int fd = find_first_free();
    if (fd < 0) {
        fprintf(stderr, "[%s]: free FD not found\n", __FILE__);
        return fd;
    }
    src = fopen(dev, "rt");
    if (src == NULL) {
        fprintf(stderr, "[%s]: fopen(%s) failed: %s\n", __FILE__, dev, strerror(errno));
        return -1;
    }
    files[fd] = toml_parse_file(src, errbuf, sizeof(errbuf));
    if (files[fd] == NULL) {
        fprintf(stderr, "[%s] toml_parse_file() failed: %s\n", __FILE__, errbuf);
        fd = -1;
    }
    fclose(src); /* we need to do this anyway, and we don't really care about the outcome */
    return fd;
}

int kvm_close(int fd)
{
    if (fd < 0 || fd > MOCK_FILE_MAX) {
        fprintf(stderr, "[%s]: close() failed: %d out of range\n", __FILE__, fd);
        return -1;
    }
    if (files[fd] != NULL) {
        toml_free(files[fd]);
        files[fd] = NULL;
    }
    return 0;
}

static toml_array_t *get_array(int fd, const char *key)
{
    char kind;
    toml_array_t* ret = NULL;

    if (fd < 0 || fd > MOCK_FILE_MAX) {
        fprintf(stderr, "[%s]: close() failed: %d out of range\n", __FILE__, fd);
        return NULL;
    }
    ret = toml_array_in(files[fd], key);
    if (ret == NULL) {
        fprintf(stderr, "[%s]: \"%s\" array not found configuration\n", __FILE__, key);
        return NULL;
    }
    kind = toml_array_kind(ret);
    if (kind != 'v' && kind != '\0') {
        /* '\0' -> empty array */
        fprintf(stderr, "[%s]: \"%s\" array has wrong kind '%c' (expected 'v')\n", __FILE__, key, kind);
        return NULL;
    }
    return ret;
}

int kvm_has_extension(int fd, unsigned int extension)
{
    toml_array_t* exts = get_array(fd, "exts");
    const char *raw = NULL;
    int j = 0;


    if (exts == NULL) {
        return 0;
    }

    for (j = 0; 0 != (raw = toml_raw_at(exts, j)); j++) {
        int64_t v = 0;
        int err = toml_rtoi(raw, &v);
        if (err) {
            fprintf(stderr, "[%s]: %s: toml_rtoi failed err=%d\n", __FILE__, __func__, err);
            return 0;
        }
        if ((int64_t)extension == v) {
            return 1;
        }
    }
    return 0;
}

extern struct kvm_msr_list *kvm_alloc_msr_list(uint32_t nmsrs);
struct kvm_msr_list *kvm_get_msr_list(int fd)
{
    struct kvm_msr_list *kvm_msr_list = NULL;
    toml_array_t* msrs = get_array(fd, "msrs");
    const char *raw = NULL;
    uint32_t nmsrs = 0;
    int j = 0;

    if (msrs != NULL) {
        nmsrs = (uint32_t)toml_array_nelem(msrs);
    }
    kvm_msr_list = kvm_alloc_msr_list((int)nmsrs);
    for (j = 0; msrs != NULL && 0 != (raw = toml_raw_at(msrs, j)); j++) {
        int64_t v = 0;
        int err = toml_rtoi(raw, &v);
        if (err) {
            fprintf(stderr, "[%s]: %s: toml_rtoi failed err=%d\n", __FILE__, __func__, err);
            return 0;
        }
        kvm_msr_list->indices[j] = (uint32_t)v;
    }
    return kvm_msr_list;
}
