#include "ulfd.h"

int main() {
    puts("ulfd_info");

#ifdef _WIN32
    puts("System: Windows");
    #if !defined(_WIN32_WINNT)
        puts("Target: Unkown");
    #elif (_WIN32_WINNT+0) >= 0x0602
        puts("Target: >= Windows 8");
    #elif (_WIN32_WINNT+0) >= 0x0600
        puts("Target: >= Windows Vista");
    #elif (_WIN32_WINNT+0) >= 0x0501
        puts("Target: >= Windows XP");
    #endif
#else
    puts("System: POSIX");
    #ifdef _POSIX_C_SOURCE
        printf("_POSIX_C_SOURCE: %ld\n", ul_static_cast(long, (_POSIX_C_SOURCE+0)));
    #endif
    #ifdef _POSIX_SOURCE
        printf("_POSIX_SOURCE: %ld\n", ul_static_cast(long, (_POSIX_SOURCE+0)));
    #endif
    #ifdef _XOPEN_SOURCE
        printf("_XOPEN_SOURCE: %ld\n", ul_static_cast(long, (_XOPEN_SOURCE+0)));
    #endif
    #ifdef _BSD_SOURCE
        printf("_BSD_SOURCE: %ld\n", ul_static_cast(long, (_BSD_SOURCE+0)));
    #endif
    #ifdef _ATFILE_SOURCE
        printf("_ATFILE_SOURCE: %ld\n", ul_static_cast(long, (_ATFILE_SOURCE+0)));
    #endif
    #ifdef _DEFAULT_SOURCE
        printf("_DEFAULT_SOURCE: %ld\n", ul_static_cast(long, (_DEFAULT_SOURCE+0)));
    #endif
    #ifdef _SVID_SOURCE
        printf("_SVID_SOURCE: %ld\n", ul_static_cast(long, (_SVID_SOURCE+0)));
    #endif
    #if defined(__GLIBC__) && defined(_GNU_SOURCE)
        printf("_GNU_SOURCE: %ld\n", ul_static_cast(long, (_GNU_SOURCE+0)));
    #endif
#endif

    puts("Macros:");
#ifdef ULFD_NO_CHOWN
    puts("\tULFD_NO_CHOWN");
#endif
#ifdef ULFD_NO_LCHOWN
    puts("\tULFD_NO_LCHOWN");
#endif
#ifdef ULFD_NO_FCHOWN
    puts("\tULFD_NO_FCHOWN");
#endif
#ifdef ULFD_NO_LINK
    puts("\tULFD_NO_LINK");
#endif
#ifdef ULFD_NO_SYMLINK
    puts("\tULFD_NO_SYMLINK");
#endif
#ifdef ULFD_NO_READLINK
    puts("\tULFD_NO_READLINK");
#endif
#ifdef ULFD_NO_LCHMOD
    puts("\tULFD_NO_LCHMOD");
#endif
#ifdef ULFD_NO_FCHMOD
    puts("\tULFD_NO_FCHMOD");
#endif
#ifdef ULFD_NO_FUTIME
    puts("\tULFD_NO_FUTIME");
#endif
#ifdef ULFD_NO_FSTAT
    puts("\tULFD_NO_FSTAT");
#endif
#ifdef ULFD_NO_FTYPE
    puts("\tULFD_NO_FTYPE");
#endif
#ifdef ULFD_NO_PREAD
    puts("\tULFD_NO_PREAD");
#endif
#ifdef ULFD_NO_PWRITE
    puts("\tULFD_NO_PWRITE");
#endif
#ifdef ULFD_NO_COPY_FILE_RANGE
    puts("\tULFD_NO_COPY_FILE_RANGE");
#endif
#ifdef ULFD_NO_FSYNC
    puts("\tULFD_NO_FSYNC");
#endif
#ifdef ULFD_NO_FFULLSYNC
    puts("\tULFD_NO_FFULLSYNC");
#endif
#ifdef ULFD_NO_FDATASYNC
    puts("\tULFD_NO_FDATASYNC");
#endif
#ifdef ULFD_NO_FTRUNCATE
    puts("\tULFD_NO_FTRUNCATE");
#endif
#ifdef ULFD_NO_FDOPEN
    puts("\tULFD_NO_FDOPEN");
#endif
#ifdef ULFD_NO_FILENO
    puts("\tULFD_NO_FILENO");
#endif
#ifdef ULFD_NO_TRUNCATE
    puts("\tULFD_NO_TRUNCATE");
#endif
#ifdef ULFD_NO_LUTIME
    puts("\tULFD_NO_LUTIME");
#endif
#ifdef ULFD_NO_LSTAT
    puts("\tULFD_NO_LSTAT");
#endif
#ifdef ULFD_NO_LTYPE
    puts("\tULFD_NO_LTYPE");
#endif
#ifdef ULFD_NO_REALPATH
    puts("\tULFD_NO_REALPATH");
#endif

    return 0;
}