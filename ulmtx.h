/*
Mutex


# Introduce
  Implements following mutexs:
  - `ulmtx_t` (normal lock)
  - `ulrmtx_t` (recursive lock)
  - `ultmtx_t` (timed lock)
  - `ulrtmtx_t` (recursive and timed lock)
  If you need spin lock, see "ulatomic.h".


# Dependences
  Following one:
  - C++11/C11(with threads support)
  - Windows
  - pthread APIs


# Config macro
  - ULMTX_NEED_MACRO_INIT:
    Disable APIs which don't support init dirrectly.
    When you're creating a mutex in global or static block, it's very useful.
  - ULMTX_NEEDED
    Define funtions even if no APIs about mutex are invalid, to ensure compatibility with mutex.
  - ULMTX_SINGLE_THREAD
    Disbale multi-thread support. It's often used with `ULMTX_NEEDED`.


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
#ifndef ULMTX_H
#define ULMTX_H

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

#ifndef ul_restrict
  #if defined(_MSC_VER)
    #define ul_restrict __restrict
  #elif defined(__GNUC__) && __GNUC__ > 3
    #define ul_restrict __restrict__
  #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #define ul_restrict restrict
  #else
    #define ul_restrict
  #endif
#endif /* ul_restrict */

#ifndef ul_reinterpret_cast
  #ifdef __cplusplus
    #define ul_reinterpret_cast(T, val) reinterpret_cast<T>(val)
  #else
    #define ul_reinterpret_cast(T, val) ((T)(val))
  #endif
#endif /* ul_reinterpret_cast */

#if defined(__cplusplus) && __cplusplus >= 201103L && __STDCPP_THREADS__ /* C++11 */
  #define ULMTX_API_CXX11
#endif
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L \
    && (defined(__STDC_NO_THREADS__) && !__STDC_NO_THREADS__) && !defined(__MINGW32__) /* C11 */
  #define ULMTX_API_C11
#endif
#if defined(_WIN32) /* Win32 API */
  #define ULMTX_API_WIN32
#endif
#if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE) /* pthread API */
  #include <unistd.h>
  #if defined(_POSIX_THREADS) && (_POSIX_THREADS+0) >= 0
    #define ULMTX_API_PTHREADS
  #endif
#endif

#ifdef ULMTX_NEED_MACRO_INIT
  #undef ULMTX_API_C11
#endif
#ifdef ULMTX_SINGLE_THREAD
  #undef ULMTX_API_CXX11
  #undef ULMTX_API_C11
  #undef ULMTX_API_WIN32
  #undef ULMTX_API_PTHREADS
#endif
#if !defined(ULMTX_API_CXX11) && !defined(ULMTX_API_C11) \
    && !defined(ULMTX_API_WIN32) && !defined(ULMTX_API_PTHREADS)
  #ifndef ULMTX_NEEDED
    #error "ulmtx.h: unsupported platform"
  #endif
#endif

#include <errno.h>

#ifndef EBUSY
  #define EBUSY 16
#endif
#ifndef EINVAL
  #define EINVAL 22
#endif
#ifdef ETIMEDOUT
  #define ULMTX_TIMEDOUT ETIMEDOUT
#else
  #define ULMTX_TIMEDOUT EBUSY
#endif

