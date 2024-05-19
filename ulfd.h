/*
File descriptor (Unix style)


# Dependence
  - Windows API
  - POSIX API


# Note
  All functions use 64-bit integer to indicate offset and size.
  In POSIX, if `_LARGEFILE64_SOURCE` is not defined, functions will test if `off_t` can hold offset and size,
    which means you can also setting `_FILE_OFFSET_BITS` to 64 to make functions work right.

  This header will define following macros to indicate that some funtions **MAY BE** invalid.
  Note: All functions are always defined, and when they're invalid, they will always return `ENOSYS`.
  Note: Even if the macros are defined, some functions **MAY** still work successfully (especially in Windows).
  | Macro                   | Functions                                                    |
  | ----------------------- | ------------------------------------------------------------ |
  | ULFD_NO_CHOWN           | ulfd_chown, ulfd_chown_w, ulfd_chown_u                       |
  | ULFD_NO_LCHOWN          | ulfd_lchown, ulfd_lchown_w, ulfd_lchown_w                    |
  | ULFD_NO_FCHOWN          | ulfd_fchown                                                  |
  | ULFD_NO_LINK            | ulfd_link, ulfd_link_w, ulfd_link_u                          |
  | ULFD_NO_SYMLINK         | ulfd_symlink, ulfd_symlink_w, ulfd_symlink_u                 |
  | ULFD_NO_READLINK        | ulfd_readlink, ulfd_readlink_w, ulfd_readlink_u              |
  | ULFD_NO_READLINK        | ulfd_readlink_alloc, ulfd_readlink_alloc_w, ulfd_readlink_alloc_u |
  | ULFD_NO_LCHMOD          | ulfd_lchmod, ulfd_lchmod_w, ulfd_lchmod_u                    |
  | ULFD_NO_FCHMOD          | ulfd_fchmod                                                  |
  | ULFD_NO_FUTIME          | ulfd_futime                                                  |
  | ULFD_NO_FSTAT           | ulfd_fstat                                                   |
  | ULFD_NO_FTYPE           | ulfd_ftype                                                   |
  | ULFD_NO_PREAD           | ulfd_pread                                                   |
  | ULFD_NO_PWRITE          | ulfd_pwrite                                                  |
  | ULFD_NO_COPY_FILE_RANGE | ulfd_copy_file_range                                         |
  | ULFD_NO_FSYNC           | ulfd_fsync                                                   |
  | ULFD_NO_FFULLSYNC       | ulfd_ffullsync                                               |
  | ULFD_NO_FDATASYNC       | ulfd_fdatasync                                               |
  | ULFD_NO_FTRUNCATE       | ulfd_ftruncate                                               |
  | ULFD_NO_FDOPEN          | ulfd_fdopen, ulfd_fdopen_w, ulfd_fdopen_u                    |
  | ULFD_NO_FILENO          | ulfd_fileno                                                  |
  | ULFD_NO_TRUNCATE        | ulfd_truncate, ulfd_truncate_w, ulfd_truncate_u              |
  | ULFD_NO_LUTIME          | ulfd_lutime, ulfd_lutime_w, ulfd_lutime_u                    |
  | ULFD_NO_LSTAT           | ulfd_lstat, ulfd_lstat_w, ulfd_lstat_u                       |
  | ULFD_NO_LTYPE           | ulfd_ltype, ulfd_ltype_w, ulfd_ltype_u                       |
  | ULFD_NO_REALPATH        | ulfd_realpath_alloc, ulfd_realpath_alloc_w, ulfd_realpath_alloc_u |


# License
  The MIT License (MIT)

  Copyright (C) 2023-2024 Jin Cai

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef ULFD_H
#define ULFD_H

#ifdef __has_builtin
  #if __has_builtin(__builtin_expect)
    #ifndef ul_likely
      #define ul_likely(x) __builtin_expect(!!(x), 1)
    #endif
    #ifndef ul_unlikely
      #define ul_unlikely(x) __builtin_expect(!!(x), 0)
    #endif
  #endif
#endif
#ifndef ul_likely
  #define ul_likely(x) (x)
#endif /* ul_likely */
#ifndef ul_unlikely
  #define ul_unlikely(x) (x)
#endif /* ul_unlikely */

#ifndef ul_unused
  #if (defined(__GNUC__) && __GNUC__ >= 3) || defined(__clang__)
    #define ul_unused __attribute__((unused))
  #elif defined(__cplusplus) && defined(__has_cpp_attribute)
    #if __has_cpp_attribute(maybe_unused)
      #define ul_unused [[maybe_unused]]
    #endif
  #endif
  #ifndef ul_unused
    #define ul_unused
  #endif
#endif /* ul_unused */

#ifndef ul_inline
  #if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
    #define ul_inline inline
  #else
    #define ul_inline
  #endif
#endif /* ul_inline */

#ifndef ul_hapi
  #define ul_hapi ul_unused static ul_inline
#endif /* ul_hapi */

#ifndef ul_nodiscard
  #if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(nodiscard)
      #define ul_nodiscard [[nodiscard]]
    #endif
  #endif
  #if !defined(ul_nodiscard) && defined(__has_attribute)
    #if __has_attribute(warn_unused_result)
      #define ul_nodiscard __attribute__((warn_unused_result))
    #endif
  #endif
  #if !defined(ul_nodiscard) && defined(_HAS_NODISCARD)
    #define ul_nodiscard _NODISCARD
  #endif
  #if !defined(ul_nodiscard) && defined(_MSC_VER) && _MSC_VER >= 1700
    #define ul_nodiscard _Check_return_
  #endif
  #ifndef ul_nodiscard
    #define ul_nodiscard
  #endif
#endif /* ul_nodiscard */

#ifndef ul_reinterpret_cast
  #ifdef __cplusplus
    #define ul_reinterpret_cast(T, val) reinterpret_cast<T>(val)
  #else
    #define ul_reinterpret_cast(T, val) ((T)(val))
  #endif
#endif /* ul_reinterpret_cast */

#ifndef ul_static_cast
  #ifdef __cplusplus
    #define ul_static_cast(T, val) static_cast<T>(val)
  #else
    #define ul_static_cast(T, val) ((T)(val))
  #endif
#endif /* ul_static_cast */

#ifndef ul_const_cast
  #ifdef __cplusplus
    #define ul_const_cast(T, val) const_cast<T>(val)
  #else
    #define ul_const_cast(T, val) ((T)(val))
  #endif
#endif /* ul_const_cast */

#ifndef ul_malloc
  #define ul_malloc(sz) malloc(sz)
#endif /* ul_malloc */
#ifndef ul_realloc
  #define ul_realloc(ptr, sz) realloc(ptr, sz)
#endif /* ul_realloc */
#ifndef ul_free
  #define ul_free(ptr) free(ptr)
  /* In POSIX, if default `free` is used, we won't copy string allocated by C library */
  #define UL_FREE_DEFAULT
#endif /* ul_free */

#ifdef _LARGEFILE64_SOURCE
  #define ULFD_HAS_LFS
#endif

#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>

#if defined(ULLONG_MAX) && ((ULLONG_MAX >> 63) >= 1)
  typedef long long ulfd_int64_t;
  typedef unsigned long long ulfd_uint64_t;
  #define ULFD_INT64_C(val) (val ## ll)
#elif defined(_WIN32)
  typedef __int64 ulfd_int64_t;
  typedef unsigned __int64 ulfd_uint64_t;
  #define ULFD_INT64_C(val) (val ## i64)
#else
  #error "ulfd.h: need 64-bit integer"
#endif

#if INT_MAX == 0x7FFFFFFF
  typedef int ulfd_int32_t;
#elif LONG_MAX == 0x7FFFFFFF
  typedef long ulfd_int32_t;
#elif SHRT_MAX == 0x7FFFFFFF
  typedef short ulfd_int32_t;
#else
  #error "ulfd.h: need 32-bit integer"
#endif

/* milliseconds from POSIX epoch (equivalte to `uldate_t` in "uldate.h") */
typedef ulfd_int64_t ulfd_time_t;

#ifdef _WIN32
  #include <Windows.h>

  typedef HANDLE ulfd_t;
  #define ULFD_INVALID_VALUE INVALID_HANDLE_VALUE

  #define ULFD_STDIN  (GetStdHandle(STD_INPUT_HANDLE))
  #define ULFD_STDOUT (GetStdHandle(STD_OUTPUT_HANDLE))
  #define ULFD_STDERR (GetStdHandle(STD_ERROR_HANDLE))

  #define ULFD_PATH_MAX     MAX_PATH
  #define ULFD_NAME_MAX     MAX_PATH
  #define ULFD_FILENAME_MAX MAX_PATH

  #define ULFD_PATH_SEP      "\\"
  #define ULFD_PATH_SEP_WIDE L"\\"

  typedef int ulfd_dev_t;
  typedef int ulfd_ino_t;
  typedef int ulfd_mode_t;
  typedef int ulfd_nlink_t;
  typedef int ulfd_uid_t;
  typedef int ulfd_gid_t;
#else
  #include <unistd.h>
  #include <sys/types.h>
  #include <limits.h>

  typedef int ulfd_t;
  #define ULFD_INVALID_VALUE -1

  #define ULFD_STDIN  STDIN_FILENO
  #define ULFD_STDOUT STDOUT_FILENO
  #define ULFD_STDERR STDERR_FILENO

  #ifdef PATH_MAX
    #define ULFD_PATH_MAX PATH_MAX
  #else
    #define ULFD_PATH_MAX 4096
  #endif
  #ifdef NAME_MAX
    #define ULFD_NAME_MAX NAME_MAX
  #else
    #define ULFD_NAME_MAX ULFD_PATH_MAX
  #endif
  #ifdef FILENAME_MAX
    #define ULFD_FILENAME_MAX FILENAME_MAX
  #else
    #define ULFD_FILENAME_MAX ULFD_PATH_MAX
  #endif

  #define ULFD_PATH_SEP      "/"
  #define ULFD_PATH_SEP_WIDE L"/"

  typedef dev_t ulfd_dev_t;
  #ifdef ULFD_HAS_LFS
    typedef ino64_t ulfd_ino_t;
  #else
    typedef ino_t ulfd_ino_t;
  #endif
  typedef ulfd_int32_t ulfd_mode_t;
  typedef nlink_t ulfd_nlink_t;
  typedef uid_t ulfd_uid_t;
  typedef gid_t ulfd_gid_t;
#endif


#define ULFD_S_IMASK   07777 /* a mask to get access of file */
#define ULFD_S_IALL    00777 /* read, write, execute permission for everyone */
#define ULFD_S_IRALL   00444 /* read permission for everyone */
#define ULFD_S_IWALL   00222 /* write permission for everyone */
#define ULFD_S_IXALL   00111 /* execute permission for everyone */

#define ULFD_S_ISUID   04000 /* POSIX: set-user-ID on execute bit */
#define ULFD_S_ISGID   02000 /* POSIX: set-group-ID on execute bit */
#define ULFD_S_ISVTX   01000 /* POSIX: the sticky bit */

#define ULFD_S_IRWXU   00700 /* equivalent to (ULFD_S_IRUSR | ULFD_S_IWUSR | ULFD_S_IXUSR) */
#define ULFD_S_IRUSR   00400 /* read permission bit for the owner of the file */
#define ULFD_S_IWUSR   00200 /* write permission bit for the owner of the file */
#define ULFD_S_IXUSR   00100 /* execute or ordinary permission bit for the owner of the file */

#define ULFD_S_IRWXG   00070 /* equivalent to (ULFD_S_IRGRP | ULFD_S_IWGRP | ULFD_S_IXGRP) */
#define ULFD_S_IRGRP   00040 /* read permission bit for the group owner of the file */
#define ULFD_S_IWGRP   00020 /* write permission bit for the group owner of the file */
#define ULFD_S_IXGRP   00010 /* execute or ordinary permission bit for the group owner of the file */

#define ULFD_S_IRWXO   00007  /* equivalent to (ULFD_S_IROTH | ULFD_S_IWOTH | ULFD_S_IXOTH) */
#define ULFD_S_IROTH   00004  /* read permission bit for other users */
#define ULFD_S_IWOTH   00002  /* write permission bit for other users */
#define ULFD_S_IXOTH   00001  /* execute or ordinary permission bit for other users */

#define ULFD_S_IREAD   ULFD_S_IRUSR /* read permission bit for the owner of the file */
#define ULFD_S_IWRITE  ULFD_S_IWUSR /* write permission bit for the owner of the file */
#define ULFD_S_IEXEC   ULFD_S_IXUSR /* execute or ordinary permission bit for the owner of the file */

#define ULFD_S_IHIDDEN 01000000 /* Windows: hidden file */

#define ULFD_S_IFMT    0170000 /* a mask to get type of file */
#define ULFD_S_IFSOCK  0140000 /* socket */
#define ULFD_S_IFLNK   0120000 /* symbolic link */
#define ULFD_S_IFREG   0100000 /* regular */
#define ULFD_S_IFBLK   0060000 /* block special */
#define ULFD_S_IFDIR   0040000 /* directory */
#define ULFD_S_IFCHR   0020000 /* character special */
#define ULFD_S_IFIFO   0010000 /* FIFO special */

#define ULFD_S_ISSOCK(val) ((val & ULFD_S_IFMT) == ULFD_S_IFSOCK) /* test for a socket */
#define ULFD_S_ISLNK(val)  ((val & ULFD_S_IFMT) == ULFD_S_IFLNK)  /* test for a symbolic link */
#define ULFD_S_ISREG(val)  ((val & ULFD_S_IFMT) == ULFD_S_IFREG)  /* test for a regular */
#define ULFD_S_ISBLK(val)  ((val & ULFD_S_IFMT) == ULFD_S_IFBLK)  /* test for a block special */
#define ULFD_S_ISDIR(val)  ((val & ULFD_S_IFMT) == ULFD_S_IFDIR)  /* test for a directory */
#define ULFD_S_ISFIFO(val) ((val & ULFD_S_IFMT) == ULFD_S_IFIFO)  /* test for a character special */
#define ULFD_S_ISCHR(val)  ((val & ULFD_S_IFMT) == ULFD_S_IFCHR)  /* test for a FIFO special */


#define ULFD_O_RDONLY    (1l << 0) /* open for reading only */
#define ULFD_O_WRONLY    (1l << 1) /* open for writing only */
#define ULFD_O_RDWR      (1l << 2) /* open for reading and writing */

#define ULFD_O_CREAT     (1l << 3) /* if the file doesn't exist, create the file */
#define ULFD_O_EXCL      (1l << 4) /* if `ULFD_O_CREAT` is set, fails if file exists */
#define ULFD_O_TRUNC     (1l << 5) /* truncate the file opened */

#define ULFD_O_APPEND    (1l << 6) /* set file offset to the end */
#define ULFD_O_TEMPORARY (1l << 7) /* when the file is closed, delete the file */
#define ULFD_O_CLOEXEC   (1l << 8) /* enable close-on-exec(In Windows, it will prevent inherting the file) */
#define ULFD_O_NOINHERIT ULFD_O_CLOEXEC /* prevent inherting the file(In POSIX, it will enable close-on-exec) */

/* POSIX: sets nonblocking I/O mode.
  (In windows, asynchronous IO needs to use different APIs, so it's ignored in Windows) */
#define ULFD_O_NONBLOCK  (1l << 16)
/* POSIX: prevent the OS from assigning the opened file
  as the process's controlling terminal when opening a TTY device file */
#define ULFD_O_NOCTTY    (1l << 17)

#define ULFD_O_DENYRD    (1l << 24) /* Windows: deny share read access */
#define ULFD_O_DENYWR    (1l << 25) /* Windows: deny share write access */
#define ULFD_O_DENYRW    (ULFD_O_DENYRD | ULFD_O_DENYWR) /* Windows: deny share read and write access */
#define ULFD_O_SECURE    (1l << 26) /* Winodws: set secure mode (share read access but exclude write access) */

ul_hapi int ulfd_open(ulfd_t* pfd, const char* path, ulfd_int32_t oflag, ulfd_mode_t mode);
ul_hapi int ulfd_open_w(ulfd_t* pfd, const wchar_t* wpath, ulfd_int32_t oflag, ulfd_mode_t mode);
ul_hapi int ulfd_creat(ulfd_t* pfd, const char* path, ulfd_mode_t mode);
ul_hapi int ulfd_creat_w(ulfd_t* pfd, const wchar_t* path, ulfd_mode_t mode);
ul_hapi int ulfd_close(ulfd_t fd);
ul_hapi int ulfd_pipe(ulfd_t pfds[2]);

ul_hapi int ulfd_read(ulfd_t fd, void* buf, size_t count, size_t* pread_bytes);
ul_hapi int ulfd_write(ulfd_t fd, const void* buf, size_t count, size_t* pwriten_bytes);
ul_hapi int ulfd_pread(ulfd_t fd, void* buf, size_t count, ulfd_int64_t off, size_t* pread_bytes);
ul_hapi int ulfd_pwrite(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes);

#define ULFD_SEEK_SET 0 /* seek to an absolute position */
#define ULFD_SEEK_CUR 1 /* seek relative to current position */
#define ULFD_SEEK_END 2 /* seek relative to end of the file */
ul_hapi int ulfd_seek(ulfd_t fd, ulfd_int64_t off, int origin, ulfd_int64_t* poff);
ul_hapi int ulfd_tell(ulfd_t fd, ulfd_int64_t* poff);

/* if `off_in` or `off_out` is NULL, use current pos */
ul_hapi int ulfd_copy_file_range(
  ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out,
  size_t len, size_t* pcopyed
);

ul_hapi int ulfd_pread_user(ulfd_t fd, void* buf, size_t count, ulfd_int64_t off, size_t* pread_bytes);
ul_hapi int ulfd_pwrite_user(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes);
/* if `off_in` or `off_out` is NULL, use current pos;
  if `buf` is NULL, the function will automatically allocate memory */
ul_hapi int ulfd_copy_file_range_user(
  ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out,
  size_t len, size_t* pcopyed, void* buf, size_t buf_len
);

ul_hapi int ulfd_pread_allowuser(ulfd_t fd, void* buf, size_t count, ulfd_int64_t off, size_t* pread_bytes);
ul_hapi int ulfd_pwrite_allowuser(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes);
/* if `off_in` or `off_out` is NULL, use current pos;
  if `buf` is NULL, the function will automatically allocate memory */
ul_hapi int ulfd_copy_file_range_allowuser(
  ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out,
  size_t len, size_t* pcopyed
);

ul_hapi int ulfd_ffullsync(ulfd_t fd);
ul_hapi int ulfd_fsync(ulfd_t fd);
ul_hapi int ulfd_fdatasync(ulfd_t fd);

#define ULFD_F_RDLCK 0 /* specify a read (or shared) lock */
#define ULFD_F_WRLCK 1 /* specify a write (or exclusive) lock */
#define ULFD_F_UNLCK 2 /* specify that the region is unlocked */
ul_hapi int ulfd_lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode);
ul_hapi int ulfd_lockw(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode); /* wait for that lock to be released */

ul_hapi int ulfd_ftruncate(ulfd_t fd, ulfd_int64_t length);
ul_hapi int ulfd_ffilelength(ulfd_t fd, ulfd_int64_t* plength);
ul_hapi int ulfd_fchmod(ulfd_t fd, ulfd_mode_t mode);
ul_hapi int ulfd_fchown(ulfd_t fd, ulfd_uid_t uid, ulfd_gid_t gid);
ul_hapi int ulfd_futime(ulfd_t fd, ulfd_int64_t atime, ulfd_int64_t mtime);

ul_hapi int ulfd_isatty(ulfd_t fd, int* presult);

#define ULFD_SET_STDIN  0
#define ULFD_SET_STDOUT 1
#define ULFD_SET_STDERR 2
ul_hapi int ulfd_setstd(int which, ulfd_t fd);

#include <stdio.h>
ul_hapi int ulfd_dup(ulfd_t* pnfd, ulfd_t ofd);
ul_hapi int ulfd_fdopen(FILE** pfp, ulfd_t fd, const char* mode);
ul_hapi int ulfd_fdopen_w(FILE** pfp, ulfd_t fd, const wchar_t* wmode);
ul_hapi int ulfd_fileno(ulfd_t* pfd, FILE* fp);


#define ULFD_PROT_NONE      (0)      /* pages may not be accessed */
#define ULFD_PROT_READ      (1 << 0) /* pages may be read */
#define ULFD_PROT_WRITE     (1 << 1) /* pages may be write */
#define ULFD_PROT_EXEC      (1 << 2) /* pages may be executed */
#define ULFD_PROT_READWRITE (ULFD_PROT_READ | ULFD_PROT_WRITE) /* pages may be read or write */

#define ULFD_MAP_SHARED     (1 << 3) /* share this mapping. otherwise, create a private copy-on-write mapping */
#define ULFD_MAP_ANONYMOUS  (1 << 4) /* the mapping isn't backed by any file */
#define ULFD_MAP_ANON       ULFD_MAP_ANONYMOUS
#define ULFD_MAP_FIXED      (1 << 5) /* place the mapping at exactly that address */

ul_hapi int ulfd_mmap(void** pmap, ulfd_t fd, void* addr, size_t len, ulfd_int64_t off, int prot_and_flags);
ul_hapi int ulfd_munmap(void* addr, size_t len);
ul_hapi int ulfd_mprotect(void* addr, size_t len, int prot);
#define ULFD_MS_ASYNC      1 /* POSIX: specifiy that an update be scheduled, but the call returns immediately */
#define ULFD_MS_SYNC       2 /* POSIX: request an update and waits for it to complete. */
#define ULFD_MS_INVALIDATE 4 /* POSIX: ask to invalidate other mappings of the same file */
ul_hapi int ulfd_msync(void* addr, size_t len, int flags);
/* note: some platforms may need `len` to be multiple of the page size */
ul_hapi int ulfd_mlock(const void* addr, size_t len);
ul_hapi int ulfd_munlock(const void* addr, size_t len);

#define ULFD_MADV_WILLNEED   3 /* expect access in the near future */
#define ULFD_MADV_DONTNEED   4 /* do not expect access in the near future */
#define ULFD_MADV_NORMAL     0 /* POSIX: no special treatment */
#define ULFD_MADV_RANDOM     1 /* POSIX: expect page references in random order */
#define ULFD_MADV_SEQUENTIAL 2 /* POSIX: expect page references in sequential order */
#define ULFD_MADV_DONTFORK   5 /* Linux: do not make the pages in this range available to the child after a fork */
#define ULFD_MADV_DOFORK     6 /* Linux: undo the effect of MADV_DONTFORK, restoring the default behavior */
#define ULFD_MADV_DODUMP     7 /* Linux: exclude from a core dump those pages specified */
#define ULFD_MADV_DONTDUMP   8 /* Linux: undo the effect of MADV_DONTDUMP */
ul_hapi int ulfd_madvise(void* addr, size_t len, int advice);

ul_hapi size_t ulfd_pagesize(void);
ul_hapi int ulfd_tmpdir_alloc(char** ppath);
ul_hapi int ulfd_tmpdir_alloc_w(wchar_t** pwpath);


ul_hapi int ulfd_chdir(const char* path);
ul_hapi int ulfd_chdir_w(const wchar_t* wpath);
ul_hapi int ulfd_getcwd(char* path, size_t length);
ul_hapi int ulfd_getcwd_w(wchar_t* wpath, size_t length);
ul_hapi int ulfd_getcwd_alloc(char** ppath);
ul_hapi int ulfd_getcwd_alloc_w(wchar_t** pwpath);

ul_hapi ulfd_mode_t ulfd_umask(ulfd_mode_t mask); /* works in POSIX */
ul_hapi int ulfd_chmod(const char* path, ulfd_mode_t mode);
ul_hapi int ulfd_chmod_w(const wchar_t* wpath, ulfd_mode_t mode);
ul_hapi int ulfd_lchmod(const char* path, ulfd_mode_t mode);
ul_hapi int ulfd_lchmod_w(const wchar_t* wpath, ulfd_mode_t mode);

ul_hapi int ulfd_chown(const char* path, ulfd_uid_t uid, ulfd_gid_t gid);
ul_hapi int ulfd_chown_w(const wchar_t* wpath, ulfd_uid_t uid, ulfd_gid_t gid);
ul_hapi int ulfd_lchown(const char* path, ulfd_uid_t uid, ulfd_gid_t gid);
ul_hapi int ulfd_lchown_w(const wchar_t* wpath, ulfd_uid_t uid, ulfd_gid_t gid);

ul_hapi int ulfd_utime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime);
ul_hapi int ulfd_utime_w(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime);
ul_hapi int ulfd_lutime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime);
ul_hapi int ulfd_lutime_w(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime);

typedef struct ulfd_stat_t {
  ulfd_time_t atime; /* time of last access */
  ulfd_time_t mtime; /* time of last modification */
  ulfd_time_t ctime; /* time of last status change */

  ulfd_int64_t size; /* total size */

  ulfd_dev_t dev; /* ID of device containing file */
  ulfd_dev_t rdev; /* device ID (if special file) */
  ulfd_nlink_t nlink; /* number of hard links */

  ulfd_ino_t ino; /* POSIX: inode number */
  ulfd_uid_t uid; /* POSIX: user ID of owner */
  ulfd_gid_t gid; /* POSIX: group ID of owner */
  ulfd_mode_t mode; /* file type and mode */
} ulfd_stat_t;

ul_hapi int ulfd_stat(const char* path, ulfd_stat_t* state);
ul_hapi int ulfd_stat_w(const wchar_t* wpath, ulfd_stat_t* state);
ul_hapi int ulfd_lstat(const char* path, ulfd_stat_t* state);
ul_hapi int ulfd_lstat_w(const wchar_t* wpath, ulfd_stat_t* state);
ul_hapi int ulfd_fstat(ulfd_t fd, ulfd_stat_t* state);

