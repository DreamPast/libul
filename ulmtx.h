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
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !__STDC_NO_THREADS__ && !defined(__MINGW32__) /* C11 */
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
#if !defined(ULMTX_API_CXX11) && !defined(ULMTX_API_C11) \
    && !defined(ULMTX_API_WIN32) && !defined(ULMTX_API_PTHREADS)
  #error "ulmtx.h: unsupported platform"
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
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi void ulmtx_destroy(ulmtx_t* mtx) noexcept {
    try {
      mtx->destroy();
    } catch(const std::system_error&) { }
  }
  /* return 1 if not locked */
  ul_hapi int ulmtx_trylock(ulmtx_t* mtx) noexcept {
    try {
      return mtx->val.try_lock() ? 0 : 1;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi int ulmtx_lock(ulmtx_t* mtx) noexcept {
    try {
      mtx->val.lock(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi int ulmtx_unlock(ulmtx_t* mtx) noexcept {
    try {
      mtx->val.unlock(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }


  typedef _ULMTX_WRAPPER<std::recursive_mutex> ulrmtx_t;
  #define ULRMTX_INIT _ULMTX_WRAPPER<std::recursive_mutex>::create()
  ul_hapi int ulrmtx_init(ulrmtx_t* mtx) noexcept {
    try {
      mtx->construct(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi void ulrmtx_destroy(ulrmtx_t* mtx) noexcept {
    try {
      mtx->destroy();
    } catch(const std::system_error&) { }
  }
  /* return 1 if not locked */
  ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) noexcept {
    try {
      return mtx->val.try_lock() ? 0 : 1;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) noexcept {
    try {
      mtx->val.lock(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) noexcept {
    try {
      mtx->val.unlock(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }


  typedef _ULMTX_WRAPPER<std::timed_mutex> ultmtx_t;
  #define ULTMTX_INIT _ULMTX_WRAPPER<std::timed_mutex>::create()
  ul_hapi int ultmtx_init(ultmtx_t* mtx) noexcept {
    try {
      mtx->construct(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi void ultmtx_destroy(ultmtx_t* mtx) noexcept {
    try {
      mtx->destroy();
    } catch(const std::system_error&) { }
  }
  /* return 1 if not locked */
  ul_hapi int ultmtx_trylock(ultmtx_t* mtx) noexcept {
    try {
      return mtx->val.try_lock() ? 0 : 1;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  /* return 1 if not locked */
  ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) noexcept {
    try {
      return mtx->val.try_lock_for(std::chrono::milliseconds(ms)) ? 0 : 1;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi int ultmtx_lock(ultmtx_t* mtx) noexcept {
    try {
      mtx->val.lock(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi int ultmtx_unlock(ultmtx_t* mtx) noexcept {
    try {
      mtx->val.unlock(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }


  typedef _ULMTX_WRAPPER<std::recursive_timed_mutex> ulrtmtx_t;
  #define ULRTMTX_INIT _ULMTX_WRAPPER<std::recursive_timed_mutex>::create()
  ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) noexcept {
    try {
      mtx->construct(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi void ulrtmtx_destroy(ulrtmtx_t* mtx) noexcept {
    try {
      mtx->destroy();
    } catch(const std::system_error&) { }
  }
  /* return 1 if not locked */
  ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) noexcept {
    try {
      return mtx->val.try_lock() ? 0 : 1;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  /* return 1 if not locked */
  ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) noexcept {
    try {
      return mtx->val.try_lock_for(std::chrono::milliseconds(ms)) ? 0 : 1;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) noexcept {
    try {
      mtx->val.lock(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }
  ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) noexcept {
    try {
      mtx->val.unlock(); return 0;
    } catch(const std::system_error&) {
      return -1;
    }
  }
#elif defined(ULMTX_API_C11)
  #include <threads.h>
  #include <time.h>
  #include <limits.h>

  ul_hapi int _ulmtx_get_timepoint(struct timespec* tp, unsigned long ms) {
    if(timespec_get(tp, TIME_UTC) != TIME_UTC) return -1;
    tp->tv_nsec += (ms % 1000 * 1000000);
    if(tp->tv_nsec >= 1000000000) { tp->tv_nsec -= 1000000000; ++tp->tv_sec; }
    tp->tv_sec += ms;
    if(tp->tv_sec < 0) return -1;
    return 0;
  }


  typedef struct ulmtx_t { mtx_t m; } ulmtx_t;
  ul_hapi int ulmtx_init(ulmtx_t* mtx) {
    return mtx_init(&mtx->m, mtx_plain) == thrd_success ? 0 : -1;
  }
  ul_hapi void ulmtx_destroy(ulmtx_t* mtx) { mtx_destroy(&mtx->m); }
  /* return 1 if not locked */
  ul_hapi int ulmtx_trylock(ulmtx_t* mtx) {
    switch(mtx_trylock(&mtx->m)) {
    case thrd_success: return 0;
    case thrd_busy: return 1;
    default: return -1;
    }
  }
  ul_hapi int ulmtx_lock(ulmtx_t* mtx) {
    return mtx_lock(&mtx->m) == thrd_success ? 0 : -1;
  }
  ul_hapi int ulmtx_unlock(ulmtx_t* mtx) {
    return mtx_unlock(&mtx->m) == thrd_success ? 0 : -1;
  }


  typedef struct ulrmtx_t { mtx_t m; } ulrmtx_t;
  ul_hapi int ulrmtx_init(ulrmtx_t* mtx) {
    return mtx_init(&mtx->m, mtx_plain | mtx_recursive) == thrd_success ? 0 : -1;
  }
  ul_hapi void ulrmtx_destroy(ulrmtx_t* mtx) { mtx_destroy(&mtx->m); }
  /* return 1 if not locked */
  ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) {
    switch(mtx_trylock(&mtx->m)) {
    case thrd_success: return 0;
    case thrd_busy: return 1;
    default: return -1;
    }
  }
  ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) {
    return mtx_lock(&mtx->m) == thrd_success ? 0 : -1;
  }
  ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) {
    return mtx_unlock(&mtx->m) == thrd_success ? 0 : -1;
  }


  typedef struct ultmtx_t { mtx_t m; } ultmtx_t;
  ul_hapi int ultmtx_init(ultmtx_t* mtx) {
    return mtx_init(&mtx->m, mtx_timed) == thrd_success ? 0 : -1;
  }
  ul_hapi void ultmtx_destroy(ultmtx_t* mtx) { mtx_destroy(&mtx->m); }
  /* return 1 if not locked */
  ul_hapi int ultmtx_trylock(ultmtx_t* mtx) {
    switch(mtx_trylock(&mtx->m)) {
    case thrd_success: return 0;
    case thrd_busy: return 1;
    default: return -1;
    }
  }
  /* return 1 if timedout */
  ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) {
    struct timespec tp;
    if(_ulmtx_get_timepoint(&tp, ms)) return -1;
    switch(mtx_timedlock(&mtx->m, &tp)) {
    case thrd_success: return 0;
    case thrd_busy: return 1;
    default: return -1;
    }
  }
  ul_hapi int ultmtx_lock(ultmtx_t* mtx) {
    return mtx_lock(&mtx->m) == thrd_success ? 0 : -1;
  }
  ul_hapi int ultmtx_unlock(ultmtx_t* mtx) {
    return mtx_unlock(&mtx->m) == thrd_success ? 0 : -1;
  }


  typedef struct ulrtmtx_t { mtx_t m; } ulrtmtx_t;
  ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) {
    return mtx_init(&mtx->m, mtx_timed | mtx_recursive) == thrd_success ? 0 : -1;
  }
  ul_hapi void ulrtmtx_destroy(ulrtmtx_t* mtx) { mtx_destroy(&mtx->m); }
  /* return 1 if not locked */
  ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) {
    switch(mtx_trylock(&mtx->m)) {
    case thrd_success: return 0;
    case thrd_busy: return 1;
    default: return -1;
    }
  }
  /* return 1 if timedout */
  ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) {
    struct timespec tp;
    if(_ulmtx_get_timepoint(&tp, ms)) return -1;
    switch(mtx_timedlock(&mtx->m, &tp)) {
    case thrd_success: return 0;
    case thrd_busy: return 1;
    default: return -1;
    }
  }
  ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) {
    return mtx_lock(&mtx->m) == thrd_success ? 0 : -1;
  }
  ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) {
    return mtx_unlock(&mtx->m) == thrd_success ? 0 : -1;
  }


#elif defined(ULMTX_API_WIN32)
  #include <Windows.h>

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
  ul_hapi void ultmtx_destroy(ultmtx_t* mtx) {
    const HANDLE old_event = ul_reinterpret_cast(HANDLE, _ulmtx_InterlockedExchange(&mtx->event, 0));
    if(old_event) CloseHandle(old_event);
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

  /* return 1 if not locked */
  ul_hapi int ultmtx_trylock(ultmtx_t* mtx) {
    return !_ulmtx_InterlockedBitTestAndSet(&mtx->count, _ULTMTX_LOCK_FLAG_BIT) ? 0 : 1;
  }
  /* return 1 if timeout */
  ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) {
    LARGE_INTEGER freq, target, now;
    _ulmtx_iptr_t old_count;

    if(ultmtx_trylock(mtx)) return 0;
    if(!QueryPerformanceFrequency(&freq)) return -1;
    if(!QueryPerformanceCounter(&target)) return -1;
    target.QuadPart += ms * freq.QuadPart / 1000; /* `ms` is 32-bit, so mostly it won't overflow */
    if(target.QuadPart < 0) return -1;

    old_count = mtx->count;
    _ultmtx_mark_waiting_and_try_lock(mtx, &old_count);
    if(old_count & _ULTMTX_LOCK_FLAG) {
      const HANDLE event = _ultmtx_get_event(mtx);
      do {
        if(!QueryPerformanceCounter(&now)) {
          _ulmtx_InterlockedExchangeAdd(&mtx->count, -1);
          return -1;
        }
        if(now.QuadPart >= target.QuadPart) {
          _ulmtx_InterlockedExchangeAdd(&mtx->count, -1);
          return 0;
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
          return -1;
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
          return -1;
        }
      } while(old_count & _ULTMTX_LOCK_FLAG);
    }
    return 0;
  }
  ul_hapi int ultmtx_unlock(ultmtx_t* mtx) {
    const _ulmtx_iptr_t old_count = _ulmtx_InterlockedExchangeAdd(&mtx->count, _ULTMTX_LOCK_FLAG);
    if(!(old_count & _ULTMTX_EVENT_FLAG) && (old_count > _ULTMTX_LOCK_FLAG)) {
      if(!_ulmtx_InterlockedBitTestAndSet(&mtx->count, _ULTMTX_EVENT_FLAG_BIT))
        SetEvent(_ultmtx_get_event(mtx));
    }
    return 0;
  }


  typedef struct ulmtx_t { ultmtx_t mtx; } ulmtx_t;
  #define ULMTX_INIT { ULTMTX_INIT }
  ul_hapi int ulmtx_init(ulmtx_t* mtx) {
    mtx->mtx.count = 0; mtx->mtx.event = 0; return 0;
  }
  ul_hapi void ulmtx_destroy(ulmtx_t* mtx) { ultmtx_destroy(&mtx->mtx); }
  /* return 1 if not locked */
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
  ul_hapi void ulrtmtx_destroy(ulrtmtx_t* mtx) { ultmtx_destroy(&mtx->mtx); }
  /* return 1 if not locked */
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
  /* return 1 if timedout */
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
  ul_hapi int ulrmtx_init(ulrmtx_t* mtx) {
    return ulrtmtx_init(&mtx->mtx);
  }
  ul_hapi void ulrmtx_destroy(ulrmtx_t* mtx) { ulrtmtx_destroy(&mtx->mtx); }
  /* return 1 if not locked */
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
  ul_hapi int ulmtx_init(ulmtx_t* mtx) {
    return pthread_mutex_init(&mtx->m, NULL) == 0 ? 0 : -1;
  }
  ul_hapi void ulmtx_destroy(ulmtx_t* mtx) {
    pthread_mutex_destroy(&mtx->m);
  }
  /* return 1 if not locked */
  ul_hapi int ulmtx_trylock(ulmtx_t* mtx) {
    switch(pthread_mutex_trylock(&mtx->m)) {
    case 0: return 0;
    case EBUSY: return 1;
    default: return -1;
    }
  }
  ul_hapi int ulmtx_lock(ulmtx_t* mtx) {
    return pthread_mutex_lock(&mtx->m) == 0 ? 0 : -1;
  }
  ul_hapi int ulmtx_unlock(ulmtx_t* mtx) {
    return pthread_mutex_unlock(&mtx->m) == 0 ? 0 : -1;
  }


  #ifdef ULMTX_API_PTHREAD_USE_TIMEDLOCK
    typedef struct ultmtx_t { pthread_mutex_t m; } ultmtx_t;
    #define ULTMTX_INIT { PTHREAD_MUTEX_INITIALIZER }
    ul_hapi int ultmtx_init(ultmtx_t* mtx) {
      return pthread_mutex_init(&mtx->m, NULL) == 0 ? 0 : -1;
    }
    ul_hapi void ultmtx_destroy(ultmtx_t* mtx) {
      pthread_mutex_destroy(&mtx->m);
    }
    /* return 1 if not locked */
    ul_hapi int ultmtx_trylock(ultmtx_t* mtx) {
      switch(pthread_mutex_trylock(&mtx->m)) {
      case EBUSY: return 1;
      case 0: return 0;
      default: return -1;
      }
    }
    /* return 1 if timedout */
    ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) {
      struct timespec tp;
      if(_ulmtx_get_timepoint(&tp, ms)) return -1;
      switch(pthread_mutex_timedlock(&mtx->m, &tp)) {
      case ETIMEDOUT: return 1;
      case 0: return 0;
      default: return -1;
      }
    }
    ul_hapi int ultmtx_lock(ultmtx_t* mtx) {
      return pthread_mutex_lock(&mtx->m) == 0 ? 0 : -1;
    }
    ul_hapi int ultmtx_unlock(ultmtx_t* mtx) {
      return pthread_mutex_unlock(&mtx->m) == 0 ? 0 : -1;
    }
  #else
    typedef struct ultmtx_t {
      pthread_mutex_t m;
      pthread_cond_t cond;
      int locked;
    } ultmtx_t;
    #define ULTMTX_INIT { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0 }
    ul_hapi int ultmtx_init(ultmtx_t* mtx) {
      if(pthread_mutex_init(&mtx->m, NULL) != 0) return -1;
      if(pthread_cond_init(&mtx->cond, NULL) != 0) {
        pthread_mutex_destroy(&mtx->m);
        return -1;
      }
      mtx->locked = 0;
      return 0;
    }
    ul_hapi void ultmtx_destroy(ultmtx_t* mtx) {
      pthread_mutex_destroy(&mtx->m);
      pthread_cond_destroy(&mtx->cond);
    }
    /* return 1 if not locked */
    ul_hapi int ultmtx_trylock(ultmtx_t* mtx) {
      int ret;
      if(pthread_mutex_lock(&mtx->m)) return -1;
      ret = mtx->locked;
      mtx->locked = 1;
      pthread_mutex_unlock(&mtx->m);
      return ret;
    }
    /* return 1 if timedout */
    ul_hapi int ultmtx_timedlock(ultmtx_t* mtx, unsigned long ms) {
      struct timespec tp;
      int ret;

      if(_ulmtx_get_timepoint(&tp, ms)) return -1;
      if(pthread_mutex_lock(&mtx->m)) return -1;
      while(mtx->locked) {
        ret = pthread_cond_timedwait(&mtx->cond, &mtx->m, &tp);
        if(ret == ETIMEDOUT) break;
        if(ret != 0) { pthread_mutex_unlock(&mtx->m); return -1; }
      }
      ret = mtx->locked;
      mtx->locked = 1;
      pthread_mutex_unlock(&mtx->m);
      return ret;
    }
    ul_hapi int ultmtx_lock(ultmtx_t* mtx) {
      if(pthread_mutex_lock(&mtx->m)) return -1;
      while(mtx->locked)
        if(pthread_cond_wait(&mtx->cond, &mtx->m)) {
          pthread_mutex_unlock(&mtx->m); return -1;
        }
      mtx->locked = 1;
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ultmtx_unlock(ultmtx_t* mtx) {
      if(pthread_mutex_lock(&mtx->m)) return -1;
      mtx->locked = 0;
      pthread_cond_signal(&mtx->cond);
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
  #endif


  #ifdef ULMTX_API_PTHREAD_USE_MUTEXATTR_SETTYPE
    typedef struct ulrmtx_t { pthread_mutex_t m; } ulrmtx_t;

    ul_hapi int ulrmtx_init(ulrmtx_t* mtx) {
      pthread_mutexattr_t attr;
      if(pthread_mutexattr_init(&attr)) return -1;
      if(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) {
        pthread_mutexattr_destroy(&attr); return -1;
      }
      if(pthread_mutex_init(&mtx->m, &attr)) {
        pthread_mutexattr_destroy(&attr); return -1;
      }
      pthread_mutexattr_destroy(&attr);
      return 0;
    }
    ul_hapi void ulrmtx_destroy(ulrmtx_t* mtx) {
      pthread_mutex_destroy(&mtx->m);
    }
    /* return 1 if not locked */
    ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) {
      switch(pthread_mutex_trylock(&mtx->m)) {
      case 0: return 0;
      case EBUSY: return 1;
      default: return -1;
      }
    }
    ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) {
      return pthread_mutex_lock(&mtx->m) == 0 ? 0 : -1;
    }
    ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) {
      return pthread_mutex_unlock(&mtx->m) == 0 ? 0 : -1;
    }
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
      if(pthread_mutex_init(&mtx->m, NULL)) return -1;
      if(pthread_cond_init(&mtx->cond, NULL)) {
        pthread_mutex_destroy(&mtx->m);
        return -1;
      }
      mtx->locked = 0;
      mtx->count = 0;
      return 0;
    }
    ul_hapi void ulrmtx_destroy(ulrmtx_t* mtx) {
      pthread_mutex_destroy(&mtx->m);
      pthread_cond_destroy(&mtx->cond);
    }
    /* return 1 if not locked */
    ul_hapi int ulrmtx_trylock(ulrmtx_t* mtx) {
      if(pthread_mutex_lock(&mtx->m)) return -1;
      if(mtx->locked && !pthread_equal(mtx->owner, pthread_self())) {
        pthread_mutex_unlock(&mtx->m); return 1;
      }
      mtx->locked = 1;
      ++mtx->count;
      mtx->owner = pthread_self();
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ulrmtx_lock(ulrmtx_t* mtx) {
      if(pthread_mutex_lock(&mtx->m)) return -1;
      if(mtx->locked && pthread_equal(mtx->owner, pthread_self())) {
        ++mtx->count; pthread_mutex_unlock(&mtx->m); return 0;
      }
      while(mtx->locked)
        if(pthread_cond_wait(&mtx->cond, &mtx->m)) {
          pthread_mutex_unlock(&mtx->m); return -1;
        }
      mtx->locked = 1;
      ++mtx->count;
      mtx->owner = pthread_self();
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ulrmtx_unlock(ulrmtx_t* mtx) {
      if(pthread_mutex_lock(&mtx->m)) return -1;
      if(!--mtx->count) mtx->locked = 0;
      pthread_cond_signal(&mtx->cond);
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
  #endif


  #ifdef ULMTX_API_PTHREAD_USE_RECURSIVE_TIMEDLOCK
    typedef struct ulrtmtx_t { pthread_mutex_t m; } ulrtmtx_t;

    ul_hapi int ulrtmtx_init(ulrtmtx_t* mtx) {
      pthread_mutexattr_t attr;
      if(pthread_mutexattr_init(&attr)) return -1;
      if(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) {
        pthread_mutexattr_destroy(&attr); return -1;
      }
      if(pthread_mutex_init(&mtx->m, &attr)) {
        pthread_mutexattr_destroy(&attr); return -1;
      }
      pthread_mutexattr_destroy(&attr);
      return 0;
    }
    ul_hapi void ulrtmtx_destroy(ulrtmtx_t* mtx) {
      pthread_mutex_destroy(&mtx->m);
    }
    /* return 1 if not locked */
    ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) {
      switch(pthread_mutex_trylock(&mtx->m)) {
      case 0: return 0;
      case EBUSY: return 1;
      default: return -1;
      }
    }
    /* return 1 if timedout */
    ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) {
      struct timespec tp;
      if(_ulmtx_get_timepoint(&tp, ms)) return -1;
      switch(pthread_mutex_timedlock(&mtx->m, &tp)) {
      case ETIMEDOUT: return 1;
      case 0: return 0;
      default: return -1;
      }
    }
    ul_hapi int ulrtmtx_lock(ulrtmtx_t* mtx) {
      return pthread_mutex_lock(&mtx->m) == 0 ? 0 : -1;
    }
    ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) {
      return pthread_mutex_unlock(&mtx->m) == 0 ? 0 : -1;
    }
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
      if(pthread_mutex_init(&mtx->m, NULL)) return -1;
      if(pthread_cond_init(&mtx->cond, NULL)) {
        pthread_mutex_destroy(&mtx->m);
        return -1;
      }
      mtx->locked = 0;
      mtx->count = 0;
      return 0;
    }
    ul_hapi void ulrtmtx_destroy(ulrtmtx_t* mtx) {
      pthread_mutex_destroy(&mtx->m);
      pthread_cond_destroy(&mtx->cond);
    }
    /* return 1 if not locked */
    ul_hapi int ulrtmtx_trylock(ulrtmtx_t* mtx) {
      if(pthread_mutex_lock(&mtx->m)) return -1;
      if(mtx->locked && !pthread_equal(mtx->owner, pthread_self())) {
        pthread_mutex_unlock(&mtx->m); return 1;
      }
      mtx->locked = 1;
      ++mtx->count;
      mtx->owner = pthread_self();
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    /* return 1 if timedout */
    ul_hapi int ulrtmtx_timedlock(ulrtmtx_t* mtx, unsigned long ms) {
      struct timespec tp;
      int ret;

      if(_ulmtx_get_timepoint(&tp, ms)) return -1;
      if(pthread_mutex_lock(&mtx->m)) return -1;
      if(mtx->locked && pthread_equal(mtx->owner, pthread_self())) {
        ++mtx->count; pthread_mutex_unlock(&mtx->m); return 0;
      }
      while(mtx->locked) {
        ret = pthread_cond_timedwait(&mtx->cond, &mtx->m, &tp);
        if(ret == ETIMEDOUT) break;
        if(ret != 0) { pthread_mutex_unlock(&mtx->m); return -1; }
      }
      if(mtx->locked) ret = 1;
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
      if(pthread_mutex_lock(&mtx->m)) return -1;
      if(mtx->locked && pthread_equal(mtx->owner, pthread_self())) {
        ++mtx->count; pthread_mutex_unlock(&mtx->m); return 0;
      }
      while(mtx->locked)
        if(pthread_cond_wait(&mtx->cond, &mtx->m)) {
          pthread_mutex_unlock(&mtx->m); return -1;
        }
      mtx->locked = 1;
      ++mtx->count;
      mtx->owner = pthread_self();
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
    ul_hapi int ulrtmtx_unlock(ulrtmtx_t* mtx) {
      if(pthread_mutex_lock(&mtx->m)) return -1;
      if(!--mtx->count) mtx->locked = 0;
      pthread_cond_signal(&mtx->cond);
      pthread_mutex_unlock(&mtx->m);
      return 0;
    }
  #endif

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
    ul_unreachable(); return -1;
  }
}
ul_hapi void ulmtx_ref_destroy(void* mtx, unsigned type) {
  switch(type) {
  case ULMTX_REF_PLAIN:
    ulmtx_destroy(ul_reinterpret_cast(ulmtx_t*, mtx)); break;
  case ULMTX_REF_TIMED:
    ultmtx_destroy(ul_reinterpret_cast(ultmtx_t*, mtx)); break;
  case ULMTX_REF_RECURSIVE:
    ulrmtx_destroy(ul_reinterpret_cast(ulrmtx_t*, mtx)); break;
  case ULMTX_REF_RECURSIVE_TIMED:
    ulrtmtx_destroy(ul_reinterpret_cast(ulrtmtx_t*, mtx)); break;
  default:
    ul_unreachable();
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
    ul_unreachable(); return -1;
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
    ul_unreachable(); return -1;
  }
}
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
    ul_unreachable(); return -1;
  }
}
ul_hapi int ulmtx_ref_timedlock(void* mtx, unsigned type, unsigned long ms) {
  switch(type) {
  case ULMTX_REF_TIMED:
    return ultmtx_timedlock(ul_reinterpret_cast(ultmtx_t*, mtx), ms);
  case ULMTX_REF_RECURSIVE_TIMED:
    return ulrtmtx_timedlock(ul_reinterpret_cast(ulrtmtx_t*, mtx), ms);
  default:
    ul_unreachable(); return -1;
  }
}


#endif /* ULMTX_H */
