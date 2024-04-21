/*
File descriptor


# Dependence
  - Windows API
  - POSIX API (need LFS support)


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

/* enable LFS in POSIX */
#ifndef _LARGEFILE64_SOURCE
  #define _LARGEFILE64_SOURCE 1
#endif

#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>

#if defined(ULLONG_MAX) && ((ULLONG_MAX >> 63) >= 1)
  typedef long long ulfd_int64_t;
#elif defined(_WIN32)
  typedef __int64 ulfd_int64_t;
#else
  #error "ulfd.h: need 64-bit integer"
#endif

#ifdef _WIN32
  #include <Windows.h>
  typedef HANDLE ulfd_t;
  #define ulfd_invalid_value INVALID_HANDLE_VALUE

  #define ulfd_stdin (GetStdHandle(STD_INPUT_HANDLE))
  #define ulfd_stdout (GetStdHandle(STD_OUTPUT_HANDLE))
  #define ulfd_stderr (GetStdHandle(STD_ERROR_HANDLE))
#else
  #include <unistd.h>

  typedef int ulfd_t;
  #define ulfd_invalid_value -1

  #define ulfd_stdin STDIN_FILENO
  #define ulfd_stdout STDOUT_FILENO
  #define ulfd_stderr STDERR_FILENO
#endif

#define ULFD_S_ISUID    04000 /* set-user-ID on execute bit */
#define ULFD_S_ISGID    02000 /* set-group-ID on execute bit */
#define ULFD_S_ISVTX    01000 /* the sticky bit */

#define ULFD_S_IRWXU    00700 /* equivalent to (ULFD_S_IRUSR | ULFD_S_IWUSR | ULFD_S_IXUSR) */
#define ULFD_S_IRUSR    00400 /* read permission bit for the owner of the file */
#define ULFD_S_IWUSR    00200 /* write permission bit for the owner of the file */
#define ULFD_S_IXUSR    00100 /* execute or ordinary permission bit for the owner of the file */

#define ULFD_S_IRWXG    00070 /* equivalent to (ULFD_S_IRGRP | ULFD_S_IWGRP | ULFD_S_IXGRP) */
#define ULFD_S_IRGRP    00040 /* read permission bit for the group owner of the file */
#define ULFD_S_IWGRP    00020 /* write permission bit for the group owner of the file */
#define ULFD_S_IXGRP    00010 /* execute or ordinary permission bit for the group owner of the file */

#define ULFD_S_IRWXO    00007  /* equivalent to (ULFD_S_IROTH | ULFD_S_IWOTH | ULFD_S_IXOTH) */
#define ULFD_S_IROTH    00004  /* read permission bit for other users */
#define ULFD_S_IWOTH    00002  /* write permission bit for other users */
#define ULFD_S_IXOTH    00001  /* execute or ordinary permission bit for other users */

#define ULFD_S_IREAD    ULFD_S_IRUSR /* read permission bit for the owner of the file */
#define ULFD_S_IWRITE   ULFD_S_IWUSR /* write permission bit for the owner of the file */
#define ULFD_S_IEXEC    ULFD_S_IXUSR /* execute or ordinary permission bit for the owner of the file */


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

ul_hapi int ulfd_open(ulfd_t* pfd, const char* path, long oflag, int mode);
ul_hapi int ulfd_open_w(ulfd_t* pfd, const wchar_t* wpath, long oflag, int mode);
ul_hapi int ulfd_creat(ulfd_t* pfd, const char* path, int mode);
ul_hapi int ulfd_creat_w(ulfd_t* pfd, const wchar_t* path, int mode);
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

#define ULFD_F_RDLCK 0 /* specify a read (or shared) lock */
#define ULFD_F_WRLCK 1 /* specify a write (or exclusive) lock */
#define ULFD_F_UNLCK 2 /* specify that the region is unlocked */

ul_hapi int ulfd_lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode);
ul_hapi int ulfd_lockw(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode);

ul_hapi int ulfd_sync(ulfd_t fd);
ul_hapi int ulfd_truncate(ulfd_t fd, ulfd_int64_t length);
ul_hapi int ulfd_filelength(ulfd_t fd, ulfd_int64_t* plength);

#define ULFD_MAP_PROT_NONE  (0)      /* pages may not be accessed */
#define ULFD_MAP_PROT_READ  (1 << 0) /* pages may be read */
#define ULFD_MAP_PROT_WRITE (1 << 1) /* pages may be writen_bytes */
#define ULFD_MAP_PROT_EXEC  (1 << 2) /* pages may be executed */