#if defined(ULMTX_API_CXX11)
  #include <chrono>
  #include <mutex>
  #include <system_error>
  #include <utility>
  #include <functional>
  #include <cstring>

  template<class T>
  struct _ULMTX_WRAPPER {
    union {
      char dummy;
      T val;
    };

    _ULMTX_WRAPPER() { }
    _ULMTX_WRAPPER(const _ULMTX_WRAPPER& o) { memcpy(reinterpret_cast<void*>(this), &o, sizeof(o)); }
    ~_ULMTX_WRAPPER() { }
    _ULMTX_WRAPPER& operator=(const _ULMTX_WRAPPER& o) {
      memcpy(reinterpret_cast<void*>(this), &o, sizeof(o)); return *this;
    }

    template<class... Args>
    static _ULMTX_WRAPPER create(Args&& ...args) {
      _ULMTX_WRAPPER ret; new(&ret.val) T(std::forward<Args>(args)...); return ret;
    }

    template<class... Args>
    void construct(Args&& ...args) { new(&val) T(std::forward<Args>(args)...); }
    void destroy() { val.~T(); }
  };

  typedef _ULMTX_WRAPPER<std::mutex> ulmtx_t;
  #define ULMTX_INIT _ULMTX_WRAPPER<std::mutex>::create()
  ul_hapi int ulmtx_init(ulmtx_t* mtx) noexcept {
    try {
      mtx->construct(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulmtx_destroy(ulmtx_t* mtx) noexcept {
    try {
      mtx->destroy(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulmtx_trylock(ulmtx_t* mtx) noexcept {
    try {
      return mtx->val.try_lock() ? 0 : EBUSY;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulmtx_lock(ulmtx_t* mtx) noexcept {
    try {
      mtx->val.lock(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulmtx_unlock(ulmtx_t* mtx) noexcept {
    try {
      mtx->val.unlock(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }


  typedef _ULMTX_WRAPPER<std::recursive_mutex> ulrmtx_t;
  #define ULRMTX_INIT _ULMTX_WRAPPER<std::recursive_mutex>::create()
  ul_hapi int ulrmtx_init(ulrmtx_t* mtx) noexcept {
    try {
      mtx->construct(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrmtx_destroy(ulrmtx_t* mtx) noexcept {
    try {
      mtx->destroy(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) noexcept {
    try {
      return mtx->val.try_lock() ? 0 : EBUSY;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) noexcept {
    try {
      mtx->val.lock(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) noexcept {
    try {
      mtx->val.unlock(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }


  typedef _ULMTX_WRAPPER<std::timed_mutex> ultmtx_t;
  #define ULTMTX_INIT _ULMTX_WRAPPER<std::timed_mutex>::create()
  ul_hapi int ultmtx_init(ultmtx_t* mtx) noexcept {
    try {
      mtx->construct(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ultmtx_destroy(ultmtx_t* mtx) noexcept {
    try {
      mtx->destroy(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ultmtx_trylock(ultmtx_t* mtx) noexcept {
    try {
      return mtx->val.try_lock() ? 0 : EBUSY;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) noexcept {
    try {
      return mtx->val.try_lock_for(std::chrono::milliseconds(ms)) ? 0 : EBUSY;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ultmtx_lock(ultmtx_t* mtx) noexcept {
    try {
      mtx->val.lock(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ultmtx_unlock(ultmtx_t* mtx) noexcept {
    try {
      mtx->val.unlock(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }


  typedef _ULMTX_WRAPPER<std::recursive_timed_mutex> ulrtmtx_t;
  #define ULRTMTX_INIT _ULMTX_WRAPPER<std::recursive_timed_mutex>::create()
  ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) noexcept {
    try {
      mtx->construct(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrtmtx_destroy(ulrtmtx_t* mtx) noexcept {
    try {
      mtx->destroy(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) noexcept {
    try {
      return mtx->val.try_lock() ? 0 : EBUSY;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) noexcept {
    try {
      return mtx->val.try_lock_for(std::chrono::milliseconds(ms)) ? 0 : EBUSY;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) noexcept {
    try {
      mtx->val.lock(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
  ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) noexcept {
    try {
      mtx->val.unlock(); return 0;
    } catch(const std::system_error& e) {
      return e.code().value();
    }
  }
#elif defined(ULMTX_API_C11)
  #include <threads.h>
  #include <time.h>
  #include <limits.h>
  #include <errno.h>

  ul_hapi int _ulmtx_get_timepoint(struct timespec* tp, unsigned long ms) {
    if(timespec_get(tp, TIME_UTC) != TIME_UTC) return -1;
    tp->tv_nsec += (ms % 1000 * 1000000);
    if(tp->tv_nsec >= 1000000000) { tp->tv_nsec -= 1000000000; ++tp->tv_sec; }
    tp->tv_sec += ms;
    if(tp->tv_sec < 0) return -1;
    return 0;
  }
  ul_hapi int _ulmtx_toerrno(int e) {
    switch(e) {
    case thrd_success: return 0;
    case thrd_timedout: return ETIMEDOUT;
    case thrd_busy: return EBUSY;
    case thrd_nomem: return ENOMEM;
    default: return EINVAL;
    }
  }


  typedef struct ulmtx_t { mtx_t m; } ulmtx_t;
  ul_hapi int ulmtx_init(ulmtx_t* mtx) { return _ulmtx_toerrno(mtx_init(&mtx->m, mtx_plain)); }
  ul_hapi int ulmtx_destroy(ulmtx_t* mtx) { mtx_destroy(&mtx->m); return 0; }
  ul_hapi int ulmtx_trylock(ulmtx_t* mtx) { return _ulmtx_toerrno(mtx_trylock(&mtx->m)); }
  ul_hapi int ulmtx_lock(ulmtx_t* mtx) { return _ulmtx_toerrno(mtx_lock(&mtx->m)); }
  ul_hapi int ulmtx_unlock(ulmtx_t* mtx) { return _ulmtx_toerrno(mtx_unlock(&mtx->m)); }


  typedef struct ulrmtx_t { mtx_t m; } ulrmtx_t;
  ul_hapi int ulrmtx_init(ulrmtx_t* mtx) { return _ulmtx_toerrno(mtx_init(&mtx->m, mtx_plain)); }
  ul_hapi int ulrmtx_destroy(ulrmtx_t* mtx) { mtx_destroy(&mtx->m); return 0; }
  ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) { return _ulmtx_toerrno(mtx_trylock(&mtx->m)); }
  ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) { return _ulmtx_toerrno(mtx_lock(&mtx->m)); }
  ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) { return _ulmtx_toerrno(mtx_unlock(&mtx->m)); }


  typedef struct ultmtx_t { mtx_t m; } ultmtx_t;
  ul_hapi int ultmtx_init(ultmtx_t* mtx) { return _ulmtx_toerrno(mtx_init(&mtx->m, mtx_plain)); }
  ul_hapi int ultmtx_destroy(ultmtx_t* mtx) { mtx_destroy(&mtx->m); return 0; }
  ul_hapi int ultmtx_trylock(ultmtx_t* mtx) { return _ulmtx_toerrno(mtx_trylock(&mtx->m)); }
  ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) {
    struct timespec tp;
    if(_ulmtx_get_timepoint(&tp, ms)) return ERANGE;
    return _ulmtx_toerrno(mtx_timedlock(&mtx->m, &tp));
  }
  ul_hapi int ultmtx_lock(ultmtx_t* mtx) { return _ulmtx_toerrno(mtx_lock(&mtx->m)); }
  ul_hapi int ultmtx_unlock(ultmtx_t* mtx) { return _ulmtx_toerrno(mtx_unlock(&mtx->m)); }


  typedef struct ulrtmtx_t { mtx_t m; } ulrtmtx_t;
  ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) { return _ulmtx_toerrno(mtx_init(&mtx->m, mtx_plain)); }
  ul_hapi int ulrtmtx_destroy(ulrtmtx_t* mtx) { mtx_destroy(&mtx->m); return 0; }
  ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) { return _ulmtx_toerrno(mtx_trylock(&mtx->m)); }
  ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) {
    struct timespec tp;
    if(_ulmtx_get_timepoint(&tp, ms)) return ERANGE;
    return _ulmtx_toerrno(mtx_timedlock(&mtx->m, &tp));
  }
  ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) { return _ulmtx_toerrno(mtx_lock(&mtx->m)); }
  ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) { return _ulmtx_toerrno(mtx_unlock(&mtx->m)); }


#elif defined(ULMTX_API_WIN32)
  #ifndef _UL_WIN32_TOERRNO_DEFINED
    #include <Windows.h>
    #include <errno.h>

    #ifndef ul_static_cast
      #ifdef __cplusplus
        #define ul_static_cast(T, val) static_cast<T>(val)
      #else
        #define ul_static_cast(T, val) ((T)(val))
      #endif
    #endif

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

  #ifndef ul_static_cast
    #ifdef __cplusplus
      #define ul_static_cast(T, val) static_cast<T>(val)
    #else
      #define ul_static_cast(T, val) ((T)(val))
    #endif
  #endif /* ul_static_cast */

  #ifdef _WIN64
    typedef __int64 _ulmtx_iptr_t;
    #define _ulmtx_InterlockedExchange InterlockedExchange64
    #define _ulmtx_InterlockedCompareExchange InterlockedCompareExchange64
    #define _ulmtx_InterlockedBitTestAndSet InterlockedBitTestAndSet64
    #define _ulmtx_InterlockedExchangeAdd InterlockedExchangeAdd64
  #else
    typedef LONG _ulmtx_iptr_t;
    #define _ulmtx_InterlockedExchange InterlockedExchange
    #if !defined(_MSC_VER) || (_MSC_VER+0) >= 1300
      #define _ulmtx_InterlockedBitTestAndSet InterlockedBitTestAndSet
      #define _ulmtx_InterlockedCompareExchange InterlockedCompareExchange
    #else
      #define _ulmtx_InterlockedCompareExchange(ptr, nv, ov) \
        ul_reinterpret_cast(LONG, InterlockedCompareExchange( \
          ul_reinterpret_cast(void**, ptr), \
          ul_reinterpret_cast(void*, nv), \
          ul_reinterpret_cast(void*, ov) \
        ))
      BOOLEAN _ulmtx_InterlockedBitTestAndSet(LONG volatile* obj, LONG off) {
        LONG ov, nv;
        do {
          ov = (LONG)*obj; nv = (LONG)(ov | (LONG)(1 << off));
        } while(_ulmtx_InterlockedCompareExchange((LONG*)obj, nv, ov) != ov);
        return !!(ov >> off);
      }
    #endif
    #define _ulmtx_InterlockedExchangeAdd InterlockedExchangeAdd
  #endif

  typedef struct ultmtx_t {
    _ulmtx_iptr_t event;
    _ulmtx_iptr_t count;
  } ultmtx_t;

  #define _ULTMTX_LOCK_FLAG_BIT (sizeof(_ulmtx_iptr_t) - 1)
  #define _ULTMTX_EVENT_FLAG_BIT (sizeof(_ulmtx_iptr_t) - 2)
  #define _ULTMTX_LOCK_FLAG ul_static_cast(_ulmtx_iptr_t, ul_static_cast(_ulmtx_iptr_t, 1) << _ULTMTX_LOCK_FLAG_BIT)
  #define _ULTMTX_EVENT_FLAG ul_static_cast(_ulmtx_iptr_t, ul_static_cast(_ulmtx_iptr_t, 1) << _ULTMTX_EVENT_FLAG_BIT)

  #define ULTMTX_INIT { 0, 0 }
  ul_hapi int ultmtx_init(ultmtx_t* mtx) {
    mtx->count = 0; mtx->event = 0; return 0;
  }
  ul_hapi int ultmtx_destroy(ultmtx_t* mtx) {
    const HANDLE old_event = ul_reinterpret_cast(HANDLE, _ulmtx_InterlockedExchange(&mtx->event, 0));
    if(old_event) return _ul_win32_toerrno(CloseHandle(old_event) ? 0 : GetLastError());
    return 0;
  }

  ul_hapi void _ultmtx_mark_waiting_and_try_lock(ultmtx_t* ul_restrict mtx, _ulmtx_iptr_t* ul_restrict old_count) {
    /* Loop until the number of waiter has been incremented or we've taken the lock flag. */
    for(;;) {
      const _ulmtx_iptr_t was_locked = *old_count & _ULTMTX_LOCK_FLAG;
      const _ulmtx_iptr_t new_count = was_locked ? (*old_count + 1) : (*old_count | _ULTMTX_LOCK_FLAG);
      const _ulmtx_iptr_t current = _ulmtx_InterlockedCompareExchange(&mtx->count, new_count, *old_count);
      if(current == *old_count) {
        if(was_locked) *old_count = new_count;
        /* else don't update old_count, outside function will check lock flag and know we've taken the lock flag */
        break;
      }
      *old_count = current;
    }
  }
  ul_hapi void _ultmtx_clear_waiting_and_try_lock(ultmtx_t* ul_restrict mtx, _ulmtx_iptr_t* ul_restrict old_count) {
    *old_count = (*old_count & ~_ULTMTX_LOCK_FLAG) | _ULTMTX_EVENT_FLAG;
    /*
    Loop until someone else has taken the lock flag and cleared the event set flag
    or until we've taken the lock flag and cleared the event set flag and decrease
    the number of waiters.
    */
    for(;;) {
      const _ulmtx_iptr_t new_count =
        ((*old_count & _ULTMTX_LOCK_FLAG) ? *old_count : ((*old_count - 1) | _ULTMTX_LOCK_FLAG)) & ~_ULTMTX_EVENT_FLAG;
      const _ulmtx_iptr_t current = _ulmtx_InterlockedCompareExchange(&mtx->count, new_count, *old_count);
      if(current == *old_count) break;
      *old_count = current;
    }
  }
  ul_hapi HANDLE _ultmtx_get_event(ultmtx_t* mtx) {
    const HANDLE current_event = ul_reinterpret_cast(HANDLE, _ulmtx_InterlockedCompareExchange(&mtx->event, 0, 0));
    if(!current_event) {
      const HANDLE new_event = CreateEvent(0, FALSE, FALSE, NULL);
      const HANDLE old_event =
        ul_reinterpret_cast(HANDLE, _ulmtx_InterlockedCompareExchange(&mtx->event, PtrToUlong(new_event), 0));
      if(!old_event) {
        CloseHandle(new_event);
        return old_event;
      } else return new_event;
    }
    return current_event;
  }

  ul_hapi int ultmtx_trylock(ultmtx_t* mtx) {
    return !_ulmtx_InterlockedBitTestAndSet(&mtx->count, _ULTMTX_LOCK_FLAG_BIT) ? 0 : EBUSY;
  }
  ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) {
    LARGE_INTEGER freq, target, now;
    _ulmtx_iptr_t old_count;

    if(ultmtx_trylock(mtx)) return 0;
    if(!QueryPerformanceFrequency(&freq)) return _ul_win32_toerrno(GetLastError());
    if(!QueryPerformanceCounter(&target)) return _ul_win32_toerrno(GetLastError());
    target.QuadPart += ms * freq.QuadPart / 1000; /* `ms` is 32-bit, so mostly it won't overflow */
    if(target.QuadPart < 0) return ERANGE;

    old_count = mtx->count;
    _ultmtx_mark_waiting_and_try_lock(mtx, &old_count);
    if(old_count & _ULTMTX_LOCK_FLAG) {
      const HANDLE event = _ultmtx_get_event(mtx);
      if(ul_unlikely(event == NULL)) return _ul_win32_toerrno(GetLastError());
      do {
        if(!QueryPerformanceCounter(&now)) {
          _ulmtx_InterlockedExchangeAdd(&mtx->count, -1);
          return _ul_win32_toerrno(GetLastError());
        }
        if(now.QuadPart >= target.QuadPart) {
          _ulmtx_InterlockedExchangeAdd(&mtx->count, -1);
          return ULMTX_TIMEDOUT;
        }
        ms -= ul_static_cast(unsigned long, (target.QuadPart - now.QuadPart) * 1000 / freq.QuadPart);
        switch(WaitForSingleObjectEx(event, ms, FALSE)) {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
          _ultmtx_clear_waiting_and_try_lock(mtx, &old_count);
          break;
        case WAIT_IO_COMPLETION:
        case WAIT_TIMEOUT:
          break;
        case WAIT_FAILED:
        default:
          _ulmtx_InterlockedExchangeAdd(&mtx->count, -1);
          return _ul_win32_toerrno(GetLastError());
        }
      } while(old_count & _ULTMTX_LOCK_FLAG);
    }
    return 0;
  }
  ul_hapi int ultmtx_lock(ultmtx_t* mtx) {
    _ulmtx_iptr_t old_count;
    if(ultmtx_trylock(mtx) == 0) return 0;
    old_count = mtx->count;
    _ultmtx_mark_waiting_and_try_lock(mtx, &old_count);
    if(old_count & _ULTMTX_LOCK_FLAG) {
      const HANDLE event = _ultmtx_get_event(mtx);
      if(ul_unlikely(event == NULL)) return _ul_win32_toerrno(GetLastError());
      do {
        switch(WaitForSingleObjectEx(event, INFINITE, FALSE)) {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
          _ultmtx_clear_waiting_and_try_lock(mtx, &old_count);
          break;
        case WAIT_IO_COMPLETION:
        case WAIT_TIMEOUT:
          break;
        case WAIT_FAILED:
        default:
          _ulmtx_InterlockedExchangeAdd(&mtx->count, -1);
          return _ul_win32_toerrno(GetLastError());
        }
      } while(old_count & _ULTMTX_LOCK_FLAG);
    }
    return 0;
  }
  ul_hapi int ultmtx_unlock(ultmtx_t* mtx) {
    const _ulmtx_iptr_t old_count = _ulmtx_InterlockedExchangeAdd(&mtx->count, _ULTMTX_LOCK_FLAG);
    if(!(old_count & _ULTMTX_EVENT_FLAG) && (old_count > _ULTMTX_LOCK_FLAG)) {
      if(!_ulmtx_InterlockedBitTestAndSet(&mtx->count, _ULTMTX_EVENT_FLAG_BIT)) {
        return SetEvent(_ultmtx_get_event(mtx)) ? 0 : _ul_win32_toerrno(GetLastError());
      }
    }
    return 0;
  }


  typedef struct ulmtx_t { ultmtx_t mtx; } ulmtx_t;
  #define ULMTX_INIT { ULTMTX_INIT }
  ul_hapi int ulmtx_init(ulmtx_t* mtx) {
    mtx->mtx.count = 0; mtx->mtx.event = 0; return 0;
  }
  ul_hapi int ulmtx_destroy(ulmtx_t* mtx) { return ultmtx_destroy(&mtx->mtx); }
  ul_hapi int ulmtx_trylock(ulmtx_t* mtx) { return ultmtx_trylock(&mtx->mtx); }
  ul_hapi int ulmtx_lock(ulmtx_t* mtx) { return ultmtx_lock(&mtx->mtx); }
  ul_hapi int ulmtx_unlock(ulmtx_t* mtx) { return ultmtx_unlock(&mtx->mtx); }


  typedef struct ulrtmtx_t {
    ultmtx_t mtx;
    LONG count;
    LONG thread_id;
  } ulrtmtx_t;

  #define ULRTMTX_INIT { ULTMTX_INIT, 0, 0 }
  ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) {
    ultmtx_init(&mtx->mtx);
    mtx->count = 0; mtx->thread_id = 0;
    return 0;
  }
  ul_hapi int ulrtmtx_destroy(ulrtmtx_t* mtx) { return ultmtx_destroy(&mtx->mtx); }
  ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) {
    int ret;
    const LONG thread_id = ul_static_cast(LONG, GetCurrentThreadId());
  #if !defined(_MSC_VER) || (_MSC_VER+0) >= 1300L
    if(InterlockedCompareExchange(&mtx->thread_id, 0, 0) == thread_id) { ++mtx->count; return 0; }
  #else
    if(ul_reinterpret_cast(LONG,
        InterlockedCompareExchange(ul_reinterpret_cast(void**, &mtx->thread_id), 0, 0)) == thread_id
    ) {
      ++mtx->count; return 0;
    }
  #endif
    ret = ultmtx_trylock(&mtx->mtx);
    if(ret == 0) {
      InterlockedExchange(&mtx->thread_id, thread_id);
      mtx->count = 1;
    }
    return ret;
  }
  ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) {
    int ret;
    const LONG thread_id = ul_static_cast(LONG, GetCurrentThreadId());
  #if !defined(_MSC_VER) || (_MSC_VER+0) >= 1300L
    if(InterlockedCompareExchange(&mtx->thread_id, 0, 0) == thread_id) { ++mtx->count; return 0; }
  #else
    if(ul_reinterpret_cast(LONG,
        InterlockedCompareExchange(ul_reinterpret_cast(void**, &mtx->thread_id), 0, 0)) == thread_id
    ) {
      ++mtx->count; return 0;
    }
  #endif
    ret = ultmtx_lock(&mtx->mtx);
    if(ret != 0) return ret;
    InterlockedExchange(&mtx->thread_id, thread_id);
    mtx->count = 1;
    return ret;
  }
  ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) {
    int ret;
    const LONG thread_id = ul_static_cast(LONG, GetCurrentThreadId());
  #if !defined(_MSC_VER) || (_MSC_VER+0) >= 1300L
    if(InterlockedCompareExchange(&mtx->thread_id, 0, 0) == thread_id) { ++mtx->count; return 0; }
  #else
    if(ul_reinterpret_cast(LONG,
        InterlockedCompareExchange(ul_reinterpret_cast(void**, &mtx->thread_id), 0, 0)) == thread_id
    ) {
      ++mtx->count; return 0;
    }
  #endif
    ret = ultmtx_timedlock(&mtx->mtx, ms);
    if(ret != 0) return ret;
    InterlockedExchange(&mtx->thread_id, thread_id);
    mtx->count = 1;
    return ret;
  }
  ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) {
    if(!--mtx->count) {
      InterlockedExchange(&mtx->count, 0);
      return ultmtx_unlock(&mtx->mtx);
    }
    return 0;
  }


  typedef struct ulrmtx_t { ulrtmtx_t mtx; } ulrmtx_t;
  #define ULRMTX_INIT { ULRTMTX_INIT }
  ul_hapi int ulrmtx_init(ulrmtx_t* mtx) { return ulrtmtx_init(&mtx->mtx); }
  ul_hapi int ulrmtx_destroy(ulrmtx_t* mtx) { return ulrtmtx_destroy(&mtx->mtx); }
  ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) { return ulrtmtx_trylock(&mtx->mtx); }
  ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) { return ulrtmtx_lock(&mtx->mtx); }
  ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) { return ulrtmtx_unlock(&mtx->mtx); }
#elif defined(ULMTX_API_PTHREADS)
  #include <pthread.h>
  #include <errno.h>
  #include <limits.h>
  #include <time.h>

  #if (defined(_POSIX_TIMEOUTS) && _POSIX_TIMEOUTS >= 200112L) || \
      defined(__ANDROID__) && defined(__ANDROID_API__) && __ANDROID_API__ >= 21
    #define ULMTX_API_PTHREAD_USE_TIMEDLOCK
  #endif
  #ifndef ULMTX_NEED_MACRO_INIT
    #if (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) || \
        defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || \
        ((defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)) && __GNUC__ >= 4)
      #define ULMTX_API_PTHREAD_USE_MUTEXATTR_SETTYPE
    #endif
  #endif
  #if defined(ULMTX_API_PTHREAD_USE_TIMEDLOCK) && defined(ULMTX_API_PTHREAD_USE_MUTEXATTR_SETTYPE)
    #define ULMTX_API_PTHREAD_USE_RECURSIVE_TIMEDLOCK
  #endif

  ul_hapi int _ulmtx_get_timepoint(struct timespec* tp, unsigned long ms) {
    if(timespec_get(tp, TIME_UTC) != TIME_UTC) return -1;
    tp->tv_nsec += (ms % 1000 * 1000000);
    if(tp->tv_nsec >= 1000000000) { tp->tv_nsec -= 1000000000; ++tp->tv_sec; }
    tp->tv_sec += ms;
    if(tp->tv_sec < 0) return -1;
    return 0;
  }

  typedef struct ulmtx_t { pthread_mutex_t m; } ulmtx_t;
  #define ULMTX_INIT { PTHREAD_MUTEX_INITIALIZER }
  ul_hapi int ulmtx_init(ulmtx_t* mtx) { return pthread_mutex_init(&mtx->m, NULL); }
  ul_hapi int ulmtx_destroy(ulmtx_t* mtx) { return pthread_mutex_destroy(&mtx->m); }
  ul_hapi int ulmtx_trylock(ulmtx_t* mtx) { return pthread_mutex_trylock(&mtx->m); }
  ul_hapi int ulmtx_lock(ulmtx_t* mtx) { return pthread_mutex_lock(&mtx->m); }
  ul_hapi int ulmtx_unlock(ulmtx_t* mtx) { return pthread_mutex_unlock(&mtx->m); }

  #ifdef ULMTX_API_PTHREAD_USE_TIMEDLOCK
    typedef struct ultmtx_t { pthread_mutex_t m; } ultmtx_t;
    #define ULTMTX_INIT { PTHREAD_MUTEX_INITIALIZER }
    ul_hapi int ultmtx_init(ultmtx_t* mtx) { return pthread_mutex_init(&mtx->m, NULL); }
    ul_hapi int ultmtx_destroy(ultmtx_t* mtx) { return pthread_mutex_destroy(&mtx->m); }
    ul_hapi int ultmtx_trylock(ultmtx_t* mtx) { return pthread_mutex_trylock(&mtx->m); }
    ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) {
      struct timespec tp;
      if(_ulmtx_get_timepoint(&tp, ms)) return ERANGE;
      return pthread_mutex_timedlock(&mtx->m, &tp);
    }
    ul_hapi int ultmtx_lock(ultmtx_t* mtx) { return pthread_mutex_lock(&mtx->m); }
    ul_hapi int ultmtx_unlock(ultmtx_t* mtx) { return pthread_mutex_unlock(&mtx->m); }
  #else
    typedef struct ultmtx_t {
      pthread_mutex_t m;
      pthread_cond_t cond;
      int locked;
    } ultmtx_t;
    #define ULTMTX_INIT { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0 }
    ul_hapi int ultmtx_init(ultmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_init(&mtx->m, NULL))) return ec;
      if((ec = pthread_cond_init(&mtx->cond, NULL))) { pthread_mutex_destroy(&mtx->m); return ec; }
      mtx->locked = 0;
      return 0;
    }
    ul_hapi int ultmtx_destroy(ultmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_destroy(&mtx->m))) pthread_cond_destroy(&mtx->cond);
      else ec = pthread_cond_destroy(&mtx->cond);
      return ec;
    }
    ul_hapi int ultmtx_trylock(ultmtx_t* mtx) {
      int ret;
      if((ret = pthread_mutex_lock(&mtx->m))) return ret;
      ret = mtx->locked;
      mtx->locked = 1;
      pthread_mutex_unlock(&mtx->m);
      return ret ? EBUSY : 0;
    }
    ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) {
      struct timespec tp;
      int ret;

      if(_ulmtx_get_timepoint(&tp, ms)) return ERANGE;
      if((ret = pthread_mutex_lock(&mtx->m))) return ret;
      while(mtx->locked) {
        ret = pthread_cond_timedwait(&mtx->cond, &mtx->m, &tp);
        if(ret == ULMTX_TIMEDOUT) break;
        if(ret != 0) { pthread_mutex_unlock(&mtx->m); return ret; }
      }
      ret = mtx->locked;
      mtx->locked = 1;
      pthread_mutex_unlock(&mtx->m);
      return ret ? ULMTX_TIMEDOUT : 0;
    }
    ul_hapi int ultmtx_lock(ultmtx_t* mtx) {
      int ret;
      if((ret = pthread_mutex_lock(&mtx->m))) return ret;
      while(mtx->locked)
        if((ret = pthread_cond_wait(&mtx->cond, &mtx->m))) { pthread_mutex_unlock(&mtx->m); return ret; }
      mtx->locked = 1;
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ultmtx_unlock(ultmtx_t* mtx) {
      int ret;
      if((ret = pthread_mutex_lock(&mtx->m))) return ret;
      mtx->locked = 0;
      ret = pthread_cond_signal(&mtx->cond);
      pthread_mutex_unlock(&mtx->m);
      return ret;
    }
  #endif

  #ifdef ULMTX_API_PTHREAD_USE_MUTEXATTR_SETTYPE
    typedef struct ulrmtx_t { pthread_mutex_t m; } ulrmtx_t;

    ul_hapi int ulrmtx_init(ulrmtx_t* mtx) {
      int ec;
      pthread_mutexattr_t attr;
      if((ec = pthread_mutexattr_init(&attr))) return ec;
      if((ec = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE))) {
        pthread_mutexattr_destroy(&attr); return ec;
      }
      if((ec = pthread_mutex_init(&mtx->m, &attr))) { pthread_mutexattr_destroy(&attr); return ec; }
      return pthread_mutexattr_destroy(&attr);
    }
    ul_hapi int ulrmtx_destroy(ulrmtx_t* mtx) { return pthread_mutex_destroy(&mtx->m); }
    ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) { return pthread_mutex_trylock(&mtx->m); }
    ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) { return pthread_mutex_lock(&mtx->m); }
    ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) { return pthread_mutex_unlock(&mtx->m); }
  #else
    typedef struct ulrmtx_t {
      pthread_mutex_t m;
      pthread_cond_t cond;
      pthread_t owner;
      unsigned long count;
      int locked;
    } ulrmtx_t;

    #define ULRMTX_INIT { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, 0 }
    ul_hapi int ulrmtx_init(ulrmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_init(&mtx->m, NULL))) return ec;
      if((ec = pthread_cond_init(&mtx->cond, NULL))) { pthread_mutex_destroy(&mtx->m); return ec; }
      mtx->locked = 0;
      mtx->count = 0;
      return 0;
    }
    ul_hapi int ulrmtx_destroy(ulrmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_destroy(&mtx->m))) pthread_cond_destroy(&mtx->cond);
      else ec = pthread_cond_destroy(&mtx->cond);
      return ec;
    }
    ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_lock(&mtx->m))) return ec;
      if(mtx->locked && !pthread_equal(mtx->owner, pthread_self())) {
        pthread_mutex_unlock(&mtx->m); return EBUSY;
      }
      mtx->locked = 1;
      ++mtx->count;
      mtx->owner = pthread_self();
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_lock(&mtx->m))) return ec;
      if(mtx->locked && pthread_equal(mtx->owner, pthread_self())) {
        ++mtx->count; pthread_mutex_unlock(&mtx->m); return 0;
      }
      while(mtx->locked)
        if((ec = pthread_cond_wait(&mtx->cond, &mtx->m))) { pthread_mutex_unlock(&mtx->m); return ec; }
      mtx->locked = 1;
      ++mtx->count;
      mtx->owner = pthread_self();
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_lock(&mtx->m))) return ec;
      if(!--mtx->count) mtx->locked = 0;
      ec = pthread_cond_signal(&mtx->cond);
      pthread_mutex_unlock(&mtx->m);
      return ec;
    }
  #endif

  #ifdef ULMTX_API_PTHREAD_USE_RECURSIVE_TIMEDLOCK
    typedef struct ulrtmtx_t { pthread_mutex_t m; } ulrtmtx_t;

    ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) {
      int ec;
      pthread_mutexattr_t attr;
      if((ec = pthread_mutexattr_init(&attr))) return ec;
      if((ec = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE))) {
        pthread_mutexattr_destroy(&attr); return ec;
      }
      if((ec = pthread_mutex_init(&mtx->m, &attr))) { pthread_mutexattr_destroy(&attr); return ec; }
      return pthread_mutexattr_destroy(&attr);
    }
    ul_hapi int ulrtmtx_destroy(ulrtmtx_t* mtx) { return pthread_mutex_destroy(&mtx->m); }
    ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) { return pthread_mutex_trylock(&mtx->m); }
    ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) {
      struct timespec tp;
      if(_ulmtx_get_timepoint(&tp, ms)) return ERANGE;
      return pthread_mutex_timedlock(&mtx, &tp);
    }
    ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) { return pthread_mutex_lock(&mtx->m); }
    ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) { return pthread_mutex_unlock(&mtx->m); }
  #else
    typedef struct ulrtmtx_t {
      pthread_mutex_t m;
      pthread_cond_t cond;
      pthread_t owner;
      unsigned long count;
      int locked;
    } ulrtmtx_t;

    #define ULRTMTX_INIT { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, 0 }
    ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_init(&mtx->m, NULL))) return ec;
      if((ec = pthread_cond_init(&mtx->cond, NULL))) { pthread_mutex_destroy(&mtx->m); return ec; }
      mtx->locked = 0;
      mtx->count = 0;
      return 0;
    }
    ul_hapi int ulrtmtx_destroy(ulrtmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_destroy(&mtx->m))) pthread_cond_destroy(&mtx->cond);
      else ec = pthread_cond_destroy(&mtx->cond);
      return ec;
    }
    ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_lock(&mtx->m))) return ec;
      if(mtx->locked && !pthread_equal(mtx->owner, pthread_self())) {
        pthread_mutex_unlock(&mtx->m); return EBUSY;
      }
      mtx->locked = 1;
      ++mtx->count;
      mtx->owner = pthread_self();
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) {
      struct timespec tp;
      int ret;

      if((ret = _ulmtx_get_timepoint(&tp, ms))) return ret;
      if((ret = pthread_mutex_lock(&mtx->m))) return ret;
      if(mtx->locked && pthread_equal(mtx->owner, pthread_self())) {
        ++mtx->count; pthread_mutex_unlock(&mtx->m); return 0;
      }
      while(mtx->locked) {
        ret = pthread_cond_timedwait(&mtx->cond, &mtx->m, &tp);
        if(ret == ULMTX_TIMEDOUT) break;
        if(ret != 0) { pthread_mutex_unlock(&mtx->m); return ret; }
      }
      if(mtx->locked) ret = ULMTX_TIMEDOUT;
      else {
        mtx->locked = 1;
        ++mtx->count;
        mtx->owner = pthread_self();
        ret = 0;
      }
      pthread_mutex_unlock(&mtx->m);
      return ret;
    }
    ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_lock(&mtx->m))) return ec;
      if(mtx->locked && pthread_equal(mtx->owner, pthread_self())) {
        ++mtx->count; pthread_mutex_unlock(&mtx->m); return 0;
      }
      while(mtx->locked)
        if((ec = pthread_cond_wait(&mtx->cond, &mtx->m))) { pthread_mutex_unlock(&mtx->m); return ec; }
      mtx->locked = 1;
      ++mtx->count;
      mtx->owner = pthread_self();
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) {
      int ec;
      if((ec = pthread_mutex_lock(&mtx->m))) return ec;
      if(!--mtx->count) mtx->locked = 0;
      ec = pthread_cond_signal(&mtx->cond);
      pthread_mutex_unlock(&mtx->m);
      return ec;
    }
  #endif


