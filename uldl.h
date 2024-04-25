/*
Dynamic library


# Dependences:
  - Windows => RTLD_* is ignored
  - <dlfcn.h> => full support


# APIs
  APIs are kept consistent with <dlfcn.h>.

  - ul_import
    Indicates that the function should be imported from the dynamic shared library.
  - ul_export
    Indicates that the function should be exported from the dynamic shared libary.
    For example:
    ```c
    ul_export void func(void);
    ```


  - void* dlopen(const char* path, int mode);
    Loads the specified dynamic shared library into the calling program, and returns it's handle.
    `mode` can be following values or their combinations:
    - RTLD_LAZY
      Relocations shall be performed when the first reference to a given symbol occurs.
    - RTLD_NOW
      All necessary relocations shall be performed when the object is first loaded.
    - RTLD_LOCAL
      Symbols shall not be made available for any other object.
    - RTLD_GLOBAL
      Symbols shall be made available for any other object.
    In POSIX, more values are availble.
  - void* dlsym(void* handle, const char* name);
    Gets the handle from dynamic loaded shared library.
  - void dlclose(void* handle);
    Unloads the shared library refered by `handle`.
  - const char* dlerror();
    Gets last error messages.
    The operation is thread-safe.

  - void* uldlopen(const char* path);
    Equivalent to `dlopen(path, RTLD_NOW | RTLD_LOCAL)`.
  - void* uldlsym(void* handle, const char* name);
    Equivalent to `dlsym`.
  - void uldlclose(void* handle);
    Equivalent to `dlclose`.
  - const char* uldlerror();
    Equivalent to `dlerror`.


# Example
  a.c:
  ```c
  #include <uldl.h>
  ul_export int sum(int a, int b) { return a + b; }
  ```
  then compile it to "a.dll"(Windows) or "a.so".

  b.c:
  ```c
  #include <uldl.h>
  #include <stdio.h>
  int main() {
  #ifdef _WIN32
    static const name[] = "a.dll";
  #else
    static const name[] = "a.so";
  #endif
    void* h = (void*)dlopen(name, RTLD_NOW | RTLD_LOCAL);
    if(!h) {
      fprintf(stderr, "%s\n", dlerror());
      return 1;
    } else {
      typedef int (*func_t)(int, int);
      void* func = (func_t)dlsym(h, "sum");
      printf("%d", func(1, 2));
      dlclose(func);
      return 0;
    }
  }
  ```


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

#ifndef ULDL_H
#define ULDL_H

#if defined(_WIN32)
  #include <Windows.h>
  #include <stdlib.h>
  #include <stddef.h>
  #include <string.h>

  #ifndef ul_import
    #define ul_import __declspec(dllimport)
  #endif
  #ifndef ul_export
    #define ul_export __declspec(dllexport)
  #endif

  #define RTLD_LAZY   1 /* not available */
  #define RTLD_NOW    2
  #define RTLD_LOCAL  4 /* not available */
  #define RTLD_GLOBAL 8

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

  #ifndef ULOS_STR_TO_WSTR_DEFINED
    #include <stddef.h>
    #include <limits.h>
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
    #include <stddef.h>
    #include <limits.h>
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

  #ifndef ul_reinterpret_cast
    #ifdef __cplusplus
      #define ul_reinterpret_cast(T, val) reinterpret_cast<T>(val)
    #else
      #define ul_reinterpret_cast(T, val) ((T)(val))
    #endif
  #endif /* ul_reinterpret_cast */

  ul_hapi void* dlopen(const char* path, int mode) {
    size_t alloced = 0;
    wchar_t *wpath;
    HMODULE module = NULL;

    (void)mode;
    if(!path) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    alloced = ul_os_str_to_wstr_len(path);
    if(!alloced) { SetLastError(ERROR_MOD_NOT_FOUND); return NULL; }
    wpath = ul_reinterpret_cast(wchar_t*, malloc(alloced * sizeof(wchar_t)));
    if(!wpath) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    ul_os_str_to_wstr(wpath, path);

    module = LoadLibraryW(ul_reinterpret_cast(WCHAR*, wpath));
    free(wpath);
    return module;
  }

  #define dlsym(handle, name) GetProcAddress(handle, name)
  #define dlclose(handle)     FreeLibrary(handle)
  #define dlerror()           strerror(_ul_win32_toerrno(GetLastError()))
#else
  #include <dlfcn.h>

  #ifndef ul_import
    #define ul_import
  #endif
  #ifndef ul_export
    #define ul_export
  #endif
#endif

#define ULDL_LAZY   RTLD_LAZY
#define ULDL_NOW    RTLD_NOW
#define ULDL_LOCAL  RTLD_LOCAL
#define ULDL_GLOBAL RTLD_GLOBAL
#define uldlopen(path)        dlopen((path), RTLD_NOW | RTLD_LOCAL)
#define uldlsym(handle, name) dlsym(handle, name)
#define uldlclose(handle)     dlclose(handle)
#define uldlerror()           dlerror()

#endif /* ULDL_H */
