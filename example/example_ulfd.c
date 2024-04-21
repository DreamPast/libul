#include "ulfd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    do {
        ulfd_t fd;
        int err;

        remove("temp.txt");
        err = ulfd_open(&fd, "temp.txt", ULFD_O_RDWR, 0664);
        if(err != ENOENT) {
            fprintf(stderr, "[ERROR] open existing file: %s\n", strerror(err));
            goto failed1;
        }

        err = ulfd_open(&fd, "temp.txt", ULFD_O_RDWR | ULFD_O_CREAT, 0664);
        if(err != 0) {
            fprintf(stderr, "[ERROR] create new file: %s\n", strerror(err));
            goto failed1;
        }
        err = ulfd_close(fd);
        if(err) {
            fprintf(stderr, "[ERROR] close file: %s\n", strerror(err)); goto failed1;
        }

        err = ulfd_open(&fd, "temp.txt", ULFD_O_RDWR | ULFD_O_CREAT | ULFD_O_EXCL, 0664);
        if(err != EEXIST) {
            fprintf(stderr, "[ERROR] create new execulsive file: %s\n", strerror(err));
            goto failed1;
        }

        remove("temp.txt");
        printf("[SUCCESS] test ULFD_O_CREAT, ULFD_O_EXCL\n");
        continue;
    failed1:
        remove("temp.txt");
        exit(1);
    } while(0);

    do {
        ulfd_t rfd, wfd;
        int err;
        size_t readed, writen;
        ulfd_int64_t length;
        char* buf = 0;

        err = ulfd_open(&rfd, "example_ulfd.c", ULFD_O_RDONLY, 0664);
        if(err) {
            fprintf(stderr, "[ERROR] open \"%s\" for reading: %s\n", "example_ulfd.c", strerror(err)); exit(1);
        }
        err = ulfd_open(&wfd, "example_ulfd.c.out", ULFD_O_WRONLY | ULFD_O_CREAT | ULFD_O_TRUNC, 0664);
        if(err) {
            fprintf(stderr, "[ERROR] open \"%s\" for writing: %s\n", "example_ulfd.c.out", strerror(err)); exit(1);
        }

        err = ulfd_write(&rfd, &buf, 4, &readed);
        if(err != EBADF) {
            fprintf(stderr, "[ERROR] write read-only file: %s\n", strerror(err)); goto failed2;
        }
        err = ulfd_filelength(rfd, &length);
        if(err) {
            fprintf(stderr, "[ERROR] get file length: %s\n", strerror(err)); goto failed2;
        }
        buf = (char*)malloc(length);
        if(!buf) { fprintf(stderr, "[ERROR] allocate memmory\n"); goto failed2; }
        err = ulfd_read(rfd, buf, length, &readed);
        if(err != 0) {
            fprintf(stderr, "[ERROR] ulfd_read(): %s\n", strerror(err)); goto failed2;
        }
        if((size_t)(length) != readed) {
            fprintf(stderr, "[ERROR] ulfd_read() reads some: %d / %d", (int)readed, (int)length); goto failed2;
        }

        err = ulfd_pread(rfd, buf, length, 0, &readed);
        if(err != 0) {
            fprintf(stderr, "[ERROR] ulfd_pread(): %s\n", strerror(err)); goto failed2;
        }
        if((size_t)(length) != readed) {
            fprintf(stderr, "[ERROR] ulfd_pread() reads some: %d / %d", (int)readed, (int)length); goto failed2;
        }

        err = ulfd_write(wfd, buf, length, &writen);
        if(err != 0) {
            fprintf(stderr, "[ERROR] ulfd_write(): %s\n", strerror(err)); goto failed2;
        }
        if(readed != writen) {
            fprintf(stderr, "[ERROR] ulfd_write() writes some: %d / %d", (int)writen, (int)readed); goto failed2;
        }

        err = ulfd_pwrite(wfd, buf, length, 0, &writen);
        if(err != 0) {
            fprintf(stderr, "[ERROR] ulfd_pwrite(): %s\n", strerror(err)); goto failed2;
        }
        if(readed != writen) {
            fprintf(stderr, "[ERROR] ulfd_pwrite() writes some: %d / %d", (int)writen, (int)readed); goto failed2;
        }
        
        free(buf);
        err = ulfd_close(rfd);
        if(err) {
            fprintf(stderr, "[ERROR] close file: %s\n", strerror(err)); goto failed2;
        }
        err = ulfd_close(wfd);
        if(err) {
            fprintf(stderr, "[ERROR] close file: %s\n", strerror(err)); goto failed2;
        }
        remove("example_ulfd.c.out");
        printf("[SUCCESS] test ulfd_filelength(), ulfd_read(), ulfd_pread(), ulfd_write(), ulfd_write()\n");
    
    failed2:
        remove("example_ulfd.c.out");
        exit(1);
    } while(0);

    return 0;
}