#elif defined(ULMTX_NEEDED)
  typedef struct ulmtx_t { int x; } ulmtx_t;
  #define ULMTX_INIT { 0 }
  ul_hapi int ulmtx_init(ulmtx_t* mtx) { mtx->x = 0; return 0; }
  ul_hapi int ulmtx_destroy(ulmtx_t* mtx) { (void)mtx; return 0; }
  ul_hapi int ulmtx_lock(ulmtx_t* mtx) { while(mtx->x) { } mtx->x = 1; return 0; }
  ul_hapi int ulmtx_unlock(ulmtx_t* mtx) { mtx->x = 0; }
  ul_hapi int ulmtx_trylock(ulmtx_t* mtx) { int r = mtx->x; mtx->x = 1; return r ? EBUSY : 0; }

  typedef struct ulrmtx_t { int x; } ulrmtx_t;
  #define ULRMTX_INIT { 0 }
  ul_hapi int ulrmtx_init(ulrmtx_t* mtx) { mtx->x = 0; return 0; }
  ul_hapi int ulrmtx_destroy(ulrmtx_t* mtx) { (void)mtx; return 0; }
  ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) { while(mtx->x) { } mtx->x = 1; return 0; }
  ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) { mtx->x = 0; }
  ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) { int r = mtx->x; mtx->x = 1; return r ? EBUSY : 0; }

  typedef struct ultmtx_t { int x; } ultmtx_t;
  #define ULTMTX_INIT { 0 }
  ul_hapi int ultmtx_init(ultmtx_t* mtx) { mtx->x = 0; return 0; }
  ul_hapi int ultmtx_destroy(ultmtx_t* mtx) { (void)mtx; return 0; }
  ul_hapi int ultmtx_lock(ultmtx_t* mtx) { while(mtx->x) { } mtx->x = 1; return 0; }
  ul_hapi int ultmtx_unlock(ultmtx_t* mtx) { mtx->x = 0; }
  ul_hapi int ultmtx_trylock(ultmtx_t* mtx) { int r = mtx->x; mtx->x = 1; return r ? EBUSY : 0; }
  ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) { (void)ms; return ultmtx_trylock(mtx); }

  typedef struct ulrtmtx_t { int x; } ulrtmtx_t;
  #define ULRTMTX_INIT { 0 }
  ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) { mtx->x = 0; return 0; }
  ul_hapi int ulrtmtx_destroy(ulrtmtx_t* mtx) { (void)mtx; return 0; }
  ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) { while(mtx->x) { } mtx->x = 1; return 0; }
  ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) { mtx->x = 0; }
  ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) { int r = mtx->x; mtx->x = 1; return r ? EBUSY : 0; }
  ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) { (void)ms; return ulrtmtx_trylock(mtx); }
