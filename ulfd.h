/*
File descriptor (Unix style)


# Dependence
  - Windows API
  - POSIX API


# Note
  All functions use 64-bit integer to indicate offset and size.
  In POSIX, if `_LARGEFILE64_SOURCE` is not defined, functions will test if `off_t` can hold offset and size,
    which means you can also setting `_FILE_OFFSET_BITS` to 64 to make functions work right.


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
  typedef mode_t ulfd_mode_t;
  typedef nlink_t ulfd_nlink_t;
  typedef uid_t ulfd_uid_t;
  typedef gid_t ulfd_gid_t;
#endif


#define ULFD_S_IMASK  07777 /* a mask to get access of file */
#define ULFD_S_IALL   00777 /* read, write, execute permission for everyone */
#define ULFD_S_IRALL  00444 /* read permission for everyone */
#define ULFD_S_IWALL  00222 /* write permission for everyone */
#define ULFD_S_IXALL  00111 /* execute permission for everyone */

#define ULFD_S_ISUID  04000 /* set-user-ID on execute bit */
#define ULFD_S_ISGID  02000 /* set-group-ID on execute bit */
#define ULFD_S_ISVTX  01000 /* the sticky bit */

#define ULFD_S_IRWXU  00700 /* equivalent to (ULFD_S_IRUSR | ULFD_S_IWUSR | ULFD_S_IXUSR) */
#define ULFD_S_IRUSR  00400 /* read permission bit for the owner of the file */
#define ULFD_S_IWUSR  00200 /* write permission bit for the owner of the file */
#define ULFD_S_IXUSR  00100 /* execute or ordinary permission bit for the owner of the file */

#define ULFD_S_IRWXG  00070 /* equivalent to (ULFD_S_IRGRP | ULFD_S_IWGRP | ULFD_S_IXGRP) */
#define ULFD_S_IRGRP  00040 /* read permission bit for the group owner of the file */
#define ULFD_S_IWGRP  00020 /* write permission bit for the group owner of the file */
#define ULFD_S_IXGRP  00010 /* execute or ordinary permission bit for the group owner of the file */

#define ULFD_S_IRWXO  00007  /* equivalent to (ULFD_S_IROTH | ULFD_S_IWOTH | ULFD_S_IXOTH) */
#define ULFD_S_IROTH  00004  /* read permission bit for other users */
#define ULFD_S_IWOTH  00002  /* write permission bit for other users */
#define ULFD_S_IXOTH  00001  /* execute or ordinary permission bit for other users */

#define ULFD_S_IREAD  ULFD_S_IRUSR /* read permission bit for the owner of the file */
#define ULFD_S_IWRITE ULFD_S_IWUSR /* write permission bit for the owner of the file */
#define ULFD_S_IEXEC  ULFD_S_IXUSR /* execute or ordinary permission bit for the owner of the file */

#define ULFD_S_IFMT   0170000 /* a mask to get type of file */
#define ULFD_S_IFSOCK 0140000 /* socket */
#define ULFD_S_IFLNK  0120000 /* symbolic link */
#define ULFD_S_IFREG  0100000 /* regular */
#define ULFD_S_IFBLK  0060000 /* block special */
#define ULFD_S_IFDIR  0040000 /* directory */
#define ULFD_S_IFCHR  0020000 /* character special */
#define ULFD_S_IFIFO  0010000 /* FIFO special */

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

/*
  POSIX: sets nonblocking I/O mode.
  (In windows, asynchronous IO needs to use different APIs, so it's ignored in Windows)
*/
#define ULFD_O_NONBLOCK  (1l << 16)
/* POSIX: prevent the OS from assigning the opened file as the process's controlling terminal when opening a TTY device file */
#define ULFD_O_NOCTTY    (1l << 17)

#define ULFD_O_DENYRD    (1l << 24) /* Windows: deny share read access */
#define ULFD_O_DENYWR    (1l << 25) /* Windows: deny share write access */
#define ULFD_O_DENYRW    (ULFD_O_DENYRD | ULFD_O_DENYWR) /* Windows: deny share read and write access */
#define ULFD_O_SECURE    (1l << 26) /* Winodws: set secure mode (share read access but exclude write access) */
#define ULFD_O_HIDDEN    (1l << 27) /* Windows: create hidden file */

ul_hapi int ulfd_open(ulfd_t* pfd, const char* path, long oflag, ulfd_mode_t mode);
ul_hapi int ulfd_open_w(ulfd_t* pfd, const wchar_t* wpath, long oflag, ulfd_mode_t mode);
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

ul_hapi int ulfd_ffullsync(ulfd_t fd);
ul_hapi int ulfd_fsync(ulfd_t fd);
ul_hapi int ulfd_fdatasync(ulfd_t fd);

#define ULFD_F_RDLCK 0 /* specify a read (or shared) lock */
#define ULFD_F_WRLCK 1 /* specify a write (or exclusive) lock */
#define ULFD_F_UNLCK 2 /* specify that the region is unlocked */
ul_hapi int ulfd_lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode);
ul_hapi int ulfd_lockw(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode); /* wait for that lock to be released */

ul_hapi int ulfd_ftruncate(ulfd_t fd, ulfd_int64_t length);
ul_hapi int ulfd_filelength(ulfd_t fd, ulfd_int64_t* plength);
/* `mode` accepts ULFD_S_IRUSR, ULFD_S_IWUSR, ULFD_S_IXUSR or their union */
ul_hapi int ulfd_fchmod(ulfd_t fd, ulfd_mode_t mode);
ul_hapi int ulfd_fchown(ulfd_t fd, ulfd_uid_t uid, ulfd_gid_t gid);
ul_hapi int ulfd_futime(ulfd_t fd, ulfd_int64_t atime, ulfd_int64_t mtime);

ul_hapi int ulfd_isatty(ulfd_t fd, int* presult);

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


ul_hapi int ulfd_chdir(const char* path);
ul_hapi int ulfd_chdir_w(const wchar_t* wpath);
ul_hapi int ulfd_getcwd(char* path, size_t length);
ul_hapi int ulfd_getcwd_w(wchar_t* wpath, size_t length);
ul_hapi int ulfd_getcwd_alloc(char** ppath);
ul_hapi int ulfd_getcwd_alloc_w(wchar_t** pwpath);