ul_hapi int ulfd_type(const char* path, ulfd_mode_t* pmode); /* access bits are invalid */
ul_hapi int ulfd_type_w(const wchar_t* wpath, ulfd_mode_t* pmode); /* access bits are invalid */
ul_hapi int ulfd_ltype(const char* path, ulfd_mode_t* pmode); /* access bits are invalid */
ul_hapi int ulfd_ltype_w(const wchar_t* wpath, ulfd_mode_t* pmode); /* access bits are invalid */
ul_hapi int ulfd_ftype(ulfd_t fd, ulfd_mode_t* state); /* access bits are invalid */

/* `mode` accepts ULFD_S_IRUSR, ULFD_S_IWUSR, ULFD_S_IXUSR or their union */
ul_hapi int ulfd_access(const char* path, ulfd_mode_t mode);
/* `mode` accepts ULFD_S_IRUSR, ULFD_S_IWUSR, ULFD_S_IXUSR or their union */
ul_hapi int ulfd_access_w(const wchar_t* wpath, ulfd_mode_t mode);
ul_hapi int ulfd_empty(const char* path, int* pempty);
ul_hapi int ulfd_empty_w(const wchar_t* wpath, int* pempty);

ul_hapi int ulfd_mkdir(const char* path, ulfd_mode_t mode);
ul_hapi int ulfd_mkdir_w(const wchar_t* wpath, ulfd_mode_t mode);
ul_hapi int ulfd_rename(const char* newpath, const char* oldpath);
ul_hapi int ulfd_rename_w(const wchar_t* newpath, const wchar_t* oldpath);
ul_hapi int ulfd_unlink(const char* path);
ul_hapi int ulfd_unlink_w(const wchar_t* wpath);
ul_hapi int ulfd_remove(const char* path);
ul_hapi int ulfd_remove_w(const wchar_t* wpath);
ul_hapi int ulfd_rmdir(const char* path);
ul_hapi int ulfd_rmdir_w(const wchar_t* wpath);

ul_hapi int ulfd_link(const char* newpath, const char* oldpath);
ul_hapi int ulfd_link_w(const wchar_t* newpath, const wchar_t* oldpath);
ul_hapi int ulfd_symlink(const char* target, const char* source);
ul_hapi int ulfd_symlink_w(const wchar_t* target, const wchar_t* source);
ul_hapi int ulfd_readlink(const char* path, char* buf, size_t len);
ul_hapi int ulfd_readlink_w(const wchar_t* wpath, wchar_t* buf, size_t len);
ul_hapi int ulfd_readlink_alloc(char** pbuf, const char* path);
ul_hapi int ulfd_readlink_alloc_w(wchar_t** pwbuf, const wchar_t* wpath);
ul_hapi int ulfd_realpath_alloc(char** presolved, const char* path);
ul_hapi int ulfd_realpath_alloc_w(wchar_t** presolved, const wchar_t* wpath);

ul_hapi int ulfd_truncate(const char* path, ulfd_int64_t size);
ul_hapi int ulfd_truncate_w(const wchar_t* wpath, ulfd_int64_t size);

#ifdef _WIN32
  typedef struct ulfd_dir_t {
    HANDLE handle;
    WIN32_FIND_DATAW data;
    wchar_t* dirpath;
    char* entry;
    size_t entry_cap;
    int cached;
  } ulfd_dir_t;
#else
  #include <dirent.h>
  typedef struct ulfd_dir_t {
    DIR* dir;
    char* entry;
    size_t entry_cap;
    wchar_t* wentry;
    size_t wentry_cap;
    int cached;
  } ulfd_dir_t;
#endif
ul_hapi int ulfd_opendir(ulfd_dir_t* dir, const char* path);
ul_hapi int ulfd_opendir_w(ulfd_dir_t* dir, const wchar_t* wpath);
ul_hapi int ulfd_rewinddir(ulfd_dir_t* dir);
ul_hapi int ulfd_closedir(ulfd_dir_t* dir);
ul_hapi int ulfd_readdir(ulfd_dir_t* dir, const char** pname);
ul_hapi int ulfd_readdir_w(ulfd_dir_t* dir, const wchar_t** pwname);

typedef struct ulfd_spaceinfo_t {
  ulfd_uint64_t capacity; /* total size of the filesystem */
  ulfd_uint64_t free; /* free space on the filesystem */
  ulfd_uint64_t available; /* free space available to non-privileged process */
} ulfd_spaceinfo_t;
ul_hapi int ulfd_space(ulfd_spaceinfo_t* info, const char* path);
ul_hapi int ulfd_space_w(ulfd_spaceinfo_t* info, const wchar_t* wpath);


#include <string.h>
ul_hapi size_t ulfd_strlen(const char* str) { return strlen(str); }

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199409L) || defined(__cplusplus)
  #ifdef __cplusplus
    #include <cwchar>
  #else
    #include <wchar.h>
  #endif
  ul_hapi size_t ulfd_wcslen(const wchar_t* wstr) { return wcslen(wstr); }
#else
  ul_hapi size_t ulfd_wcslen(const wchar_t* wstr) {
    size_t n;
    for(n = 0; wstr[n]; ++n) { }
    return n;
  }
#endif

ul_hapi char* ulfd_strdup(const char* str) {
  char* ret;
  size_t len = ulfd_strlen(str);
  ret = ul_reinterpret_cast(char*, ul_malloc(len + 1));
  if(ul_unlikely(ret == NULL)) return NULL;
  memcpy(ret, str, len + 1);
  return ret;
}
ul_hapi wchar_t* ulfd_wcsdup(const wchar_t* wstr) {
  wchar_t* ret;
  size_t bytes = (ulfd_wcslen(wstr) + 1) * sizeof(wchar_t);
  ret = ul_reinterpret_cast(wchar_t*, ul_malloc(bytes));
  if(ul_unlikely(ret == NULL)) return NULL;
  memcpy(ret, wstr, bytes);
  return ret;
}


/* aliases of native Unicode version */
#ifdef _WIN32
  typedef wchar_t ulfd_uchar_t;
  #define _ULFD_UTEXT(text) L##text
  #define ULFD_UTEXT(text) _ULFD_UTEXT(text)

  ul_hapi int ulfd_open_u(ulfd_t* pfd, const ulfd_uchar_t* path, ulfd_int32_t oflag, ulfd_mode_t mode) {
    return ulfd_open_w(pfd, path, oflag, mode);
  }
  ul_hapi int ulfd_creat_u(ulfd_t* pfd, const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_creat_w(pfd, path, mode);
  }
  ul_hapi int ulfd_fdopen_u(FILE** pfp, ulfd_t fd, const ulfd_uchar_t* mode) {
    return ulfd_fdopen_w(pfp, fd, mode);
  }
  ul_hapi int ulfd_chdir_u(const ulfd_uchar_t* path) {
    return ulfd_chdir_w(path);
  }
  ul_hapi int ulfd_getcwd_u(ulfd_uchar_t* path, size_t length) {
    return ulfd_getcwd_w(path, length);
  }
  ul_hapi int ulfd_getcwd_alloc_u(ulfd_uchar_t** ppath) {
    return ulfd_getcwd_alloc_w(ppath);
  }
  ul_hapi int ulfd_chmod_u(const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_chmod_w(path, mode);
  }
  ul_hapi int ulfd_lchmod_u(const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_lchmod_w(path, mode);
  }
  ul_hapi int ulfd_chown_u(const ulfd_uchar_t* path, ulfd_uid_t uid, ulfd_gid_t gid) {
    return ulfd_chown_w(path, uid, gid);
  }
  ul_hapi int ulfd_lchown_u(const ulfd_uchar_t* path, ulfd_uid_t uid, ulfd_gid_t gid) {
    return ulfd_lchown_w(path, uid, gid);
  }
  ul_hapi int ulfd_utime_u(const ulfd_uchar_t* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    return ulfd_utime_w(path, atime, mtime);
  }
  ul_hapi int ulfd_lutime_u(const ulfd_uchar_t* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    return ulfd_lutime_w(path, atime, mtime);
  }
  ul_hapi int ulfd_stat_u(const ulfd_uchar_t* path, ulfd_stat_t* state) {
    return ulfd_stat_w(path, state);
  }
  ul_hapi int ulfd_lstat_u(const ulfd_uchar_t* path, ulfd_stat_t* state) {
    return ulfd_lstat_w(path, state);
  }
  ul_hapi int ulfd_type_u(const ulfd_uchar_t* path, ulfd_mode_t* pmode) {
    return ulfd_type_w(path, pmode);
  }
  ul_hapi int ulfd_ltype_u(const ulfd_uchar_t* path, ulfd_mode_t* pmode) {
    return ulfd_ltype_w(path, pmode);
  }
  ul_hapi int ulfd_access_u(const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_access_w(path, mode);
  }
  ul_hapi int ulfd_empty_u(const ulfd_uchar_t* path, int* pempty) {
    return ulfd_empty_w(path, pempty);
  }
  ul_hapi int ulfd_mkdir_u(const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_mkdir_w(path, mode);
  }
  ul_hapi int ulfd_rename_u(const ulfd_uchar_t* newpath, const ulfd_uchar_t* oldpath) {
    return ulfd_rename_w(newpath, oldpath);
  }
  ul_hapi int ulfd_unlink_u(const ulfd_uchar_t* path) {
    return ulfd_unlink_w(path);
  }
  ul_hapi int ulfd_remove_u(const ulfd_uchar_t* path) {
    return ulfd_remove_w(path);
  }
  ul_hapi int ulfd_rmdir_u(const ulfd_uchar_t* path) {
    return ulfd_rmdir_w(path);
  }
  ul_hapi int ulfd_link_u(const ulfd_uchar_t* newpath, const ulfd_uchar_t* oldpath) {
    return ulfd_link_w(newpath, oldpath);
  }
  ul_hapi int ulfd_symlink_u(const ulfd_uchar_t* target, const ulfd_uchar_t* source) {
    return ulfd_symlink_w(target, source);
  }
  ul_hapi int ulfd_readlink_u(const ulfd_uchar_t* path, ulfd_uchar_t* buf, size_t len) {
    return ulfd_readlink_w(path, buf, len);
  }
  ul_hapi int ulfd_readlink_alloc_u(ulfd_uchar_t** pbuf, const ulfd_uchar_t* path) {
    return ulfd_readlink_alloc_w(pbuf, path);
  }
  ul_hapi int ulfd_realpath_alloc_u(ulfd_uchar_t** presolved, const ulfd_uchar_t* path) {
    return ulfd_realpath_alloc_w(presolved, path);
  }
  ul_hapi int ulfd_truncate_u(const ulfd_uchar_t* path, ulfd_int64_t size) {
    return ulfd_truncate_w(path, size);
  }
  ul_hapi int ulfd_opendir_u(ulfd_dir_t* dir, const ulfd_uchar_t* path) {
    return ulfd_opendir_w(dir, path);
  }
  ul_hapi int ulfd_readdir_u(ulfd_dir_t* dir, const ulfd_uchar_t** pname) {
    return ulfd_readdir_w(dir, pname);
  }
  ul_hapi int ulfd_space_u(ulfd_spaceinfo_t* info, const ulfd_uchar_t* path) {
    return ulfd_space_w(info, path);
  }

  ul_hapi size_t ulfd_ucslen(const ulfd_uchar_t* str) {
    return ulfd_wcslen(str);
  }
  ul_hapi ulfd_uchar_t* ulfd_ucsdup(const ulfd_uchar_t* str) {
    return ulfd_wcsdup(str);
  }
#else
  typedef char ulfd_uchar_t;
  #define ULFD_UTEXT(text) text

  ul_hapi int ulfd_open_u(ulfd_t* pfd, const ulfd_uchar_t* path, ulfd_int32_t oflag, ulfd_mode_t mode) {
    return ulfd_open(pfd, path, oflag, mode);
  }
  ul_hapi int ulfd_creat_u(ulfd_t* pfd, const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_creat(pfd, path, mode);
  }
  ul_hapi int ulfd_fdopen_u(FILE** pfp, ulfd_t fd, const ulfd_uchar_t* mode) {
    return ulfd_fdopen(pfp, fd, mode);
  }
  ul_hapi int ulfd_chdir_u(const ulfd_uchar_t* path) {
    return ulfd_chdir(path);
  }
  ul_hapi int ulfd_getcwd_u(ulfd_uchar_t* path, size_t length) {
    return ulfd_getcwd(path, length);
  }
  ul_hapi int ulfd_getcwd_alloc_u(ulfd_uchar_t** ppath) {
    return ulfd_getcwd_alloc(ppath);
  }
  ul_hapi int ulfd_chmod_u(const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_chmod(path, mode);
  }
  ul_hapi int ulfd_lchmod_u(const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_lchmod(path, mode);
  }
  ul_hapi int ulfd_chown_u(const ulfd_uchar_t* path, ulfd_uid_t uid, ulfd_gid_t gid) {
    return ulfd_chown(path, uid, gid);
  }
  ul_hapi int ulfd_lchown_u(const ulfd_uchar_t* path, ulfd_uid_t uid, ulfd_gid_t gid) {
    return ulfd_lchown(path, uid, gid);
  }
  ul_hapi int ulfd_utime_u(const ulfd_uchar_t* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    return ulfd_utime(path, atime, mtime);
  }
  ul_hapi int ulfd_lutime_u(const ulfd_uchar_t* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    return ulfd_lutime(path, atime, mtime);
  }
  ul_hapi int ulfd_stat_u(const ulfd_uchar_t* path, ulfd_stat_t* state) {
    return ulfd_stat(path, state);
  }
  ul_hapi int ulfd_lstat_u(const ulfd_uchar_t* path, ulfd_stat_t* state) {
    return ulfd_lstat(path, state);
  }
  ul_hapi int ulfd_type_u(const ulfd_uchar_t* path, ulfd_mode_t* pmode) {
    return ulfd_type(path, pmode);
  }
  ul_hapi int ulfd_ltype_u(const ulfd_uchar_t* path, ulfd_mode_t* pmode) {
    return ulfd_ltype(path, pmode);
  }
  ul_hapi int ulfd_access_u(const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_access(path, mode);
  }
  ul_hapi int ulfd_empty_u(const ulfd_uchar_t* path, int* pempty) {
    return ulfd_empty(path, pempty);
  }
  ul_hapi int ulfd_mkdir_u(const ulfd_uchar_t* path, ulfd_mode_t mode) {
    return ulfd_mkdir(path, mode);
  }
  ul_hapi int ulfd_rename_u(const ulfd_uchar_t* newpath, const ulfd_uchar_t* oldpath) {
    return ulfd_rename(newpath, oldpath);
  }
  ul_hapi int ulfd_unlink_u(const ulfd_uchar_t* path) {
    return ulfd_unlink(path);
  }
  ul_hapi int ulfd_remove_u(const ulfd_uchar_t* path) {
    return ulfd_remove(path);
  }
  ul_hapi int ulfd_rmdir_u(const ulfd_uchar_t* path) {
    return ulfd_rmdir(path);
  }
  ul_hapi int ulfd_link_u(const ulfd_uchar_t* newpath, const ulfd_uchar_t* oldpath) {
    return ulfd_link(newpath, oldpath);
  }
  ul_hapi int ulfd_symlink_u(const ulfd_uchar_t* target, const ulfd_uchar_t* source) {
    return ulfd_symlink(target, source);
  }
  ul_hapi int ulfd_readlink_u(const ulfd_uchar_t* path, ulfd_uchar_t* buf, size_t len) {
    return ulfd_readlink(path, buf, len);
  }
  ul_hapi int ulfd_readlink_alloc_u(ulfd_uchar_t** pbuf, const ulfd_uchar_t* path) {
    return ulfd_readlink_alloc(pbuf, path);
  }
  ul_hapi int ulfd_realpath_alloc_u(ulfd_uchar_t** presolved, const ulfd_uchar_t* path) {
    return ulfd_realpath_alloc(presolved, path);
  }
  ul_hapi int ulfd_truncate_u(const ulfd_uchar_t* path, ulfd_int64_t size) {
    return ulfd_truncate(path, size);
  }
  ul_hapi int ulfd_opendir_u(ulfd_dir_t* dir, const ulfd_uchar_t* path) {
    return ulfd_opendir(dir, path);
  }
  ul_hapi int ulfd_readdir_u(ulfd_dir_t* dir, const ulfd_uchar_t** pname) {
    return ulfd_readdir(dir, pname);
  }
  ul_hapi int ulfd_space_u(ulfd_spaceinfo_t* info, const ulfd_uchar_t* path) {
    return ulfd_space(info, path);
  }

  ul_hapi size_t ulfd_ucslen(const ulfd_uchar_t* str) {
    return ulfd_strlen(str);
  }
  ul_hapi ulfd_uchar_t* ulfd_ucsdup(const ulfd_uchar_t* str) {
    return ulfd_strdup(str);
  }
#endif


#ifdef _WIN32
  #if (_WIN32_WINNT+0) >= 0x0501 /* Windows XP */
    #define ULFD_WIN32_HAS_CreateHardLink
    #define ULFD_WIN32_HAS_CreateSymbolicLink
  #endif
  #if (_WIN32_WINNT+0) >= 0x0600 /* Windows Vista */
    #define ULFD_WIN32_HAS_GetFinalPathNameByHandle
  #endif
  #if (_WIN32_WINNT+0) >= 0x0602 /* Windows 8 */
    #define ULFD_WIN32_HAS_PrefetchVirtualMemory
  #endif
#else
  #ifdef _POSIX_C_SOURCE
    #if (_POSIX_C_SOURCE+0) >= 200809L
      #define ULFD_POSIX_HAS_pread
      #define ULFD_POSIX_HAS_pwrite
      #define ULFD_POSIX_HAS_utimensat
      #define ULFD_POSIX_HAS_fchmod
      #define ULFD_POSIX_HAS_fchmodat
      #define ULFD_POSIX_HAS_fchown
      #define ULFD_POSIX_HAS_fchownat
      #define ULFD_POSIX_HAS_truncate
      #define ULFD_POSIX_STAT_HAS_TIM
    #endif
    #if (_POSIX_C_SOURCE+0) >= 200112L
      #define ULFD_POSIX_HAS_fsync
      #define ULFD_POSIX_HAS_readlink
      #define ULFD_POSIX_HAS_lstat
      #define ULFD_POSIX_HAS_symlink
      #define ULFD_POSIX_HAS_ftruncate
    #endif
    #if (_POSIX_C_SOURCE+0) >= 199309L
      #define ULFD_POSIX_HAS_fdatasync
    #endif
    #if (_POSIX_C_SOURCE+0) >= 1
      #define ULFD_POSIX_HAS_fileno
      #define ULFD_POSIX_HAS_fdopen
    #endif
  #endif

  #if defined(_POSIX_SOURCE) && (_POSIX_SOURCE+0)
    #define ULFD_POSIX_HAS_fileno
    #define ULFD_POSIX_HAS_fdopen
  #endif

  #ifdef _XOPEN_SOURCE
    #if (_XOPEN_SOURCE+0) >= 700
      #define ULFD_POSIX_HAS_utimensat
      #define ULFD_POSIX_HAS_fchownat
      #define ULFD_POSIX_HAS_fchmodat
    #endif
    #if (_XOPEN_SOURCE+0) >= 500
      #define ULFD_POSIX_HAS_pread
      #define ULFD_POSIX_HAS_pwrite
      #define ULFD_POSIX_HAS_realpath
      #define ULFD_POSIX_HAS_readlink
      #define ULFD_POSIX_HAS_lstat
      #define ULFD_POSIX_HAS_symlink
      #define ULFD_POSIX_HAS_truncate
      #define ULFD_POSIX_HAS_fchmod
      #define ULFD_POSIX_HAS_ftruncate
      #define ULFD_POSIX_HAS_fchown
      #define ULFD_POSIX_HAS_fdatasync
    #endif
    #if (_XOPEN_SOURCE+0)
      #define ULFD_POSIX_HAS_fileno
      #define ULFD_POSIX_HAS_fdopen
      #define ULFD_POSIX_HAS_fsync
    #endif
    #ifdef _XOPEN_SOURCE_EXTENDED
      #if (_XOPEN_SOURCE+0) && (_XOPEN_SOURCE_EXTENDED+0)
        #define ULFD_POSIX_HAS_realpath
        #define ULFD_POSIX_HAS_readlink
        #define ULFD_POSIX_HAS_lstat
        #define ULFD_POSIX_HAS_truncate
        #define ULFD_POSIX_HAS_fchmod
        #define ULFD_POSIX_HAS_ftruncate
        #define ULFD_POSIX_HAS_fchown
      #endif
    #endif
  #endif

  #if defined(_BSD_SOURCE) && (_BSD_SOURCE+0)
    #define ULFD_POSIX_HAS_realpath
    #define ULFD_POSIX_HAS_readlink
    #define ULFD_POSIX_HAS_lstat
    #define ULFD_POSIX_HAS_symlink
    #define ULFD_POSIX_HAS_truncate
    #define ULFD_POSIX_HAS_fchmod
    #define ULFD_POSIX_HAS_ftruncate
    #define ULFD_POSIX_HAS_futimes
    #define ULFD_POSIX_HAS_fchown
    #define ULFD_POSIX_HAS_fsync
    #define ULFD_POSIX_STAT_HAS_TIM
  #endif

  #if defined(_ATFILE_SOURCE) && (_ATFILE_SOURCE+0)
    #define ULFD_POSIX_HAS_utimensat
    #define ULFD_POSIX_HAS_fchownat
    #define ULFD_POSIX_HAS_fchmodat
  #endif

  #if defined(_DEFAULT_SOURCE) && (_DEFAULT_SOURCE+0)
    #define ULFD_POSIX_HAS_futimes
  #endif

  #if defined(_SVID_SOURCE) && (_SVID_SOURCE+0)
    #define ULFD_POSIX_STAT_HAS_TIM
  #endif

  #ifdef __GLIBC__
    #if defined(_GNU_SOURCE) && (_GNU_SOURCE+0)
      #define ULFD_POSIX_HAS_canonicalize_file_name
      #define ULFD_POSIX_HAS_get_current_dir_name
      #define ULFD_POSIX_HAS_copy_file_range
      #define ULFD_POSIX_STAT_HAS_TIM
    #endif
  #endif
#endif


#ifdef _WIN32
  #define ULFD_NO_COPY_FILE_RANGE
  #define ULFD_NO_CHOWN
  #define ULFD_NO_LCHOWN
  #define ULFD_NO_FCHOWN

  #ifndef ULFD_WIN32_HAS_CreateHardLink
    #define ULFD_NO_LINK
  #endif
  #ifndef ULFD_WIN32_HAS_CreateSymbolicLink
    #define ULFD_NO_SYMLINK
  #endif
  #ifndef ULFD_WIN32_HAS_GetFinalPathNameByHandle
    #define ULFD_NO_READLINK
    #define ULFD_NO_FCHMOD
    #define ULFD_NO_FUTIME
    #define ULFD_NO_FSTAT
    #define ULFD_NO_FTYPE
  #endif
#else
  #ifndef ULFD_POSIX_HAS_pread
    #define ULFD_NO_PREAD
  #endif
  #ifndef ULFD_POSIX_HAS_pwrite
    #define ULFD_NO_PWRITE
  #endif
  #ifndef ULFD_POSIX_HAS_copy_file_range
    #define ULFD_NO_COPY_FILE_RANGE
  #endif
  #ifndef ULFD_POSIX_HAS_fsync
    #define ULFD_NO_FSYNC
    #define ULFD_NO_FFULLSYNC
    #define ULFD_NO_FDATASYNC
  #endif
  #ifndef ULFD_POSIX_HAS_fdatasync
    #define ULFD_NO_FDATASYNC
  #else
    #undef ULFD_NO_FDATASYNC /* nearly impossible */
  #endif
  #ifndef ULFD_POSIX_HAS_ftruncate
    #define ULFD_NO_FTRUNCATE
  #endif
  #ifndef ULFD_POSIX_HAS_fchmod
    #define ULFD_NO_FCHMOD
  #endif
  #ifndef ULFD_POSIX_HAS_fchown
    #define ULFD_NO_FCHOWN
  #endif
  #ifndef ULFD_POSIX_HAS_futimes
    #define ULFD_NO_FUTIME
  #endif
  #ifndef ULFD_POSIX_HAS_fdopen
    #define ULFD_NO_FDOPEN
  #endif
  #ifndef ULFD_POSIX_HAS_fileno
    #define ULFD_NO_FILENO
  #endif
  #ifndef ULFD_POSIX_HAS_truncate
    #define ULFD_NO_TRUNCATE
  #endif
  #ifndef ULFD_POSIX_HAS_fchmodat
    #define ULFD_NO_LCHMOD
  #endif
  #ifndef ULFD_POSIX_HAS_fchownat
    #define ULFD_NO_LCHOWN
  #endif
  #ifndef ULFD_POSIX_HAS_utimensat
    #define ULFD_NO_LUTIME
  #endif
  #ifndef ULFD_POSIX_HAS_lstat
    #define ULFD_NO_LSTAT
    #define ULFD_NO_LTYPE
  #endif
  #ifndef ULFD_POSIX_HAS_symlink
    #define ULFD_NO_SYMLINK
  #endif
  #ifndef ULFD_POSIX_HAS_readlink
    #define ULFD_NO_READLINK
  #endif
  #if !defined(ULFD_POSIX_HAS_canonicalize_file_name) && !defined(ULFD_POSIX_HAS_realpath)
    #define ULFD_NO_REALPATH
  #endif
#endif