#define ULFD_MAP_ANONYMOUS (1 << 3) /* the mapping isn't backed by any file */
#define ULFD_MAP_ANON      ULFD_MAP_ANONYMOUS
#define ULFD_MAP_FIXED     (1 << 4) /* place the mapping at exactly that address */

#define ULFD_MAP_SHARED    (1 << 5) /* POSIX: share this mapping */
#define ULFD_MAP_PRIVATE   (1 << 6) /* POSIX: create a private copy-on-write mapping */

ul_hapi int ulfd_mmap(void** pmap, ulfd_t fd, void* addr, size_t len, ulfd_int64_t off, int flags);
ul_hapi int ulfd_munmap(void* addr, size_t len);
ul_hapi int ulfd_mprotect(void* addr, size_t len, int prot);
#define ULFD_MS_ASYNC      1 /* POSIX: specifiy that an update be scheduled, but the call returns immediately */
#define ULFD_MS_SYNC       2 /* POSIX: request an update and waits for it to complete. */
#define ULFD_MS_INVALIDATE 4 /* POSIX: ask to invalidate other mappings of the same file */
ul_hapi int ulfd_msync(void* addr, size_t len, int ms_flags);
ul_hapi int ulfd_mlock(const void* addr, size_t len);
ul_hapi int ulfd_munlock(const void* addr, size_t len);

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

