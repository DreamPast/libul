#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "ulfd.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int err;
    ulfd_dir_t dir;
    const char* path;

    if(argc > 1) {
        int err = ulfd_chdir(argv[1]);
        if(err) {
            fprintf(stderr, "[ERROR] %s\n", strerror(err));
            exit(1);
        }
    }

#ifdef _WIN32
    if(!SetConsoleOutputCP(65001)) {
        fputs("cannot set UTF-8 in Windows", stderr);
        exit(1);
    }
#endif

    err = ulfd_opendir(&dir, ".");
    if(err) {
        fprintf(stderr, "[ERROR] %s\n", strerror(err));
        exit(1);
    }

    for(;;) {
        err = ulfd_readdir(&dir, &path);
        if(err) {
            fprintf(stderr, "[ERROR] %s\n", strerror(err));
            exit(1);
        }
        if(path == NULL) break;
        puts(path);
    }
    
    err = ulfd_closedir(&dir);
    if(err) {
        fprintf(stderr, "[ERROR] %s\n", strerror(err));
        exit(1);
    }
    return 0;
}