#endif


#ifndef ul_unreachable
  #if defined(__GNUC__) || defined(__clang__)
    #define ul_unreachable() __builtin_unreachable()
  #elif defined(_MSC_VER)
    #define ul_unreachable() __assume(0)
  #else
    #define ul_unreachable() ((void)0)
  #endif
#endif /* ul_unreachable */

#define ULMTX_REF_PLAIN           0u
#define ULMTX_REF_TIMED           1u
#define ULMTX_REF_RECURSIVE       2u
#define ULMTX_REF_RECURSIVE_TIMED 3u
#define ULMTX_REF_TIMED_RECURSIVE 3u

ul_hapi int ulmtx_ref_init(void* mtx, unsigned type) {
  switch(type) {
  case ULMTX_REF_PLAIN:
    return ulmtx_init(ul_reinterpret_cast(ulmtx_t*, mtx));
  case ULMTX_REF_TIMED:
    return ultmtx_init(ul_reinterpret_cast(ultmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE:
    return ulrmtx_init(ul_reinterpret_cast(ulrmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE_TIMED:
    return ulrtmtx_init(ul_reinterpret_cast(ulrtmtx_t*, mtx));
  default:
    ul_unreachable(); return EINVAL;
  }
}
ul_hapi int ulmtx_ref_destroy(void* mtx, unsigned type) {
  switch(type) {
  case ULMTX_REF_PLAIN:
    return ulmtx_destroy(ul_reinterpret_cast(ulmtx_t*, mtx));
  case ULMTX_REF_TIMED:
    return ultmtx_destroy(ul_reinterpret_cast(ultmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE:
    return ulrmtx_destroy(ul_reinterpret_cast(ulrmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE_TIMED:
    return ulrtmtx_destroy(ul_reinterpret_cast(ulrtmtx_t*, mtx));
  default:
    ul_unreachable(); return EINVAL;
  }
}
ul_hapi int ulmtx_ref_lock(void* mtx, unsigned type) {
  switch(type) {
  case ULMTX_REF_PLAIN:
    return ulmtx_lock(ul_reinterpret_cast(ulmtx_t*, mtx));
  case ULMTX_REF_TIMED:
    return ultmtx_lock(ul_reinterpret_cast(ultmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE:
    return ulrmtx_lock(ul_reinterpret_cast(ulrmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE_TIMED:
    return ulrtmtx_lock(ul_reinterpret_cast(ulrtmtx_t*, mtx));
  default:
    ul_unreachable(); return EINVAL;
  }
}
ul_hapi int ulmtx_ref_unlock(void* mtx, unsigned type) {
  switch(type) {
  case ULMTX_REF_PLAIN:
    return ulmtx_unlock(ul_reinterpret_cast(ulmtx_t*, mtx));
  case ULMTX_REF_TIMED:
    return ultmtx_unlock(ul_reinterpret_cast(ultmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE:
    return ulrmtx_unlock(ul_reinterpret_cast(ulrmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE_TIMED:
    return ulrtmtx_unlock(ul_reinterpret_cast(ulrtmtx_t*, mtx));
  default:
    ul_unreachable(); return EINVAL;
  }
}
/* return `EBUSY` if not locked */
ul_hapi int ulmtx_ref_trylock(void* mtx, unsigned type) {
  switch(type) {
  case ULMTX_REF_PLAIN:
    return ulmtx_trylock(ul_reinterpret_cast(ulmtx_t*, mtx));
  case ULMTX_REF_TIMED:
    return ultmtx_trylock(ul_reinterpret_cast(ultmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE:
    return ulrmtx_trylock(ul_reinterpret_cast(ulrmtx_t*, mtx));
  case ULMTX_REF_RECURSIVE_TIMED:
    return ulrtmtx_trylock(ul_reinterpret_cast(ulrtmtx_t*, mtx));
  default:
    ul_unreachable(); return EINVAL;
  }
}
/* return `ETIMEDOUT` or `EBUSY` if not locked */
ul_hapi int ulmtx_ref_timedlock(void* mtx, unsigned type, unsigned long ms) {
  switch(type) {
  case ULMTX_REF_TIMED:
    return ultmtx_timedlock(ul_reinterpret_cast(ultmtx_t*, mtx), ms);
  case ULMTX_REF_RECURSIVE_TIMED:
    return ulrtmtx_timedlock(ul_reinterpret_cast(ulrtmtx_t*, mtx), ms);
  default:
    ul_unreachable(); return EINVAL;
  }
}


#endif /* ULMTX_H */