#ifndef ULOS_STR_TO_WSTR_DEFINED
  #if UINT_MAX >= 0xFFFFFFFF
    typedef unsigned ulos_str_to_wstr_u32_t;
  #elif ULONG_MAX >= 0xFFFFFFFF
    typedef unsigned long ulos_str_to_wstr_u32_t;
  #else
    #error "unsigned long is less than 32 bits"
  #endif
  #ifndef ul_static_cast
    #ifdef __cplusplus
      #define ul_static_cast(T, val) static_cast<T>(val)
    #else
      #define ul_static_cast(T, val) ((T)(val))
    #endif
  #endif /* ul_static_cast */
  ul_hapi size_t ul_os_str_to_wstr_len(const char* s) {
    ulos_str_to_wstr_u32_t u; int i, l; size_t r = 1;
    static const ulos_str_to_wstr_u32_t MINLIM[4] = { 0, 0x80u, 0x800u, 0x10000u };
    while((u = ul_static_cast(unsigned char, *s++))) {
      if(u < 128u) l = 0;
      else if(u < 194u) return 0;
      else if(u < 224u) { l = 1; u &= 0x1Fu; }
      else if(u < 240u) { l = 2; u &= 0xFu; }
      else if(u < 245u) { l = 3; u &= 0x7u; }
      else return 0;
      for(i = l;
        i-- && (ul_static_cast(unsigned, *s) & 0xC0u) == 0x80u;
        u = (u << 6) | (ul_static_cast(unsigned, *s++) & 0x3Fu)) { }
      if(u > 0x10FFFFu || (0xD800u <= u && u <= 0xDFFFu) || u < MINLIM[l]) return 0;
    #ifdef _WIN32
      r += (u >= 0x10000u) + 1;
    #else
      ++r;
    #endif
    }
    return r;
  }
  ul_hapi wchar_t* ul_os_str_to_wstr(wchar_t* o, const char* s) {
    ulos_str_to_wstr_u32_t u; int l;
    while((u = ul_static_cast(unsigned char, *s++))) {
      if(u < 128u) l = 0;
      else if(u < 224u) { l = 1; u &= 0x1Fu; }
      else if(u < 240u) { l = 2; u &= 0xFu; }
      else { l = 3; u &= 0x7u; }
      while(l--) u = (u << 6) | (ul_static_cast(unsigned, *s++) & 0x3Fu);
    #ifdef _WIN32
      if(u < 0x10000u) *o++ = ul_static_cast(wchar_t, u);
      else {
        *o++ = ul_static_cast(wchar_t, 0xD800u | ((u - 0x10000u) >> 10));
        *o++ = ul_static_cast(wchar_t, 0xDC00u | (u & 0x3FF));
      }
    #else
      *o++ = ul_static_cast(wchar_t, u);
    #endif
    }
    *o++ = 0; return o;
  }
  #define ULOS_STR_TO_WSTR_DEFINED
#endif /* ULOS_STR_TO_WSTR_DEFINED */

#ifndef ULOS_WSTR_TO_STR_DEFINED
  #if UINT_MAX >= 0xFFFFFFFF
    typedef unsigned ulos_wstr_to_str_u32_t;
  #elif ULONG_MAX >= 0xFFFFFFFF
    typedef unsigned long ulos_wstr_to_str_u32_t;
  #else
    #error "unsigned long is less than 32 bits"
  #endif
  #ifndef ul_static_cast
    #ifdef __cplusplus
      #define ul_static_cast(T, val) static_cast<T>(val)
    #else
      #define ul_static_cast(T, val) ((T)(val))
    #endif
  #endif /* ul_static_cast */
  ul_hapi size_t ul_os_wstr_to_str_len(const wchar_t* s) {
    ulos_wstr_to_str_u32_t u; size_t r = 1;
    while((u = ul_static_cast(ulos_wstr_to_str_u32_t, *s++))) {
    #ifdef _WIN32
      if(u >= 0xD800u && u <= 0xDFFFu) {
        if(u >= 0xDC00u || *s < 0xDC00u || *s > 0xDFFFu) return 0;
        else { ++s; u = 0x10FFFFu; }
      }
    #endif
      if(u <= 0x7Fu) r += 1;
      else if(u <= 0x7FFu) r += 2;
      else if(u <= 0xFFFFu) r += 3;
      else r += 4;
    }
    return r;
  }
  ul_hapi char* ul_os_wstr_to_str(char* o, const wchar_t* s) {
    ulos_wstr_to_str_u32_t u;
    while((u = ul_static_cast(ulos_wstr_to_str_u32_t, *s++))) {
    #ifdef _WIN32
      if((u & 0xF800) == 0xD800)
        u = (((u & 0x3FFu) << 10) | (*s++ & 0x3FFu)) + 0x10000u;
    #endif
      if(u <= 0x7Fu) *o++ = ul_static_cast(char, u);
      else {
        if(u <= 0x7FFu) *o++ = ul_static_cast(char, (u >> 6) | 0xC0u);
        else {
          if(u <= 0xFFFFu) *o++ = ul_static_cast(char, (u >> 12) | 0xE0u);
          else {
            *o++ = ul_static_cast(char, (u >> 18) | 0xF0u);
            *o++ = ul_static_cast(char, ((u >> 12) & 0x3Fu) | 0x80u);
          }
          *o++ = ul_static_cast(char, ((u >> 6) & 0x3Fu) | 0x80u);
        }
        *o++ = ul_static_cast(char, (u & 0x3Fu) | 0x80u);
      }
    }
    *o++ = 0; return o;
  }
  #define ULOS_WSTR_TO_STR_DEFINED
#endif /* ULOS_WSTR_TO_STR_DEFINED */

ul_hapi int ulfd_str_to_wstr_alloc(wchar_t** pwstr, const char* str) {
  size_t len;
  wchar_t* wstr;
  len = ul_os_str_to_wstr_len(str);
  if(ul_unlikely(len == 0)) return EILSEQ;
  wstr = ul_reinterpret_cast(wchar_t*, ul_malloc(len * sizeof(wchar_t)));
  if(ul_unlikely(wstr == NULL)) return ENOMEM;
  ul_os_str_to_wstr(wstr, str);
  *pwstr = wstr; return 0;
}
ul_hapi int ulfd_wstr_to_str_alloc(char** pstr, const wchar_t* wstr) {
  size_t len;
  char* str;
  len = ul_os_wstr_to_str_len(wstr);
  if(ul_unlikely(len == 0)) return EILSEQ;
  str = ul_reinterpret_cast(char*, ul_malloc(len));
  if(ul_unlikely(wstr == NULL)) return ENOMEM;
  ul_os_wstr_to_str(str, wstr);
  *pstr = str; return 0;
}



/**************
 * IMPLEMENTS *
 **************/



ul_hapi int ulfd_copy_file_range_user(
  ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out,
  size_t len, size_t* pcopyed, void* buf, size_t buf_len
) {
  void* nbuf;
  ulfd_int64_t nin, nout;
  size_t nread, nwriten;
  int err = 0;
  size_t copyed = 0;

  if(off_in) {
    if(*off_in < 0) return EINVAL;
    nin = *off_in;
    if(ULFD_INT64_C(0x7FFFFFFFFFFFFFFF) - nin < ul_static_cast(ulfd_int64_t, len)) return EINVAL;
  } else nin = -1;
  if(off_out) {
    if(*off_out < 0) return EINVAL;
    nout = *off_out;
    if(ULFD_INT64_C(0x7FFFFFFFFFFFFFFF) - nout < ul_static_cast(ulfd_int64_t, len)) return EINVAL;
  } else nout = -1;

  if(buf) {
    if(buf_len == 0) return EINVAL;
    else nbuf = buf;
  } else {
    if(buf_len == 0) {
      buf_len = 16384;
      do {
        nbuf = ul_malloc(buf_len);
        if(ul_likely(nbuf)) break;
        buf_len >>= 1;
      } while(buf_len > 0);
    } else nbuf = ul_malloc(buf_len);
    if(ul_unlikely(nbuf == NULL)) return ENOMEM;
  }

  while(len >= buf_len) {
  go_back:
    nwriten = 0;
    if(nin >= 0) {
      err = ulfd_pread(fd_in, nbuf, buf_len, nin, &nread);
      nin += nread;
    } else err = ulfd_read(fd_in, nbuf, buf_len, &nread);
    if(err) goto do_return;

    if(nread) {
      if(nout >= 0) {
        err = ulfd_pwrite(fd_out, nbuf, nread, nout, &nwriten);
        nout += nwriten;
      } else err = ulfd_write(fd_out, nbuf, buf_len, &nwriten);
    }
    if(err) goto do_return;

    copyed += nwriten;
    if(nwriten < buf_len) goto do_return;
    len -= buf_len;
  }
  if(ul_unlikely(len != 0)) {
    /* we set `buf_len` to smaller `len`, then we go back to the loop.
     after it, `len` will be 0, as 0 < `buf_len`, the loop won't continue. */
    buf_len = len; goto go_back;
  }

do_return:
  *pcopyed = copyed;
  if(buf) ul_free(nbuf);
  return err;
}
ul_hapi int ulfd_pread_user(ulfd_t fd, void* buf, size_t count, ulfd_int64_t off, size_t* pread_bytes) {
  ulfd_int64_t pos, npos;
  int err;

  err = ulfd_tell(fd, &pos);
  if(err) return err;
  err = ulfd_seek(fd, off, ULFD_SEEK_SET, &npos);
  if(err) return err;
  err = ulfd_read(fd, buf, count, pread_bytes);
  if(err) return err;
  err = ulfd_seek(fd, off, ULFD_SEEK_SET, &npos);
  return err;
}
ul_hapi int ulfd_pwrite_user(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes) {
  ulfd_int64_t pos, npos;
  int err;

  err = ulfd_tell(fd, &pos);
  if(err) return err;
  err = ulfd_seek(fd, off, ULFD_SEEK_SET, &npos);
  if(err) return err;
  err = ulfd_write(fd, buf, count, pwriten_bytes);
  if(err) return err;
  err = ulfd_seek(fd, off, ULFD_SEEK_SET, &npos);
  return err;
}

ul_hapi int ulfd_pread_allowuser(ulfd_t fd, void* buf, size_t count, ulfd_int64_t off, size_t* pread_bytes) {
#ifndef ULFD_NO_PREAD
  return ulfd_pread(fd, buf, count, off, pread_bytes);
#else
  return ulfd_pread_user(fd, buf, count, off, pread_bytes);
#endif
}
ul_hapi int ulfd_pwrite_allowuser(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes) {
#ifndef ULFD_NO_PREAD
  return ulfd_pwrite(fd, buf, count, off, pwriten_bytes);
#else
  return ulfd_pwrite_user(fd, buf, count, off, pwriten_bytes);
#endif
}
ul_hapi int ulfd_copy_file_range_allowuser(
  ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out,
  size_t len, size_t* pcopyed
) {
#ifndef ULFD_NO_COPY_FILE_RANGE
  return ulfd_copy_file_range(fd_in, off_in, fd_out, off_out, len, pcopyed);
#else
  return ulfd_copy_file_range_user(fd_in, off_in, fd_out, off_out, len, pcopyed, NULL, 0);
#endif
}


#define _ulfd_begin_to_str(varname, wstr) do { \
  char* varname; int _ulfd_bts_err1 = ulfd_wstr_to_str_alloc(&(varname), (wstr)); \
  if(ul_unlikely(_ulfd_bts_err1)) return _ulfd_bts_err1
#define _ulfd_end_to_str(varname) ul_free((varname)); } while(0)

#define _ulfd_begin_to_str2(varname, wstr, prev_varname) do { \
  char* varname; int _ulfd_bts_err2 = ulfd_wstr_to_str_alloc(&(varname), (wstr)); \
  if(ul_unlikely(_ulfd_bts_err2)) { ul_free(prev_varname); return _ulfd_bts_err2; } (void)0
#define _ulfd_end_to_str2(varname) ul_free((varname)); } while(0)

#define _ulfd_begin_to_wstr(varname, str) do { \
  wchar_t* varname; int _ulfd_bts_err1 = ulfd_str_to_wstr_alloc(&(varname), (str)); \
  if(ul_unlikely(_ulfd_bts_err1)) return _ulfd_bts_err1
#define _ulfd_end_to_wstr(varname) ul_free((varname)); } while(0)

#define _ulfd_begin_to_wstr2(varname, str, prev_varname) do { \
  wchar_t* varname; int _ulfd_bts_err2 = ulfd_str_to_wstr_alloc(&(varname), (str)); \
  if(ul_unlikely(_ulfd_bts_err2)) { ul_free(prev_varname); return _ulfd_bts_err2; } (void)0
#define _ulfd_end_to_wstr2(varname) ul_free((varname)); } while(0)

