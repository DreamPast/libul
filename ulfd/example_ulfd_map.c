#include "ulfd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    ulfd_t fd;
    int err;
    void* map;
    ulfd_int64_t length;

    err = ulfd_open(&fd, "example_ulfd_map.c", ULFD_O_RDONLY, 0664);
    if(err) {
        fprintf(stderr, "fail to open: %s\n", strerror(err)); exit(1);
    }

    err = ulfd_filelength(fd, &length);
    if(err) {
        fprintf(stderr, "fail to get length: %s\n", strerror(err)); exit(1);
    }

    err = ulfd_mmap(&map, fd, NULL, (size_t)length, 0, ULFD_PROT_READ);
    if(err) {
        fprintf(stderr, "fail to map: %s\n", strerror(err)); exit(1);
    }

    fwrite(map, 1, (size_t)length, stdout);
    err = ulfd_munmap(map, (size_t)length);
    if(err) {
        fprintf(stderr, "fail to umap: %s\n", strerror(err)); exit(1);
    }

    err = ulfd_close(fd);
    if(err) {
        fprintf(stderr, "fail to close: %s\n", strerror(err)); exit(1);
    }
    return 0;
}