ul_hapi ulfd_mode_t ulfd_umask(ulfd_mode_t mask);
/* `mode` accepts ULFD_S_IRUSR, ULFD_S_IWUSR, ULFD_S_IXUSR or their union */
ul_hapi int ulfd_chmod(const char* path, ulfd_mode_t mode);
/* `mode` accepts ULFD_S_IRUSR, ULFD_S_IWUSR, ULFD_S_IXUSR or their union */
ul_hapi int ulfd_chmod_w(const wchar_t* wpath, ulfd_mode_t mode);
ul_hapi int ulfd_chown(const char* path, ulfd_uid_t uid, ulfd_gid_t gid);
ul_hapi int ulfd_chown_w(const wchar_t* wpath, ulfd_uid_t uid, ulfd_gid_t gid);
ul_hapi int ulfd_utime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime);
ul_hapi int ulfd_utime_w(const wchar_t* path, ulfd_int64_t atime, ulfd_int64_t mtime);

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

ul_hapi int ulfd_stat(const char* path, ulfd_stat_t* stat);
ul_hapi int ulfd_stat_w(const wchar_t* wpath, ulfd_stat_t* stat);
ul_hapi int ulfd_fstat(ulfd_t fd, ulfd_stat_t* stat);
ul_hapi int ulfd_access(const char* path, ulfd_mode_t mode);
ul_hapi int ulfd_access_w(const wchar_t* wpath, ulfd_mode_t mode);

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
  } ulfd_dir_t;
#endif
ul_hapi int ulfd_opendir(ulfd_dir_t* dir, const char* path);
ul_hapi int ulfd_opendir_w(ulfd_dir_t* dir, const wchar_t* wpath);
ul_hapi int ulfd_rewinddir(ulfd_dir_t* dir);
ul_hapi int ulfd_closedir(ulfd_dir_t* dir);
ul_hapi int ulfd_readdir(ulfd_dir_t* dir, const char** pname);
ul_hapi int ulfd_readdir_w(ulfd_dir_t* dir, const wchar_t** pwname);


#include <string.h>
ul_hapi char* ulfd_strdup(const char* str) {
  char* ret;
  size_t len = strlen(str);
  ret = ul_reinterpret_cast(char*, ul_malloc(len + 1));
  if(ul_unlikely(ret == NULL)) return NULL;
  memcpy(ret, str, len + 1);
  return ret;
}
ul_hapi wchar_t* ulfd_wcsdup(const wchar_t* wstr) {
  wchar_t* ret;
  size_t bytes;
  for(bytes = 0; wstr[bytes]; ++bytes) { }
  bytes = (bytes + 1) * sizeof(wchar_t);
  ret = ul_reinterpret_cast(wchar_t*, ul_malloc(bytes));
  if(ul_unlikely(ret == NULL)) return NULL;
  memcpy(ret, wstr, bytes);
  return ret;
}


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
      for(i = l; i-- && (ul_static_cast(unsigned, *s) & 0xC0u) == 0x80u; u = (u << 6) | (ul_static_cast(unsigned, *s++) & 0x3Fu)) { }
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