#ifdef _WIN32
  #ifndef _UL_WIN32_TOERRNO_DEFINED
    #include <Windows.h>
    #include <errno.h>

    /* some errno codes don't exist in older compiler, so we ignore them */
    ul_hapi int _ul_win32_toerrno(DWORD error) {
      size_t i;
      static const unsigned short map[][2] = {
        { ERROR_SUCCESS,                   0 },            /*    0 */
        { ERROR_INVALID_FUNCTION,          EINVAL },       /*    1 */ /* EBADRQC */
        { ERROR_FILE_NOT_FOUND,            ENOENT },       /*    2 */
        { ERROR_PATH_NOT_FOUND,            ENOENT },       /*    3 */
        { ERROR_TOO_MANY_OPEN_FILES,       EMFILE },       /*    4 */
        { ERROR_ACCESS_DENIED,             EACCES },       /*    5 */
        { ERROR_INVALID_HANDLE,            EBADF },        /*    6 */ /* EINVAL */
        { ERROR_ARENA_TRASHED,             ENOMEM },       /*    7 */
        { ERROR_NOT_ENOUGH_MEMORY,         ENOMEM },       /*    8 */
        { ERROR_INVALID_BLOCK,             ENOMEM },       /*    9 */
        { ERROR_BAD_ENVIRONMENT,           E2BIG },        /*   10 */
        { ERROR_BAD_FORMAT,                ENOEXEC },      /*   11 */
        { ERROR_INVALID_ACCESS,            EINVAL },       /*   12 */
        { ERROR_INVALID_DATA,              EINVAL },       /*   13 */
        { ERROR_OUTOFMEMORY,               ENOMEM },       /*   14 */
        { ERROR_INVALID_DRIVE,             ENOENT },       /*   15 */
        { ERROR_CURRENT_DIRECTORY,         EACCES },       /*   16 */
        { ERROR_NOT_SAME_DEVICE,           EXDEV },        /*   17 */
        { ERROR_NO_MORE_FILES,             ENOENT },       /*   18 */ /* ENMFILE */
        { ERROR_WRITE_PROTECT,             EROFS },        /*   19 */
        { ERROR_BAD_UNIT,                  ENODEV },       /*   20 */
        { ERROR_NOT_READY,                 ENOENT },       /*   21 */ /* ENOMEDIUM */
        { ERROR_CRC,                       EIO },          /*   23 */
        { ERROR_SEEK,                      EINVAL },       /*   25 */
        { ERROR_SECTOR_NOT_FOUND,          EINVAL },       /*   27 */
        { ERROR_SHARING_VIOLATION,         EBUSY },        /*   32 */
        { ERROR_LOCK_VIOLATION,            EBUSY },        /*   33 */
        { ERROR_SHARING_BUFFER_EXCEEDED,   ENOLCK },       /*   36 */
      #if defined(ERROR_HANDLE_EOF) && defined(ENODATA)
        { ERROR_HANDLE_EOF,                ENODATA },      /*   38 */
      #endif
        { ERROR_HANDLE_DISK_FULL,          ENOSPC },       /*   39 */
        { ERROR_NOT_SUPPORTED,             ENOSYS },       /*   50 */
      /*{ ERROR_REM_NOT_LIST,              ENONET },       //   51 */
      /*{ ERROR_DUP_NAME,                  ENOTUNIQ },     //   52 */
        { ERROR_BAD_NETPATH,               ENOENT },       /*   53 */
        { ERROR_DEV_NOT_EXIST,             ENOENT },       /*   55 */
        { ERROR_BAD_NET_RESP,              ENOSYS },       /*   58 */
        { ERROR_UNEXP_NET_ERR,             EIO },          /*   59 */
        { ERROR_NETNAME_DELETED,           ENOENT },       /*   64 */
        { ERROR_NETWORK_ACCESS_DENIED,     EACCES },       /*   65 */
        { ERROR_BAD_NET_NAME,              ENOENT },       /*   67 */
        { ERROR_FILE_EXISTS,               EEXIST },       /*   80 */
        { ERROR_CANNOT_MAKE,               EACCES },       /*   82 */
        { ERROR_FAIL_I24,                  EACCES },       /*   83 */
        { ERROR_INVALID_PARAMETER,         EINVAL },       /*   87 */
        { ERROR_NO_PROC_SLOTS,             EAGAIN },       /*   89 */
        { ERROR_INVALID_AT_INTERRUPT_TIME, EINTR },        /*  104 */
        { ERROR_DRIVE_LOCKED,              EACCES },       /*  108 */
        { ERROR_BROKEN_PIPE,               EPIPE },        /*  109 */
        { ERROR_OPEN_FAILED,               EIO },          /*  110 */
        { ERROR_DISK_FULL,                 ENOSPC },       /*  112 */
        { ERROR_NO_MORE_SEARCH_HANDLES,    ENFILE },       /*  113 */
        { ERROR_INVALID_TARGET_HANDLE,     EBADF },        /*  114 */
        { ERROR_CALL_NOT_IMPLEMENTED,      ENOSYS },       /*  120 */
        { ERROR_INVALID_NAME,              ENOENT },       /*  123 */
        { ERROR_MOD_NOT_FOUND,             ENOENT },       /*  126 */
        { ERROR_PROC_NOT_FOUND,            ESRCH },        /*  127 */
        { ERROR_WAIT_NO_CHILDREN,          ECHILD },       /*  128 */
        { ERROR_CHILD_NOT_COMPLETE,        ECHILD },       /*  129 */
        { ERROR_DIRECT_ACCESS_HANDLE,      EBADF },        /*  130 */
        { ERROR_NEGATIVE_SEEK,             EINVAL },       /*  131 */
        { ERROR_SEEK_ON_DEVICE,            EACCES },       /*  132 */
        { ERROR_DIR_NOT_EMPTY,             ENOTEMPTY },    /*  145 */
        { ERROR_SIGNAL_REFUSED,            EIO },          /*  156 */
        { ERROR_NOT_LOCKED,                EACCES },       /*  158 */
        { ERROR_BAD_PATHNAME,              ENOENT },       /*  161 */
        { ERROR_SIGNAL_PENDING,            EBUSY },        /*  162 */
        { ERROR_MAX_THRDS_REACHED,         EAGAIN },       /*  164 */
        { ERROR_LOCK_FAILED,               EACCES },       /*  167 */
        { ERROR_BUSY,                      EBUSY },        /*  170 */
        { ERROR_ALREADY_EXISTS,            EEXIST },       /*  183 */
        { ERROR_INVALID_EXE_SIGNATURE,     ENOEXEC },      /*  191 */
        { ERROR_EXE_MARKED_INVALID,        ENOEXEC },      /*  192 */
        { ERROR_BAD_EXE_FORMAT,            ENOEXEC },      /*  193 */
        { ERROR_IOPL_NOT_ENABLED,          ENOEXEC },      /*  197 */
        { ERROR_NO_SIGNAL_SENT,            EIO },          /*  205 */
        { ERROR_FILENAME_EXCED_RANGE,      ENAMETOOLONG }, /*  206 */
        { ERROR_META_EXPANSION_TOO_LONG,   EINVAL },       /*  208 */
        { ERROR_INVALID_SIGNAL_NUMBER,     EINVAL },       /*  209 */
        { ERROR_THREAD_1_INACTIVE,         EINVAL },       /*  210 */
        { ERROR_NESTING_NOT_ALLOWED,       EAGAIN },       /*  215 */
        { ERROR_EXE_MACHINE_TYPE_MISMATCH, ENOEXEC },      /*  216 */
        { ERROR_BAD_PIPE,                  EINVAL },       /*  230 */
        { ERROR_PIPE_BUSY,                 EBUSY },        /*  231 */
        { ERROR_NO_DATA,                   EPIPE },        /*  232 */
      /*{ ERROR_PIPE_NOT_CONNECTED,        ECOMM },        //  233 */
      #if defined(ERROR_MORE_DATA) && defined(EMSGSIZE)
        { ERROR_MORE_DATA,                 EMSGSIZE },     /*  234 */
      #endif
        { ERROR_INVALID_EA_NAME,           EINVAL },       /*  254 */
        { ERROR_EA_LIST_INCONSISTENT,      EINVAL },       /*  255 */
      /*{ ERROR_NO_MORE_ITEMS,             ENMFILE },      //  259 */
        { ERROR_DIRECTORY,                 ENOTDIR },      /*  267 */
        { ERROR_EAS_DIDNT_FIT,             ENOSPC },       /*  275 */
        { ERROR_EA_TABLE_FULL,             ENOSPC },       /*  277 */
      #if defined(ERROR_EAS_NOT_SUPPORTED) && defined(ENOTSUP)
        { ERROR_EAS_NOT_SUPPORTED,         ENOTSUP },      /*  282 */
      #endif
        { ERROR_NOT_OWNER,                 EPERM },        /*  288 */
        { ERROR_INVALID_ADDRESS,           EINVAL },       /*  487 */
        { ERROR_PIPE_CONNECTED,            EBUSY },        /*  535 */
      /*{ ERROR_PIPE_LISTENING,            ECOMM },        //  536 */
        { ERROR_IO_INCOMPLETE,             EAGAIN },       /*  996 */
        { ERROR_IO_PENDING,                EAGAIN },       /*  997 */
        { ERROR_NOACCESS,                  EFAULT },       /*  998 */
        { ERROR_FILE_INVALID,              ENXIO },        /* 1006 */
        { ERROR_NO_TOKEN,                  EINVAL },       /* 1008 */
        { ERROR_SERVICE_REQUEST_TIMEOUT,   EBUSY },        /* 1053 */
        { ERROR_PROCESS_ABORTED,           EFAULT },       /* 1067 */
        { ERROR_END_OF_MEDIA,              ENOSPC },       /* 1100 */
        { ERROR_FILEMARK_DETECTED,         EIO },          /* 1101 */
        { ERROR_BEGINNING_OF_MEDIA,        EIO },          /* 1102 */
        { ERROR_SETMARK_DETECTED,          EIO },          /* 1103 */
        { ERROR_NO_DATA_DETECTED,          EIO },          /* 1104 */
        { ERROR_INVALID_BLOCK_LENGTH,      EIO },          /* 1106 */
        { ERROR_BUS_RESET,                 EIO },          /* 1111 */
      /*{ ERROR_NO_MEDIA_IN_DRIVE,         ENOMEDIUM },    // 1112 */
        { ERROR_NO_UNICODE_TRANSLATION,    EILSEQ },       /* 1113 */
        { ERROR_IO_DEVICE,                 EIO },          /* 1177 */
        { ERROR_EOM_OVERFLOW,              EIO },          /* 1129 */
        { ERROR_POSSIBLE_DEADLOCK,         EDEADLOCK },    /* 1131 */
        { ERROR_TOO_MANY_LINKS,            EMLINK },       /* 1142 */
        { ERROR_DEVICE_REQUIRES_CLEANING,  EIO },          /* 1165 */
        { ERROR_DEVICE_DOOR_OPEN,          EIO },          /* 1166 */
        { ERROR_CANCELLED,                 EINTR },        /* 1223 */
        { ERROR_BAD_DEVICE,                ENODEV },       /* 1200 */
      #if defined(ERROR_CONNECTION_REFUSED) && defined(ECONNREFUSED)
        { ERROR_CONNECTION_REFUSED,        ECONNREFUSED }, /* 1225 */
      #endif
        { ERROR_PRIVILEGE_NOT_HELD,        EPERM },        /* 1314 */
        { ERROR_NONE_MAPPED,               EINVAL },       /* 1332 */
        { ERROR_FILE_CORRUPT,              EEXIST },       /* 1392 */
        { ERROR_DISK_CORRUPT,              EIO },          /* 1393 */
        { ERROR_COMMITMENT_LIMIT,          EAGAIN },       /* 1445 */
        { ERROR_NO_SYSTEM_RESOURCES,       EFBIG },        /* 1450 */
        { ERROR_NONPAGED_SYSTEM_RESOURCES, EAGAIN },       /* 1451 */
        { ERROR_PAGED_SYSTEM_RESOURCES,    EAGAIN },       /* 1452 */
        { ERROR_WORKING_SET_QUOTA,         EAGAIN },       /* 1453 */
        { ERROR_PAGEFILE_QUOTA,            EAGAIN },       /* 1454 */
        { ERROR_TIMEOUT,                   EBUSY },        /* 1460 */
        { ERROR_NOT_ENOUGH_QUOTA,          ENOMEM },       /* 1816 */
        { ERROR_BAD_USERNAME,              EINVAL },       /* 2202 */
      #if defined(ERROR_NOT_CONNECTED) && defined(ENOLINK)
        { ERROR_NOT_CONNECTED,             ENOLINK },      /* 2250 */
      #endif
        { ERROR_OPEN_FILES,                EAGAIN },       /* 2401 */
        { ERROR_ACTIVE_CONNECTIONS,        EACCES },       /* 2402 */
        { ERROR_DEVICE_IN_USE,             EAGAIN },       /* 2404 */
      #if defined(ERROR_DS_GENERIC_ERROR) && defined(EIO)
        { ERROR_DS_GENERIC_ERROR,          EIO },          /* 8341 */
      #endif

      #if defined(WSAEWOULDBLOCK) && defined(EWOULDBLOCK)
        /* WinSock error codes */
        { WSAEINTR,                        EINTR },           /* 10004 */
        { WSAEBADF,                        EBADF },           /* 10009 */
        { WSAEACCES,                       EACCES },          /* 10013 */
        { WSAEFAULT,                       EFAULT },          /* 10014 */
        { WSAEINVAL,                       EINVAL },          /* 10022 */
        { WSAEMFILE,                       EMFILE },          /* 10024 */
        { WSAEWOULDBLOCK,                  EWOULDBLOCK },     /* 10035 */
        { WSAEINPROGRESS,                  EINPROGRESS },     /* 10036 */
        { WSAEALREADY,                     EALREADY },        /* 10037 */
        { WSAENOTSOCK,                     ENOTSOCK },        /* 10038 */
        { WSAEDESTADDRREQ,                 EDESTADDRREQ },    /* 10039 */
        { WSAEMSGSIZE,                     EMSGSIZE },        /* 10040 */
        { WSAEPROTOTYPE,                   EPROTOTYPE },      /* 10041 */
        { WSAENOPROTOOPT,                  ENOPROTOOPT },     /* 10042 */
        { WSAEPROTONOSUPPORT,              EPROTONOSUPPORT }, /* 10043 */
      /*{ WSAESOCKTNOSUPPORT,              ESOCKTNOSUPPORT }, // 10044 */
        { WSAEOPNOTSUPP,                   EOPNOTSUPP },      /* 10045 */
      /*{ WSAEPFNOSUPPORT,                 EPFNOSUPPORT },    // 10046 */
        { WSAEAFNOSUPPORT,                 EAFNOSUPPORT },    /* 10047 */
        { WSAEADDRINUSE,                   EADDRINUSE },      /* 10048 */
        { WSAEADDRNOTAVAIL,                EADDRNOTAVAIL },   /* 10049 */
        { WSAENETDOWN,                     ENETDOWN },        /* 10050 */
        { WSAENETUNREACH,                  ENETUNREACH },     /* 10051 */
        { WSAENETRESET,                    ENETRESET },       /* 10052 */
        { WSAECONNABORTED,                 ECONNABORTED },    /* 10053 */
        { WSAECONNRESET,                   ECONNRESET },      /* 10054 */
        { WSAENOBUFS,                      ENOBUFS },         /* 10055 */
        { WSAEISCONN,                      EISCONN },         /* 10056 */
        { WSAENOTCONN,                     ENOTCONN },        /* 10057 */
      /*{ WSAESHUTDOWN,                    ESHUTDOWN },       // 10058 */
      /*{ WSAETOOMANYREFS,                 ETOOMANYREFS },    // 10059 */
        { WSAETIMEDOUT,                    ETIMEDOUT },       /* 10060 */
        { WSAECONNREFUSED,                 ECONNREFUSED },    /* 10061 */
        { WSAELOOP,                        ELOOP },           /* 10062 */
        { WSAENAMETOOLONG,                 ENAMETOOLONG },    /* 10063 */
      /*{ WSAEHOSTDOWN,                    EHOSTDOWN },       // 10064 */
        { WSAEHOSTUNREACH,                 EHOSTUNREACH },    /* 10065 */
        { WSAENOTEMPTY,                    ENOTEMPTY },       /* 10066 */
        { WSAEPROCLIM,                     EAGAIN },          /* 10067 */
      /*{ WSAEUSERS,                       EUSERS },          // 10068 */
      /*{ WSAEDQUOT,                       EDQUOT },          // 10069 */
      /*{ WSAESTALE,                       ESTALE },          // 10070 */
      /*{ WSAEREMOTE,                      EREMOTE },         // 10071 */
      #endif

        /* Side By Side error codes */
      /*{ ERROR_SXS_CANT_GEN_ACTCTX,       ELIBBAD },         // 14001 */
      };

      for(i = 0; i < (sizeof(map) / sizeof(map[0])); ++i)
        if(map[i][0] == error) return ul_static_cast(int, map[i][1]);

      if(error >= ERROR_WRITE_PROTECT && error <= ERROR_SHARING_BUFFER_EXCEEDED)
        return EACCES; /* 19 - 36 */
      else if(error >= ERROR_INVALID_STARTING_CODESEG && error <= ERROR_INFLOOP_IN_RELOC_CHAIN)
        return ENOEXEC; /* 188 - 202 */
      else
        return EINVAL;
    }
    #define _UL_WIN32_TOERRNO_DEFINED
  #endif /* _UL_WIN32_TOERRNO_DEFINED */

  /* VC6 polyfill */
  #ifndef INVALID_SET_FILE_POINTER
    #define INVALID_SET_FILE_POINTER ul_static_cast(DWORD, -1)
  #endif
  #ifndef FILE_MAP_EXECUTE
    #define FILE_MAP_EXECUTE (0x0020)
  #endif
  #ifndef SYMBOLIC_LINK_FLAG_DIRECTORY
    #define SYMBOLIC_LINK_FLAG_DIRECTORY (0x1)
  #endif
  #ifndef SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
    #define SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE (0x2)
  #endif
  #ifndef INVALID_FILE_ATTRIBUTES
    #define INVALID_FILE_ATTRIBUTES ul_static_cast(DWORD, -1)
  #endif

  #define _ULFD_POINTER_TO_FUNCTION(T, val) ul_reinterpret_cast(T, ul_reinterpret_cast(UINT_PTR, (val)))

  #if (_WIN32_WINNT+0) >= 0x0500 /* Windows 2000 */
    #define _ulfd_SetFilePointerEx(hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod) \
      SetFilePointerEx(hFile, liDistanceToMove, lpNewFilePointer, dwMoveMethod)
    #define _ulfd_GetFileSizeEx(hFile, lpFileSize) GetFileSizeEx(hFile, lpFileSize)
  #else
    BOOL _ulfd_SetFilePointerEx(
      HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer,
      DWORD dwMoveMethod
    ) {
      LONG lDistanceToMove, lDistanceToMoveHigh;
      DWORD dwErrorCode;
      lDistanceToMove = ul_static_cast(LONG, liDistanceToMove.LowPart);
      lDistanceToMoveHigh = liDistanceToMove.HighPart;
      lDistanceToMove = SetFilePointer(hFile, lDistanceToMove, &lDistanceToMoveHigh, dwMoveMethod);
      if(lDistanceToMove == INVALID_SET_FILE_POINTER) {
        dwErrorCode = GetLastError();
        if(dwErrorCode) { SetLastError(dwErrorCode); return FALSE; }
      }
      if(lpNewFilePointer) {
        lpNewFilePointer->LowPart = lDistanceToMove;
        lpNewFilePointer->HighPart = ul_static_cast(LONG, lDistanceToMoveHigh);
      }
      return TRUE;
    }
    BOOL _ulfd_GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize) {
      DWORD dwFileSize, dwFileSizeHigh;
      DWORD dwErrorCode;
      dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
      if(dwFileSize == INVALID_SET_FILE_POINTER) {
        dwErrorCode = GetLastError();
        if(dwErrorCode) { SetLastError(dwErrorCode); return FALSE; }
      }
      lpFileSize->LowPart = dwFileSize;
      lpFileSize->HighPart = ul_static_cast(LONG, dwFileSizeHigh);
      return TRUE;
    }
  #endif

  #if (defined(_MSC_VER) && _MSC_VER < 1300)
    ul_hapi void* _ulfd_compare_exchange(void** obj, void* exchange, void* compareand) {
      return InterlockedCompareExchange(obj, exchange, compareand);
    }
  #elif defined(_WIN64)
    ul_hapi void* _ulfd_compare_exchange(void** obj, void* exchange, void* compareand) {
      return ul_reinterpret_cast(void*,
        InterlockedCompareExchange64(
          ul_reinterpret_cast(LONG64*, obj),
          ul_reinterpret_cast(LONG64, exchange),
          ul_reinterpret_cast(LONG64, compareand)
        )
      );
    }
  #else
    ul_hapi void* _ulfd_compare_exchange(void** obj, void* exchange, void* compareand) {
      return ul_reinterpret_cast(void*,
        InterlockedCompareExchange(
          ul_reinterpret_cast(LONG*, obj),
          ul_reinterpret_cast(LONG, exchange),
          ul_reinterpret_cast(LONG, compareand)
        )
      );
    }
  #endif

  ul_hapi HMODULE _ulfd_dll(HANDLE* hold, const wchar_t* name) {
    HANDLE stored;
    stored = _ulfd_compare_exchange(hold, NULL, NULL);
    if(stored == INVALID_HANDLE_VALUE) return NULL;
    if(stored != NULL) return ul_reinterpret_cast(HMODULE, stored);
    stored = GetModuleHandleW(name);
    if(stored == NULL) _ulfd_compare_exchange(hold, INVALID_HANDLE_VALUE, NULL);
    else _ulfd_compare_exchange(hold, stored, NULL);
    return ul_reinterpret_cast(HMODULE, stored);
  }

  /* get kernel32 module */
  /* For any function supported since Windows Vista or higher, if _WIN32_WINNT is smaller,
    the library will dynamically load kernel32.dll to check available */
  ul_hapi HMODULE _ulfd_kernel32(void) {
    static HANDLE hold = NULL;
    return _ulfd_dll(&hold, L"kernel32.dll");
  }
  ul_hapi HANDLE _ulfd_kernel32_function(HANDLE* hold, const char* name) {
    HANDLE stored;
    HMODULE _kernel32;
    _kernel32 = _ulfd_kernel32();
    if(_kernel32 == NULL) return NULL;
    stored = _ulfd_compare_exchange(hold, NULL, NULL);
    if(stored == INVALID_HANDLE_VALUE) return NULL;
    if(stored != NULL) return ul_reinterpret_cast(HANDLE, stored);
    stored = ul_reinterpret_cast(HANDLE, ul_reinterpret_cast(UINT_PTR, GetProcAddress(_kernel32, name)));
    if(stored == NULL) _ulfd_compare_exchange(hold, INVALID_HANDLE_VALUE, NULL);
    else _ulfd_compare_exchange(hold, stored, NULL);
    return ul_reinterpret_cast(HANDLE, stored);
  }

  typedef DWORD (WINAPI *_ulfd_GetFinalPathNameByHandleW_t)(
    HANDLE hFile, LPWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags
  );
  #ifdef ULFD_WIN32_HAS_GetFinalPathNameByHandle
    ul_hapi _ulfd_GetFinalPathNameByHandleW_t _ulfd_get_GetFinalPathNameByHandleW(void) {
      return _ULFD_POINTER_TO_FUNCTION(_ulfd_GetFinalPathNameByHandleW_t, GetFinalPathNameByHandleW);
    }
  #else
    ul_hapi _ulfd_GetFinalPathNameByHandleW_t _ulfd_get_GetFinalPathNameByHandleW(void) {
      static HANDLE hold = NULL;
      return _ULFD_POINTER_TO_FUNCTION(_ulfd_GetFinalPathNameByHandleW_t,
        _ulfd_kernel32_function(&hold, "GetFinalPathNameByHandleW"));
    }
  #endif

  ul_hapi int ulfd_open_w(ulfd_t* pfd, const wchar_t* wpath, ulfd_int32_t oflag, int mode) {
    DWORD access = 0;
    DWORD share;
    DWORD create;
    DWORD flags_attr = FILE_ATTRIBUTE_NORMAL;
    SECURITY_ATTRIBUTES security_attributes;
    HANDLE handle;

    if(oflag & ULFD_O_RDONLY) access |= GENERIC_READ;
    if(oflag & ULFD_O_WRONLY) access |= GENERIC_WRITE;
    if(oflag & ULFD_O_RDWR) access |= GENERIC_READ | GENERIC_WRITE;

    if(oflag & ULFD_O_CREAT) {
      if(oflag & ULFD_O_EXCL) create = CREATE_NEW;
      else if(oflag & ULFD_O_TRUNC) create = CREATE_ALWAYS;
      else create = OPEN_ALWAYS;
    } else {
      if(oflag & ULFD_O_TRUNC) create = TRUNCATE_EXISTING;
      else create = OPEN_EXISTING;
    }

    if(oflag & ULFD_O_SECURE) share = ul_static_cast(DWORD, access == GENERIC_READ ? FILE_SHARE_READ : 0);
    else if(oflag & ULFD_O_DENYRW) share = 0;
    else if(oflag & ULFD_O_DENYRD) share = FILE_SHARE_WRITE;
    else if(oflag & ULFD_O_DENYWR) share = FILE_SHARE_READ;
    else share = FILE_SHARE_READ | FILE_SHARE_WRITE;

    if(oflag & ULFD_O_CREAT) {
      if((mode & ULFD_S_IWRITE) == 0) flags_attr |= FILE_ATTRIBUTE_READONLY;
      if(mode & ULFD_S_IHIDDEN) flags_attr |= FILE_ATTRIBUTE_HIDDEN;
    }

    if(oflag & ULFD_O_TEMPORARY) {
      share |= FILE_SHARE_DELETE;
      access |= DELETE;
      flags_attr |= ul_static_cast(DWORD, FILE_FLAG_DELETE_ON_CLOSE | FILE_ATTRIBUTE_TEMPORARY);
    }

    security_attributes.nLength = sizeof(security_attributes);
    security_attributes.lpSecurityDescriptor = NULL;
    security_attributes.bInheritHandle = (oflag & ULFD_O_CLOEXEC) == 0;

    handle = CreateFileW(wpath, access, share, &security_attributes, create, flags_attr, NULL);
    if(handle == INVALID_HANDLE_VALUE) {
      if((access & GENERIC_WRITE) && (oflag & ULFD_O_WRONLY)) {
        access &= ~GENERIC_READ;
        handle = CreateFileW(wpath, access, share, &security_attributes, create, flags_attr, NULL);
      }
    }
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());

    if((oflag & ULFD_O_APPEND)) {
      if(SetFilePointer(handle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
        CloseHandle(handle); return _ul_win32_toerrno(GetLastError());
      }
    }

    *pfd = handle;
    return 0;
  }
  ul_hapi int ulfd_open(ulfd_t* pfd, const char* path, ulfd_int32_t oflag, int mode) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_open_w(pfd, wpath, oflag, mode);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }
  ul_hapi int ulfd_creat(ulfd_t* pfd, const char* path, int mode) {
    return ulfd_open(pfd, path, ULFD_O_CREAT | ULFD_O_TRUNC | ULFD_O_RDWR, mode);
  }
  ul_hapi int ulfd_creat_w(ulfd_t* pfd, const wchar_t* path, int mode) {
    return ulfd_open_w(pfd, path, ULFD_O_CREAT | ULFD_O_TRUNC | ULFD_O_RDWR, mode);
  }
  ul_hapi int ulfd_close(ulfd_t fd) {
    return CloseHandle(fd) ? 0 : _ul_win32_toerrno(GetLastError());
  }

  ul_hapi int ulfd_pipe(ulfd_t pfds[2]) {
    return CreatePipe(pfds + 0, pfds + 1, NULL, 0) ? 0 : _ul_win32_toerrno(GetLastError());
  }

  ul_hapi int ulfd_read(ulfd_t fd, void* buf, size_t count, size_t* pread_bytes) {
    size_t sum_read_bytes = 0;
    DWORD read_bytes;
    DWORD error = 0;
    BOOL call_ret;

  #ifdef _WIN64
    while(ul_unlikely(count > 0xFFFFFFFFu)) {
      call_ret = ReadFile(fd, buf, 0xFFFFFFFFu, &read_bytes, NULL);
      if(!call_ret) { error = GetLastError(); goto do_return; }
      if(read_bytes < 0xFFFFFFFFu) goto do_return;
      buf = ul_reinterpret_cast(char*, buf) + count;
      count -= 0xFFFFFFFFu; sum_read_bytes += 0xFFFFFFFFu;
    }
  #endif
    call_ret = ReadFile(fd, buf, ul_static_cast(DWORD, count), &read_bytes, NULL);
    if(!call_ret) error = GetLastError();

  #ifdef _WIN64
  do_return:
  #endif
    *pread_bytes = sum_read_bytes + read_bytes;
    if(error == ERROR_ACCESS_DENIED) return EBADF;
    else if(error == ERROR_BROKEN_PIPE) return 0;
    else if(error == ERROR_HANDLE_EOF) return 0;
    else return _ul_win32_toerrno(error);
  }
  ul_hapi int ulfd_write(ulfd_t fd, const void* buf, size_t count, size_t* pwriten_bytes) {
    size_t sum_writen_bytes = 0;
    DWORD writen_bytes;
    DWORD error = 0;
    BOOL call_ret;

  #ifdef _WIN64
    while(ul_unlikely(count > 0xFFFFFFFFu)) {
      call_ret = WriteFile(fd, buf, 0xFFFFFFFFu, &writen_bytes, NULL);
      if(!call_ret) { error = GetLastError(); goto do_return; }
      if(writen_bytes < 0xFFFFFFFFu) goto do_return;
      buf = ul_reinterpret_cast(const char*, buf) + count;
      count -= 0xFFFFFFFFu; sum_writen_bytes += 0xFFFFFFFFu;
    }
  #endif
    call_ret = WriteFile(fd, buf, ul_static_cast(DWORD, count), &writen_bytes, NULL);
    if(!call_ret) error = GetLastError();

  #ifdef _WIN64
  do_return:
  #endif
    if((*pwriten_bytes = sum_writen_bytes + writen_bytes) == 0 && error == 0) return ENOSPC;
    if(error == ERROR_ACCESS_DENIED) return EBADF;
    else return _ul_win32_toerrno(error);
  }

  ul_hapi int ulfd_pread(ulfd_t fd, void* buf, size_t count, ulfd_int64_t off, size_t* pread_bytes) {
    OVERLAPPED overlapped;
    size_t sum_read_bytes = 0;
    DWORD read_bytes;
    DWORD error = 0;
    BOOL call_ret;

    if(ul_unlikely(off < 0)) return EINVAL;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.Offset = ul_static_cast(DWORD, off & 0xFFFFFFFFu);
    overlapped.OffsetHigh = ul_static_cast(DWORD, off >> 32);
  #ifdef _WIN64
    while(ul_unlikely(count > 0xFFFFFFFFu)) {
      call_ret = ReadFile(fd, buf, 0xFFFFFFFFu, &read_bytes, &overlapped);
      if(!call_ret) { error = GetLastError(); goto do_return; }
      if(read_bytes < 0xFFFFFFFFu) goto do_return;
      buf = ul_reinterpret_cast(char*, buf) + count;
      count -= 0xFFFFFFFFu; sum_read_bytes += 0xFFFFFFFFu;
      if(overlapped.Offset-- == 0) ++overlapped.OffsetHigh;
    }
  #endif
    call_ret = ReadFile(fd, buf, ul_static_cast(DWORD, count), &read_bytes, &overlapped);
    if(!call_ret) error = GetLastError();

  #ifdef _WIN64
  do_return:
  #endif
    *pread_bytes = sum_read_bytes + read_bytes;
    if(error == ERROR_ACCESS_DENIED) return EBADF;
    else if(error == ERROR_BROKEN_PIPE) return 0;
    else if(error == ERROR_HANDLE_EOF) return 0;
    else return _ul_win32_toerrno(error);
  }
  ul_hapi int ulfd_pwrite(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes) {
    OVERLAPPED overlapped;
    size_t sum_writen_bytes = 0;
    DWORD writen_bytes;
    DWORD error = 0;
    BOOL call_ret;

    if(ul_unlikely(off < 0)) return EINVAL;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.Offset = ul_static_cast(DWORD, off & 0xFFFFFFFFu);
    overlapped.OffsetHigh = ul_static_cast(DWORD, off >> 32);
  #ifdef _WIN64
    while(ul_unlikely(count > 0xFFFFFFFFu)) {
      call_ret = WriteFile(fd, buf, 0xFFFFFFFFu, &writen_bytes, &overlapped);
      if(!call_ret) { error = GetLastError(); goto do_return; }
      if(writen_bytes < 0xFFFFFFFFu) goto do_return;
      buf = ul_reinterpret_cast(const char*, buf) + count;
      count -= 0xFFFFFFFFu; sum_writen_bytes += 0xFFFFFFFFu;
      if(overlapped.Offset-- == 0) ++overlapped.OffsetHigh;
    }
  #endif
    call_ret = WriteFile(fd, buf, ul_static_cast(DWORD, count), &writen_bytes, &overlapped);
    if(!call_ret) error = GetLastError();

  #ifdef _WIN64
  do_return:
  #endif
    if((*pwriten_bytes = sum_writen_bytes + writen_bytes) == 0 && error == 0) return ENOSPC;
    if(error == ERROR_ACCESS_DENIED) return EBADF;
    else return _ul_win32_toerrno(error);
  }

  ul_hapi int ulfd_seek(ulfd_t fd, ulfd_int64_t off, int origin, ulfd_int64_t* poff) {
    LARGE_INTEGER move, new_pos;
    DWORD method;

    move.QuadPart = off;
    if(origin == ULFD_SEEK_SET) method = FILE_BEGIN;
    else if(origin == ULFD_SEEK_CUR) method = FILE_CURRENT;
    else if(origin == ULFD_SEEK_END) method = FILE_END;
    else return EINVAL;

    if(!_ulfd_SetFilePointerEx(fd, move, &new_pos, method)) return _ul_win32_toerrno(GetLastError());
    *poff = new_pos.QuadPart;
    return 0;
  }
  ul_hapi int ulfd_tell(ulfd_t fd, ulfd_int64_t* poff) {
    return ulfd_seek(fd, 0, ULFD_SEEK_CUR, poff);
  }

  ul_hapi int ulfd_copy_file_range(
    ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out,
    size_t len, size_t* pcopyed
  ) {
    (void)fd_in; (void)off_in; (void)fd_out; (void)off_out; (void)len; (void)pcopyed;
    return ENOSYS;
  }

  ul_hapi int ulfd_fsync(ulfd_t fd) {
    return FlushFileBuffers(fd) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_ffullsync(ulfd_t fd) { return ulfd_fsync(fd); }
  ul_hapi int ulfd_fdatasync(ulfd_t fd) { return ulfd_fsync(fd); }

  ul_hapi int _ulfd_lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode, DWORD flags) {
    OVERLAPPED overlapped;
    DWORD error;

    if(len < 0 || off < 0) return EINVAL;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.Offset = ul_static_cast(DWORD, off);
    overlapped.OffsetHigh = ul_static_cast(DWORD, off >> 32);

    if(mode == ULFD_F_UNLCK) {
      error = UnlockFileEx(
          fd, 0,
          ul_static_cast(DWORD, len), ul_static_cast(DWORD, len >> 32),
          &overlapped
        ) ? 0 : GetLastError();
    } else {
      if(mode == ULFD_F_WRLCK) flags |= LOCKFILE_EXCLUSIVE_LOCK;
      else if(mode == ULFD_F_RDLCK) { }
      else return EINVAL;
      error = LockFileEx(
          fd, flags, 0,
          ul_static_cast(DWORD, len), ul_static_cast(DWORD, len >> 32),
          &overlapped
        ) ? 0 : GetLastError();
    }

    if(error && mode != ULFD_F_UNLCK) return EDEADLOCK;
    return _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode) {
    return _ulfd_lock(fd, off, len, mode, LOCKFILE_FAIL_IMMEDIATELY);
  }
  ul_hapi int ulfd_lockw(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode) {
    return _ulfd_lock(fd, off, len, mode, 0);
  }

  ul_hapi int ulfd_ftruncate(ulfd_t fd, ulfd_int64_t length) {
    ulfd_int64_t nul_off;
    int ret;

    ret = ulfd_seek(fd, length, ULFD_SEEK_SET, &nul_off);
    if(ret) return ret;
    return SetEndOfFile(fd) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_ffilelength(ulfd_t fd, ulfd_int64_t* plength) {
    LARGE_INTEGER length;
    if(!_ulfd_GetFileSizeEx(fd, &length)) return _ul_win32_toerrno(GetLastError());
    *plength = length.QuadPart; return 0;
  }

  ul_hapi int _ulfd_get_path_from_handle(ulfd_t fd, wchar_t** pwpath) {
    _ulfd_GetFinalPathNameByHandleW_t sysfunc;
    DWORD need_len;
    wchar_t* wpath;

    sysfunc = _ulfd_get_GetFinalPathNameByHandleW();
    if(ul_unlikely(sysfunc == NULL)) return ENOSYS;
    need_len = sysfunc(fd, NULL, 0, 0);
    wpath = ul_reinterpret_cast(wchar_t*, ul_malloc(need_len * sizeof(wchar_t)));
    if(ul_unlikely(wpath == NULL)) return ENOMEM;
    if(sysfunc(fd, NULL, 0, 0) > need_len) { ul_free(wpath); return ERANGE; }
    wpath[need_len] = 0;
    *pwpath = wpath;
    return 0;
  }
  ul_hapi int ulfd_fchmod(ulfd_t fd, ulfd_mode_t mode) {
    wchar_t* wpath;
    int ret;

    ret = _ulfd_get_path_from_handle(fd, &wpath);
    if(ret) return ret;
    ret = ulfd_chmod_w(wpath, mode);
    ul_free(wpath); return ret;
  }
  ul_hapi int ulfd_fchown(ulfd_t fd, ulfd_uid_t uid, ulfd_gid_t gid) {
    (void)fd; (void)uid; (void)gid; return ENOSYS;
  }
  ul_hapi int ulfd_futime(ulfd_t fd, ulfd_int64_t atime, ulfd_int64_t mtime) {
    wchar_t* wpath;
    int ret;

    ret = _ulfd_get_path_from_handle(fd, &wpath);
    if(ret) return ret;
    ret = ulfd_utime_w(wpath, atime, mtime);
    ul_free(wpath); return ret;
  }
  ul_hapi int ulfd_fstat(ulfd_t fd, ulfd_stat_t* state) {
    wchar_t* wpath;
    int ret;

    ret = _ulfd_get_path_from_handle(fd, &wpath);
    if(ret) return ret;
    ret = ulfd_lstat_w(wpath, state);
    ul_free(wpath); return ret;
  }
  ul_hapi int ulfd_ftype(ulfd_t fd, ulfd_mode_t* pmode) {
    wchar_t* wpath;
    int ret;

    ret = _ulfd_get_path_from_handle(fd, &wpath);
    if(ret) return ret;
    ret = ulfd_ltype_w(wpath, pmode);
    ul_free(wpath); return ret;
  }


  ul_hapi int ulfd_isatty(ulfd_t fd, int* presult) {
    DWORD ret;
    ret = GetFileType(fd);
    if(ret == FILE_TYPE_UNKNOWN) { *presult = 0; return _ul_win32_toerrno(GetLastError()); }
    *presult = (ret == FILE_TYPE_CHAR); return 0;
  }

  ul_hapi int ulfd_setstd(int which, ulfd_t fd) {
    DWORD std_handle;
    switch(which) {
    case ULFD_SET_STDIN: std_handle = STD_INPUT_HANDLE; break;
    case ULFD_SET_STDOUT: std_handle = STD_OUTPUT_HANDLE; break;
    case ULFD_SET_STDERR: std_handle = STD_ERROR_HANDLE; break;
    default: return EINVAL;
    }
    return SetStdHandle(std_handle, fd) ? 0 : _ul_win32_toerrno(GetLastError());
  }

  ul_hapi int ulfd_dup(ulfd_t* pnfd, ulfd_t ofd) {
    return DuplicateHandle(
        GetCurrentProcess(), ofd,
        GetCurrentProcess(), pnfd,
        0, TRUE, DUPLICATE_SAME_ACCESS
      ) ? 0 : _ul_win32_toerrno(GetLastError());
  }

  #include <io.h>
  #include <fcntl.h>
  ul_hapi int ulfd_fdopen(FILE** pfp, ulfd_t fd, const char* mode) {
    int cfd;
    int flags = 0;
    const char* p = mode;

    while((*p >= 9 && *p < 13) || *p == ' ') ++p;
    switch(*p++) {
    case 'r': flags = _O_RDONLY; break;
    case 'w': flags = _O_WRONLY | _O_CREAT | _O_TRUNC; break;
    case 'a': flags = _O_WRONLY | _O_CREAT | _O_APPEND; break;
    default: return EINVAL;
    }
    while(*p) {
      switch(*p) {
      case '+':
        if(flags & _O_RDWR) return EINVAL;
        flags |= _O_RDWR; flags &= ~(_O_RDONLY | _O_WRONLY); break;
      case 'b':
        if(flags & (_O_TEXT | _O_BINARY)) return EINVAL;
        flags |= _O_BINARY; break;
      case 't':
        if(flags & (_O_TEXT | _O_BINARY)) return EINVAL;
        flags |= _O_TEXT; break;
      default: break; /* we ignore other characters */
      }
    }

    cfd = _open_osfhandle(ul_reinterpret_cast(INT_PTR, fd), flags);
    if(cfd < 0) return errno;
    *pfp = _fdopen(cfd, mode);
    if(*pfp == NULL) { int err = errno; _close(cfd); return err; }
    return 0;
  }
  ul_hapi int ulfd_fdopen_w(FILE** pfp, ulfd_t fd, const wchar_t* wmode) {
    int cfd;
    int flags = 0;
    const wchar_t* p = wmode;

    while((*p >= 9 && *p < 13) || *p == L' ') ++p;
    switch(*p++) {
    case L'r': flags = _O_RDONLY; break;
    case L'w': flags = _O_WRONLY | _O_CREAT | _O_TRUNC; break;
    case L'a': flags = _O_WRONLY | _O_CREAT | _O_APPEND; break;
    default: return EINVAL;
    }
    while(*p) {
      switch(*p) {
      case L'+':
        if(flags & _O_RDWR) return EINVAL;
        flags |= _O_RDWR; flags &= ~(_O_RDONLY | _O_WRONLY); break;
      case L'b':
        if(flags & (_O_TEXT | _O_BINARY)) return EINVAL;
        flags |= _O_BINARY; break;
      case L't':
        if(flags & (_O_TEXT | _O_BINARY)) return EINVAL;
        flags |= _O_TEXT; break;
      default: break; /* we ignore other characters */
      }
    }

    cfd = _open_osfhandle(ul_reinterpret_cast(INT_PTR, fd), flags);
    if(cfd < 0) return errno;
    *pfp = _wfdopen(cfd, wmode);
    if(*pfp == NULL) { int err = errno; _close(cfd); return err; }
    return 0;
  }
  ul_hapi int ulfd_fileno(ulfd_t* pfd, FILE* fp) {
    int cfd = _fileno(fp);
    if(cfd < 0) return errno;
    *pfd = ul_reinterpret_cast(ulfd_t, _get_osfhandle(cfd));
    if(*pfd == INVALID_HANDLE_VALUE) return errno;
    return 0;
  }

  ul_hapi int ulfd_mmap(void** pmap, ulfd_t fd, void* addr, size_t len, ulfd_int64_t off, int flags) {
    DWORD off_low, off_high;
    DWORD maxsize_low, maxsize_high;
    DWORD protect, access = 0;
    HANDLE filemap, map;
    SECURITY_ATTRIBUTES security_attributes;

    if(off < 0) return EINVAL;
    off_low = ul_static_cast(DWORD, off);
    off_high = ul_static_cast(DWORD, off >> 32);
    off += len;
    maxsize_low = ul_static_cast(DWORD, off);
    maxsize_high = ul_static_cast(DWORD, off >> 32);

    security_attributes.nLength = sizeof(security_attributes);
    security_attributes.lpSecurityDescriptor = NULL;

    if((flags & (ULFD_PROT_READWRITE | ULFD_PROT_EXEC)) == ULFD_PROT_NONE)
      protect = 0;
    else {
      if((flags & ULFD_PROT_READ) == 0) return EINVAL;
      if(flags & ULFD_PROT_EXEC)
        protect = ul_static_cast(DWORD, (flags & ULFD_PROT_WRITE) != 0 ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ);
      else
        protect = ul_static_cast(DWORD, (flags & ULFD_PROT_WRITE) != 0 ? PAGE_READWRITE : PAGE_READONLY);
    }

    if(flags != ULFD_PROT_NONE) {
      if(flags & ULFD_PROT_READ) access |= FILE_MAP_READ;
      if(flags & ULFD_PROT_WRITE) access |= FILE_MAP_WRITE;
      if(flags & ULFD_PROT_EXEC) access |= FILE_MAP_EXECUTE;
    }

    if(flags & ULFD_MAP_SHARED) {
      security_attributes.bInheritHandle = TRUE;
    } else {
      if((flags & ULFD_PROT_WRITE) == 0) return EINVAL;
      security_attributes.bInheritHandle = FALSE;
      if(flags & ULFD_PROT_EXEC) protect = PAGE_EXECUTE_WRITECOPY;
      else protect = PAGE_WRITECOPY;
    }

    if(len == 0 || (flags & (ULFD_PROT_READWRITE | ULFD_PROT_EXEC)) == ULFD_PROT_EXEC) return EINVAL;
    if(flags & ULFD_MAP_ANONYMOUS) fd = INVALID_HANDLE_VALUE;

    filemap = CreateFileMappingW(fd, &security_attributes, protect, maxsize_high, maxsize_low, NULL);
    if(filemap == NULL) return _ul_win32_toerrno(GetLastError());

    if(flags & ULFD_MAP_FIXED) map = MapViewOfFile(filemap, access, off_high, off_low, len);
    else map = MapViewOfFileEx(filemap, access, off_high, off_low, len, addr);

    CloseHandle(filemap);
    if(map == NULL) return _ul_win32_toerrno(GetLastError());
    *pmap = map; return 0;
  }
  ul_hapi int ulfd_munmap(void* addr, size_t len) {
    (void)len; return UnmapViewOfFile(addr) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_mprotect(void* addr, size_t len, int prot) {
    DWORD new_protect;
    DWORD old_protect = 0;

    if((prot & (ULFD_PROT_READWRITE | ULFD_PROT_EXEC)) == ULFD_PROT_NONE)
      new_protect = 0;
    else {
      if((prot & ULFD_PROT_READ) == 0) return EINVAL;
      if(prot & ULFD_PROT_EXEC)
        new_protect = ul_static_cast(DWORD, (prot & ULFD_PROT_WRITE) ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ);
      else
        new_protect = ul_static_cast(DWORD, (prot & ULFD_PROT_WRITE) ? PAGE_READWRITE : PAGE_READONLY);
    }

    return VirtualProtect(addr, len, new_protect, &old_protect) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_msync(void* addr, size_t len, int flags) {
    (void)flags;
    return FlushViewOfFile(addr, len) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_mlock(const void* addr, size_t len) {
    return VirtualLock(ul_reinterpret_cast(LPVOID, ul_reinterpret_cast(UINT_PTR, addr)), len)
      ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_munlock(const void* addr, size_t len) {
    return VirtualUnlock(ul_reinterpret_cast(LPVOID, ul_reinterpret_cast(UINT_PTR, addr)), len)
      ? 0 : _ul_win32_toerrno(GetLastError());
  }

  typedef struct _ulfd_WIN32_MEMORY_RANGE_ENTRY {
    PVOID VirtualAddress;
    SIZE_T NumberOfBytes;
  } _ulfd_WIN32_MEMORY_RANGE_ENTRY;
  typedef BOOL (WINAPI *_ulfd_PrefetchVirtualMemory_t)(
    HANDLE hProcess, unsigned __int64 NumberOfEntries, _ulfd_WIN32_MEMORY_RANGE_ENTRY* VirtualAddresses, ULONG Flags
  );
  #ifdef ULFD_WIN32_HAS_PrefetchVirtualMemory
    ul_hapi _ulfd_PrefetchVirtualMemory_t _ulfd_get_PrefetchVirtualMemory(void) {
      return _ULFD_POINTER_TO_FUNCTION(_ulfd_PrefetchVirtualMemory_t, PrefetchVirtualMemory);
    }
  #else
    ul_hapi _ulfd_PrefetchVirtualMemory_t _ulfd_get_PrefetchVirtualMemory(void) {
      static HANDLE hold = NULL;
      return _ULFD_POINTER_TO_FUNCTION(_ulfd_PrefetchVirtualMemory_t,
        _ulfd_kernel32_function(&hold, "PrefetchVirtualMemory"));
    }
  #endif
  ul_hapi int ulfd_madvise(void* addr, size_t len, int advice) {
    if(advice == ULFD_MADV_WILLNEED) {
      _ulfd_WIN32_MEMORY_RANGE_ENTRY entry;

      _ulfd_PrefetchVirtualMemory_t desire_func;
      desire_func = _ulfd_get_PrefetchVirtualMemory();
      if(ul_unlikely(desire_func == NULL)) return ENOSYS;

      entry.VirtualAddress = addr;
      entry.NumberOfBytes = len;
      return desire_func(GetCurrentProcess(), 1, &entry, 0) ? 0 : _ul_win32_toerrno(GetLastError());
    } else if(advice == ULFD_MADV_DONTNEED) {
      return VirtualUnlock(addr, len) ? 0 : _ul_win32_toerrno(GetLastError());
    } else return EINVAL;
  }

  ul_hapi size_t ulfd_pagesize(void) {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
  }
  ul_hapi int ulfd_tmpdir_alloc_w(wchar_t** pwpath) {
    DWORD len, writen;
    wchar_t* wpath;
    len = GetTempPathW(0, NULL);
    if(len == 0) return _ul_win32_toerrno(GetLastError());
    wpath = ul_reinterpret_cast(wchar_t*, ul_malloc(len * sizeof(wchar_t)));
    if(ul_unlikely(wpath == NULL)) return ENOMEM;
    writen = GetTempPathW(len, wpath);
    if(writen > len) { ul_free(wpath); return ERANGE; }
    *pwpath = wpath; return 0;
  }
  ul_hapi int ulfd_tmpdir_alloc(char** ppath) {
    wchar_t* wpath;
    char* path;
    int err;

    err = ulfd_tmpdir_alloc_w(&wpath);
    if(err) return err;
    err = ulfd_wstr_to_str_alloc(&path, wpath);
    ul_free(wpath);
    *ppath = path;
    return err;
  }


  ul_hapi int ulfd_chdir_w(const wchar_t* wpath) {
    wchar_t rpath[ULFD_PATH_MAX + 1];
    wchar_t key[] = L"=A:";

    if(!SetCurrentDirectoryW(wpath)) return _ul_win32_toerrno(GetLastError());

    /*
      After chdir(), we need set current directory for every drive.
      For exchange, if the current directory is "D:\onedir\anotherdir", we need to set "=D:" to "D:\onedir\anotherdir".
      It aims to be compatible with old MS-DOS behaviour.
      (more details can be found in https://devblogs.microsoft.com/oldnewthing/20100506-00)
      If any error occurs, ignore it and still return 0.
    */

    if(!GetCurrentDirectoryW(sizeof(rpath) / sizeof(rpath[0]), rpath)) return 0;
    if((rpath[0] == L'\\' || rpath[0] == L'/') && rpath[0] == rpath[1]) return 0; /* "//" or "\\\\" is a UNC name */
    if(rpath[0] >= L'a' && rpath[0] <= L'z') key[1] = ul_static_cast(wchar_t, rpath[0] - L'a' + L'A');
    else if(rpath[0] >= L'A' && rpath[0] <= L'Z') key[1] = rpath[0];
    SetEnvironmentVariableW(key, rpath);
    return 0;
  }
  ul_hapi int ulfd_chdir(const char* path) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_chdir_w(wpath);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

  ul_hapi int ulfd_getcwd_w(wchar_t* wpath, size_t length) {
    DWORD adjust_length;
    adjust_length = length >= 0xFFFFFFFFu ? 0xFFFFFFFFu : ul_static_cast(DWORD, length);
    adjust_length = GetCurrentDirectoryW(adjust_length, wpath);
    if(adjust_length == 0) return _ul_win32_toerrno(GetLastError());
    return adjust_length > length ? ENOMEM : 0;
  }
  ul_hapi int ulfd_getcwd(char* path, size_t length) {
    wchar_t* wpath;
    DWORD need_len, writen_len;
    size_t cast_len;

    need_len = GetCurrentDirectoryW(0, NULL);
    if(need_len == 0) return _ul_win32_toerrno(GetLastError());
    wpath = ul_reinterpret_cast(wchar_t*, ul_malloc(need_len * sizeof(wchar_t)));
    if(ul_unlikely(wpath == NULL)) return ENOMEM;
    writen_len = GetCurrentDirectoryW(need_len, wpath);
    if(ul_unlikely(writen_len == 0)) { ul_free(wpath); return _ul_win32_toerrno(GetLastError()); }
    if(ul_unlikely(writen_len > need_len)) { ul_free(wpath); return ERANGE; }

    cast_len = ul_os_wstr_to_str_len(wpath);
    if(ul_unlikely(cast_len == 0)) { ul_free(wpath); return EILSEQ; }
    if(cast_len > length) { ul_free(wpath); return ERANGE; }
    ul_os_wstr_to_str(path, wpath);

    ul_free(wpath);
    return 0;
  }
  ul_hapi int ulfd_getcwd_alloc_w(wchar_t** pwpath) {
    DWORD need_len, writen_len;
    wchar_t* wpath;
    need_len = GetCurrentDirectoryW(0, NULL);
    if(need_len == 0) return _ul_win32_toerrno(GetLastError());
    wpath = ul_reinterpret_cast(wchar_t*, ul_malloc(need_len * sizeof(wchar_t)));
    if(ul_unlikely(wpath == NULL)) return ENOMEM;
    writen_len = GetCurrentDirectoryW(need_len, wpath);
    if(ul_unlikely(writen_len == 0)) { ul_free(wpath); return _ul_win32_toerrno(GetLastError()); }
    if(ul_unlikely(writen_len > need_len)) { ul_free(wpath); return ERANGE; }
    *pwpath = wpath; return 0;
  }
  ul_hapi int ulfd_getcwd_alloc(char** ppath) {
    wchar_t* wpath;
    char* path;
    int err;

    err = ulfd_getcwd_alloc_w(&wpath);
    if(err) return err;
    err = ulfd_wstr_to_str_alloc(&path, wpath);
    ul_free(wpath);
    *ppath = path;
    return err;
  }

  ul_hapi ulfd_int64_t _ulfd_filetime_to_time_t(const FILETIME file_time, const ulfd_int64_t fallback) {
    static const ULONGLONG EPOCH = ULFD_INT64_C(116444736000000000);
    SYSTEMTIME system_time;
    SYSTEMTIME local_time;
    FILETIME local_file_time;
    ULONGLONG time;

    if(file_time.dwLowDateTime == 0 && file_time.dwHighDateTime == 0) return fallback;
    if(
      !FileTimeToSystemTime(&file_time, &system_time) ||
      !SystemTimeToTzSpecificLocalTime(NULL, &system_time, &local_time)
    ) return fallback;

    if(!SystemTimeToFileTime(&local_time, &local_file_time)) return fallback;
    time = ul_static_cast(ULONGLONG, local_file_time.dwLowDateTime)
      | (ul_static_cast(ULONGLONG, local_file_time.dwHighDateTime) << 32);
    return ul_static_cast(ulfd_int64_t, (time - EPOCH) / 10000);
  }

  #if (_WIN32_WINNT+0) < 0x0501 /* Windows XP */
    typedef BOOL (WINAPI *_ulfd_TzSpecificLocalTimeToSystemTime_t)(
      const TIME_ZONE_INFORMATION* lpTimeZoneInformation, const SYSTEMTIME* lpLocalTime, LPSYSTEMTIME lpUniversalTime
    );
    ul_hapi _ulfd_TzSpecificLocalTimeToSystemTime_t _ulfd_get_TzSpecificLocalTimeToSystemTime(void) {
      static HANDLE hold = NULL; /* Windows XP (but VC6 don't define it) */
      return _ULFD_POINTER_TO_FUNCTION(_ulfd_TzSpecificLocalTimeToSystemTime_t,
        _ulfd_kernel32_function(&hold, "TzSpecificLocalTimeToSystemTime"));
    }
  #endif

  ul_hapi int _ulfd_time64_to_filetime(FILETIME* out, ulfd_int64_t time) {
    static const LONGLONG EPOCH = ULFD_INT64_C(116444736000000000);
    SYSTEMTIME local_time, system_time;
    FILETIME local_file_time;
  #if (_WIN32_WINNT+0) < 0x0501 /* Windows XP */
    _ulfd_TzSpecificLocalTimeToSystemTime_t sysfunc;
    sysfunc = _ulfd_get_TzSpecificLocalTimeToSystemTime();
    if(sysfunc == NULL) return -1; /* it shouldn't happen */
  #endif

    time = (time + EPOCH) * 10000;
    if(time < 0) return -1;
    local_file_time.dwLowDateTime = ul_static_cast(DWORD, time);
    local_file_time.dwHighDateTime = ul_static_cast(DWORD, time >> 32);
    if(!FileTimeToSystemTime(&local_file_time, &local_time)) return -1;
    if(
  #if (_WIN32_WINNT+0) >= 0x0501 /* Windows XP */
      !TzSpecificLocalTimeToSystemTime(NULL, &local_time, &system_time) ||
  #else
      sysfunc(NULL, &local_time, &system_time) ||
  #endif
      !SystemTimeToFileTime(&system_time, out)
    ) return -1;
    return 0;
  }

  ul_hapi ulfd_mode_t ulfd_umask(ulfd_mode_t mask) {
    (void)mask; return ULFD_S_IMASK;
  }
  ul_hapi int ulfd_lchmod_w(const wchar_t* wpath, ulfd_mode_t mode) {
    DWORD attributes = GetFileAttributesW(wpath);
    if(attributes == INVALID_FILE_ATTRIBUTES) return _ul_win32_toerrno(GetLastError());
    if((mode & ULFD_S_IWRITE)) attributes &= ul_static_cast(DWORD, ~FILE_ATTRIBUTE_READONLY);
    else attributes |= FILE_ATTRIBUTE_READONLY;
    if((mode & ULFD_S_IHIDDEN)) attributes |= FILE_ATTRIBUTE_HIDDEN;
    else attributes &= ul_static_cast(DWORD, ~FILE_ATTRIBUTE_HIDDEN);
    return SetFileAttributesW(wpath, attributes) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_lchmod(const char* path, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_lchmod_w(wpath, mode);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }
  ul_hapi int ulfd_chmod_w(const wchar_t* wpath, ulfd_mode_t mode) {
    ulfd_mode_t fmode;
    int err;
    err = ulfd_ltype_w(wpath, &fmode);
    if(err) return err;
    if(!ULFD_S_ISLNK(fmode)) return ulfd_lchmod_w(wpath, mode);
    else {
      wchar_t* wbuf;
      err = ulfd_readlink_alloc_w(&wbuf, wpath);
      if(err) return err;
      err = ulfd_lchmod_w(wbuf, mode);
      ul_free(wbuf);
      return err;
    }
  }
  ul_hapi int ulfd_chmod(const char* path, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_chmod_w(wpath, mode);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

  ul_hapi int ulfd_chown_w(const wchar_t* wpath, ulfd_uid_t uid, ulfd_gid_t gid) {
    (void)wpath; (void)uid; (void)gid; return ENOSYS;
  }
  ul_hapi int ulfd_chown(const char* path, ulfd_uid_t uid, ulfd_gid_t gid) {
    (void)path; (void)uid; (void)gid; return ENOSYS;
  }
  ul_hapi int ulfd_lchown_w(const wchar_t* wpath, ulfd_uid_t uid, ulfd_gid_t gid) {
    (void)wpath; (void)uid; (void)gid; return ENOSYS;
  }
  ul_hapi int ulfd_lchown(const char* path, ulfd_uid_t uid, ulfd_gid_t gid) {
    (void)path; (void)uid; (void)gid; return ENOSYS;
  }

  ul_hapi int _ulfd_utime(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime, DWORD flag_attr) {
    FILETIME access_time, write_time;
    HANDLE handle;
    DWORD error = 0;

    if(!_ulfd_time64_to_filetime(&access_time, atime)) return ERANGE;
    if(!_ulfd_time64_to_filetime(&write_time, mtime)) return ERANGE;
    handle = CreateFileW(wpath,
      FILE_WRITE_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL, OPEN_EXISTING, flag_attr, NULL
      );
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());
    if(!SetFileTime(handle, NULL, &access_time, &write_time)) error = GetLastError();
    CloseHandle(handle);
    return _ul_win32_toerrno(error);
  }
  ul_hapi int ulfd_utime_w(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime) {
    return _ulfd_utime(wpath, atime, mtime, FILE_FLAG_BACKUP_SEMANTICS);
  }
  ul_hapi int ulfd_utime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = _ulfd_utime(wpath, atime, mtime, FILE_FLAG_BACKUP_SEMANTICS);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }
  ul_hapi int ulfd_lutime_w(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime) {
    return _ulfd_utime(wpath, atime, mtime, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
  }
  ul_hapi int ulfd_lutime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = _ulfd_utime(wpath, atime, mtime, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

  ul_hapi int _ulfd_is_slash(wchar_t wc) { return wc == L'/' || wc == L'\\'; }
  ul_hapi int _ulfd_is_root_or_empty(const wchar_t* wpath) {
    if((*wpath >= L'A' && *wpath <= L'Z') || (*wpath >= L'a' && *wpath <= L'z')) {
      if(wpath[1] == L':') wpath += 2;
    }
    if(*wpath == L'\0') return 1;
    if(_ulfd_is_slash(*wpath) && wpath[1] == L'\0') return 1;
    return 0;
  }
  ul_hapi int _ulfd_is_executable_v1(const wchar_t* wpath) {
    size_t n;
    for(n = 0; *wpath; ++wpath) ++n;
    if(n < 4) return 0;
    if(wpath[-4] != L'.') return 0;
    switch(wpath[-3]) {
    case L'e': case L'E':
      if(wpath[-2] == L'x' || wpath[-2] == L'X') {
        if(wpath[-1] == L'e' || wpath[-1] == L'E')
          return 1; /* .exe */
      }
      break;
    case L'c': case L'C':
      switch(wpath[-2]) {
      case L'o': case L'O':
        if(wpath[-1] == L'm' || wpath[-1] == L'M') return 1; /* .com */
        break;
      case L'm': case L'M':
        if(wpath[-1] == L'd' || wpath[-1] == L'D') return 1; /* .bat */
        break;
      }
      break;
    case L'b': case L'B':
      if(wpath[-2] == L'a' || wpath[-2] == L'A') {
        if(wpath[-1] == L't' || wpath[-1] == L'T')
          return 1; /* .bat */
      }
    }
    return 0;
  }
  #include <shellapi.h>
  ul_hapi int _ulfd_is_executable(const wchar_t* wpath) {
  #if defined(_WIN32_DCOM)
    #ifdef _MSC_VER
      #pragma comment(lib, "Ole32.lib")
      #pragma comment(lib, "Shell32.lib")
    #endif
    /* If you meet compiling errors about `CoInitializeEx`, please link "Ole32.lib" and "Shell32.lib" manually */
    switch(CoInitializeEx(NULL, COINIT_MULTITHREADED)) {
    case S_OK: case S_FALSE: case RPC_E_CHANGED_MODE:
      return SHGetFileInfoW(wpath, 0, NULL, 0, SHGFI_EXETYPE) != 0;
    }
  #endif
    return _ulfd_is_executable_v1(wpath);
  }
  ul_hapi ulfd_mode_t _ulfd_stat_mode_cast(DWORD attr, const wchar_t* wpath) {
    ulfd_mode_t mode;

    if(attr & FILE_ATTRIBUTE_REPARSE_POINT)
      mode = ULFD_S_IFLNK;
    else if((attr & FILE_ATTRIBUTE_DIRECTORY) || _ulfd_is_root_or_empty(wpath))
      /* check if `wpath` is a directory or root of a disk */
      mode = ULFD_S_IFDIR | ULFD_S_IEXEC;
    else
      mode = ULFD_S_IFREG;

    if(attr & FILE_ATTRIBUTE_HIDDEN) mode |= ULFD_S_IHIDDEN;

    mode |= (attr & FILE_ATTRIBUTE_READONLY) ? ULFD_S_IREAD : ULFD_S_IREAD | ULFD_S_IWRITE;
    if(_ulfd_is_executable(wpath)) mode |= ULFD_S_IEXEC;

    /* propagate user bits to group/other fields */
    mode |= (mode & 0700) >> 3;
    mode |= (mode & 0700) >> 3;
    return mode;
  }
  ul_hapi int _ulfd_is_root_unc_name(const wchar_t* wpath) {
    size_t n;
    for(n = 0; wpath[n] && n < 5; ++n) { } /* shortest name is like `//x/y` */
    if(n < 5) return 0;
    if(!_ulfd_is_slash(wpath[0]) || !_ulfd_is_slash(wpath[1]) || _ulfd_is_slash(wpath[2])) return 0;
    for(wpath += 2; *++wpath && !_ulfd_is_slash(*wpath); ) { }
    if(wpath[0] == L'\0' || wpath[1] == L'\0') return 0;
    while(*++wpath && !_ulfd_is_slash(*wpath)) { }
    if(wpath[0] == L'\0' || wpath[1] == L'\0') return 1;
    return 0;
  }
  ul_hapi int _ulfd_is_usable_drive_or_unc_root(const wchar_t* wpath) {
    const wchar_t* ptr;
    wchar_t* full_path;
    DWORD full_path_len;

    for(ptr = wpath; *ptr; ++ptr)
      if(*ptr == L'.'  && _ulfd_is_slash(*ptr)) return 0;

    full_path_len = GetFullPathNameW(wpath, 0, NULL, NULL);
    if(full_path_len == 0) return 0;
    full_path = ul_reinterpret_cast(wchar_t*, ul_malloc(full_path_len * sizeof(wchar_t)));
    if(ul_unlikely(full_path == NULL)) return ENOMEM;
    if(ul_unlikely(GetFullPathNameW(wpath, full_path_len, full_path, NULL) != full_path_len)) {
      ul_free(full_path); return 0;
    }

    if(full_path_len != 4 && !_ulfd_is_root_unc_name(wpath)) { ul_free(full_path); return 0; }
    if(GetDriveTypeW(full_path) <= 1) { ul_free(full_path); return 0; }
    ul_free(full_path); return 1;
  }
  ul_hapi int _ulfd_getdrive(void) {
    DWORD need_len, writen_len;
    wchar_t* wpath;
    int ret;

    need_len = GetCurrentDirectoryW(0, NULL);
    if(need_len == 0) return _ul_win32_toerrno(GetLastError());
    wpath = ul_reinterpret_cast(wchar_t*, ul_malloc(need_len * sizeof(wchar_t)));
    if(ul_unlikely(wpath == NULL)) return ENOMEM;
    writen_len = GetCurrentDirectoryW(need_len, wpath);
    if(ul_unlikely(writen_len == 0)) return _ul_win32_toerrno(GetLastError());
    if(ul_unlikely(writen_len > need_len)) return ERANGE;

    if(wpath[0] == L'\0' || wpath[1] != L':') ret = 0;
    else {
      if(wpath[0] >= 'A' && wpath[0] <= 'Z') ret = wpath[0] - L'A' + 1;
      else if(wpath[0] >= 'a' && wpath[0] <= 'z') ret = wpath[0] - L'a' + 1;
      else ret = 0;
    }
    return ret;
  }
  ul_hapi int _ulfd_get_drive_number_from_path(const wchar_t* wpath, int* pdrive_number) {
    int drive_number = -1;
    if(wpath[0] >= L'A' && wpath[1] <= L'Z') drive_number = wpath[0] - L'A' + 1;
    else if(wpath[0] >= L'a' && wpath[1] <= L'z') drive_number = wpath[0] - L'a' + 1;
    if(drive_number != -1 && wpath[1] == L':') {
      if(wpath[2] == L'\0') return _ul_win32_toerrno(GetLastError());
    } else {
      drive_number = _ulfd_getdrive();
    }
    *pdrive_number = drive_number;
    return 0;
  }

  ul_hapi int _ulfd_stat(const wchar_t* wpath, ulfd_stat_t* state, DWORD flags_attr) {
    HANDLE handle;
    int ret = 0;

    memset(state, 0, sizeof(*state));
    handle = CreateFileW(
        wpath,
        FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, flags_attr, NULL
      );
    if(handle != INVALID_HANDLE_VALUE) {
      DWORD file_type = ul_static_cast(DWORD, GetFileType(handle) & ul_static_cast(DWORD, ~FILE_TYPE_REMOTE));
      if(file_type == FILE_TYPE_DISK) {
        BY_HANDLE_FILE_INFORMATION file_info;

        do {
          int drive_number;
          ret = _ulfd_get_drive_number_from_path(wpath, &drive_number);
          if(ret) goto do_return;
          state->rdev = drive_number - 1;
          state->dev = drive_number - 1;
        } while(0);

        if(!GetFileInformationByHandle(handle, &file_info)) {
          ret = _ul_win32_toerrno(GetLastError()); goto do_return;
        }
        state->nlink = ul_static_cast(ulfd_nlink_t, file_info.nNumberOfLinks);
        state->mode = _ulfd_stat_mode_cast(file_info.dwFileAttributes, wpath);
        state->mtime = _ulfd_filetime_to_time_t(file_info.ftLastWriteTime, 0);
        state->atime = _ulfd_filetime_to_time_t(file_info.ftLastAccessTime, state->mtime);
        state->ctime = _ulfd_filetime_to_time_t(file_info.ftCreationTime, state->mtime);

        state->size = ul_static_cast(
          ulfd_int64_t,
          (ul_static_cast(ULONGLONG, file_info.nFileSizeHigh) << 32) | file_info.nFileSizeLow
        );
        goto do_return;
      } else if(file_type == FILE_TYPE_CHAR || file_type == FILE_TYPE_PIPE) {
        state->mode = file_type == FILE_TYPE_CHAR ? ULFD_S_IFCHR : ULFD_S_IFIFO;
        state->nlink = 1;
        state->rdev = -1;
        state->dev = -1;
        if(file_type != FILE_TYPE_CHAR) {
          DWORD available;
          if(PeekNamedPipe(handle, NULL, 0, NULL, &available, NULL))
            state->size = ul_static_cast(ulfd_int64_t, available);
        }
        goto do_return;
      } else if(file_type == FILE_TYPE_UNKNOWN) {
        ret = EBADF; goto do_return;
      } else {
        ret = _ul_win32_toerrno(GetLastError()); goto do_return;
      }
    } else {
      int drive_number;
      if(!_ulfd_is_usable_drive_or_unc_root(wpath)) {
        ret = ENOENT; goto do_return;
      }
      state->mode = _ulfd_stat_mode_cast(FILE_ATTRIBUTE_DIRECTORY, wpath);
      state->nlink = 1;
      ret = _ulfd_get_drive_number_from_path(wpath, &drive_number);
      if(ret) goto do_return;
      state->rdev = drive_number - 1;
      state->dev = drive_number - 1;
      state->mtime = -1;
      state->atime = state->mtime;
      state->ctime = state->mtime;
      goto do_return;
    }

  do_return:
    if(handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
    return ret;
  }
  ul_hapi int ulfd_stat_w(const wchar_t* wpath, ulfd_stat_t* state) {
    return _ulfd_stat(wpath, state, FILE_FLAG_BACKUP_SEMANTICS);
  }
  ul_hapi int ulfd_stat(const char* path, ulfd_stat_t* state) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = _ulfd_stat(wpath, state, FILE_FLAG_BACKUP_SEMANTICS);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }
  ul_hapi int ulfd_lstat_w(const wchar_t* wpath, ulfd_stat_t* state) {
    return _ulfd_stat(wpath, state, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
  }
  ul_hapi int ulfd_lstat(const char* path, ulfd_stat_t* state) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = _ulfd_stat(wpath, state, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

  ul_hapi int _ulfd_type(const wchar_t* wpath, ulfd_mode_t* pmode, DWORD flags_attr) {
    HANDLE handle;
    int ret = 0;

    handle = CreateFileW(
        wpath,
        FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, flags_attr, NULL
      );
    if(handle != INVALID_HANDLE_VALUE) {
      DWORD file_type = ul_static_cast(DWORD, GetFileType(handle) & ul_static_cast(DWORD, ~FILE_TYPE_REMOTE));
      if(file_type == FILE_TYPE_DISK) {
        BY_HANDLE_FILE_INFORMATION file_info;
        if(!GetFileInformationByHandle(handle, &file_info)) {
          ret = _ul_win32_toerrno(GetLastError());
        } else {
          if(file_info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            *pmode = ULFD_S_IFLNK;
          else if((file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || _ulfd_is_root_or_empty(wpath))
            /* check if `wpath` is a directory or root of a disk */
            *pmode = ULFD_S_IFDIR | ULFD_S_IEXEC;
          else
            *pmode = ULFD_S_IFREG;
          if(file_info.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) *pmode |= ULFD_S_IHIDDEN;
        }
      } else if(file_type == FILE_TYPE_CHAR || file_type == FILE_TYPE_PIPE) {
        *pmode = file_type == FILE_TYPE_CHAR ? ULFD_S_IFCHR : ULFD_S_IFIFO;
      } else if(file_type == FILE_TYPE_UNKNOWN) {
        ret = EBADF;
      } else {
        ret = _ul_win32_toerrno(GetLastError());
      }
      CloseHandle(handle);
    } else {
      if(!_ulfd_is_usable_drive_or_unc_root(wpath)) ret = ENOENT;
      else *pmode = _ulfd_stat_mode_cast(FILE_ATTRIBUTE_DIRECTORY, wpath);
    }
    return ret;
  }
  ul_hapi int ulfd_type_w(const wchar_t* wpath, ulfd_mode_t* pmode) {
    return _ulfd_type(wpath, pmode, FILE_FLAG_BACKUP_SEMANTICS);
  }
  ul_hapi int ulfd_type(const char* path, ulfd_mode_t* pmode) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = _ulfd_type(wpath, pmode, FILE_FLAG_BACKUP_SEMANTICS);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }
  ul_hapi int ulfd_ltype_w(const wchar_t* wpath, ulfd_mode_t* pmode) {
    return _ulfd_type(wpath, pmode, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
  }
  ul_hapi int ulfd_ltype(const char* path, ulfd_mode_t* pmode) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = _ulfd_type(wpath, pmode, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

  ul_hapi int ulfd_access_w(const wchar_t* wpath, ulfd_mode_t mode) {
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    if(!GetFileAttributesExW(wpath, GetFileExInfoStandard, &attributes))
      return _ul_win32_toerrno(GetLastError());
    if(attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return 0;
    if((attributes.dwFileAttributes & FILE_ATTRIBUTE_READONLY) && (mode & ULFD_S_IWRITE))
      return EACCES;
    return 0;
  }
  ul_hapi int ulfd_access(const char* path, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_access_w(wpath, mode);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

  ul_hapi int _ulfd_emptydir(const wchar_t* dir, int* pempty) {
    /* "dir" must end with "\\*" */
    HANDLE handle;
    WIN32_FIND_DATAW data;
    int err;
    char meet_dot = 0, meet_dotdot = 0;
    handle = FindFirstFileW(dir, &data);
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());
    for(;;) { /* skip L"." and L".." */
      if(data.cFileName[0] != L'.') break;
      if(data.cFileName[1] == 0) { meet_dot = 1; }
      else if(data.cFileName[1] == L'.' && data.cFileName[2] == 0) { meet_dotdot = 1; }
      else break;
      if(meet_dot && meet_dotdot) break;
      if(!FindNextFileW(handle, &data)) {
        FindClose(handle);
        return _ul_win32_toerrno(GetLastError());
      }
    }
    if(!FindNextFileW(handle, &data)) {
      DWORD error = GetLastError();
      if(error == ERROR_NO_MORE_FILES) { *pempty = 1; err = 0; }
      else err = _ul_win32_toerrno(error);
    } else { *pempty = 0; err = 0; }
    FindClose(handle);
    return err;
  }
  ul_hapi int ulfd_empty_w(const wchar_t* wpath, int* pempty) {
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    if(!GetFileAttributesExW(wpath[0] ? wpath : L".", GetFileExInfoStandard, &attributes))
      return _ul_win32_toerrno(GetLastError());
    if(attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      wchar_t* dir;
      size_t len;
      int err;

      len = ulfd_wcslen(wpath);
      dir = ul_reinterpret_cast(wchar_t*, ul_malloc((len + 4) * sizeof(wchar_t)));
      if(ul_unlikely(dir == NULL)) return ENOMEM;
      if(len == 0) dir[len++] = L'.';
      if(!_ulfd_is_slash(dir[len - 1])) dir[len++] = L'\\';
      dir[len++] = L'*'; dir[len] = 0;
      err = _ulfd_emptydir(dir, pempty);
      ul_free(dir); return err;
    }
    *pempty = attributes.nFileSizeHigh == 0 && attributes.nFileSizeLow == 0;
    return 0;
  }
  ul_hapi int ulfd_empty(const char* path, int* pempty) {
    wchar_t* dir;
    size_t len;
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    int err;

    len = ul_os_str_to_wstr_len(path);
    if(ul_unlikely(len == 0)) return EILSEQ;
    --len;
    dir = ul_reinterpret_cast(wchar_t*, ul_malloc((len + 4) * sizeof(wchar_t)));
    if(ul_unlikely(len == 0)) { dir[len++] = L'.'; dir[len] = 0; }
    else ul_os_str_to_wstr(dir, path);

    if(!GetFileAttributesExW(dir, GetFileExInfoStandard, &attributes))
      err = _ul_win32_toerrno(GetLastError());
    else {
      if(attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if(!_ulfd_is_slash(dir[len - 1])) dir[len++] = L'\\';
        dir[len++] = L'*'; dir[len] = 0;
        err = _ulfd_emptydir(dir, pempty);
      } else {
        *pempty = attributes.nFileSizeHigh == 0 && attributes.nFileSizeLow == 0;
        err = 0;
      }
    }
    ul_free(dir);
    return err;
  }

  ul_hapi int ulfd_rename_w(const wchar_t* newpath, const wchar_t* oldpath) {
    return MoveFileExW(oldpath, newpath, MOVEFILE_COPY_ALLOWED) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_rename(const char* newpath, const char* oldpath) {
    int ret;
    _ulfd_begin_to_wstr(_newpath, newpath);
    _ulfd_begin_to_wstr2(_oldpath, oldpath, _newpath);
    ret = ulfd_rename_w(_newpath, _oldpath);
    _ulfd_end_to_wstr2(_oldpath);
    _ulfd_end_to_wstr(_newpath);
    return ret;
  }
  ul_hapi int ulfd_unlink_w(const wchar_t* wpath) {
    DWORD attr, error;
    if(DeleteFileW(wpath)) return 0;
    attr = GetFileAttributesW(wpath);
    if(attr == INVALID_FILE_ATTRIBUTES) return _ul_win32_toerrno(GetLastError());
    if(attr & FILE_ATTRIBUTE_READONLY) {
      if(!SetFileAttributesW(wpath, ul_static_cast(DWORD, attr & ul_static_cast(DWORD, ~FILE_ATTRIBUTE_READONLY))))
        return _ul_win32_toerrno(GetLastError());
    }
    if(DeleteFileW(wpath)) return 0;
    error = GetLastError();
    if(attr & FILE_ATTRIBUTE_READONLY) /* we cannot remove it, so we have to restore attribute */
      SetFileAttributesW(wpath, attr);
    return _ul_win32_toerrno(error);
  }
  ul_hapi int ulfd_unlink(const char* path) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_unlink_w(wpath);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }
  ul_hapi int ulfd_remove_w(const wchar_t* wpath) {
    DWORD attr, error;
    attr = GetFileAttributesW(wpath);
    if(attr == INVALID_FILE_ATTRIBUTES) return _ul_win32_toerrno(GetLastError());
    if(attr & FILE_ATTRIBUTE_READONLY) {
      if(!SetFileAttributesW(wpath, ul_static_cast(DWORD, attr & ul_static_cast(DWORD, ~FILE_ATTRIBUTE_READONLY))))
        return _ul_win32_toerrno(GetLastError());
    }
    if(attr & FILE_ATTRIBUTE_DIRECTORY) { if(RemoveDirectoryW(wpath)) return 0; }
    else { if(DeleteFileW(wpath)) return 0; }
    error = GetLastError();
    if(attr & FILE_ATTRIBUTE_READONLY) /* we cannot remove it, so we have to restore attribute */
      SetFileAttributesW(wpath, attr);
    return _ul_win32_toerrno(error);
  }
  ul_hapi int ulfd_remove(const char* path) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_remove_w(wpath);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

  ul_hapi int ulfd_mkdir_w(const wchar_t* wpath, ulfd_mode_t mode) {
    if(!CreateDirectoryW(wpath, NULL)) return _ul_win32_toerrno(GetLastError());
    if(mode & ULFD_S_IHIDDEN) {
      DWORD attr = GetFileAttributesW(wpath);
      if(attr == INVALID_FILE_ATTRIBUTES) return _ul_win32_toerrno(GetLastError());
      attr |= FILE_ATTRIBUTE_HIDDEN;
      if(!SetFileAttributesW(wpath, attr)) return _ul_win32_toerrno(GetLastError());
    }
    return 0;
  }
  ul_hapi int ulfd_mkdir(const char* path, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_mkdir_w(wpath, mode);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }
  ul_hapi int ulfd_rmdir_w(const wchar_t* wpath) {
    DWORD attr, error;
    if(RemoveDirectoryW(wpath)) return 0;
    attr = GetFileAttributesW(wpath);
    if(attr == INVALID_FILE_ATTRIBUTES) return _ul_win32_toerrno(GetLastError());
    if(attr & FILE_ATTRIBUTE_READONLY) {
      if(!SetFileAttributesW(wpath, ul_static_cast(DWORD, attr & ul_static_cast(DWORD, ~FILE_ATTRIBUTE_READONLY))))
        return _ul_win32_toerrno(GetLastError());
    }
    if(RemoveDirectoryW(wpath)) return 0;
    error = GetLastError();
    if(attr & FILE_ATTRIBUTE_READONLY) /* we cannot remove it, so we have to restore attribute */
      SetFileAttributesW(wpath, attr);
    return _ul_win32_toerrno(error);
  }
  ul_hapi int ulfd_rmdir(const char* path) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_rmdir_w(wpath);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

  #ifdef ULFD_WIN32_HAS_CreateHardLink
    ul_hapi int ulfd_link_w(const wchar_t* newpath, const wchar_t* oldpath) {
      return CreateHardLinkW(newpath, oldpath, NULL) ? 0 : _ul_win32_toerrno(GetLastError());
    }
  #else
    /* I don't known why VC6 don't define it? */
    typedef BOOL (WINAPI *_ulfd_CreateHardLinkW_t)(
      LPCWSTR lpFileName, LPCWSTR lpExistingFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
    ul_hapi _ulfd_CreateHardLinkW_t _ulfd_get_CreateHardLinkW(void) {
      static HANDLE hold = NULL;
      return _ULFD_POINTER_TO_FUNCTION(_ulfd_CreateHardLinkW_t, _ulfd_kernel32_function(&hold, "CreateHardLinkW"));
    }
    ul_hapi int ulfd_link_w(const wchar_t* newpath, const wchar_t* oldpath) {
      _ulfd_CreateHardLinkW_t sysfunc = _ulfd_get_CreateHardLinkW();
      if(sysfunc == NULL) return ENOSYS;
      return sysfunc(newpath, oldpath, NULL) ? 0 : _ul_win32_toerrno(GetLastError());
    }
  #endif
  ul_hapi int ulfd_link(const char* newpath, const char* oldpath) {
    int ret;
    _ulfd_begin_to_wstr(w_newpath, newpath);
    _ulfd_begin_to_wstr2(w_oldpath, oldpath, w_newpath);
    ret = ulfd_link_w(w_newpath, w_oldpath);
    _ulfd_end_to_wstr2(w_oldpath);
    _ulfd_end_to_wstr(w_newpath);
    return ret;
  }

  typedef BOOLEAN (WINAPI *_ulfd_CreateSymbolicLinkW_t)(
    LPCWSTR lpSymlinkFileName, LPCWSTR lpTargetFileName, DWORD dwFlags
  );
  #ifdef ULFD_WIN32_HAS_CreateSymbolicLink
    ul_hapi _ulfd_CreateSymbolicLinkW_t _ulfd_get_CreateSymbolicLinkW(void) {
      return _ULFD_POINTER_TO_FUNCTION(_ulfd_CreateSymbolicLinkW_t, CreateSymbolicLinkW);
    }
  #else
    ul_hapi _ulfd_CreateSymbolicLinkW_t _ulfd_get_CreateSymbolicLinkW(void) {
      static HANDLE hold = NULL;
      return _ULFD_POINTER_TO_FUNCTION(_ulfd_CreateSymbolicLinkW_t,
        _ulfd_kernel32_function(&hold, "CreateSymbolicLinkW"));
    }
  #endif
  ul_hapi int ulfd_symlink_w(const wchar_t* target, const wchar_t* source) {
    DWORD attribute;
    _ulfd_CreateSymbolicLinkW_t sysfunc;

    sysfunc = _ulfd_get_CreateSymbolicLinkW();
    if(ul_unlikely(sysfunc == NULL)) return ENOSYS;

    attribute = GetFileAttributesW(source);
    if(attribute == INVALID_FILE_ATTRIBUTES) return _ul_win32_toerrno(GetLastError());
    if(!sysfunc(
        target, source,
        ul_static_cast(DWORD, ((attribute & FILE_ATTRIBUTE_DIRECTORY) ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0)
          | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
        )
      )
    ) return _ul_win32_toerrno(GetLastError());
    return 0;
  }
  ul_hapi int ulfd_symlink(const char* target, const char* source) {
    int ret;
    _ulfd_begin_to_wstr(w_target, target);
    _ulfd_begin_to_wstr2(w_source, source, w_target);
    ret = ulfd_symlink_w(w_target, w_source);
    _ulfd_end_to_wstr2(w_source);
    _ulfd_end_to_wstr(w_target);
    return ret;
  }

  ul_hapi int ulfd_readlink_w(const wchar_t* wpath, wchar_t* buf, size_t len) {
    HANDLE handle;
    DWORD adjust_len;
    DWORD writen, error = 0;
    _ulfd_GetFinalPathNameByHandleW_t sysfunc;

    handle = CreateFileW(
        wpath,
        FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL
      );
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());

    sysfunc = _ulfd_get_GetFinalPathNameByHandleW();
    if(ul_unlikely(sysfunc == NULL)) return ENOSYS;

    adjust_len = len >= 0xFFFFFFFFu ? 0xFFFFFFFFu : ul_static_cast(DWORD, len);
    writen = sysfunc(handle, buf, adjust_len, 0);
    if(writen == 0) error = GetLastError();
    CloseHandle(handle);

    if(writen == 0) return _ul_win32_toerrno(error);
    if(writen >= len) return ERANGE;
    buf[writen] = 0; return 0;
  }
  ul_hapi int ulfd_readlink_alloc_w(wchar_t** pwbuf, const wchar_t* wpath) {
    HANDLE handle;
    _ulfd_GetFinalPathNameByHandleW_t sysfunc;
    DWORD len;
    int err = 0;
    wchar_t* dir = NULL;

    handle = CreateFileW(
        wpath,
        FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL
      );
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());

    sysfunc = _ulfd_get_GetFinalPathNameByHandleW();
    if(ul_unlikely(sysfunc == NULL)) return ENOSYS;

    len = sysfunc(handle, NULL, 0, 0);
    if(len == 0) return _ul_win32_toerrno(GetLastError());
    dir = ul_reinterpret_cast(wchar_t*, malloc((len + 1) * sizeof(wchar_t)));
    if(ul_unlikely(dir == NULL)) err = ENOMEM;
    else {
      DWORD writen = sysfunc(handle, dir, ul_static_cast(DWORD, len), 0);
      if(ul_unlikely(writen == 0)) err = _ul_win32_toerrno(GetLastError());
      else if(writen > len) err = ERANGE;
    }
    CloseHandle(handle);
    if(err) { ul_free(dir); return err; }
    else { *pwbuf = dir; return 0; }
  }
  ul_hapi int ulfd_readlink(const char* path, char* buf, size_t len) {
    wchar_t* wbuf;
    int err;
    size_t cast_len;

    _ulfd_begin_to_wstr(wpath, path);
    err = ulfd_readlink_alloc_w(&wbuf, wpath);
    _ulfd_end_to_wstr(wpath);
    if(err) return err;

    cast_len = ul_os_wstr_to_str_len(wbuf);
    if(ul_unlikely(cast_len == 0)) { ul_free(wbuf); return EILSEQ; }
    if(cast_len > len) { ul_free(wbuf); return ERANGE; }
    ul_os_wstr_to_str(buf, wbuf);
    ul_free(wbuf); return 0;
  }
  ul_hapi int ulfd_readlink_alloc(char** pbuf, const char* path) {
    wchar_t* wbuf;
    int err;

    _ulfd_begin_to_wstr(wpath, path);
    err = ulfd_readlink_alloc_w(&wbuf, wpath);
    _ulfd_end_to_wstr(wpath);
    if(err) return err;

    err = ulfd_wstr_to_str_alloc(pbuf, wbuf);
    ul_free(wbuf);
    return err;
  }

  ul_hapi int ulfd_realpath_alloc_w(wchar_t** presolved, const wchar_t* wpath) {
    DWORD len, writen;
    wchar_t* resolved;
    if(wpath[0] == '\0') return ulfd_getcwd_alloc_w(presolved);
    len = GetFullPathNameW(wpath, 0, 0, NULL);
    if(len == 0) return _ul_win32_toerrno(GetLastError());
    resolved = ul_reinterpret_cast(wchar_t*, ul_malloc(len * sizeof(wchar_t)));
    if(ul_unlikely(resolved == NULL)) return ENOMEM;
    writen = GetFullPathNameW(wpath, len, resolved, NULL);
    if(writen > len) { ul_free(resolved); return ERANGE; }
    *presolved = resolved; return 0;
  }
  ul_hapi int ulfd_realpath_alloc(char** presolved, const char* path) {
    wchar_t* wresolved;
    int ret;

    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_realpath_alloc_w(&wresolved, wpath);
    _ulfd_end_to_wstr(wpath);
    if(ret) return ret;

    ret = ulfd_wstr_to_str_alloc(presolved, wresolved);
    ul_free(wresolved);
    return ret;
  }

  ul_hapi int ulfd_truncate_w(const wchar_t* wpath, ulfd_int64_t size) {
    HANDLE handle;
    LARGE_INTEGER sz;
    int err;

    handle = CreateFileW(wpath,
      GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());
    sz.QuadPart = size;
    if(!_ulfd_SetFilePointerEx(handle, sz, NULL, FILE_BEGIN)) return _ul_win32_toerrno(GetLastError());

    err = SetEndOfFile(handle) ? 0 : _ul_win32_toerrno(GetLastError());
    CloseHandle(handle); return err;
  }
  ul_hapi int ulfd_truncate(const char* path, ulfd_int64_t size) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_truncate_w(wpath, size);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }


  ul_hapi int ulfd_rewinddir(ulfd_dir_t* dir) {
    char meet_dot = 0, meet_dotdot = 0;
    if(ul_likely(dir->handle != INVALID_HANDLE_VALUE)) FindClose(dir->handle);

    dir->cached = 1;
    dir->handle = FindFirstFileW(dir->dirpath, &dir->data);
    if(dir->handle == INVALID_HANDLE_VALUE) {
      DWORD error = GetLastError();
      return _ul_win32_toerrno(error);
    }

    for(;;) { /* skip L"." and L".." */
      if(dir->data.cFileName[0] != L'.') break;
      if(dir->data.cFileName[1] == 0) { meet_dot = 1; }
      else if(dir->data.cFileName[1] == L'.' && dir->data.cFileName[2] == 0) { meet_dotdot = 1; }
      else break;

      if(meet_dot && meet_dotdot) { dir->cached = 0; break; }
      if(!FindNextFileW(dir->handle, &dir->data)) {
        FindClose(dir->handle); dir->handle = INVALID_HANDLE_VALUE;
        return _ul_win32_toerrno(GetLastError());
      }
    }
    return 0;
  }
  ul_hapi int ulfd_opendir(ulfd_dir_t* dir, const char* path) {
    wchar_t* dirpath = NULL;
    size_t len;
    int err;

    dir->dirpath = NULL;
    dir->entry = NULL;
    dir->entry_cap = 0;
    dir->handle = INVALID_HANDLE_VALUE;

    len = ul_os_str_to_wstr_len(path);
    if(ul_unlikely(len == 0)) return EILSEQ;
    --len;
    dirpath = ul_reinterpret_cast(wchar_t*, ul_malloc((len + 4) * sizeof(wchar_t)));
    if(ul_unlikely(dirpath == NULL)) return ENOMEM;
    if(ul_unlikely(path[0] == 0)) dirpath[len++] = L'.';
    else ul_os_str_to_wstr(dirpath, path);
    if(!_ulfd_is_slash(dirpath[len - 1])) dirpath[len++] = L'\\';
    dirpath[len++] = L'*'; dirpath[len] = 0;
    dir->dirpath = dirpath;

    err = ulfd_rewinddir(dir);
    if(err) {
      ul_free(dir->dirpath);
      dir->dirpath = NULL;
      return err;
    }
    return 0;
  }
  ul_hapi int ulfd_opendir_w(ulfd_dir_t* dir, const wchar_t* wpath) {
    wchar_t* dirpath = NULL;
    size_t len;
    int err;

    dir->dirpath = NULL;
    dir->entry = NULL;
    dir->entry_cap = 0;
    dir->handle = INVALID_HANDLE_VALUE;

    len = ulfd_wcslen(wpath);
    if(ul_unlikely(len == 0)) return ENOENT;
    dirpath = ul_reinterpret_cast(wchar_t*, ul_malloc((len + 4) * sizeof(wchar_t)));
    if(ul_unlikely(dirpath == NULL)) return ENOMEM;
    if(ul_unlikely(wpath[0] == 0)) dirpath[len++] = L'.';
    else memcpy(dirpath, wpath, len * sizeof(wchar_t));
    if(!_ulfd_is_slash(dirpath[len - 1])) dirpath[len++] = L'\\';
    dirpath[len++] = L'*'; dirpath[len] = 0;
    dir->dirpath = dirpath;

    err = ulfd_rewinddir(dir);
    if(err) {
      ul_free(dir->dirpath);
      dir->dirpath = NULL;
      return err;
    }
    return 0;
  }
  ul_hapi int ulfd_closedir(ulfd_dir_t* dir) {
    if(ul_likely(dir->dirpath)) ul_free(dir->dirpath);
    if(dir->entry) ul_free(dir->entry);
    if(ul_unlikely(dir->handle == INVALID_HANDLE_VALUE)) return 0;
    return FindClose(dir->handle) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_readdir(ulfd_dir_t* dir, const char** pname) {
    size_t cast_len;

    if(ul_unlikely(dir->handle == INVALID_HANDLE_VALUE)) return EBADF;
    if(ul_unlikely(dir->cached)) dir->cached = 0;
    else {
      if(FindNextFileW(dir->handle, &dir->data) == FALSE) {
        DWORD error = GetLastError();
        if(ul_likely(error == ERROR_NO_MORE_FILES)) { *pname = NULL; return 0; }
        return _ul_win32_toerrno(error);
      }
    }

    cast_len = ul_os_wstr_to_str_len(dir->data.cFileName);
    if(ul_unlikely(cast_len == 0)) return EILSEQ;
    if(cast_len > dir->entry_cap) {
      char* entry;
      size_t cap2 = dir->entry_cap + (dir->entry_cap >> 1) + 1;
      if(cap2 < cast_len) cap2 = cast_len;
      entry = ul_reinterpret_cast(char*, ul_realloc(dir->entry, cap2));
      if(ul_unlikely(entry == NULL)) return ENOMEM;
      dir->entry = entry;
    }
    ul_os_wstr_to_str(dir->entry, dir->data.cFileName);
    *pname = dir->entry;
    return 0;
  }
  ul_hapi int ulfd_readdir_w(ulfd_dir_t* dir, const wchar_t** pwname) {
    if(ul_unlikely(dir->handle == INVALID_HANDLE_VALUE)) return EBADF;
    if(ul_unlikely(dir->cached)) dir->cached = 0;
    else {
      if(FindNextFileW(dir->handle, &dir->data) == FALSE) {
        DWORD error = GetLastError();
        if(ul_likely(error == ERROR_NO_MORE_FILES)) { *pwname = NULL; return 0; }
        return _ul_win32_toerrno(error);
      }
    }
    *pwname = dir->data.cFileName;
    return 0;
  }


  ul_hapi int _ulfd_space(ulfd_spaceinfo_t* info, const wchar_t* wpath) {
    ULARGE_INTEGER avail, total, free;
    if(!GetDiskFreeSpaceExW(wpath, &avail, &total, &free)) return _ul_win32_toerrno(GetLastError());
    info->available = ul_static_cast(ulfd_uint64_t, avail.QuadPart);
    info->capacity = ul_static_cast(ulfd_uint64_t, total.QuadPart);
    info->free = ul_static_cast(ulfd_uint64_t, free.QuadPart);
    return 0;
  }
  ul_hapi int ulfd_space_w(ulfd_spaceinfo_t* info, const wchar_t* wpath) {
    ulfd_stat_t state;
    int err;
    wchar_t* resolved;
    size_t len;

    err = ulfd_stat_w(wpath, &state);
    if(err) return err;
    if(ULFD_S_ISDIR(state.mode)) return _ulfd_space(info, wpath);

    err = ulfd_realpath_alloc_w(&resolved, wpath);
    if(err) return err;
    len = ulfd_wcslen(resolved);
    if(ul_unlikely(_ulfd_is_slash(resolved[len - 1]) || resolved[len - 1] == L':')) {
      ul_free(resolved); return EINVAL;
    }
    do { --len; } while(len > 0 && !_ulfd_is_slash(resolved[len - 1]) && resolved[len - 1] != L':');
    if(ul_unlikely(len == 0)) { ul_free(resolved); return _ulfd_space(info, L".\\"); }
    else {
      if(ul_unlikely(resolved[len - 1] == L':')) resolved[len++] = L'\\';
      resolved[len] = 0;
    }
    err = _ulfd_space(info, resolved);
    ul_free(resolved);
    return err;
  }
  int ulfd_space(ulfd_spaceinfo_t* info, const char* path) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_space_w(info, wpath);
    _ulfd_end_to_wstr(wpath);
    return ret;
  }

#else
  #include <fcntl.h>
  #include <sys/stat.h>

  #define _ulfd_to_access_mode(mode) ul_static_cast(mode_t, (mode) & ULFD_S_IMASK)
  #define _ulfd_from_access_mode(mode) ul_static_cast(ulfd_mode_t, (mode) & ULFD_S_IMASK)
  ul_hapi mode_t _ulfd_to_full_mode(ulfd_mode_t mode) {
    mode_t ret = ul_static_cast(mode_t, mode & 0777);
    switch(mode & ULFD_S_IFMT) {
    case ULFD_S_IFSOCK: ret |= S_IFSOCK; break;
    case ULFD_S_IFLNK: ret |= S_IFLNK; break;
    case ULFD_S_IFREG: ret |= S_IFREG; break;
    case ULFD_S_IFBLK: ret |= S_IFBLK; break;
    case ULFD_S_IFDIR: ret |= S_IFDIR; break;
    case ULFD_S_IFCHR: ret |= S_IFCHR; break;
    case ULFD_S_IFIFO: ret |= S_IFIFO; break;
    default: break;
    }
    return ret;
  }
  ul_hapi ulfd_mode_t _ulfd_from_full_mode(mode_t mode) {
    ulfd_mode_t ret = ul_static_cast(ulfd_mode_t, mode & 0777);
    switch(mode & S_IFMT) {
    case S_IFSOCK: ret |= ULFD_S_IFSOCK; break;
    case S_IFLNK: ret |= ULFD_S_IFLNK; break;
    case S_IFREG: ret |= ULFD_S_IFREG; break;
    case S_IFBLK: ret |= ULFD_S_IFBLK; break;
    case S_IFDIR: ret |= ULFD_S_IFDIR; break;
    case S_IFCHR: ret |= ULFD_S_IFCHR; break;
    case S_IFIFO: ret |= ULFD_S_IFIFO; break;
    default: break;
    }
    return ret;
  }


  ul_hapi int ulfd_open(ulfd_t* pfd, const char* path, ulfd_int32_t oflag, ulfd_mode_t mode) {
    int flag = 0;
    int fd;

    if(oflag & ULFD_O_RDONLY) flag |= O_RDONLY;
    if(oflag & ULFD_O_WRONLY) flag |= O_WRONLY;
    if(oflag & ULFD_O_RDWR) flag |= O_RDWR;

    if(oflag & ULFD_O_CREAT) flag |= O_CREAT;
    if(oflag & ULFD_O_EXCL) flag |= O_EXCL;
    if(oflag & ULFD_O_TRUNC) flag |= O_TRUNC;

    if(oflag & ULFD_O_APPEND) flag |= O_APPEND;
  #ifdef O_CLOEXEC
    if(oflag & ULFD_O_CLOEXEC) flag |= O_CLOEXEC;
  #else
    return EINVAL;
  #endif

    if(oflag & ULFD_O_NONBLOCK) flag |= O_NONBLOCK;
    if(oflag & ULFD_O_NOCTTY) flag |= O_NOCTTY;

  #if defined(ULFD_HAS_LFS) && defined(O_LARGEFILE)
    #if !defined(_FILE_OFFSET_BITS) || _FILE_OFFSET_BITS != 64
      flag |= O_LARGEFILE;
    #endif
  #endif

    fd = open(path, flag, _ulfd_to_access_mode(mode));

    if(fd < 0) return errno;
    if(oflag & ULFD_O_TEMPORARY) {
      if(unlink(path) < 0) {
        close(fd); return errno;
      }
    }
    *pfd = fd;
    return 0;
  }
  ul_hapi int ulfd_open_w(ulfd_t* pfd, const wchar_t* wpath, ulfd_int32_t oflag, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_open(pfd, path, oflag, mode);
    _ulfd_end_to_str2(path);
    return ret;
  }
  ul_hapi int ulfd_creat(ulfd_t* pfd, const char* path, ulfd_mode_t mode) {
    int fd = creat(path, _ulfd_to_access_mode(mode));
    if(fd < 0) return errno;
    *pfd = fd; return 0;
  }
  ul_hapi int ulfd_creat_w(ulfd_t* pfd, const wchar_t* wpath, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_creat(pfd, path, mode);
    _ulfd_end_to_str2(path);
    return ret;
  }
  ul_hapi int ulfd_close(ulfd_t fd) {
    return close(fd) < 0 ? errno : 0;
  }

  ul_hapi int ulfd_pipe(ulfd_t pfds[2]) {
    return pipe(pfds) < 0 ? errno : 0;
  }

  ul_hapi int ulfd_read(ulfd_t fd, void* buf, size_t count, size_t* pread_bytes) {
    ssize_t ret;
    ret = read(fd, buf, count);
    if(ret < 0) return errno;
    *pread_bytes = ul_static_cast(size_t, ret);
    return 0;
  }
  ul_hapi int ulfd_write(ulfd_t fd, const void* buf, size_t count, size_t* pwriten_bytes) {
    ssize_t ret;
    ret = write(fd, buf, count);
    if(ret < 0) return errno;
    *pwriten_bytes = ul_static_cast(size_t, ret);
    return 0;
  }
  ul_hapi int ulfd_pread(ulfd_t fd, void* buf, size_t count, ulfd_int64_t off, size_t* pread_bytes) {
  #ifdef ULFD_POSIX_HAS_pread
    ssize_t ret;
    #ifdef ULFD_HAS_LFS
      ret = pread64(fd, buf, count, off);
    #else
      if(ul_static_cast(off_t, off) != off) return EOVERFLOW;
      ret = pread(fd, buf, count, ul_static_cast(off_t, off));
    #endif
    if(ret < 0) return errno;
    *pread_bytes = ul_static_cast(size_t, ret);
    return 0;
  #else
    (void)fd; (void)buf; (void)count; (void)off; (void)pread_bytes;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_pwrite(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes) {
  #ifdef ULFD_POSIX_HAS_pwrite
    ssize_t ret;
    #ifdef ULFD_HAS_LFS
      ret = pwrite64(fd, buf, count, off);
    #else
      if(ul_static_cast(off_t, off) != off) return EOVERFLOW;
      ret = pwrite(fd, buf, count, ul_static_cast(off_t, off));
    #endif
    if(ret < 0) return errno;
    *pwriten_bytes = ul_static_cast(size_t, ret);
    return 0;
  #else
    (void)fd; (void)buf; (void)count; (void)off; (void)pwriten_bytes;
    return ENOSYS;
  #endif
  }

  ul_hapi int ulfd_seek(ulfd_t fd, ulfd_int64_t off, int origin, ulfd_int64_t* poff) {
  #ifdef ULFD_HAS_LFS
    off64_t new_off;
  #else
    off_t new_off;
  #endif
    int whence;

    if(origin == ULFD_SEEK_CUR) whence = SEEK_CUR;
    else if(origin == ULFD_SEEK_CUR) whence = SEEK_CUR;
    else if(origin == ULFD_SEEK_END) whence = SEEK_END;
    else return EINVAL;

  #ifdef ULFD_HAS_LFS
    new_off = lseek64(fd, off, whence);
  #else
    if(ul_static_cast(off_t, off) != off) return EOVERFLOW;
    new_off = lseek(fd, ul_static_cast(off_t, off), whence);
  #endif
    if(new_off < 0) return errno;
    *poff = new_off;
    return 0;
  }
  ul_hapi int ulfd_tell(ulfd_t fd, ulfd_int64_t* poff) {
    return ulfd_seek(fd, 0, ULFD_SEEK_END, poff);
  }

  ul_hapi int ulfd_copy_file_range(
    ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out,
    size_t len, size_t* pcopyed
  ) {
  #ifdef ULFD_POSIX_HAS_copy_file_range
    off_t nin, nout;
    off_t* pin, *pout;
    ssize_t copyed;

    if(off_in) {
      nin = ul_static_cast(off_t, *off_in); pin = &nin;
      if(ul_unlikely(nin != *off_in)) return EOVERFLOW;
    } else pin = NULL;
    if(off_out) {
      nout = ul_static_cast(off_t, *off_out); pout = &nout;
      if(ul_unlikely(nout != *off_out)) return EOVERFLOW;
    } else pout = NULL;

    copyed = copy_file_range(fd_in, pin, fd_out, pout, len, 0u);
    if(copyed < 0) return errno;
    *pcopyed = ul_static_cast(size_t, copyed); return 0;
  #else
    (void)fd_in; (void)off_in; (void)fd_out; (void)off_out; (void)len; (void)pcopyed;
    return ENOSYS;
  #endif
  }

  ul_hapi int ulfd_fsync(ulfd_t fd) {
  #ifdef ULFD_POSIX_HAS_fsync
    return fsync(fd) < 0 ? errno : 0;
  #else
    (void)fd; return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_ffullsync(ulfd_t fd) {
  #ifdef F_FULLFSYNC
    return fcntl(fd, F_FULLFSYNC, 0) < 0 ? errno : 0;
  #else
    return ulfd_fsync(fd);
  #endif
  }
  ul_hapi int ulfd_fdatasync(ulfd_t fd) {
  #ifdef ULFD_POSIX_HAS_fdatasync
    return fdatasync(fd) < 0 ? errno : 0;
  #else
    return ulfd_fsync(fd);
  #endif
  }

  ul_hapi int ulfd_lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode) {
  #ifdef ULFD_HAS_LFS
    struct flock64 lock;
    lock.l_len = len;
    lock.l_start = off;
  #else
    struct flock lock;
    if(ul_static_cast(off_t, len) != len) return EOVERFLOW;
    lock.l_len = ul_static_cast(off_t, len);
    if(ul_static_cast(off_t, off) != off) return EOVERFLOW;
    lock.l_start = ul_static_cast(off_t, off);
  #endif
    lock.l_whence = SEEK_SET;
    if(mode == ULFD_F_RDLCK) lock.l_type = F_RDLCK;
    else if(mode == ULFD_F_WRLCK) lock.l_type = F_WRLCK;
    else if(mode == ULFD_F_UNLCK) lock.l_type = F_UNLCK;
    else return EINVAL;

  #ifdef ULFD_HAS_LFS
    return fcntl(fd, F_SETLK64, &lock) < 0 ? errno : 0;
  #else
    return fcntl(fd, F_SETLK, &lock) < 0 ? errno : 0;
  #endif
  }
  ul_hapi int ulfd_lockw(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode) {
  #ifdef ULFD_HAS_LFS
    struct flock64 lock;
    lock.l_len = len;
    lock.l_start = off;
  #else
    struct flock lock;
    if(ul_static_cast(off_t, len) != len) return EOVERFLOW;
    lock.l_len = ul_static_cast(off_t, len);
    if(ul_static_cast(off_t, off) != off) return EOVERFLOW;
    lock.l_start = ul_static_cast(off_t, off);
  #endif
    lock.l_whence = SEEK_SET;
    if(mode == ULFD_F_RDLCK) lock.l_type = F_RDLCK;
    else if(mode == ULFD_F_WRLCK) lock.l_type = F_WRLCK;
    else if(mode == ULFD_F_UNLCK) lock.l_type = F_UNLCK;
    else return EINVAL;

  #ifdef ULFD_HAS_LFS
    return fcntl(fd, F_SETLKW64, &lock) < 0 ? errno : 0;
  #else
    return fcntl(fd, F_SETLKW, &lock) < 0 ? errno : 0;
  #endif
  }

  ul_hapi int ulfd_ftruncate(ulfd_t fd, ulfd_int64_t length) {
  #ifdef ULFD_POSIX_HAS_ftruncate
    #ifdef ULFD_HAS_LFS
      return ftruncate64(fd, length) < 0 ? errno : 0;
    #else
      if(ul_static_cast(off_t, length) != length) return EOVERFLOW;
      return ftruncate(fd, ul_static_cast(off_t, length)) < 0 ? errno : 0;
    #endif
  #else
    (void)fd; (void)length;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_ffilelength(ulfd_t fd, ulfd_int64_t* plength) {
  #ifdef ULFD_HAS_LFS
    struct stat64 state;
    if(fstat64(fd, &state) < 0) return errno;
  #else
    struct stat state;
    if(fstat(fd, &state) < 0) return errno;
  #endif
    *plength = state.st_size;
    return 0;
  }
  ul_hapi int ulfd_fchmod(ulfd_t fd, ulfd_mode_t mode) {
  #ifdef ULFD_POSIX_HAS_fchmod
    return fchmod(fd, _ulfd_to_access_mode(mode)) < 0 ? errno : 0;
  #else
    (void)fd; (void)mode;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_fchown(ulfd_t fd, ulfd_uid_t uid, ulfd_gid_t gid) {
  #ifdef ULFD_POSIX_HAS_fchown
    return fchown(fd, uid, gid) < 0 ? errno : 0;
  #else
    (void)fd; (void)uid; (void)gid;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_ftype(ulfd_t fd, ulfd_mode_t* pmode) {
    struct stat state;
    if(fstat(fd, &state) < 0) return errno;
    *pmode = _ulfd_from_full_mode(state.st_mode); return 0;
  }
  #include <sys/time.h>
  ul_hapi int ulfd_futime(ulfd_t fd, ulfd_int64_t atime, ulfd_int64_t mtime) {
  #ifdef ULFD_POSIX_HAS_futimes
    struct timeval tv[2];
    tv[0].tv_sec = ul_static_cast(time_t, atime / 1000);
    tv[0].tv_usec = ul_static_cast(suseconds_t, (atime % 1000) * 1000000);
    tv[1].tv_sec = ul_static_cast(time_t, mtime / 1000);
    tv[1].tv_usec = ul_static_cast(suseconds_t, (mtime % 1000) * 1000000);
    return futimes(fd, tv) < 0 ? errno : 0;
  #else
    (void)fd; (void)atime; (void)mtime;
    return ENOSYS;
  #endif
  }

  ul_hapi int ulfd_isatty(ulfd_t fd, int* presult) {
    int r = isatty(fd);
    if(r < 0) return errno;
    *presult = (r > 0); return 0;
  }

  ul_hapi int ulfd_setstd(int which, ulfd_t fd) {
    ulfd_t nfd;
    switch(which) {
    case ULFD_SET_STDIN: nfd = STDIN_FILENO; break;
    case ULFD_SET_STDOUT: nfd = STDOUT_FILENO; break;
    case ULFD_SET_STDERR: nfd = STDERR_FILENO; break;
    default: return EINVAL;
    }
    return dup2(fd, nfd) < 0 ? errno : 0;
  }

  ul_hapi int ulfd_dup(ulfd_t* pnfd, ulfd_t ofd) {
    return (*pnfd = dup(ofd)) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_fdopen(FILE** pfp, ulfd_t fd, const char* mode) {
  #ifdef ULFD_POSIX_HAS_fdopen
    return (*pfp = fdopen(fd, mode)) == NULL ? errno : 0;
  #else
    (void)pfp; (void)fd; (void)mode;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_fdopen_w(FILE** pfp, ulfd_t fd, const wchar_t* wmode) {
    int ret;
    _ulfd_begin_to_str(mode, wmode);
    ret = ulfd_fdopen(pfp, fd, mode);
    _ulfd_end_to_str(mode);
    return ret;
  }
  ul_hapi int ulfd_fileno(ulfd_t* pfd, FILE* fp) {
  #ifdef ULFD_POSIX_HAS_fileno
    return (*pfd = fileno(fp)) < 0 ? errno : 0;
  #else
    (void)pfd; (void)fp;
    return ENOSYS;
  #endif
  }

  ul_hapi int ulfd_truncate(const char* path, ulfd_int64_t length) {
  #ifdef ULFD_POSIX_HAS_truncate
    #ifdef ULFD_HAS_LFS
      return truncate64(path, length) < 0 ? errno : 0;
    #else
      if(ul_static_cast(off_t, length) != length) return EOVERFLOW;
      return truncate(path, ul_static_cast(off_t, length)) < 0 ? errno : 0;
    #endif
  #else
    (void)path; (void)length;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_truncate_w(const wchar_t* wpath, ulfd_int64_t length) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_truncate(path, length);
    _ulfd_end_to_str(path);
    return ret;
  }

  #include <sys/mman.h>
  ul_hapi int ulfd_mmap(void** pmap, ulfd_t fd, void* addr, size_t len, ulfd_int64_t off, int flags) {
    int prot, flag = 0;
    void* map;

    if((flags & (ULFD_PROT_READ | ULFD_PROT_WRITE | ULFD_PROT_EXEC)) == 0) {
      prot = PROT_NONE;
    } else {
      prot = 0;
      if(flags & ULFD_PROT_READ) prot |= PROT_READ;
      if(flags & ULFD_PROT_WRITE) prot |= PROT_WRITE;
      if(flags & ULFD_PROT_EXEC) prot |= PROT_EXEC;
    }

    if(flags & ULFD_MAP_ANONYMOUS) flag |= MAP_ANONYMOUS;
    if(flags & ULFD_MAP_FIXED) flag |= MAP_FIXED;
    if(flags & ULFD_MAP_SHARED) flag |= MAP_SHARED;
    else flag |= MAP_PRIVATE;

  #ifdef ULFD_HAS_LFS
    map = mmap64(addr, len, prot, flag, fd, off);
  #else
    if(ul_static_cast(off_t, off) != off) return EOVERFLOW;
    map = mmap(addr, len, prot, flag, fd, ul_static_cast(off_t, off));
  #endif
    if(map == MAP_FAILED) return errno;
    *pmap = map; return 0;
  }
  ul_hapi int ulfd_munmap(void* addr, size_t len) {
    return munmap(addr, len) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_mprotect(void* addr, size_t len, int flags) {
    int prot;
    if((flags & (ULFD_PROT_READ | ULFD_PROT_WRITE | ULFD_PROT_EXEC)) == 0) {
      prot = PROT_NONE;
    } else {
      prot = 0;
      if(flags & ULFD_PROT_READ) prot |= PROT_READ;
      if(flags & ULFD_PROT_WRITE) prot |= PROT_WRITE;
      if(flags & ULFD_PROT_EXEC) prot |= PROT_EXEC;
    }
    return mprotect(addr, len, prot) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_msync(void* addr, size_t len, int flags) {
    int flag = 0;
    if(flags & ULFD_MS_ASYNC) flag |= MS_ASYNC;
    if(flags & ULFD_MS_SYNC) flag |= MS_SYNC;
    if(flags & ULFD_MS_INVALIDATE) flag |= MS_INVALIDATE;
    return msync(addr, len, flag) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_mlock(const void* addr, size_t len) {
    return mlock(addr, len) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_munlock(const void* addr, size_t len) {
    return munlock(addr, len) < 0 ? errno : 0;
  }

  ul_hapi int ulfd_madvise(void* addr, size_t len, int advice) {
  #if defined(MADV_NORMAL)
    int adv;
    switch(advice) {
    case ULFD_MADV_WILLNEED:   adv = MADV_WILLNEED; break;
    case ULFD_MADV_DONTNEED:   adv = MADV_DONTNEED; break;
    case ULFD_MADV_NORMAL:     adv = MADV_NORMAL; break;
    case ULFD_MADV_RANDOM:     adv = MADV_RANDOM; break;
    case ULFD_MADV_SEQUENTIAL: adv = MADV_SEQUENTIAL; break;

    #ifdef MADV_DONTFORK
    case ULFD_MADV_DONTFORK:   adv = MADV_DONTFORK; break;
    #endif

    #ifdef MADV_DOFORK
    case ULFD_MADV_DOFORK:     adv = MADV_DOFORK; break;
    #endif

    #ifdef MADV_DODUMP
    case ULFD_MADV_DODUMP:     adv = MADV_DODUMP; break;
    #endif

    #ifdef MADV_DONTDUMP
    case ULFD_MADV_DONTDUMP:   adv = MADV_DONTDUMP; break;
    #endif

    default: return EINVAL;
    }
    return madvise(addr, len, adv) < 0 ? errno : 0;
  #elif defined(POSIX_MADV_NORMAL)
    int adv;
    switch(advice) {
    case ULFD_MADV_WILLNEED:   adv = POSIX_FADV_WILLNEED; break;
    case ULFD_MADV_DONTNEED:   adv = POSIX_FADV_DONTNEED; break;
    case ULFD_MADV_NORMAL:     adv = POSIX_FADV_NORMAL; break;
    case ULFD_MADV_RANDOM:     adv = POSIX_FADV_RANDOM; break;
    case ULFD_MADV_SEQUENTIAL: adv = POSIX_FADV_SEQUENTIAL; break;
    default: return EINVAL;
    }
    adv = posix_madvise(addr, len, adv);
    return adv;
  #else
    (void)addr; (void)len; (void)advice;
    return ENOSYS;
  #endif
  }

  ul_hapi size_t ulfd_pagesize(void) {
    return ul_static_cast(size_t, sysconf(_SC_PAGESIZE));
  }
  ul_hapi int ulfd_tmpdir_alloc(char** ppath) {
    char* s;
    if((s = getenv("TMPDIR"))) return (*ppath = ulfd_strdup(s)) == NULL ? ENOMEM : 0;
    if((s = getenv("TMP"))) return (*ppath = ulfd_strdup(s)) == NULL ? ENOMEM : 0;
    if((s = getenv("TEMP"))) return (*ppath = ulfd_strdup(s)) == NULL ? ENOMEM : 0;
    if((s = getenv("TEMPDIR"))) return (*ppath = ulfd_strdup(s)) == NULL ? ENOMEM : 0;
    return (*ppath = ulfd_strdup("/tmp")) ? ENOMEM : 0;
  }
  ul_hapi int ulfd_tmpdir_alloc_w(wchar_t** pwpath) {
    char* s;
    if((s = getenv("TMPDIR"))) return ulfd_str_to_wstr_alloc(pwpath, s);
    if((s = getenv("TMP"))) return ulfd_str_to_wstr_alloc(pwpath, s);
    if((s = getenv("TEMP"))) return ulfd_str_to_wstr_alloc(pwpath, s);
    if((s = getenv("TEMPDIR"))) return ulfd_str_to_wstr_alloc(pwpath, s);
    return ulfd_str_to_wstr_alloc(pwpath, "/tmp");
  }

  ul_hapi int ulfd_chdir(const char* path) {
    return chdir(path) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_chdir_w(const wchar_t* wpath) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_chdir(path);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi int ulfd_getcwd(char* path, size_t length) {
    char* ret;
    ret = getcwd(path, length);
    if(ret == NULL) return errno;
    return 0;
  }
  ul_hapi int ulfd_getcwd_w(wchar_t* wpath, size_t length) {
    char* path;
    size_t cast_len;
    int ret = 0;

    path = ul_reinterpret_cast(char*, ul_malloc(ULFD_PATH_MAX + 1));
    if(ul_unlikely(path == NULL)) return ENOMEM;
    if(getcwd(path, ULFD_PATH_MAX + 1) == NULL) { ret = errno; goto do_return; }
    cast_len = ul_os_str_to_wstr_len(path);
    if(ul_unlikely(cast_len == 0)) { ret = EILSEQ; goto do_return; }
    if(cast_len > length) { ret = ERANGE; goto do_return; }
    ul_os_str_to_wstr(wpath, path);

  do_return:
    ul_free(path); return ret;
  }
  ul_hapi int ulfd_getcwd_alloc(char** ppath) {
  #ifdef ULFD_POSIX_HAS_get_current_dir_name
    char* path = get_current_dir_name();
    #ifdef UL_FREE_DEFAULT
      *ppath = path;
      return *ppath == NULL ? errno : 0;
    #else
      if(ul_unlikely(path == NULL)) return errno;
      *ppath = ulfd_strdup(path);
      free(path);
      return ul_likely(*ppath) ? 0 : ENOMEM;
    #endif
  #else
    size_t cap = ULFD_PATH_MAX;
    char* path = NULL;
    for(;;) {
      char* npath = ul_reinterpret_cast(char*, ul_realloc(path, cap));
      if(ul_unlikely(npath == NULL)) break;
      path = npath;
      if(ul_likely(getcwd(path, cap))) {
        *ppath = path; return 0;
      }
      if(errno != ERANGE) return errno;
      cap = cap + (cap >> 1);
    }
    ul_free(path);
    return ENOMEM;
  #endif
  }
  ul_hapi int ulfd_getcwd_alloc_w(wchar_t** pwpath) {
  #ifdef ULFD_POSIX_HAS_get_current_dir_name
    int err;
    char* path = get_current_dir_name();
    if(ul_unlikely(path == NULL)) return errno;
    err = ulfd_str_to_wstr_alloc(pwpath, path);
    free(path);
    return err;
  #else
    size_t cap = ULFD_PATH_MAX;
    char* path = NULL;
    int err = ENOMEM;
    for(;;) {
      char* npath = ul_reinterpret_cast(char*, ul_realloc(path, cap));
      if(ul_unlikely(npath == NULL)) break;
      path = npath;
      if(ul_likely(getcwd(path, cap))) {
        wchar_t* wpath;
        size_t need_len = ul_os_str_to_wstr_len(path);
        if(ul_unlikely(need_len == 0)) { err = EILSEQ; break; }
        wpath = ul_reinterpret_cast(wchar_t*, ul_malloc(need_len * sizeof(wchar_t)));
        if(ul_unlikely(wpath == NULL)) { err = ENOMEM; break; }
        ul_os_str_to_wstr(wpath, path);
        err = 0;
        break;
      }
      if(errno != ERANGE) return errno;
      cap = cap + (cap >> 1);
    }
    err = ulfd_str_to_wstr_alloc(pwpath, path);
    ul_free(path);
    return err;
  #endif
  }

  ul_hapi ulfd_mode_t ulfd_umask(ulfd_mode_t mask) {
    return _ulfd_from_access_mode(umask(_ulfd_to_access_mode(mask)));
  }
  ul_hapi int ulfd_chmod(const char* path, ulfd_mode_t mode) {
    return chmod(path, _ulfd_to_access_mode(mode)) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_chmod_w(const wchar_t* wpath, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_chmod(path, mode);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_lchmod(const char* path, ulfd_mode_t mode) {
  #ifdef ULFD_POSIX_HAS_fchmodat
    return fchmodat(AT_FDCWD, path, _ulfd_to_full_mode(mode), AT_SYMLINK_NOFOLLOW);
  #else
    (void)path; (void)mode;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_lchmod_w(const wchar_t* wpath, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_lchmod(path, mode);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi int ulfd_chown(const char* path, ulfd_uid_t uid, ulfd_gid_t gid) {
    return chown(path, uid, gid) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_chown_w(const wchar_t* wpath, ulfd_uid_t uid, ulfd_gid_t gid) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_chown(path, uid, gid);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_lchown(const char* path, ulfd_uid_t uid, ulfd_gid_t gid) {
  #ifdef ULFD_POSIX_HAS_fchownat
    return fchownat(AT_FDCWD, path, uid, gid, AT_SYMLINK_NOFOLLOW);
  #else
    (void)path; (void)uid; (void)gid;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_lchown_w(const wchar_t* wpath, ulfd_uid_t uid, ulfd_gid_t gid) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_lchown(path, uid, gid);
    _ulfd_end_to_str(path);
    return ret;
  }

  #include <sys/time.h>
  ul_hapi int ulfd_utime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    struct timeval tv[2];
    tv[0].tv_sec = ul_static_cast(time_t, atime / 1000);
    tv[0].tv_usec = ul_static_cast(suseconds_t, (atime % 1000) * 1000);
    tv[1].tv_sec = ul_static_cast(time_t, mtime / 1000);
    tv[1].tv_usec = ul_static_cast(suseconds_t, (mtime % 1000) * 1000);
    return utimes(path, tv) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_utime_w(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_utime(path, atime, mtime);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_lutime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
  #ifdef ULFD_POSIX_HAS_utimensat
    struct timespec tv[2];
    tv[0].tv_sec = ul_static_cast(time_t, atime / 1000);
    tv[0].tv_nsec = ul_static_cast(suseconds_t, (atime % 1000) * 1000000);
    tv[1].tv_sec = ul_static_cast(time_t, mtime / 1000);
    tv[1].tv_nsec = ul_static_cast(suseconds_t, (mtime % 1000) * 1000000);
    return utimensat(AT_FDCWD, path, tv, AT_SYMLINK_NOFOLLOW) < 0 ? errno : 0;
  #else
    (void)path; (void)atime; (void)mtime;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_lutime_w(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_lutime(path, atime, mtime);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi void _ulfd_stat_parse(
    ulfd_stat_t* out,
  #ifdef ULFD_HAS_LFS
    const struct stat64* state
  #else
    const struct stat* state
  #endif
  ) {
    out->dev = state->st_dev;
    out->rdev = state->st_rdev;
    out->ino = state->st_ino;
    out->mode = _ulfd_from_full_mode(state->st_mode);
    out->nlink = state->st_nlink;
    out->uid = state->st_uid;
    out->gid = state->st_gid;
    out->size = ul_static_cast(ulfd_int64_t, state->st_size);
  #ifdef ULFD_POSIX_STAT_HAS_TIM
    out->atime = ul_static_cast(ulfd_time_t, state->st_atim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state->st_atim.tv_nsec / 1000000);
    out->mtime = ul_static_cast(ulfd_time_t, state->st_mtim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state->st_mtim.tv_nsec / 1000000);
    out->ctime = ul_static_cast(ulfd_time_t, state->st_ctim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state->st_ctim.tv_nsec / 1000000);
  #else
    out->atime = ul_static_cast(ulfd_int64_t, state->st_atime * 1000);
    out->mtime = ul_static_cast(ulfd_int64_t, state->st_mtime * 1000);
    out->ctime = ul_static_cast(ulfd_int64_t, state->st_ctime * 1000);
  #endif
  }

  ul_hapi int ulfd_stat(const char* path, ulfd_stat_t* out) {
  #ifdef ULFD_HAS_LFS
    struct stat64 state;
    if(stat64(path, &state) < 0) return errno;
  #else
    struct stat state;
    if(stat(path, &state) < 0) return errno;
  #endif

    _ulfd_stat_parse(out, &state);
    return 0;
  }
  ul_hapi int ulfd_stat_w(const wchar_t* wpath, ulfd_stat_t* state) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_stat(path, state);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_fstat(ulfd_t fd, ulfd_stat_t* out) {
  #ifdef ULFD_HAS_LFS
    struct stat64 state;
    if(fstat64(fd, &state) < 0) return errno;
  #else
    struct stat state;
    if(fstat(fd, &state) < 0) return errno;
  #endif

    _ulfd_stat_parse(out, &state);
    return 0;
  }
  ul_hapi int ulfd_lstat(const char* path, ulfd_stat_t* out) {
  #ifdef ULFD_POSIX_HAS_lstat
    #ifdef ULFD_HAS_LFS
      struct stat64 state;
      if(lstat64(path, &state) < 0) return errno;
    #else
      struct stat state;
      if(lstat(path, &state) < 0) return errno;
    #endif
    _ulfd_stat_parse(out, &state);
    return 0;
  #else
    (void)path; (void)out;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_lstat_w(const wchar_t* wpath, ulfd_stat_t* state) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_lstat(path, state);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi int ulfd_type(const char* path, ulfd_mode_t* pmode) {
    struct stat state;
    if(stat(path, &state) < 0) return errno;
    *pmode = _ulfd_from_full_mode(state.st_mode);
    return 0;
  }
  ul_hapi int ulfd_type_w(const wchar_t* wpath, ulfd_mode_t* pmode) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_type(path, pmode);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_ltype(const char* path, ulfd_mode_t* pmode) {
  #ifdef ULFD_POSIX_HAS_lstat
    struct stat state;
    if(lstat(path, &state) < 0) return errno;
    *pmode = _ulfd_from_full_mode(state.st_mode);
    return 0;
  #else
    (void)path; (void)pmode;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_ltype_w(const wchar_t* wpath, ulfd_mode_t* pmode) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_ltype(path, pmode);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi int ulfd_access(const char* path, ulfd_mode_t mode) {
    int mode2 = 0;
    if(mode & ULFD_S_IREAD) mode2 |= R_OK;
    if(mode & ULFD_S_IWRITE) mode2 |= W_OK;
    if(mode & ULFD_S_IEXEC) mode2 |= X_OK;
    return access(path, mode2) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_access_w(const wchar_t* wpath, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_access(path, mode);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_empty(const char* path, int* pempty) {
  #ifdef ULFD_HAS_LFS
    struct stat64 state;
    if(stat64(path, &state) < 0) return errno;
  #else
    struct stat state;
    if(stat(path, &state) < 0) return errno;
  #endif

    if(S_ISDIR(state.st_mode)) {
      DIR* dir;
      struct dirent* dirent;
      int err = 0;

      dir = opendir(path);
      if(dir == NULL) return errno;
      errno = 0;
      dirent = readdir(dir);
      err = errno;
      *pempty = (dirent == NULL && err == 0);
      closedir(dir);
      return err;
    } else {
      *pempty = (state.st_size == 0);
      return 0;
    }
  }
  ul_hapi int ulfd_empty_w(const wchar_t* wpath, int* pempty) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_empty(path, pempty);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi int ulfd_rename(const char* newpath, const char* oldpath) {
    errno = EINVAL;
    return rename(oldpath, newpath) ? errno : 0;
  }
  ul_hapi int ulfd_rename_w(const wchar_t* newpath, const wchar_t* oldpath) {
    int ret;
    _ulfd_begin_to_str(_newpath, newpath);
    _ulfd_begin_to_str2(_oldpath, oldpath, _newpath);
    ret = ulfd_rename(_newpath, _oldpath);
    _ulfd_end_to_str2(_oldpath);
    _ulfd_end_to_str(_newpath);
    return ret;
  }
  ul_hapi int ulfd_unlink(const char* path) {
    return unlink(path) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_unlink_w(const wchar_t* wpath) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_unlink(path);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_remove(const char* path) {
    errno = 0;
    return remove(path) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_remove_w(const wchar_t* wpath) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_remove(path);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi int ulfd_mkdir(const char* path, ulfd_mode_t mode) {
    return mkdir(path, _ulfd_to_access_mode(mode)) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_mkdir_w(const wchar_t* wpath, ulfd_mode_t mode) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_mkdir(path, mode);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_rmdir(const char* path) {
    return rmdir(path) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_rmdir_w(const wchar_t* wpath) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_rmdir(path);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi int ulfd_link(const char* newpath, const char* oldpath) {
    return link(oldpath, newpath) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_link_w(const wchar_t* newpath, const wchar_t* oldpath) {
    int ret;
    _ulfd_begin_to_str(_newpath, newpath);
    _ulfd_begin_to_str2(_oldpath, oldpath, _newpath);
    ret = ulfd_link(_newpath, _oldpath);
    _ulfd_end_to_str2(_oldpath);
    _ulfd_end_to_str(_newpath);
    return ret;
  }
  ul_hapi int ulfd_symlink(const char* target, const char* source) {
  #ifdef ULFD_POSIX_HAS_symlink
    return symlink(source, target) < 0 ? errno : 0;
  #else
    (void)target; (void)source;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_symlink_w(const wchar_t* target, const wchar_t* source) {
    int ret;
    _ulfd_begin_to_str(_target, target);
    _ulfd_begin_to_str2(_source, source, _target);
    ret = ulfd_symlink(_target, _source);
    _ulfd_end_to_str2(_source);
    _ulfd_end_to_str(_target);
    return ret;
  }
  ul_hapi int ulfd_readlink(const char* path, char* buf, size_t len) {
  #ifdef ULFD_POSIX_HAS_readlink
    ssize_t l;
    l = readlink(path, buf, len);
    if(l < 0) return errno;
    if(ul_static_cast(size_t, l) == len) return ERANGE;
    buf[l] = 0;
    return 0;
  #else
    (void)path; (void)buf; (void)len;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_readlink_alloc(char** pbuf, const char* path) {
  #ifdef ULFD_POSIX_HAS_readlink
    char* buf;
    ssize_t sret;

    buf = ul_reinterpret_cast(char*, ul_malloc(ULFD_PATH_MAX + 1));
    if(ul_unlikely(buf == NULL)) return ENOMEM;
    sret = readlink(path, buf, ULFD_PATH_MAX + 1);
    if(sret < 0) { ul_free(buf); return errno; }
    else if(sret == ULFD_PATH_MAX + 1) { ul_free(buf); return ERANGE; }

    buf[sret] = 0;
    *pbuf = buf;
    return 0;
  #else
    (void)pbuf; (void)path;
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_readlink_w(const wchar_t* wpath, wchar_t* buf, size_t len) {
    char* tmp;
    int ret = 0;
    size_t cast_len;

    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_readlink_alloc(&tmp, path);
    _ulfd_end_to_str(path);
    if(ret) return ret;

    cast_len = ul_os_str_to_wstr_len(tmp);
    if(ul_unlikely(cast_len == 0)) ret = EILSEQ;
    if(cast_len > len) ret = ERANGE;
    else ul_os_str_to_wstr(buf, tmp);
    ul_free(tmp);
    return ret;
  }
  ul_hapi int ulfd_readlink_alloc_w(wchar_t** pwbuf, const wchar_t* wpath) {
    char* tmp;
    int ret;

    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_readlink_alloc(&tmp, path);
    _ulfd_end_to_str(path);
    if(ret) return ret;

    ret = ulfd_str_to_wstr_alloc(pwbuf, tmp);
    ul_free(tmp);
    return ret;
  }
  ul_hapi int ulfd_realpath_alloc(char** presolved, const char* path) {
  #ifdef ULFD_POSIX_HAS_canonicalize_file_name
    char* resolved = canonicalize_file_name(path);
    #ifdef UL_FREE_DEFAULT
      *presolved = resolved;
      return *presolved == NULL ? errno : 0;
    #else
      if(resolved == NULL) return errno;
      *presolved = ulfd_strdup(resolved);
      free(resolved);
      return *presolved == NULL ? ENOMEM : 0;
    #endif
  #elif defined(ULFD_POSIX_HAS_realpath)
    #if (_POSIX_C_SOURCE+0) >= 200809L
      char* resolved = realpath(path, NULL);
      #ifdef UL_FREE_DEFAULT
        *presolved = resolved;
        return *presolved == NULL ? errno : 0;
      #else
        if(resolved == NULL) return errno;
        *presolved = ulfd_strdup(resolved);
        free(resolved);
        return *presolved == NULL ? ENOMEM : 0;
      #endif
    #else
      char resolved[ULFD_PATH_MAX];
      if(!realpath(path, resolved)) return errno;
      return (*presolved = ulfd_strdup(resolved)) == NULL ? ENOMEM : 0;
    #endif
  #else
    (void)presolved; (void)path; return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_realpath_alloc_w(wchar_t** presolved, const wchar_t* wpath) {
    char* resolved;
    int ret;

    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_realpath_alloc(&resolved, path);
    _ulfd_end_to_str(path);
    if(ret) return ret;

    ret = ulfd_str_to_wstr_alloc(presolved, resolved);
    ul_free(resolved);
    return ret;
  }


  ul_hapi int _ulfd_dir_store(ulfd_dir_t* dir, const char* name) {
    size_t len = strlen(name) + 1;
    if(len > dir->entry_cap) {
      char* entry;
      size_t cap2 = dir->entry_cap + (dir->entry_cap >> 1) + 1;
      if(cap2 < len) cap2 = len;
      entry = ul_reinterpret_cast(char*, ul_realloc(dir->entry, cap2));
      if(ul_unlikely(entry == NULL)) return ENOMEM;
      dir->entry = entry; dir->entry_cap = cap2;
    }
    memcpy(dir->entry, name, len);
    return 0;
  }
  ul_hapi int _ulfd_dir_store_w(ulfd_dir_t* dir, const char* name) {
    size_t need_len = ul_os_str_to_wstr_len(name);
    if(ul_unlikely(need_len == 0)) return EILSEQ;
    if(need_len > dir->wentry_cap) {
      wchar_t* wentry;
      size_t cap2 = dir->wentry_cap + (dir->wentry_cap >> 1) + 1;
      if(cap2 < need_len) cap2 = need_len;
      wentry = ul_reinterpret_cast(wchar_t*, ul_realloc(dir->wentry, cap2 * sizeof(wchar_t)));
      if(ul_unlikely(wentry == NULL)) return ENOMEM;
      dir->wentry = wentry; dir->wentry_cap = cap2;
    }
    ul_os_str_to_wstr(dir->wentry, name);
    return 0;
  }
  ul_hapi int _ulfd_dir_skip_dot(ulfd_dir_t* dir) {
    struct dirent* dirent;
    char meet_dot = 0, meet_dotdot = 0;
    dir->cached = 0;
    for(;;) {
      dirent = readdir(dir->dir);
      if(dirent == NULL) return errno;
      if(dirent->d_name[0] != '.') break;
      if(dirent->d_name[1] == 0) meet_dot = 1;
      else if(dirent->d_name[1] == '.' && dirent->d_name[2] == 0) meet_dotdot = 1;
      else break;
      if(meet_dot && meet_dotdot) return 0;
    }
    dir->cached = 1;
    return _ulfd_dir_store(dir, dirent->d_name);
  }
  ul_hapi int ulfd_opendir(ulfd_dir_t* dir, const char* path) {
    int err;
    dir->entry = NULL;
    dir->entry_cap = 0;
    dir->wentry = NULL;
    dir->wentry_cap = 0;
    dir->dir = opendir(path);
    if(dir->dir == NULL) return errno;
    err = _ulfd_dir_skip_dot(dir);
    if(ul_unlikely(err)) { closedir(dir->dir); return err; }
    return 0;
  }
  ul_hapi int ulfd_opendir_w(ulfd_dir_t* dir, const wchar_t* wpath) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_opendir(dir, path);
    _ulfd_end_to_str(path);
    return ret;
  }
  ul_hapi int ulfd_rewinddir(ulfd_dir_t* dir) {
    rewinddir(dir->dir);
    return _ulfd_dir_skip_dot(dir);
  }
  ul_hapi int ulfd_closedir(ulfd_dir_t* dir) {
    if(ul_likely(dir->entry)) ul_free(dir->entry);
    if(ul_unlikely(dir->wentry)) ul_free(dir->wentry);
    if(ul_unlikely(!dir->dir)) return 0;
    return closedir(dir->dir) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_readdir(ulfd_dir_t* dir, const char** pname) {
    int err = 0;
    if(dir->cached) {
      dir->cached = 0;
      *pname = dir->entry;
    } else {
      struct dirent* dirent;
      errno = 0;
      dirent = readdir(dir->dir);
      if(dirent == NULL) {
        err = errno;
        *pname = NULL;
      } else {
        err = _ulfd_dir_store(dir, dirent->d_name);
        *pname = dir->entry;
      }
    }
    return err;
  }
  ul_hapi int ulfd_readdir_w(ulfd_dir_t* dir, const wchar_t** pwname) {
    int err = 0;
    if(dir->cached) {
      dir->cached = 0;
      err = _ulfd_dir_store_w(dir, dir->entry);
      *pwname = dir->wentry;
    } else {
      struct dirent* dirent;
      errno = 0;
      dirent = readdir(dir->dir);
      if(dirent == NULL) {
        err = errno;
        *pwname = NULL;
      } else {
        err = _ulfd_dir_store_w(dir, dirent->d_name);
        *pwname = dir->wentry;
      }
    }
    return err;
  }


  #include <sys/statvfs.h>
  int ulfd_space(ulfd_spaceinfo_t* info, const char* path) {
  #ifdef ULFD_HAS_LFS
    struct statvfs64 vfs;
    if(statvfs64(path, &vfs) < 0) return errno;
  #else
    struct statvfs vfs;
    if(statvfs(path, &vfs) < 0) return errno;
  #endif

    info->capacity = ul_static_cast(ulfd_uint64_t, vfs.f_blocks) * vfs.f_frsize;
    info->free = ul_static_cast(ulfd_uint64_t, vfs.f_bfree) * vfs.f_frsize;
    info->available = ul_static_cast(ulfd_uint64_t, vfs.f_bavail) * vfs.f_frsize;
    return 0;
  }
  int ulfd_space_w(ulfd_spaceinfo_t* info, const wchar_t* wpath) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_space(info, path);
    _ulfd_end_to_str(path);
    return ret;
  }

#endif

#endif /* ULFD_H */