#ifdef _WIN32
  #ifndef _UL_WIN32_TOERRNO_DEFINED
    #include <Windows.h>
    #include <errno.h>

    /* some errno codes don't exist in older compiler */
    /* TODO: fix these errno codes or test more compilers? */
    #if (defined(_MSC_VER) && _MSC_VER >= 1900) || defined(__GNUC__)
      #define UL_WIN32_ERRNO_MODERN
    #endif

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
      /*{ ERROR_NOT_READY,                 ENOMEDIUM },    //   21 */
        { ERROR_CRC,                       EIO },          /*   23 */
        { ERROR_SEEK,                      EINVAL },       /*   25 */
        { ERROR_SECTOR_NOT_FOUND,          EINVAL },       /*   27 */
        { ERROR_SHARING_VIOLATION,         EBUSY },        /*   32 */
        { ERROR_LOCK_VIOLATION,            EBUSY },        /*   33 */
        { ERROR_SHARING_BUFFER_EXCEEDED,   ENOLCK },       /*   36 */
      #ifdef UL_WIN32_ERRNO_MODERN
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
      #ifdef UL_WIN32_ERRNO_MODERN
        { ERROR_MORE_DATA,                 EMSGSIZE },     /*  234 */
      #endif
        { ERROR_INVALID_EA_NAME,           EINVAL },       /*  254 */
        { ERROR_EA_LIST_INCONSISTENT,      EINVAL },       /*  255 */
      /*{ ERROR_NO_MORE_ITEMS,             ENMFILE },      //  259 */
        { ERROR_DIRECTORY,                 ENOTDIR },      /*  267 */
        { ERROR_EAS_DIDNT_FIT,             ENOSPC },       /*  275 */
        { ERROR_EA_TABLE_FULL,             ENOSPC },       /*  277 */
      #ifdef UL_WIN32_ERRNO_MODERN
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
      #ifdef UL_WIN32_ERRNO_MODERN
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
      #ifdef UL_WIN32_ERRNO_MODERN
        { ERROR_NOT_CONNECTED,             ENOLINK },      /* 2250 */
      #endif
        { ERROR_OPEN_FILES,                EAGAIN },       /* 2401 */
        { ERROR_ACTIVE_CONNECTIONS,        EACCES },       /* 2402 */
        { ERROR_DEVICE_IN_USE,             EAGAIN },       /* 2404 */
      #ifdef UL_WIN32_ERRNO_MODERN
        { ERROR_DS_GENERIC_ERROR,          EIO },          /* 8341 */
      #endif

      #ifdef UL_WIN32_ERRNO_MODERN
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
      /* if(oflag & (ULFD_O_EXCL | ULFD_O_TRUNC)) create = CREATE_NEW; */
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
    wchar_t wpath_buf[MAX_PATH];
    wchar_t* wpath;
    size_t len;
    int ret;

    len = ul_os_str_to_wstr_len(path);
    if(ul_unlikely(len == 0)) return _ul_win32_toerrno(ERROR_FILE_NOT_FOUND);
    if(ul_likely(len <= MAX_PATH)) wpath = wpath_buf;
    else wpath = ul_reinterpret_cast(wchar_t*, malloc(sizeof(wchar_t) * len));

    ul_os_str_to_wstr(wpath, path);
    ret = ulfd_open_w(pfd, wpath, oflag, mode);
    if(ul_unlikely(len > MAX_PATH)) free(wpath);
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

  do_return:
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

  do_return:
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

  do_return:
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

  do_return:
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

    if(!SetFilePointerEx(fd, move, &new_pos, method)) return _ul_win32_toerrno(GetLastError());
    *poff = new_pos.QuadPart;
    return 0;
  }
  ul_hapi int ulfd_tell(ulfd_t fd, ulfd_int64_t* poff) {
    return ulfd_seek(fd, 0, ULFD_SEEK_CUR, poff);
  }

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

  ul_hapi int ulfd_sync(ulfd_t fd) {
    return FlushFileBuffers(fd) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_truncate(ulfd_t fd, ulfd_int64_t length) {
    ulfd_int64_t nul_off;
    int ret;

    ret = ulfd_seek(fd, length, ULFD_SEEK_SET, &nul_off);
    if(ret) return ret;
    return SetEndOfFile(fd) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_filelength(ulfd_t fd, ulfd_int64_t* plength) {
    LARGE_INTEGER length;
    if(!GetFileSizeEx(fd, &length)) return _ul_win32_toerrno(GetLastError());
    *plength = length.QuadPart; return 0;
  }

  ul_hapi int ulfd_mmap(void** pmap, ulfd_t fd, void* addr, size_t len, ulfd_int64_t off, int flags) {
    DWORD off_low, off_high;
    DWORD maxsize_low, maxsize_high;
    DWORD protect, access = 0;
    HANDLE fm, map;

    if(off < 0) return EINVAL;
    off_low = ul_static_cast(DWORD, off);
    off_high = ul_static_cast(DWORD, off >> 32);
    off += len;
    maxsize_low = ul_static_cast(DWORD, off);
    maxsize_high = ul_static_cast(DWORD, off >> 32);

    if((flags & (ULFD_MAP_PROT_READ | ULFD_MAP_PROT_WRITE | ULFD_MAP_PROT_EXEC)) == ULFD_MAP_PROT_NONE)
      protect = 0;
    else
      if(flags & ULFD_MAP_PROT_EXEC)
        protect = ul_static_cast(DWORD, (flags & ULFD_MAP_PROT_WRITE) != 0 ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ);
      else
        protect = ul_static_cast(DWORD, (flags & ULFD_MAP_PROT_WRITE) != 0 ? PAGE_READWRITE : PAGE_READONLY);

    if(flags != ULFD_MAP_PROT_NONE) {
      if(flags & ULFD_MAP_PROT_READ) access |= FILE_MAP_READ;
      if(flags & ULFD_MAP_PROT_WRITE) access |= FILE_MAP_WRITE;
      if(flags & ULFD_MAP_PROT_EXEC) access |= FILE_MAP_EXECUTE;
    }

    if(len == 0 && (flags & ~ULFD_MAP_PROT_EXEC) == ULFD_MAP_PROT_EXEC) return EINVAL;
    if(flags & ULFD_MAP_ANONYMOUS) fd = INVALID_HANDLE_VALUE;

    fm = CreateFileMapping(fd, NULL, protect, maxsize_high, maxsize_low, NULL);
    if(fm == NULL) return _ul_win32_toerrno(GetLastError());

    if(flags & ULFD_MAP_FIXED) map = MapViewOfFile(fm, access, off_high, off_low, len);
    else map = MapViewOfFileEx(fm, access, off_high, off_low, len, addr);

    CloseHandle(fm);
    if(map == NULL) return _ul_win32_toerrno(GetLastError());
    *pmap = map; return 0;
  }
  ul_hapi int ulfd_munmap(void* addr, size_t len) {
    (void)len; return UnmapViewOfFile(addr) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_mprotect(void* addr, size_t len, int prot) {
    DWORD new_protect;
    DWORD old_protect = 0;

    if((prot & (ULFD_MAP_PROT_READ | ULFD_MAP_PROT_WRITE | ULFD_MAP_PROT_EXEC)) == ULFD_MAP_PROT_NONE)
      new_protect = 0;
    else
      if(prot & ULFD_MAP_PROT_EXEC)
        new_protect = ul_static_cast(DWORD, (prot & ULFD_MAP_PROT_WRITE) != 0 ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ);
      else
        new_protect = ul_static_cast(DWORD, (prot & ULFD_MAP_PROT_WRITE) != 0 ? PAGE_READWRITE : PAGE_READONLY);

    return VirtualProtect(addr, len, new_protect, &old_protect) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_msync(void* addr, size_t len, int ms_flags) {
    (void)ms_flags;
    return FlushViewOfFile(addr, len) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_mlock(const void* addr, size_t len) {
    return VirtualLock(ul_const_cast(LPVOID, addr), len) ? 0 : _ul_win32_toerrno(GetLastError());
  }
  ul_hapi int ulfd_munlock(const void* addr, size_t len) {
    return VirtualUnlock(ul_const_cast(LPVOID, addr), len) ? 0 : _ul_win32_toerrno(GetLastError());
  }
#else
  #include <fcntl.h>
  #include <sys/stat.h>

  ul_hapi int ulfd_open(ulfd_t* pfd, const char* path, long oflag, int mode) {
    int flag = 0;
    int fd;

    if(oflag & ULFD_O_RDONLY) flag |= O_RDONLY;
    if(oflag & ULFD_O_WRONLY) flag |= O_WRONLY;
    if(oflag & ULFD_O_RDWR) flag |= O_RDWR;

    if(oflag & ULFD_O_CREAT) flag |= O_CREAT;
    if(oflag & ULFD_O_EXCL) flag |= O_EXCL;
    if(oflag & ULFD_O_TRUNC) flag |= O_TRUNC;

    if(oflag & ULFD_O_APPEND) flag |= O_APPEND;
    if(oflag & ULFD_O_CLOEXEC) flag |= O_CLOEXEC;

    if(oflag & ULFD_O_NONBLOCK) flag |= O_NONBLOCK;
    if(oflag & ULFD_O_NOCTTY) flag |= O_NOCTTY;

    fd = open(path, flag, mode);

    if(fd < 0) return errno;
    if(oflag & ULFD_O_TEMPORARY) {
      if(unlink(path) < 0) {
        close(fd); return errno;
      }
    }
    *pfd = fd;
    return 0;
  }
  ul_hapi int ulfd_open_w(ulfd_t* pfd, const wchar_t* wpath, long oflag, int mode) {
    char path_buf[PATH_MAX];
    char* path;
    size_t len;
    int ret;

    len = ul_os_wstr_to_str_len(wpath);
    if(ul_unlikely(len == 0)) return ENOENT;
    if(ul_likely(len <= PATH_MAX)) path = path_buf;
    else path = ul_reinterpret_cast(char*, malloc(len));

    ul_os_wstr_to_str(path, wpath);
    ret = ulfd_open(pfd, path, oflag, mode);
    if(ul_unlikely(len > PATH_MAX)) free(path);
    return ret;
  }
  ul_hapi int ulfd_creat(ulfd_t* pfd, const char* path, int mode) {
    int fd = creat(path, ul_static_cast(mode_t, mode));
    if(fd < 0) return errno;
    *pfd = fd; return 0;
  }
  ul_hapi int ulfd_creat_w(ulfd_t* pfd, const wchar_t* wpath, int mode) {
    char path_buf[PATH_MAX];
    char* path;
    size_t len;
    int ret;

    len = ul_os_wstr_to_str_len(wpath);
    if(ul_unlikely(len == 0)) return ENOENT;
    if(ul_likely(len <= PATH_MAX)) path = path_buf;
    else path = ul_reinterpret_cast(char*, malloc(len));

    ul_os_wstr_to_str(path, wpath);
    ret = ulfd_creat(pfd, path, mode);
    if(ul_unlikely(len > PATH_MAX)) free(path);
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
    ssize_t ret;
    ret = pread64(fd, buf, count, off);
    if(ret < 0) return errno;
    *pread_bytes = ul_static_cast(size_t, ret);
    return 0;
  }
  ul_hapi int ulfd_pwrite(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off, size_t* pwriten_bytes) {
    ssize_t ret;
    ret = pwrite64(fd, buf, count, off);
    if(ret < 0) return errno;
    *pwriten_bytes = ul_static_cast(size_t, ret);
    return 0;
  }

  ul_hapi int ulfd_seek(ulfd_t fd, ulfd_int64_t off, int origin, ulfd_int64_t* poff) {
    loff_t new_off;
    int whence;

    if(origin == ULFD_SEEK_CUR) whence = SEEK_CUR;
    else if(origin == ULFD_SEEK_CUR) whence = SEEK_CUR;
    else if(origin == ULFD_SEEK_END) whence = SEEK_END;
    else return EINVAL;

    new_off = lseek64(fd, off, whence);
    if(new_off < 0) return errno;
    *poff = new_off;
    return 0;
  }
  ul_hapi int ulfd_tell(ulfd_t fd, ulfd_int64_t* poff) {
    return ulfd_seek(fd, 0, ULFD_SEEK_END, poff);
  }

  ul_hapi int ulfd_lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode) {
    struct flock64 lock;
    lock.l_len = len;
    lock.l_start = off;
    lock.l_whence = SEEK_SET;
    if(mode == ULFD_F_RDLCK) lock.l_type = F_RDLCK;
    else if(mode == ULFD_F_WRLCK) lock.l_type = F_WRLCK;
    else if(mode == ULFD_F_UNLCK) lock.l_type = F_UNLCK;
    else return EINVAL;

    return fcntl64(fd, F_SETLK64, &lock) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_lockw(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode) {
    struct flock64 lock;
    lock.l_len = len;
    lock.l_start = off;
    lock.l_whence = SEEK_SET;
    if(mode == ULFD_F_RDLCK) lock.l_type = F_RDLCK;
    else if(mode == ULFD_F_WRLCK) lock.l_type = F_WRLCK;
    else if(mode == ULFD_F_UNLCK) lock.l_type = F_UNLCK;
    else return EINVAL;

    return fcntl64(fd, F_SETLKW64, &lock) < 0 ? errno : 0;
  }

  ul_hapi int ulfd_sync(ulfd_t fd) {
    return fsync(fd) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_truncate(ulfd_t fd, ulfd_int64_t length) {
    return ftruncate64(fd, length) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_filelength(ulfd_t fd, ulfd_int64_t* plength) {
    struct stat64 stat;
    if(fstat64(fd, &stat) < 0) return errno;
    *plength = stat.st_size;
    return 0;
  }

  #include <sys/mman.h>
  ul_hapi int ulfd_mmap(void** pmap, ulfd_t fd, void* addr, size_t len, ulfd_int64_t off, int flags) {
    int prot, flag = 0;
    void* map;

    if((flags & (ULFD_MAP_PROT_READ | ULFD_MAP_PROT_WRITE | ULFD_MAP_PROT_EXEC)) == 0) {
      prot = PROT_NONE;
    } else {
      prot = 0;
      if(flags & ULFD_MAP_PROT_READ) prot |= PROT_READ;
      if(flags & ULFD_MAP_PROT_WRITE) prot |= PROT_WRITE;
      if(flags & ULFD_MAP_PROT_EXEC) prot |= PROT_EXEC;
    }

    if(flags & ULFD_MAP_ANONYMOUS) flag |= MAP_ANONYMOUS;
    if(flags & ULFD_MAP_FIXED) flag |= MAP_FIXED;
    if(flags & ULFD_MAP_SHARED) flag |= MAP_SHARED;
    if(flags & ULFD_MAP_PRIVATE) flag |= MAP_PRIVATE;

    map = mmap64(addr, len, prot, flag, fd, off);
    if(map == MAP_FAILED) return errno;
    *pmap = map; return 0;
  }
  ul_hapi int ulfd_munmap(void* addr, size_t len) {
    return munmap(addr, len) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_mprotect(void* addr, size_t len, int flags) {
    int prot;
    if((flags & (ULFD_MAP_PROT_READ | ULFD_MAP_PROT_WRITE | ULFD_MAP_PROT_EXEC)) == 0) {
      prot = PROT_NONE;
    } else {
      prot = 0;
      if(flags & ULFD_MAP_PROT_READ) prot |= PROT_READ;
      if(flags & ULFD_MAP_PROT_WRITE) prot |= PROT_WRITE;
      if(flags & ULFD_MAP_PROT_EXEC) prot |= PROT_EXEC;
    }
    return mprotect(addr, len, prot) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_msync(void* addr, size_t len, int ms_flags) {
    int flag = 0;
    if(ms_flags & ULFD_MS_ASYNC) flag |= MS_ASYNC;
    if(ms_flags & ULFD_MS_SYNC) flag |= MS_SYNC;
    if(ms_flags & ULFD_MS_INVALIDATE) flag |= MS_INVALIDATE;
    return msync(addr, len, flag) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_mlock(const void* addr, size_t len) {
    return mlock(addr, len) < 0 ? errno : 0;
  }
  ul_hapi int ulfd_munlock(const void* addr, size_t len) {
    return munlock(addr, len) < 0 ? errno : 0;
  }
#endif

#endif /* ULFD_H */