#define _ulfd_begin_to_str(varname, wstr) do { \
  char* (varname); size_t varname ## _len; \
  (varname ## _len) = ul_os_wstr_to_str_len(wstr); \
  if(ul_unlikely((varname ## _len) == 0)) return EILSEQ; \
  (varname) = ul_reinterpret_cast(char*, ul_malloc(sizeof((varname)) * (varname ## _len))); \
  if(ul_unlikely((varname) == NULL)) return ENOMEM; \
  ul_os_wstr_to_str((varname), wstr)
#define _ulfd_end_to_str(varname) ul_free((varname)); } while(0)

#define _ulfd_begin_to_str2(varname, wstr, prev_varname) do { \
  char* (varname); size_t varname ## _len; \
  (varname ## _len) = ul_os_wstr_to_str_len(wstr); \
  if(ul_unlikely((varname ## _len) == 0)) { ul_free(prev_varname); return EILSEQ; } \
  (varname) = ul_reinterpret_cast(char*, ul_malloc(sizeof((varname)) * (varname ## _len))); \
  if(ul_unlikely((varname) == NULL)) { ul_free(prev_varname); return ENOMEM; } \
  ul_os_wstr_to_str((varname), wstr)
#define _ulfd_end_to_str2(varname) ul_free((varname)); } while(0)

#define _ulfd_begin_to_wstr(varname, str) do { \
  wchar_t* (varname); size_t varname ## _len; \
  (varname ## _len) = ul_os_str_to_wstr_len(str); \
  if(ul_unlikely((varname ## _len) == 0)) return EILSEQ; \
  (varname) = ul_reinterpret_cast(wchar_t*, ul_malloc(sizeof((varname)) * (varname ## _len))); \
  if(ul_unlikely((varname) == NULL)) return ENOMEM; \
  ul_os_str_to_wstr((varname), str)
#define _ulfd_end_to_wstr(varname) ul_free((varname)); } while(0)

#define _ulfd_begin_to_wstr2(varname, str, prev_varname) do { \
  wchar_t* (varname); size_t varname ## _len; \
  (varname ## _len) = ul_os_str_to_wstr_len(str); \
  if(ul_unlikely((varname ## _len) == 0)) { ul_free(prev_varname); return EILSEQ; } \
  (varname) = ul_reinterpret_cast(wchar_t*, ul_malloc(sizeof((varname)) * (varname ## _len))); \
  if(ul_unlikely((varname) == NULL)) { ul_free(prev_varname); return ENOMEM; } \
  ul_os_str_to_wstr((varname), str)
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
    #define FILE_MAP_EXECUTE 0x0020
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

  /* get kernel32 module */
  /* For any function supported since Windows Vista or higher, if _WIN32_WINNT is smaller,
    the library will dynamically load kernel32.dll to check available */
  ul_hapi HMODULE _ulfd_kernel32(void) {
    static HANDLE _ulfd_kernel32_hold = NULL;
    HANDLE stored;
    stored = _ulfd_compare_exchange(&_ulfd_kernel32_hold, NULL, NULL);
    if(stored == INVALID_HANDLE_VALUE) return NULL;
    if(stored != NULL) return ul_reinterpret_cast(HMODULE, stored);
    stored = GetModuleHandle(TEXT("kernel32.dll"));
    if(stored == NULL) _ulfd_compare_exchange(&_ulfd_kernel32_hold, INVALID_HANDLE_VALUE, NULL);
    else _ulfd_compare_exchange(&_ulfd_kernel32_hold, stored, NULL);
    return ul_reinterpret_cast(HMODULE, stored);
  }
  ul_hapi HANDLE _ulfd_kernel32_function(HANDLE* hold, const char* name) {
    HANDLE stored;
    HMODULE _kernel32;
    _kernel32 = _ulfd_kernel32();
    if(_kernel32 == NULL) return NULL;
    stored = _ulfd_compare_exchange(hold, NULL, NULL);
    if(stored == INVALID_HANDLE_VALUE) return NULL;
    if(stored != NULL) return ul_reinterpret_cast(HANDLE, stored);
    stored = ul_reinterpret_cast(HANDLE, GetProcAddress(_kernel32, name));
    if(stored == NULL) _ulfd_compare_exchange(hold, INVALID_HANDLE_VALUE, NULL);
    else _ulfd_compare_exchange(hold, stored, NULL);
    return ul_reinterpret_cast(HANDLE, stored);
  }

  typedef DWORD (WINAPI *_ulfd_GetFinalPathNameByHandleW_t)(HANDLE hFile, LPWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags);
  #if (_WIN32_WINNT+0) >= 0x0600 /* Windows Vista */
    ul_hapi _ulfd_GetFinalPathNameByHandleW_t _ulfd_get_GetFinalPathNameByHandleW(void) {
      return ul_reinterpret_cast(_ulfd_GetFinalPathNameByHandleW_t, GetFinalPathNameByHandleW);
    }
  #else
    ul_hapi _ulfd_GetFinalPathNameByHandleW_t _ulfd_get_GetFinalPathNameByHandleW(void) {
      static HANDLE hold = NULL;
      return ul_reinterpret_cast(_ulfd_GetFinalPathNameByHandleW_t, _ulfd_kernel32_function(&hold, "GetFinalPathNameByHandleW"));
    }
  #endif

  ul_hapi int ulfd_open_w(ulfd_t* pfd, const wchar_t* wpath, long oflag, int mode) {
    DWORD access = 0;
    DWORD share;
    DWORD create;
    DWORD flags_attr = 0;
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

    if(oflag & ULFD_O_CREAT && (mode & ULFD_S_IWRITE) == 0)
      flags_attr = FILE_ATTRIBUTE_READONLY;
    else
      flags_attr = FILE_ATTRIBUTE_NORMAL;

    if(oflag & ULFD_O_TEMPORARY) {
      share |= FILE_SHARE_DELETE;
      access |= DELETE;
      flags_attr |= FILE_FLAG_DELETE_ON_CLOSE;
    }
    if(oflag & ULFD_O_HIDDEN) flags_attr |= FILE_ATTRIBUTE_HIDDEN;

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
  ul_hapi int ulfd_open(ulfd_t* pfd, const char* path, long oflag, int mode) {
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
    OVERLAPPED overlapped = { 0 };
    size_t sum_read_bytes = 0;
    DWORD read_bytes;
    DWORD error = 0;
    BOOL call_ret;

    if(ul_unlikely(off < 0)) return EINVAL;
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
    else return _ul_win32_toerrno(error);
  }
  ul_hapi int ulfd_pwrite(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes) {
    OVERLAPPED overlapped = { 0 };
    size_t sum_writen_bytes = 0;
    DWORD writen_bytes;
    DWORD error = 0;
    BOOL call_ret;

    if(ul_unlikely(off < 0)) return EINVAL;
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

  ul_hapi int ulfd_fsync(ulfd_t fd) {
    return FlushFileBuffers(fd) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_ffullsync(ulfd_t fd) { return ulfd_fsync(fd); }
  ul_hapi int ulfd_fdatasync(ulfd_t fd) { return ulfd_fsync(fd); }

  ul_hapi int _ulfd_lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode, DWORD flags) {
    OVERLAPPED overlapped = { 0 };
    DWORD error;

    if(len < 0 || off < 0) return EINVAL;
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
  ul_hapi int ulfd_filelength(ulfd_t fd, ulfd_int64_t* plength) {
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
  ul_hapi int ulfd_fstat(ulfd_t fd, ulfd_stat_t* stat) {
    wchar_t* wpath;
    int ret;

    ret = _ulfd_get_path_from_handle(fd, &wpath);
    if(ret) return ret;
    ret = ulfd_stat_w(wpath, stat);
    ul_free(wpath); return ret;
  }

  ul_hapi int ulfd_isatty(ulfd_t fd, int* presult) {
    DWORD ret;
    ret = GetFileType(fd);
    if(ret == FILE_TYPE_UNKNOWN) { *presult = 0; return _ul_win32_toerrno(GetLastError()); }
    *presult = (ret == FILE_TYPE_CHAR); return 0;
  }

  ul_hapi int ulfd_dup(ulfd_t* pnfd, ulfd_t ofd) {
    return DuplicateHandle(
        GetCurrentProcess(), ofd,
        GetCurrentProcess(), pnfd,
        0L, TRUE, DUPLICATE_SAME_ACCESS
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

    if(len == 0 && (flags & ~ULFD_PROT_EXEC) == ULFD_PROT_EXEC) return EINVAL;
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
        new_protect = ul_static_cast(DWORD, (prot & ULFD_PROT_WRITE) != 0 ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ);
      else
        new_protect = ul_static_cast(DWORD, (prot & ULFD_PROT_WRITE) != 0 ? PAGE_READWRITE : PAGE_READONLY);
    }

    return VirtualProtect(addr, len, new_protect, &old_protect) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_msync(void* addr, size_t len, int flags) {
    (void)flags;
    return FlushViewOfFile(addr, len) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_mlock(const void* addr, size_t len) {
    return VirtualLock(ul_const_cast(LPVOID, addr), len) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_munlock(const void* addr, size_t len) {
    return VirtualUnlock(ul_const_cast(LPVOID, addr), len) ? 0 : _ul_win32_toerrno(GetLastError());
  }

  typedef struct _ulfd_WIN32_MEMORY_RANGE_ENTRY {
    PVOID VirtualAddress;
    SIZE_T NumberOfBytes;
  } _ulfd_WIN32_MEMORY_RANGE_ENTRY;
  typedef BOOL (WINAPI *_ulfd_PrefetchVirtualMemory_t)(HANDLE, unsigned __int3264, _ulfd_WIN32_MEMORY_RANGE_ENTRY*, ULONG);
  #if (_WIN32_WINNT+0) >= 0x0602 /* Windows 8 */
    ul_hapi _ulfd_PrefetchVirtualMemory_t _ulfd_get_PrefetchVirtualMemory(void) {
      return ul_reinterpret_cast(_ulfd_PrefetchVirtualMemory_t, PrefetchVirtualMemory);
    }
  #else
    ul_hapi _ulfd_PrefetchVirtualMemory_t _ulfd_get_PrefetchVirtualMemory(void) {
      static HANDLE hold = NULL;
      return ul_reinterpret_cast(_ulfd_PrefetchVirtualMemory_t, _ulfd_kernel32_function(&hold, "PrefetchVirtualMemory"));
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

  ul_hapi int ulfd_chdir_w(const wchar_t* wpath) {
    wchar_t rpath[ULFD_PATH_MAX + 1];
    wchar_t key[] = L"=A:";

    if(!SetCurrentDirectoryW(wpath)) return _ul_win32_toerrno(GetLastError());

    /*
      After chdir(), we need set current directory for every drive.
      For exchange, if the current directory is "D:\onedir\anotherdir", we need to set "=D:" to "D:\onedir\anotherdir".
      It aims to be compatible with old MS-DOS behaviour(more details can be found in https://devblogs.microsoft.com/oldnewthing/20100506-00).
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
    size_t cast_len;
    char* path;
    int err;

    err = ulfd_getcwd_alloc_w(&wpath);
    if(err) return err;
    cast_len = ul_os_wstr_to_str_len(wpath);
    if(ul_unlikely(cast_len == 0)) { ul_free(wpath); return EILSEQ; }
    path = ul_reinterpret_cast(char*, ul_malloc(cast_len));
    if(ul_unlikely(path == NULL)) { ul_free(wpath); return ENOMEM; }
    ul_os_wstr_to_str(path, wpath);
    ul_free(wpath);
    *ppath = path;
    return 0;
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
      return ul_reinterpret_cast(_ulfd_TzSpecificLocalTimeToSystemTime_t, _ulfd_kernel32_function(&hold, "TzSpecificLocalTimeToSystemTime"));
    }
  #endif

  ul_hapi int _ulfd_time64_to_filetime(FILETIME* out, ulfd_int64_t time) {
    static const LONGLONG EPOCH = ULFD_INT64_C(116444736000000000);
    SYSTEMTIME local_time, system_time;
    FILETIME local_file_time;
  #if (_WIN32_WINNT+0) < 0x0501 /* Windows XP */
    _ulfd_TzSpecificLocalTimeToSystemTime_t sysfunc;
    sysfunc = _ulfd_get_TzSpecificLocalTimeToSystemTime();
    if(sysfunc == NULL) return -1; /* It shouldn't happen */
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
    (void)mask; return 0777;
  }
  ul_hapi int ulfd_chmod_w(const wchar_t* wpath, ulfd_mode_t mode) {
    DWORD attributes = GetFileAttributesW(wpath);
    if(attributes == INVALID_FILE_ATTRIBUTES) return _ul_win32_toerrno(GetLastError());
    if((mode & ULFD_S_IWRITE) == 0) attributes &= ul_static_cast(DWORD, ~FILE_ATTRIBUTE_READONLY);
    else attributes |= FILE_ATTRIBUTE_READONLY;
    return SetFileAttributesW(wpath, attributes) ? 0 : _ul_win32_toerrno(GetLastError());
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
  ul_hapi int ulfd_utime_w(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime) {
    FILETIME access_time, write_time;
    HANDLE handle;
    DWORD error = 0;

    if(!_ulfd_time64_to_filetime(&access_time, atime)) return ERANGE;
    if(!_ulfd_time64_to_filetime(&write_time, mtime)) return ERANGE;
    handle = CreateFileW(wpath,
      FILE_WRITE_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL
      );
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());
    if(!SetFileTime(handle, NULL, &access_time, &write_time)) error = GetLastError();
    CloseHandle(handle);
    return _ul_win32_toerrno(error);
  }
  ul_hapi int ulfd_utime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_utime_w(wpath, atime, mtime);
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

    attr &= 0xFFu;

    /* check if `wpath` is a directory or root of a disk */
    mode = ((attr & FILE_ATTRIBUTE_DIRECTORY) || _ulfd_is_root_or_empty(wpath))
      ? ULFD_S_IFDIR | ULFD_S_IEXEC : ULFD_S_IFREG;

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

  ul_hapi int ulfd_stat_w(const wchar_t* wpath, ulfd_stat_t* stat) {
    HANDLE handle;
    int ret = 0;

    memset(stat, 0, sizeof(*stat));
    handle = CreateFileW(
        wpath,
        FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL
      );
    if(handle != INVALID_HANDLE_VALUE) {
      DWORD file_type = ul_static_cast(DWORD, GetFileType(handle) & ul_static_cast(DWORD, ~FILE_TYPE_REMOTE));
      if(file_type == FILE_TYPE_DISK) {
        BY_HANDLE_FILE_INFORMATION file_info;

        do {
          int drive_number;
          ret = _ulfd_get_drive_number_from_path(wpath, &drive_number);
          if(ret) goto do_return;
          stat->rdev = drive_number - 1;
          stat->dev = drive_number - 1;
        } while(0);

        if(!GetFileInformationByHandle(handle, &file_info)) {
          ret = _ul_win32_toerrno(GetLastError()); goto do_return;
        }
        stat->nlink = ul_static_cast(ulfd_nlink_t, file_info.nNumberOfLinks);
        stat->mode = _ulfd_stat_mode_cast(file_info.dwFileAttributes, wpath);
        stat->mtime = _ulfd_filetime_to_time_t(file_info.ftLastWriteTime, 0);
        stat->atime = _ulfd_filetime_to_time_t(file_info.ftLastAccessTime, stat->mtime);
        stat->ctime = _ulfd_filetime_to_time_t(file_info.ftCreationTime, stat->mtime);

        stat->size = ul_static_cast(
          ulfd_int64_t,
          (ul_static_cast(ULONGLONG, file_info.nFileSizeHigh) << 32) | file_info.nFileSizeLow
        );
        ret = 0; goto do_return;
      } else if(file_type == FILE_TYPE_CHAR || file_type == FILE_TYPE_PIPE) {
        stat->mode = file_type == FILE_TYPE_CHAR ? ULFD_S_IFCHR : ULFD_S_IFIFO;
        stat->nlink = 1;
        stat->rdev = -1;
        stat->dev = -1;
        if(file_type != FILE_TYPE_CHAR) {
          DWORD available;
          if(PeekNamedPipe(handle, NULL, 0, NULL, &available, NULL))
            stat->size = ul_static_cast(ulfd_int64_t, available);
        }
        ret = 0; goto do_return;
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
      stat->mode = _ulfd_stat_mode_cast(FILE_ATTRIBUTE_DIRECTORY, wpath);
      stat->nlink = 1;
      ret = _ulfd_get_drive_number_from_path(wpath, &drive_number);
      if(ret) goto do_return;
      stat->rdev = drive_number - 1;
      stat->dev = drive_number - 1;
      stat->mtime = -1;
      stat->atime = stat->mtime;
      stat->ctime = stat->mtime;
      ret = 0; goto do_return;
    }

  do_return:
    if(handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
    return ret;
  }
  ul_hapi int ulfd_stat(const char* path, ulfd_stat_t* stat) {
    int ret;
    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_stat_w(wpath, stat);
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
    (void)mode; return CreateDirectoryW(wpath, NULL) ? 0 : _ul_win32_toerrno(GetLastError());
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

  #if _WIN32_WINNT >= 0x0501 /* Windows XP */
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
      return ul_reinterpret_cast(_ulfd_CreateHardLinkW_t, _ulfd_kernel32_function(&hold, "CreateHardLinkW"));
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
  #if (_WIN32_WINNT+0) >= 0x0600 /* Windows Vista */
    ul_hapi _ulfd_CreateSymbolicLinkW_t _ulfd_get_CreateSymbolicLinkW(void) {
      return ul_reinterpret_cast(_ulfd_CreateSymbolicLinkW_t, CreateSymbolicLinkW);
    }
  #else
    ul_hapi _ulfd_CreateSymbolicLinkW_t _ulfd_get_CreateSymbolicLinkW(void) {
      static HANDLE hold = NULL;
      return ul_reinterpret_cast(_ulfd_CreateSymbolicLinkW_t, _ulfd_kernel32_function(&hold, "CreateSymbolicLinkW"));
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
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL
      );
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());

    sysfunc = _ulfd_get_GetFinalPathNameByHandleW();
    if(ul_unlikely(sysfunc == NULL)) return ENOSYS;

    adjust_len = len >= 0xFFFFFFFFu ? 0xFFFFFFFFu : ul_static_cast(DWORD, len);
    writen = sysfunc(handle, buf, adjust_len, 0);
    if(writen == 0) error = GetLastError();
    CloseHandle(handle);

    if(writen == 0) return _ul_win32_toerrno(error);
    if(writen >= len) return ENOMEM;
    buf[writen] = 0; return 0;
  }
  ul_hapi int ulfd_readlink(const char* path, char* buf, size_t len) {
    wchar_t* wpath;
    size_t path_len, cast_len;
    HANDLE handle;
    DWORD writen;
    _ulfd_GetFinalPathNameByHandleW_t sysfunc;

    _ulfd_begin_to_wstr(wpath_, path);
    handle = CreateFileW(
        wpath_,
        FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL
      );
    _ulfd_end_to_wstr(wpath_);
    if(handle == INVALID_HANDLE_VALUE) return _ul_win32_toerrno(GetLastError());

    sysfunc = _ulfd_get_GetFinalPathNameByHandleW();
    if(ul_unlikely(sysfunc == NULL)) return ENOSYS;

    path_len = sysfunc(handle, NULL, 0, 0);
    if(path_len == 0) return _ul_win32_toerrno(GetLastError());
    wpath = ul_reinterpret_cast(wchar_t*, malloc((path_len + 1) * sizeof(wchar_t)));
    if(ul_unlikely(wpath == NULL)) return ENOMEM;
    writen = sysfunc(handle, wpath, ul_static_cast(DWORD, path_len), 0);
    if(writen == 0) { CloseHandle(handle); ul_free(wpath); return _ul_win32_toerrno(GetLastError()); }

    CloseHandle(handle);
    wpath[writen] = 0;
    cast_len = ul_os_wstr_to_str_len(wpath);
    if(ul_unlikely(cast_len == 0)) { ul_free(wpath); return EILSEQ; }
    if(cast_len > len) { ul_free(wpath); return ERANGE; }
    ul_os_wstr_to_str(buf, wpath);
    ul_free(wpath); return 0;
  }
  ul_hapi int ulfd_realpath_alloc_w(wchar_t** presolved, const wchar_t* wpath) {
    DWORD len, writen;
    wchar_t* resolved;
    if(wpath[0] == '\0') return ulfd_getcwd_alloc_w(presolved);
    len = GetFullPathNameW(wpath, NULL, 0, NULL);
    if(len == 0) return _ul_win32_toerrno(GetLastError());
    resolved = ul_reinterpret_cast(wchar_t*, ul_malloc(len * sizeof(wchar_t)));
    if(ul_unlikely(resolved == NULL)) return ENOMEM;
    writen = GetFullPathNameW(wpath, len, resolved, NULL);
    if(writen > len) { ul_free(resolved); return ERANGE; }
    *presolved = resolved; return 0;
  }
  ul_hapi int ulfd_realpath_alloc(char** presolved, const char* path) {
    wchar_t* wresolved;
    char* resolved;
    size_t cast_len;
    int ret;

    _ulfd_begin_to_wstr(wpath, path);
    ret = ulfd_realpath_alloc_w(&wresolved, wpath);
    _ulfd_end_to_wstr(wpath);
    if(ret) return ret;

    cast_len = ul_os_wstr_to_str_len(wresolved);
    if(ul_unlikely(cast_len == 0)) { ul_free(wresolved); return EILSEQ; }
    resolved = ul_reinterpret_cast(char*, ul_malloc(cast_len));
    if(ul_unlikely(resolved == NULL)) { ul_free(wresolved); return ENOMEM; }
    ul_os_wstr_to_str(resolved, wresolved);
    ul_free(wresolved);
    *presolved = resolved;
    return 0;
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
    if(ul_likely(dir->handle)) FindClose(dir->handle);

    dir->cached = 1;
    dir->handle = FindFirstFileW(dir->dirpath, &dir->data);
    if(dir->handle == INVALID_HANDLE_VALUE) {
      DWORD error = GetLastError();
      return _ul_win32_toerrno(error);
    }

    for(;;) { /* skip L"." and L".." */
      if(dir->data.cFileName[0] != L'.') break;
      if(dir->data.cFileName[1] == 0) { }
      else if(dir->data.cFileName[1] == L'.' && dir->data.cFileName[2] == 0) { }
      else break;

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

    len = ul_os_str_to_wstr_len(path);
    if(ul_unlikely(len == 0)) return EILSEQ;
    --len;
    dirpath = ul_reinterpret_cast(wchar_t*, ul_malloc((len + 4) * sizeof(wchar_t)));
    if(ul_unlikely(dirpath == NULL)) return ENOMEM;
    ul_os_str_to_wstr(dirpath, path);
    if(path[0] == 0) dirpath[len++] = L'.';
    if(!_ulfd_is_slash(dirpath[len - 1])) dirpath[len++] = L'\\';
    dirpath[len++] = L'*'; dirpath[len] = 0;

    dir->dirpath = dirpath;
    dir->entry = NULL;
    dir->entry_cap = 0;
    dir->handle = INVALID_HANDLE_VALUE;

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

    for(len = 0; wpath[len]; ++len) { }
    if(ul_unlikely(len == 0)) return ENOENT;
    dirpath = ul_reinterpret_cast(wchar_t*, ul_malloc((len + 4) * sizeof(wchar_t)));
    if(ul_unlikely(dirpath == NULL)) return ENOMEM;
    memcpy(dirpath, wpath, len * sizeof(wchar_t));
    if(wpath[0] == 0) dirpath[len++] = L'.';
    if(!_ulfd_is_slash(dirpath[len - 1])) dirpath[len++] = L'\\';
    dirpath[len++] = L'*'; dirpath[len] = 0;

    dir->dirpath = dirpath;
    dir->entry = NULL;
    dir->entry_cap = 0;

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

#else
  #include <fcntl.h>
  #include <sys/stat.h>

  #ifdef __GLIBC__
    #define _ULFD_GLIBC_CHECK(MAJOR, MINOR) \
      (__GLIBC__ > (MAJOR) || (__GLIBC__ == (MAJOR) && __GLIBC_MINOR__ >= (MINOR)))
    #define _ULFD_GLIBC_CHECK_BELOW(MAJOR, MINOR) \
      (__GLIBC__ < (MAJOR) || (__GLIBC__ == (MAJOR) && __GLIBC_MINOR__ <= (MINOR)))
  #else
    #define _ULFD_GLIBC_CHECK(MAJOR, MINOR) (0)
    #define _ULFD_GLIBC_CHECK_BELOW(MAJOR, MINOR) (0)
  #endif

  #define _ulfd_to_access_mode(mode) ul_static_cast(mode_t, (mode) & ULFD_S_IMASK)
  #define _ulfd_from_access_mode(mode) ul_static_cast(mode_t, (mode) & ULFD_S_IMASK)
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
    ulfd_mode_t ret = ul_static_cast(mode_t, mode & 0777);
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


  ul_hapi int ulfd_open(ulfd_t* pfd, const char* path, long oflag, ulfd_mode_t mode) {
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
  ul_hapi int ulfd_open_w(ulfd_t* pfd, const wchar_t* wpath, long oflag, ulfd_mode_t mode) {
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
  #if (_XOPEN_SOURCE+0) >= 500 || \
      (_ULFD_GLIBC_CHECK(2, 12) && (_POSIX_C_SOURCE+0) >= 200809L)
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
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_pwrite(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes) {
  #if (_XOPEN_SOURCE+0) >= 500 || \
      (_ULFD_GLIBC_CHECK(2, 12) && (_POSIX_C_SOURCE+0) >= 200809L)
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

  ul_hapi int ulfd_ffullsync(ulfd_t fd) {
  #ifdef F_FULLFSYNC
    return fcntl(fd, F_FULLFSYNC, 0) < 0 ? errno : 0;
  #else
    (void)fd; return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_fsync(ulfd_t fd) {
  #if _ULFD_GLIBC_CHECK(2, 16) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 15) && \
        ((BSD_SOURCE+0) || (_XOPEN_SOURCE+0) || (_ULFD_GLIBC_CHECK(2, 8) && (_POSIX_C_SOURCE+0) >= 200112L)))
    return fsync(fd) < 0 ? errno : 0;
  #else
    (void)fd; return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_fdatasync(ulfd_t fd) {
  #if (_POSIX_C_SOURCE+0) >= 199309L || (_XOPEN_SOURCE+0) >= 500
    return fdatasync(fd) < 0 ? errno : 0;
  #else
    (void)fd; return ENOSYS;
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
  #if (_XOPEN_SOURCE+0) >= 500 || \
      (_ULFD_GLIBC_CHECK(2, 4) && (_POSIX_C_SOURCE+0) >= 200112L) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) || (_BSD_SOURCE+0)) || \
      ((_XOPEN_SOURCE+0) && (_XOPEN_SOURCE_EXTENDED+0))
    #ifdef ULFD_HAS_LFS
      return ftruncate64(fd, length) < 0 ? errno : 0;
    #else
      if(ul_static_cast(off_t, length) != length) return EOVERFLOW;
      return ftruncate(fd, ul_static_cast(off_t, length)) < 0 ? errno : 0;
    #endif
  #else
    (void)fd; (void)length; return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_filelength(ulfd_t fd, ulfd_int64_t* plength) {
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
  #if (_ULFD_GLIBC_CHECK(2, 24) && (_POSIX_C_SOURCE+0) >= 199309L) || \
      (_ULFD_GLIBC_CHECK(2, 19) && _ULFD_GLIBC_CHECK_BELOW(2, 23) && (_POSIX_C_SOURCE+0)) || \
      (_ULFD_GLIBC_CHECK(2, 16) && _ULFD_GLIBC_CHECK_BELOW(2, 19) && ((_BSD_SOURCE+0) || (_POSIX_C_SOURCE+0))) || \
      (_ULFD_GLIBC_CHECK(2, 12) && _ULFD_GLIBC_CHECK_BELOW(2, 16) && \
        ((_BSD_SOURCE+0) || (_XOPEN_SOURCE+0) >= 500 || (_POSIX_C_SOURCE+0) >= 200809L)) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 11) && ((_BSD_SOURCE+0) || (_XOPEN_SOURCE+0) >= 500)) || \
      ((_XOPEN_SOURCE+0) && (_XOPEN_SOURCE_EXTENDED+0))
    return fchmod(fd, _ulfd_to_access_mode(mode)) < 0 ? errno : 0;
  #else
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_fchown(ulfd_t fd, ulfd_uid_t uid, ulfd_gid_t gid) {
  #if (_ULFD_GLIBC_CHECK(2, 12) && (_POSIX_C_SOURCE+0) >= 200809L) || \
      (_XOPEN_SOURCE+0) >= 500 || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) && (_BSD_SOURCE+0)) || \
      ((_XOPEN_SOURCE+0) && (_XOPEN_SOURCE_EXTENDED+0))
    return fchown(fd, uid, gid) < 0 ? errno : 0;
  #else
    return ENOSYS;
  #endif
  }
  #include <sys/time.h>
  ul_hapi int ulfd_futime(ulfd_t fd, ulfd_int64_t atime, ulfd_int64_t mtime) {
  #if (_ULFD_GLIBC_CHECK(2, 19) && (_DEFAULT_SOURCE+0)) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) && (_BSD_SOURCE+0))
    struct timeval tv[2];
    tv[0].tv_sec = ul_static_cast(time_t, atime / 1000);
    tv[0].tv_usec = ul_static_cast(suseconds_t, (atime % 1000) * 1000000);
    tv[1].tv_sec = ul_static_cast(time_t, mtime / 1000);
    tv[1].tv_usec = ul_static_cast(suseconds_t, (mtime % 1000) * 1000000);
    return futimes(fd, tv) < 0 ? errno : 0;
  #else
    return ENOSYS;
  #endif
  }

  ul_hapi int ulfd_isatty(ulfd_t fd, int* presult) {
    int r = isatty(fd);
    if(r < 0) return errno;
    *presult = (r > 0); return 0;
  }

  ul_hapi int ulfd_dup(ulfd_t* pnfd, ulfd_t ofd) {
    return (*pnfd = dup(ofd)) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_fdopen(FILE** pfp, ulfd_t fd, const char* mode) {
  #if (_POSIX_C_SOURCE+0) >= 1 || (_XOPEN_SOURCE+0) || (_POSIX_SOURCE+0)
    return (*pfp = fdopen(fd, mode)) == NULL ? errno : 0;
  #else
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
  #if (_POSIX_C_SOURCE+0) >= 1 || (_XOPEN_SOURCE+0) || (_POSIX_SOURCE+0)
    return (*pfd = fileno(fp)) < 0 ? errno : 0;
  #else
    return ENOSYS;
  #endif
  }

  ul_hapi int ulfd_truncate(const char* path, ulfd_int64_t length) {
  #if (_XOPEN_SOURCE+0) >= 500 || \
      (_ULFD_GLIBC_CHECK(2, 12) && (_POSIX_C_SOURCE+0) >= 200809L) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) && (_BSD_SOURCE+0)) || \
      ((_XOPEN_SOURCE+0) && (_XOPEN_SOURCE_EXTENDED+0))
    #ifdef ULFD_HAS_LFS
      return truncate64(path, length) < 0 ? errno : 0;
    #else
      if(ul_static_cast(off_t, length) != length) return EOVERFLOW;
      return truncate(path, ul_static_cast(off_t, length)) < 0 ? errno : 0;
    #endif
  #else
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
    return ENOSYS;
  #endif
  }

  ul_hapi size_t ulfd_pagesize(void) {
    return ul_static_cast(size_t, sysconf(_SC_PAGESIZE));
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
  #if (_GNU_SOURCE+0)
    char* ret;
    char* path = get_current_dir_name();
    if(ul_unlikely(path == NULL)) return errno;
    ret = ulfd_strdup(path);
    free(path);
    *ppath = ret;
    return ul_likely(ret) ? 0 : ENOMEM;
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
  #if (_GNU_SOURCE+0)
    wchar_t* ret;
    size_t need_len;
    char* path = get_current_dir_name();
    if(ul_unlikely(path == NULL)) return errno;
    need_len = ul_os_str_to_wstr_len(path);
    if(ul_unlikely(need_len == 0)) { free(path); return EILSEQ; }
    ret = ul_reinterpret_cast(wchar_t*, ul_malloc(need_len * sizeof(wchar_t)));
    if(ul_unlikely(ret == NULL)) { free(path); return ENOMEM; }
    ul_os_str_to_wstr(ret, path);
    free(path);
    *pwpath = ret;
    return 0;
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

  #include <sys/time.h>
  ul_hapi int ulfd_utime(const char* path, ulfd_int64_t atime, ulfd_int64_t mtime) {
    struct timeval tv[2];
    tv[0].tv_sec = ul_static_cast(time_t, atime / 1000);
    tv[0].tv_usec = ul_static_cast(suseconds_t, (atime % 1000) * 1000000);
    tv[1].tv_sec = ul_static_cast(time_t, mtime / 1000);
    tv[1].tv_usec = ul_static_cast(suseconds_t, (mtime % 1000) * 1000000);
    return utimes(path, tv) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_utime_w(const wchar_t* wpath, ulfd_int64_t atime, ulfd_int64_t mtime) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_utime(path, atime, mtime);
    _ulfd_end_to_str(path);
    return ret;
  }

  ul_hapi int ulfd_stat(const char* path, ulfd_stat_t* out) {
  #ifdef ULFD_HAS_LFS
    struct stat64 state;
    if(stat64(path, &state) < 0) return errno;
  #else
    struct stat state;
    if(stat(path, &state) < 0) return errno;
  #endif

    out->dev = state.st_dev;
    out->rdev = state.st_rdev;
    out->ino = state.st_ino;
    out->mode = _ulfd_from_full_mode(state.st_mode);
    out->nlink = state.st_nlink;
    out->uid = state.st_uid;
    out->gid = state.st_gid;
    out->size = ul_static_cast(ulfd_int64_t, state.st_size);
  #if (_ULFD_GLIBC_CHECK(2, 12) && ((_POSIX_C_SOURCE+0) >= 200809L || (_XOPEN_SOURCE+0) >= 700)) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) && ((_BSD_SOURCE+0) || (_SVID_SOURCE+0)))
    out->atime = ul_static_cast(ulfd_time_t, state.st_atim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state.st_atim.tv_nsec / 1000000);
    out->mtime = ul_static_cast(ulfd_time_t, state.st_mtim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state.st_mtim.tv_nsec / 1000000);
    out->ctime = ul_static_cast(ulfd_time_t, state.st_ctim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state.st_ctim.tv_nsec / 1000000);
  #else
    out->atime = ul_static_cast(ulfd_int64_t, state.st_atime * 1000);
    out->mtime = ul_static_cast(ulfd_int64_t, state.st_mtime * 1000);
    out->ctime = ul_static_cast(ulfd_int64_t, state.st_ctime * 1000);
  #endif
    return 0;
  }
  ul_hapi int ulfd_stat_w(const wchar_t* wpath, ulfd_stat_t* stat) {
    int ret;
    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_stat(path, stat);
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

    out->dev = state.st_dev;
    out->rdev = state.st_rdev;
    out->ino = state.st_ino;
    out->mode = _ulfd_from_full_mode(state.st_mode);
    out->nlink = state.st_nlink;
    out->uid = state.st_uid;
    out->gid = state.st_gid;
    out->size = ul_static_cast(ulfd_int64_t, state.st_size);
  #if (_ULFD_GLIBC_CHECK(2, 12) && ((_POSIX_C_SOURCE+0) >= 200809L || (_XOPEN_SOURCE+0) >= 700)) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) && ((_BSD_SOURCE+0) || (_SVID_SOURCE+0)))
    out->atime = ul_static_cast(ulfd_time_t, state.st_atim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state.st_atim.tv_nsec / 1000000);
    out->mtime = ul_static_cast(ulfd_time_t, state.st_mtim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state.st_mtim.tv_nsec / 1000000);
    out->ctime = ul_static_cast(ulfd_time_t, state.st_ctim.tv_sec) * 1000
      + ul_static_cast(ulfd_time_t, state.st_ctim.tv_nsec / 1000000);
  #else
    out->atime = ul_static_cast(ulfd_int64_t, state.st_atime * 1000);
    out->mtime = ul_static_cast(ulfd_int64_t, state.st_mtime * 1000);
    out->ctime = ul_static_cast(ulfd_int64_t, state.st_ctime * 1000);
  #endif
    return 0;
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
  #if (_XOPEN_SOURCE+0) >= 500 || (_POSIX_C_SOURCE+0) >= 200112L || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) && (_BSD_SOURCE+0))
    return symlink(source, target) < 0 ? errno : 0;
  #else
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
  #if ((_XOPEN_SOURCE+0) >= 500 || (_POSIX_C_SOURCE+0) >= 200112L) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) && (_BSD_SOURCE+0)) || \
      ((_XOPEN_SOURCE+0) && (_XOPEN_SOURCE_EXTENDED+0))
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
  ul_hapi int ulfd_readlink_w(const wchar_t* wpath, wchar_t* buf, size_t len) {
  #if ((_XOPEN_SOURCE+0) >= 500 || (_POSIX_C_SOURCE+0) >= 200112L) || \
      (_ULFD_GLIBC_CHECK_BELOW(2, 19) && (_BSD_SOURCE+0)) || \
      ((_XOPEN_SOURCE+0) && (_XOPEN_SOURCE_EXTENDED+0))
    char* tmp;
    size_t cast_len;
    ssize_t sret;
    int ret = 0;

    tmp = ul_reinterpret_cast(char*, ul_malloc(ULFD_PATH_MAX + 1));
    if(ul_unlikely(tmp == NULL)) return ENOMEM;

    _ulfd_begin_to_str(path, wpath);
    sret = readlink(path, tmp, ULFD_PATH_MAX + 1);
    _ulfd_end_to_str(path);

    if(sret < 0) { ret = errno; goto do_return; }
    if(sret == ULFD_PATH_MAX + 1) { ret = ERANGE; goto do_return; }
    tmp[sret] = 0;

    cast_len = ul_os_str_to_wstr_len(tmp);
    if(ul_unlikely(cast_len == 0)) { ret = EILSEQ; goto do_return; }
    if(cast_len > len) { ret = ERANGE; goto do_return; }
    ul_os_str_to_wstr(buf, tmp);

  do_return:
    ul_free(tmp); return ret;
  #else
    (void)wpath; (void)buf; (void)len; return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_realpath_alloc(char** presolved, const char* path) {
  #if (_GNU_SOURCE+0)
    char* resolved = canonicalize_file_name(path);
    if(resolved == NULL) return errno;
    *presolved = ulfd_strdup(resolved);
    free(resolved);
    return *presolved == NULL ? ENOMEM : 0;
  #elif (_BSD_SOURCE+0) || (_XOPEN_SOURCE+0) >= 500 || ((_XOPEN_SOURCE+0) && (_XOPEN_SOURCE_EXTENDED+0))
    #if (_POSIX_C_SOURCE+0) >= 200809L
      char* resolved = realpath(path, NULL);
      if(resolved == NULL) return errno;
      *presolved = ulfd_strdup(resolved);
      free(resolved);
      return *presolved == NULL ? ENOMEM : 0;
    #else
      char resolved[ULFD_PATH_MAX];
      if(!realpath(path, resolved)) return errno;
      return (*presolved = ulfd_strdup(resolved)) == NULL ? ENOMEM : 0;
    #endif
  #else
    return ENOSYS;
  #endif
  }
  ul_hapi int ulfd_realpath_alloc_w(wchar_t** presolved, const wchar_t* wpath) {
    wchar_t* wresolved;
    char* resolved;
    size_t cast_len;
    int ret;

    _ulfd_begin_to_str(path, wpath);
    ret = ulfd_realpath_alloc(&resolved, path);
    _ulfd_end_to_str(path);
    if(ret) return ret;

    cast_len = ul_os_str_to_wstr_len(resolved);
    if(ul_unlikely(cast_len == 0)) { ul_free(resolved); return EILSEQ; }
    wresolved = ul_reinterpret_cast(wchar_t*, ul_malloc(cast_len));
    if(ul_unlikely(resolved == NULL)) { ul_free(resolved); return ENOMEM; }
    ul_os_str_to_wstr(wresolved, resolved);
    ul_free(resolved);
    *presolved = wresolved;
    return 0;
  }


  ul_hapi int ulfd_opendir(ulfd_dir_t* dir, const char* path) {
    dir->dir = opendir(path);
    if(dir->dir == NULL) return errno;
    dir->entry = NULL;
    dir->entry_cap = 0;
    dir->wentry = NULL;
    dir->wentry_cap = 0;
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
    rewinddir(dir->dir); return 0;
  }
  ul_hapi int ulfd_closedir(ulfd_dir_t* dir) {
    if(ul_likely(dir->entry)) ul_free(dir->entry);
    if(ul_unlikely(dir->wentry)) ul_free(dir->wentry);
    if(ul_unlikely(!dir->dir)) return 0;
    return closedir(dir->dir) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_readdir(ulfd_dir_t* dir, const char** pname) {
    struct dirent* dirent;
    size_t len;

    errno = 0; dirent = readdir(dir->dir);
    if(!dirent) { *pname = NULL; return errno; }
    len = strlen(dirent->d_name) + 1;
    if(len > dir->entry_cap) {
      char* entry;
      size_t cap2 = dir->entry_cap + (dir->entry_cap >> 1) + 1;
      if(cap2 < len) cap2 = len;
      entry = ul_reinterpret_cast(char*, ul_realloc(dir->entry, cap2));
      if(ul_unlikely(entry == NULL)) return ENOMEM;
      dir->entry = entry; dir->entry_cap = cap2;
    }
    memcpy(dir->entry, dirent->d_name, len);
    *pname = dir->entry; return 0;
  }
  ul_hapi int ulfd_readdir_w(ulfd_dir_t* dir, const wchar_t** pwname) {
    struct dirent* dirent;
    size_t need_len;

    errno = 0; dirent = readdir(dir->dir);
    if(!dirent) { *pwname = NULL; return errno; }
    need_len = ul_os_str_to_wstr_len(dirent->d_name);
    if(ul_unlikely(need_len == 0)) return EILSEQ;
    if(need_len > dir->wentry_cap) {
      wchar_t* wentry;
      size_t cap2 = dir->wentry_cap + (dir->wentry_cap >> 1) + 1;
      if(cap2 < need_len) cap2 = need_len;
      wentry = ul_reinterpret_cast(wchar_t*, ul_realloc(dir->wentry, cap2 * sizeof(wchar_t)));
      if(ul_unlikely(wentry == NULL)) return ENOMEM;
      dir->wentry = wentry; dir->wentry_cap = cap2;
    }
    memcpy(dir->entry, dirent->d_name, need_len * sizeof(wchar_t));
    *pwname = dir->wentry; return 0;
  }

#endif

#endif /* ULFD_H */
