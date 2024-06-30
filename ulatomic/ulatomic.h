/*
Atomic (32-bit, and optional 64-bit)


# Dependences
  - C++11 / C11 => full support if atomic is available
  - Visual Studio => 32-bit atomic, and 64-bit for newer versions
  - GNUC 4.7 => full support
  - 32-bit integer, and optional 64-bit integer => compatible support (by defining `ULATOMIC_NEEDED`)


# Config macro
  - ULATOMIC_NEEDED
    Define atomic macro even if atomic APIs not found, to ensure compatibility with atomic operations.
  - ULATOMIC_SINGLE_THREAD
    Disbale multi-thread support. It's often used with `ULATOMIC_NEEDED`.

# APIs
  Macros are similar to C11, but you need to add suffixes because C89 does not support generics.
  The specific function of the API can be referred to in the C11/C++11 documentation.

  ## Memory synchronization ordering
    +----------------------------------------------------------------
    | typedef ... ulatomic_memory_order_t;
    |
    | ulatomic_memory_order_t ulatomic_memory_order_relaxed = ...;
    | ulatomic_memory_order_t ulatomic_memory_order_consume = ...;
    | ulatomic_memory_order_t ulatomic_memory_order_acquire = ...;
    | ulatomic_memory_order_t ulatomic_memory_order_release = ...;
    | ulatomic_memory_order_t ulatomic_memory_order_acq_rel = ...;
    | ulatomic_memory_order_t ulatomic_memory_order_seq_cst = ...;
    +----------------------------------------------------------------
    | void ulatomic_thread_fence(ulatomic_memory_order_t ord);
    | void ulatomic_signal_fence(ulatomic_memory_order_t ord);
    | void ulatomic_compiler_fence();
    +----------------------------------------------------------------

  ## Atomic flag and operations
    +----------------------------------------------------------------
    | typedef ... ulatomic_flag_t;
    | ulatomic_flag_t ULATOMIC_FLAG_INIT = ...;
    | int ulatomic_flag_test_and_set(ulatomic_flag_t* obj);
    | int ulatomic_flag_test_and_set_explicit(ulatomic_flag_t* obj, ulatomic_memory_order_t ord);
    | void ulatomic_flag_clear(ulatomic_flag_t* obj);
    | void ulatomic_flag_clear_explicit(ulatomic_flag_t* obj, ulatomic_memory_order_t ord);
    +----------------------------------------------------------------

  ## Atomic type and operations
    +----------------------------------------------------------------
    | typedef ... ulatomic32_t;
    | typedef ... ulatomic32_raw_t;
    | ulatomic32_t ULATOMIC32_INIT = ...;
    |
    | void ulatomic32_store(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic32_load(ulatomic32_t* obj);
    | ulatomic32_raw_t ulatomic32_exchange(ulatomic32_t* obj, ulatomic32_raw_t val);
    | void ulatomic32_store_explicit(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic32_load_explicit(ulatomic32_t* obj, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic32_exchange_explicit(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    |
    | int ulatomic32_compare_exchange_strong(ulatomic32_t* obj, ulatomic32_raw_t* expected, ulatomic32_raw_t val);
    | int ulatomic32_compare_exchange_weak(ulatomic32_t* obj, ulatomic32_raw_t* expected, ulatomic32_raw_t val);
    | int ulatomic32_compare_exchange_strong_explicit(ulatomic32_t* obj, ulatomic32_raw_t* expected, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | int ulatomic32_compare_exchange_weak_explicit(ulatomic32_t* obj, ulatomic32_raw_t* expected, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomic32_raw_t ulatomic32_fetch_add(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic32_fetch_sub(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic32_fetch_add_explicit(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic32_fetch_sub_explicit(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomic32_raw_t ulatomic32_fetch_or(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic32_fetch_xor(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic32_fetch_and(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic32_fetch_or_explicit(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic32_fetch_xor_explicit(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic32_fetch_and_explicit(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    +----------------------------------------------------------------
    | typedef ... ulatomic64_t;
    | typedef ... ulatomic64_raw_t;
    | ulatomic64_t ULATOMIC64_INIT = ...;
    |
    | void ulatomic64_store(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic64_load(ulatomic64_t* obj);
    | ulatomic64_raw_t ulatomic64_exchange(ulatomic64_t* obj, ulatomic64_raw_t val);
    | void ulatomic64_store_explicit(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic64_load_explicit(ulatomic64_t* obj, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic64_exchange_explicit(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    |
    | int ulatomic64_compare_exchange_strong(ulatomic64_t* obj, ulatomic64_raw_t* expected, ulatomic64_raw_t val);
    | int ulatomic64_compare_exchange_weak(ulatomic64_t* obj, ulatomic64_raw_t* expected, ulatomic64_raw_t val);
    | int ulatomic64_compare_exchange_strong_explicit(ulatomic64_t* obj, ulatomic64_raw_t* expected, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | int ulatomic64_compare_exchange_weak_explicit(ulatomic64_t* obj, ulatomic64_raw_t* expected, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomic64_raw_t ulatomic64_fetch_add(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic64_fetch_sub(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic64_fetch_add_explicit(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic64_fetch_sub_explicit(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomic64_raw_t ulatomic64_fetch_or(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic64_fetch_xor(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic64_fetch_and(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic64_fetch_or_explicit(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic64_fetch_xor_explicit(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic64_fetch_and_explicit(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    +----------------------------------------------------------------
    | typedef ... ulatomiciptr_t;
    | typedef ... ulatomiciptr_raw_t;
    | ulatomiciptr_t ULATOMICIPTR_INIT = ...;
    |
    | void ulatomic_store_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val);
    | ulatomiciptr_raw_t ulatomic_load_iptr(ulatomiciptr_t* obj);
    | ulatomiciptr_raw_t ulatomic_exchange_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val);
    | void ulatomic_store_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    | ulatomiciptr_raw_t ulatomic_load_explicit_iptr(ulatomiciptr_t* obj, ulatomic_memory_order_t ord);
    | ulatomiciptr_raw_t ulatomic_exchange_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    |
    | int ulatomic_compare_exchange_strong_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t* expected, ulatomiciptr_raw_t val);
    | int ulatomic_compare_exchange_weak_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t* expected, ulatomiciptr_raw_t val);
    | int ulatomic_compare_exchange_strong_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t* expected, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    | int ulatomic_compare_exchange_weak_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t* expected, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomiciptr_raw_t ulatomic_fetch_add_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val);
    | ulatomiciptr_raw_t ulatomic_fetch_sub_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val);
    | ulatomiciptr_raw_t ulatomic_fetch_add_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    | ulatomiciptr_raw_t ulatomic_fetch_sub_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomiciptr_raw_t ulatomic_fetch_or_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val);
    | ulatomiciptr_raw_t ulatomic_fetch_xor_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val);
    | ulatomiciptr_raw_t ulatomic_fetch_and_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val);
    | ulatomiciptr_raw_t ulatomic_fetch_or_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    | ulatomiciptr_raw_t ulatomic_fetch_xor_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    | ulatomiciptr_raw_t ulatomic_fetch_and_explicit_iptr(ulatomiciptr_t* obj, ulatomiciptr_raw_t val, ulatomic_memory_order_t ord);
    +----------------------------------------------------------------

  ## Spinlock
    +----------------------------------------------------------------
    | typedef ... ulatomic_spinlock_t
    | ulatomic_spinlock_t ULATOMIC_SPINLOCK_INIT = ...;
    | void ulatomic_spin_init(ulatomic_spinlock_t* lck);
    | void ulatomic_spin_lock(ulatomic_spinlock_t* lck);
    | int ulatomic_spin_trylock(ulatomic_spinlock_t* lck);
    | void ulatomic_spin_unlock(ulatomic_spinlock_t* lck);
    +----------------------------------------------------------------

  ## Spin Reader Writer Lock
    +----------------------------------------------------------------
    | typedef ... ulatomic_rwlock_t
    | ulatomic_rwlock_t ULATOMIC_RWLOCK_INIT = ...;
    | ul_hapi void ulatomic_rw_init(ulatomic_rwlock_t* lck);
    | ul_hapi void ulatomic_rw_rlock(ulatomic_rwlock_t* lck);
    | ul_hapi int ulatomic_rw_tryrlock(ulatomic_rwlock_t* lck);
    | ul_hapi int ulatomic_rw_trywlock(ulatomic_rwlock_t* lck);
    | ul_hapi void ulatomic_rw_unrlock(ulatomic_rwlock_t* lck);
    | ul_hapi void ulatomic_rw_unwlock(ulatomic_rwlock_t* lck);
    +----------------------------------------------------------------

  ## Wait / Notify
    +----------------------------------------------------------------
    | void ulatomic32_wait(ulatomic32_t* obj, ulatomic32_raw_t expected);
    | void ulatomic32_notify_one(ulatomic32_t* obj);
    | void ulatomic32_notify_all(ulatomic32_t* obj);
    +----------------------------------------------------------------
    | void ulatomic64_wait(ulatomic64_t* obj, ulatomic64_raw_t expected);
    | void ulatomic64_notify_one(ulatomic64_t* obj);
    | void ulatomic64_notify_all(ulatomic64_t* obj);
    +----------------------------------------------------------------
    | void ulatomiciptr_wait(ulatomiciptr_t* obj, ulatomiciptr_raw_t expected);
    | void ulatomiciptr_notify_one(ulatomiciptr_t* obj);
    | void ulatomiciptr_notify_all(ulatomiciptr_t* obj);
    +----------------------------------------------------------------

  ## Yield / Pause
    +----------------------------------------------------------------
    | void ulatomic_yield(void);
    | void ulatomic_pause(void);
    +----------------------------------------------------------------

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

#ifndef ULATOMIC_H
#define ULATOMIC_H

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

#ifndef ul_static_cast
  #ifdef __cplusplus
    #define ul_static_cast(T, v) (static_cast<T>(v))
  #else
    #define ul_static_cast(T, v) ((T)(v))
  #endif
#endif /* ul_static_cast */

#ifndef ul_reinterpret_cast
  #ifdef __cplusplus
    #define ul_reinterpret_cast(T, v) (reinterpret_cast<T>(v))
  #else
    #define ul_reinterpret_cast(T, v) ((T)(v))
  #endif
#endif /* ul_reinterpret_cast */

#if defined(__cplusplus)
  #if __cplusplus >= 201103L && __STDCPP_THREADS__ /* C++11 and above */
    #define ULATOMIC_API_CXX11
  #endif
  #if __cplusplus >= 202002L /* C++20 and above */
    #define ULATOMIC_API_CXX20
  #endif
#elif __STDC_VERSION__ >= 201112L && (!defined(__STDC_NO_ATOMICS__) || !__STDC_NO_ATOMICS__) /* C11 and above */
  #define ULATOMIC_API_C11
#endif
#if defined(_MSC_VER)
  #define ULATOMIC_API_MSVC
#endif
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) /* gcc 4.7 and above */
  #define ULATOMIC_API_GNUC
#endif

#ifdef ULATOMIC_SINGLE_THREAD
  #undef ULATOMIC_API_CXX11
  #undef ULATOMIC_API_CXX20
  #undef ULATOMIC_API_C11
  #undef ULATOMIC_API_MSVC
  #undef ULATOMIC_API_GNUC
#endif

#define _ulatomic_memory_order_of_fail(success) ((success) == ulatomic_memory_order_acq_rel \
  ? ulatomic_memory_order_acquire \
  : ((success) == ulatomic_memory_order_release ? ulatomic_memory_order_relaxed : (success)))
#if defined(ULATOMIC_API_C11)
  #include <stdatomic.h>
  #include <stdint.h>

  typedef memory_order ulatomic_memory_order_t;
  #define ulatomic_memory_order_relaxed memory_order_relaxed
  #define ulatomic_memory_order_consume memory_order_consume
  #define ulatomic_memory_order_acquire memory_order_acquire
  #define ulatomic_memory_order_release memory_order_release
  #define ulatomic_memory_order_acq_rel memory_order_acq_rel
  #define ulatomic_memory_order_seq_cst memory_order_seq_cst

  #define ulatomic_thread_fence(ord) atomic_thread_fence(ord)
  #define ulatomic_signal_fence(ord) atomic_signal_fence(ord)
  #define ulatomic_compiler_fence()  ulatomic_thread_fence(ulatomic_memory_order_seq_cst)

  typedef atomic_flag ulatomic_flag_t;
  #define ULATOMIC_FLAG_INIT ATOMIC_FLAG_INIT
  #define ulatomic_flag_test_and_set(obj)               ul_static_cast(int, atomic_flag_test_and_set(obj))
  #define ulatomic_flag_test_and_set_explicit(obj, ord) ul_static_cast(int, atomic_flag_test_and_set_explicit(obj, ord))
  #define ulatomic_flag_clear(obj)                      atomic_flag_clear(obj)
  #define ulatomic_flag_clear_explicit(obj, ord)        atomic_flag_clear_explicit(obj, ord)

  typedef _Atomic int32_t ulatomic32_t;
  typedef int32_t ulatomic32_raw_t;
  #if __STDC_VERSION__ >= 201710L
    #define ULATOMIC32_INIT (0)
  #else
    #define ULATOMIC32_INIT ATOMIC_VAR_INIT(0)
  #endif
  #define ulatomic32_store(obj, val)                             atomic_store(obj, val)
  #define ulatomic32_load(obj)                                   atomic_load(obj)
  #define ulatomic32_exchange(obj, val)                          atomic_exchange(obj, val)
  #define ulatomic32_compare_exchange_strong(obj, expected, val) ul_static_cast(int, atomic_compare_exchange_strong(obj, expected, val))
  #define ulatomic32_compare_exchange_weak(obj, expected, val)   ul_static_cast(int, atomic_compare_exchange_weak(obj, expected, val))
  #define ulatomic32_fetch_add(obj, val)                         atomic_fetch_add(obj, val)
  #define ulatomic32_fetch_sub(obj, val)                         atomic_fetch_sub(obj, val)
  #define ulatomic32_fetch_or(obj, val)                          atomic_fetch_or(obj, val)
  #define ulatomic32_fetch_xor(obj, val)                         atomic_fetch_xor(obj, val)
  #define ulatomic32_fetch_and(obj, val)                         atomic_fetch_and(obj, val)
  #define ulatomic32_store_explicit(obj, val, ord)                             atomic_store_explicit(obj, val, ord)
  #define ulatomic32_load_explicit(obj, ord)                                   atomic_load_explicit(obj, ord)
  #define ulatomic32_exchange_explicit(obj, val, ord)                          atomic_exchange_explicit(obj, val, ord)
  #define ulatomic32_compare_exchange_strong_explicit(obj, expected, val, ord) \
    ul_static_cast(int, atomic_compare_exchange_strong_explicit(obj, expected, val, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic32_compare_exchange_weak_explicit(obj, expected, val, ord) \
    ul_static_cast(int, atomic_compare_exchange_weak_explicit(obj, expected, val, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic32_fetch_add_explicit(obj, val, ord)                         atomic_fetch_add_explicit(obj, val, ord)
  #define ulatomic32_fetch_sub_explicit(obj, val, ord)                         atomic_fetch_sub_explicit(obj, val, ord)
  #define ulatomic32_fetch_or_explicit(obj, val, ord)                          atomic_fetch_or_explicit(obj, val, ord)
  #define ulatomic32_fetch_xor_explicit(obj, val, ord)                         atomic_fetch_xor_explicit(obj, val, ord)
  #define ulatomic32_fetch_and_explicit(obj, val, ord)                         atomic_fetch_and_explicit(obj, val, ord)

  typedef _Atomic int64_t ulatomic64_t;
  typedef int64_t ulatomic64_raw_t;
  #if __STDC_VERSION__ >= 201710L
    #define ULATOMIC64_INIT (0)
  #else
    #define ULATOMIC64_INIT ATOMIC_VAR_INIT(0)
  #endif
  #define ulatomic64_store(obj, val)                             atomic_store(obj, val)
  #define ulatomic64_load(obj)                                   atomic_load(obj)
  #define ulatomic64_exchange(obj, val)                          atomic_exchange(obj, val)
  #define ulatomic64_compare_exchange_strong(obj, expected, val) ul_static_cast(int, atomic_compare_exchange_strong(obj, expected, val))
  #define ulatomic64_compare_exchange_weak(obj, expected, val)   ul_static_cast(int, atomic_compare_exchange_weak(obj, expected, val))
  #define ulatomic64_fetch_add(obj, val)                         atomic_fetch_add(obj, val)
  #define ulatomic64_fetch_sub(obj, val)                         atomic_fetch_sub(obj, val)
  #define ulatomic64_fetch_or(obj, val)                          atomic_fetch_or(obj, val)
  #define ulatomic64_fetch_xor(obj, val)                         atomic_fetch_xor(obj, val)
  #define ulatomic64_fetch_and(obj, val)                         atomic_fetch_and(obj, val)
  #define ulatomic64_store_explicit(obj, val, ord)                             atomic_store_explicit(obj, val, ord)
  #define ulatomic64_load_explicit(obj, ord)                                   atomic_load_explicit(obj, ord)
  #define ulatomic64_exchange_explicit(obj, val, ord)                          atomic_exchange_explicit(obj, val, ord)
  #define ulatomic64_compare_exchange_strong_explicit(obj, expected, val, ord) \
    ul_static_cast(int, atomic_compare_exchange_strong_explicit(obj, expected, val, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic64_compare_exchange_weak_explicit(obj, expected, val, ord) \
    ul_static_cast(int, atomic_compare_exchange_weak_explicit(obj, expected, val, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic64_fetch_add_explicit(obj, val, ord)                         atomic_fetch_add_explicit(obj, val, ord)
  #define ulatomic64_fetch_sub_explicit(obj, val, ord)                         atomic_fetch_sub_explicit(obj, val, ord)
  #define ulatomic64_fetch_or_explicit(obj, val, ord)                          atomic_fetch_or_explicit(obj, val, ord)
  #define ulatomic64_fetch_xor_explicit(obj, val, ord)                         atomic_fetch_xor_explicit(obj, val, ord)
  #define ulatomic64_fetch_and_explicit(obj, val, ord)                         atomic_fetch_and_explicit(obj, val, ord)
#elif defined(ULATOMIC_API_CXX11)
  #include <atomic>
  #include <cstdint>

  typedef ::std::memory_order ulatomic_memory_order_t;
  #define ulatomic_memory_order_relaxed ::std::memory_order_relaxed
  #define ulatomic_memory_order_consume ::std::memory_order_consume
  #define ulatomic_memory_order_acquire ::std::memory_order_acquire
  #define ulatomic_memory_order_release ::std::memory_order_release
  #define ulatomic_memory_order_acq_rel ::std::memory_order_acq_rel
  #define ulatomic_memory_order_seq_cst ::std::memory_order_seq_cst

  #define ulatomic_thread_fence(ord) ::std::atomic_thread_fence(ord)
  #define ulatomic_signal_fence(ord) ::std::atomic_signal_fence(ord)
  #define ulatomic_compiler_fence()  ulatomic_thread_fence(ulatomic_memory_order_seq_cst)

  typedef ::std::atomic_flag ulatomic_flag_t;
  #define ULATOMIC_FLAG_INIT ATOMIC_FLAG_INIT
  #define ulatomic_flag_test_and_set(obj)               ul_static_cast(int, ::std::atomic_flag_test_and_set(obj))
  #define ulatomic_flag_test_and_set_explicit(obj, ord) ul_static_cast(int, ::std::atomic_flag_test_and_set_explicit(obj, ord))
  #define ulatomic_flag_clear(obj)                      ::std::atomic_flag_clear(obj)
  #define ulatomic_flag_clear_explicit(obj, ord)        ::std::atomic_flag_clear_explicit(obj, ord)

  typedef ::std::atomic<int32_t> ulatomic32_t;
  typedef int32_t ulatomic32_raw_t;
  #define ULATOMIC32_INIT { 0 }
  #define ulatomic32_store(obj, val)                             ::std::atomic_store(obj, val)
  #define ulatomic32_load(obj)                                   ::std::atomic_load(obj)
  #define ulatomic32_exchange(obj, val)                          ::std::atomic_exchange(obj, val)
  #define ulatomic32_compare_exchange_strong(obj, expected, val) ul_static_cast(int, ::std::atomic_compare_exchange_strong(obj, expected, val))
  #define ulatomic32_compare_exchange_weak(obj, expected, val)   ul_static_cast(int, ::std::atomic_compare_exchange_weak(obj, expected, val))
  #define ulatomic32_fetch_add(obj, val)                         ::std::atomic_fetch_add(obj, val)
  #define ulatomic32_fetch_sub(obj, val)                         ::std::atomic_fetch_sub(obj, val)
  #define ulatomic32_fetch_or(obj, val)                          ::std::atomic_fetch_or(obj, val)
  #define ulatomic32_fetch_xor(obj, val)                         ::std::atomic_fetch_xor(obj, val)
  #define ulatomic32_fetch_and(obj, val)                         ::std::atomic_fetch_and(obj, val)
  #define ulatomic32_store_explicit(obj, val, ord)                             ::std::atomic_store_explicit(obj, val, ord)
  #define ulatomic32_load_explicit(obj, ord)                                   ::std::atomic_load_explicit(obj, ord)
  #define ulatomic32_exchange_explicit(obj, val, ord)                          ::std::atomic_exchange_explicit(obj, val, ord)
  #define ulatomic32_compare_exchange_strong_explicit(obj, expected, val, ord) \
    ul_static_cast(int, ::std::atomic_compare_exchange_strong_explicit(obj, expected, val, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic32_compare_exchange_weak_explicit(obj, expected, val, ord) \
    ul_static_cast(int, ::std::atomic_compare_exchange_weak_explicit(obj, expected, val, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic32_fetch_add_explicit(obj, val, ord)                         ::std::atomic_fetch_add_explicit(obj, val, ord)
  #define ulatomic32_fetch_sub_explicit(obj, val, ord)                         ::std::atomic_fetch_sub_explicit(obj, val, ord)
  #define ulatomic32_fetch_or_explicit(obj, val, ord)                          ::std::atomic_fetch_or_explicit(obj, val, ord)
  #define ulatomic32_fetch_xor_explicit(obj, val, ord)                         ::std::atomic_fetch_xor_explicit(obj, val, ord)
  #define ulatomic32_fetch_and_explicit(obj, val, ord)                         ::std::atomic_fetch_and_explicit(obj, val, ord)

  typedef ::std::atomic<int64_t> ulatomic64_t;
  typedef int64_t ulatomic64_raw_t;
  #define ULATOMIC64_INIT { 0 }
  #define ulatomic64_store(obj, val)                             ::std::atomic_store(obj, val)
  #define ulatomic64_load(obj)                                   ::std::atomic_load(obj)
  #define ulatomic64_exchange(obj, val)                          ::std::atomic_exchange(obj, val)
  #define ulatomic64_compare_exchange_strong(obj, expected, val) ul_static_cast(int, ::std::atomic_compare_exchange_strong(obj, expected, val))
  #define ulatomic64_compare_exchange_weak(obj, expected, val)   ul_static_cast(int, ::std::atomic_compare_exchange_weak(obj, expected, val))
  #define ulatomic64_fetch_add(obj, val)                         ::std::atomic_fetch_add(obj, val)
  #define ulatomic64_fetch_sub(obj, val)                         ::std::atomic_fetch_sub(obj, val)
  #define ulatomic64_fetch_or(obj, val)                          ::std::atomic_fetch_or(obj, val)
  #define ulatomic64_fetch_xor(obj, val)                         ::std::atomic_fetch_xor(obj, val)
  #define ulatomic64_fetch_and(obj, val)                         ::std::atomic_fetch_and(obj, val)
  #define ulatomic64_store_explicit(obj, val, ord)                             ::std::atomic_store_explicit(obj, val, ord)
  #define ulatomic64_load_explicit(obj, ord)                                   ::std::atomic_load_explicit(obj, ord)
  #define ulatomic64_exchange_explicit(obj, val, ord)                          ::std::atomic_exchange_explicit(obj, val, ord)
  #define ulatomic64_compare_exchange_strong_explicit(obj, expected, val, ord) \
    ul_static_cast(int, ::std::atomic_compare_exchange_strong_explicit(obj, expected, val, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic64_compare_exchange_weak_explicit(obj, expected, val, ord) \
    ul_static_cast(int, ::std::atomic_compare_exchange_weak_explicit(obj, expected, val, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic64_fetch_add_explicit(obj, val, ord)                         ::std::atomic_fetch_add_explicit(obj, val, ord)
  #define ulatomic64_fetch_sub_explicit(obj, val, ord)                         ::std::atomic_fetch_sub_explicit(obj, val, ord)
  #define ulatomic64_fetch_or_explicit(obj, val, ord)                          ::std::atomic_fetch_or_explicit(obj, val, ord)
  #define ulatomic64_fetch_xor_explicit(obj, val, ord)                         ::std::atomic_fetch_xor_explicit(obj, val, ord)
  #define ulatomic64_fetch_and_explicit(obj, val, ord)                         ::std::atomic_fetch_and_explicit(obj, val, ord)
#elif defined(ULATOMIC_API_MSVC)
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>

  typedef int ulatomic_memory_order_t;
  #define ulatomic_memory_order_relaxed 0
  #define ulatomic_memory_order_consume 1
  #define ulatomic_memory_order_acquire 2
  #define ulatomic_memory_order_release 3
  #define ulatomic_memory_order_acq_rel 4
  #define ulatomic_memory_order_seq_cst 5

  #if defined(__arm__) || defined(_M_ARM)
    #define __ULATOMIC_API_MSVC_ARM32
  #endif
  #if defined(__arm64) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
    #define __ULATOMIC_API_MSVC_ARM64
  #endif
  #if defined(__x86_64__) || defined(_M_X64)
    #define __ULATOMIC_API_MSVC_X64
  #endif
  #if defined(__i386) || defined(_M_IX86)
    #define __ULATOMIC_API_MSVC_X86
  #endif

  #if !defined(__ULATOMIC_API_MSVC_ARM32) && !defined(__ULATOMIC_API_MSVC_ARM64) && !defined(__ULATOMIC_API_MSVC_X64) && !defined(__ULATOMIC_API_MSVC_X86)
    #error "ulatomic.h: Visual Studio founded but unknown architecture"
  #endif

  #if _MSC_VER >= 1300
    #define ulatomic_thread_fence(ord) MemoryBarrier()
  #else
    ul_hapi void ulatomic_thread_fence(int ord) {
      (void)ord;
      __asm {
        lock add [esp], 0
      }
    }
  #endif

  #define ulatomic_signal_fence(ord) ((void)(ord))
  #define ulatomic_compiler_fence()  ulatomic_thread_fence(ulatomic_memory_order_seq_cst)

  typedef int ulatomic32_t;
  typedef int ulatomic32_raw_t;
  #define ULATOMIC32_INIT (0)
  #if defined(__ULATOMIC_API_MSVC_ARM32) || defined(__ULATOMIC_API_MSVC_ARM64)
    ul_hapi int ulatomic32_load_explicit(int* obj, int ord) {
      switch(ord) {
      case ulatomic_memory_order_relaxed:
        return ul_static_cast(int, _InterlockedCompareExchange_nf(ul_reinterpret_cast(long*, obj), 0, 0));
      case ulatomic_memory_order_consume:
      case ulatomic_memory_order_acquire:
        return ul_static_cast(int, _InterlockedCompareExchange_acq(ul_reinterpret_cast(long*, obj), 0, 0));
      case ulatomic_memory_order_release:
        return ul_static_cast(int, _InterlockedCompareExchange_rel(ul_reinterpret_cast(long*, obj), 0, 0));
      case ulatomic_memory_order_acq_rel:
      case ulatomic_memory_order_seq_cst:
      default:
        return ul_static_cast(int, _InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), 0, 0));
      }
    }
    ul_hapi int ulatomic32_exchange_explicit(int* obj, int val, int ord) {
      switch(ord) {
      case ulatomic_memory_order_relaxed:
        return ul_static_cast(int, _InterlockedExchange_nf(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_consume:
      case ulatomic_memory_order_acquire:
        return ul_static_cast(int, _InterlockedExchange_acq(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_release:
        return ul_static_cast(int, _InterlockedExchange_rel(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_acq_rel:
      case ulatomic_memory_order_seq_cst:
      default:
        return ul_static_cast(int, _InterlockedExchange(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }

    }
    ul_hapi int ulatomic32_compare_exchange_strong_explicit(int* obj, int* expected, int val, int ord) {
      int old = *expected;
      switch(ord) {
      case ulatomic_memory_order_relaxed:
        return (*expected = ul_static_cast(int, _InterlockedCompareExchange_nf(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val), ul_static_cast(long, old))) == old);
      case ulatomic_memory_order_consume:
      case ulatomic_memory_order_acquire:
        return (*expected = ul_static_cast(int, _InterlockedCompareExchange_acq(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val), ul_static_cast(long, old))) == old);
      case ulatomic_memory_order_release:
        return (*expected = ul_static_cast(int, _InterlockedCompareExchange_rel(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val), ul_static_cast(long, old))) == old);
      case ulatomic_memory_order_acq_rel:
      case ulatomic_memory_order_seq_cst:
      default:
        return (*expected = ul_static_cast(int, _InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val), ul_static_cast(long, old))) == old);
      }
    }
    ul_hapi int ulatomic32_fetch_add_explicit(int* obj, int val, int ord) {
      switch(ord) {
      case ulatomic_memory_order_relaxed:
        return ul_static_cast(int, _InterlockedExchangeAdd_nf(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_consume:
      case ulatomic_memory_order_acquire:
        return ul_static_cast(int, _InterlockedExchangeAdd_acq(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_release:
        return ul_static_cast(int, _InterlockedExchangeAdd_rel(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_acq_rel:
      case ulatomic_memory_order_seq_cst:
      default:
        return ul_static_cast(int, _InterlockedExchangeAdd(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }
    }
    ul_hapi int ulatomic32_fetch_sub_explicit(int* obj, int val, int ord) {
      switch(ord) {
      case ulatomic_memory_order_relaxed:
        return ul_static_cast(int, _InterlockedExchangeAdd_nf(ul_reinterpret_cast(long*, obj), -ul_static_cast(long, val)));
      case ulatomic_memory_order_consume:
      case ulatomic_memory_order_acquire:
        return ul_static_cast(int, _InterlockedExchangeAdd_acq(ul_reinterpret_cast(long*, obj), -ul_static_cast(long, val)));
      case ulatomic_memory_order_release:
        return ul_static_cast(int, _InterlockedExchangeAdd_rel(ul_reinterpret_cast(long*, obj), -ul_static_cast(long, val)));
      case ulatomic_memory_order_acq_rel:
      case ulatomic_memory_order_seq_cst:
      default:
        return ul_static_cast(int, _InterlockedExchangeAdd(ul_reinterpret_cast(long*, obj), -ul_static_cast(long, val)));
      }
    }
    ul_hapi int ulatomic32_fetch_or_explicit(int* obj, int val, int ord) {
      switch(ord) {
      case ulatomic_memory_order_relaxed:
        return ul_static_cast(int, _InterlockedOr_nf(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_consume:
      case ulatomic_memory_order_acquire:
        return ul_static_cast(int, _InterlockedOr_acq(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_release:
        return ul_static_cast(int, _InterlockedOr_rel(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_acq_rel:
      case ulatomic_memory_order_seq_cst:
      default:
        return ul_static_cast(int, _InterlockedOr(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }
    }
    ul_hapi int ulatomic32_fetch_xor_explicit(int* obj, int val, int ord) {
      switch(ord) {
      case ulatomic_memory_order_relaxed:
        return ul_static_cast(int, _InterlockedXor_nf(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_consume:
      case ulatomic_memory_order_acquire:
        return ul_static_cast(int, _InterlockedXor_acq(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_release:
        return ul_static_cast(int, _InterlockedXor_rel(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_acq_rel:
      case ulatomic_memory_order_seq_cst:
      default:
        return ul_static_cast(int, _InterlockedXor(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }
    }
    ul_hapi int ulatomic32_fetch_and_explicit(int* obj, int val, int ord) {
      switch(ord) {
      case ulatomic_memory_order_relaxed:
        return ul_static_cast(int, _InterlockedAnd_nf(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_consume:
      case ulatomic_memory_order_acquire:
        return ul_static_cast(int, _InterlockedAnd_acq(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_release:
        return ul_static_cast(int, _InterlockedAnd_rel(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      case ulatomic_memory_order_acq_rel:
      case ulatomic_memory_order_seq_cst:
      default:
        return ul_static_cast(int, _InterlockedAnd(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }
    }
  #else /* !(defined(__ULATOMIC_API_MSVC_ARM32) || defined(__ULATOMIC_API_MSVC_ARM64)) */
    ul_hapi int ulatomic32_exchange_explicit(int* obj, int val, int ord) {
      (void)ord; return ul_static_cast(int, InterlockedExchange(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
    }
    #if _MSC_VER >= 1300
      ul_hapi int ulatomic32_load_explicit(int* obj, int ord) {
        (void)ord; return ul_static_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), 0, 0));
      }
      ul_hapi int ulatomic32_compare_exchange_strong_explicit(int* obj, int* expected, int val, int ord) {
        int old = *expected; (void)ord;
        return (*expected = ul_static_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val), ul_static_cast(long, old))) == old);
      }
    #else
      ul_hapi int ulatomic32_load_explicit(int* obj, int ord) {
        (void)ord; return ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), 0, 0));
      }
      ul_hapi int ulatomic32_compare_exchange_strong_explicit(int* obj, int* expected, int val, int ord) {
        int old = *expected; (void)ord;
        return (*expected = ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), ul_reinterpret_cast(void*, val), ul_reinterpret_cast(void*, old))) == old);
      }
    #endif
    ul_hapi int ulatomic32_fetch_add_explicit(int* obj, int val, int ord) {
      (void)ord; return ul_static_cast(int, InterlockedExchangeAdd(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
    }
    ul_hapi int ulatomic32_fetch_sub_explicit(int* obj, int val, int ord) {
      (void)ord; return ul_static_cast(int, InterlockedExchangeAdd(ul_reinterpret_cast(long*, obj), -ul_static_cast(long, val)));
    }
    #if _MSC_VER >= 1500 || defined(_AMD64_) /* older MSVC don't obtain these APIs */
      ul_hapi int ulatomic32_fetch_or_explicit(int* obj, int val, int ord) {
        (void)ord; return ul_static_cast(int, _InterlockedOr(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }
      ul_hapi int ulatomic32_fetch_xor_explicit(int* obj, int val, int ord) {
        (void)ord; return ul_static_cast(int, _InterlockedXor(ul_reinterpret_cast(long*, obj), -ul_static_cast(long, val)));
      }
      ul_hapi int ulatomic32_fetch_and_explicit(int* obj, int val, int ord) {
        (void)ord; return ul_static_cast(int, _InterlockedAnd(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }
      #define ulatomic32_fetch_or(obj, val)  ulatomic32_fetch_or_explicit(obj, val, ulatomic_memory_order_seq_cst)
      #define ulatomic32_fetch_xor(obj, val) ulatomic32_fetch_xor_explicit(obj, val, ulatomic_memory_order_seq_cst)
      #define ulatomic32_fetch_and(obj, val) ulatomic32_fetch_and_explicit(obj, val, ulatomic_memory_order_seq_cst)
    #else
      #if _MSC_VER >= 1300
        ul_hapi int ulatomic32_fetch_or(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov | val);
          } while(InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), nv, ov) != ov);
          return ov;
        }
        ul_hapi int ulatomic32_fetch_xor(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov ^ val);
          } while(InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), nv, ov) != ov);
          return ov;
        }
        ul_hapi int ulatomic32_fetch_and(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov & val);
          } while(InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), nv, ov) != ov);
          return ov;
        }
      #else
        ul_hapi int ulatomic32_fetch_or(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov | val);
          } while(ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), ul_reinterpret_cast(void*, nv), ul_reinterpret_cast(void*, ov))) != ov);
          return ov;
        }
        ul_hapi int ulatomic32_fetch_xor(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov ^ val);
          } while(ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), ul_reinterpret_cast(void*, nv), ul_reinterpret_cast(void*, ov))) != ov);
          return ov;
        }
        ul_hapi int ulatomic32_fetch_and(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov & val);
          } while(ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), ul_reinterpret_cast(void*, nv), ul_reinterpret_cast(void*, ov))) != ov);
          return ov;
        }
      #endif
      #define ulatomic32_fetch_or_explicit(obj, val, ord)  ulatomic32_fetch_or(obj, val)
      #define ulatomic32_fetch_xor_explicit(obj, val, ord) ulatomic32_fetch_xor(obj, val)
      #define ulatomic32_fetch_and_explicit(obj, val, ord) ulatomic32_fetch_and(obj, val)
    #endif
  #endif
  #define ulatomic32_store_explicit(obj, val, ord)                           (void)(ulatomic32_exchange_explicit(obj, val, ord))
  #define ulatomic32_compare_exchange_weak_explicit(obj, expected, val, ord) ulatomic32_compare_exchange_strong_explicit(obj, expected, val, ord)

  #define ulatomic32_store(obj, val)                             ulatomic32_store_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_load(obj)                                   ulatomic32_load_explicit(obj, ulatomic_memory_order_seq_cst)
  #define ulatomic32_exchange(obj, val)                          ulatomic32_exchange_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_compare_exchange_strong(obj, expected, val) ulatomic32_compare_exchange_strong_explicit(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_compare_exchange_weak(obj, expected, val)   ulatomic32_compare_exchange_weak_explicit(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_fetch_add(obj, val)                         ulatomic32_fetch_add_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_fetch_sub(obj, val)                         ulatomic32_fetch_sub_explicit(obj, val, ulatomic_memory_order_seq_cst)

  #if defined(__ULATOMIC_API_MSVC_X64) || defined(__ULATOMIC_API_MSVC_ARM32) || defined(__ULATOMIC_API_MSVC_ARM64)
    typedef __int64 ulatomic64_t;
    typedef __int64 ulatomic64_raw_t;
    #define ULATOMIC64_INIT (0)
    #if defined(__ULATOMIC_API_MSVC_ARM32) || defined(__ULATOMIC_API_MSVC_ARM64)
      ul_hapi __int64 ulatomic64_load_explicit(__int64* obj, int ord) {
        switch(ord) {
        case ulatomic_memory_order_relaxed:
          return ul_static_cast(__int64, _InterlockedCompareExchange64_nf(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, 0), ul_static_cast(__int64, 0)));
        case ulatomic_memory_order_consume:
        case ulatomic_memory_order_acquire:
          return ul_static_cast(__int64, _InterlockedCompareExchange64_acq(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, 0), ul_static_cast(__int64, 0)));
        case ulatomic_memory_order_release:
          return ul_static_cast(__int64, _InterlockedCompareExchange64_rel(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, 0), ul_static_cast(__int64, 0)));
        case ulatomic_memory_order_acq_rel:
        case ulatomic_memory_order_seq_cst:
        default:
          return ul_static_cast(__int64, _InterlockedCompareExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, 0), ul_static_cast(__int64, 0)));
        }
      }
      ul_hapi __int64 ulatomic64_exchange_explicit(__int64* obj, __int64 val, int ord) {
        switch(ord) {
        case ulatomic_memory_order_relaxed:
          return ul_static_cast(__int64, _InterlockedExchange64_nf(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_consume:
        case ulatomic_memory_order_acquire:
          return ul_static_cast(__int64, _InterlockedExchange64_acq(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_release:
          return ul_static_cast(__int64, _InterlockedExchange64_rel(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_acq_rel:
        case ulatomic_memory_order_seq_cst:
        default:
          return ul_static_cast(__int64, _InterlockedExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        }

      }
      ul_hapi __int64 ulatomic64_compare_exchange_strong_explicit(__int64* obj, __int64* expected, __int64 val, int ord) {
        __int64 old = *expected;
        switch(ord) {
        case ulatomic_memory_order_relaxed:
          return (*expected = ul_static_cast(__int64, _InterlockedCompareExchange64_nf(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val), ul_static_cast(__int64, old)))) == old;
        case ulatomic_memory_order_consume:
        case ulatomic_memory_order_acquire:
          return (*expected = ul_static_cast(__int64, _InterlockedCompareExchange64_acq(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val), ul_static_cast(__int64, old)))) == old;
        case ulatomic_memory_order_release:
          return (*expected = ul_static_cast(__int64, _InterlockedCompareExchange64_rel(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val), ul_static_cast(__int64, old)))) == old;
        case ulatomic_memory_order_acq_rel:
        case ulatomic_memory_order_seq_cst:
        default:
          return (*expected = ul_static_cast(__int64, _InterlockedCompareExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val), ul_static_cast(__int64, old)))) == old;
        }
      }
      ul_hapi __int64 ulatomic64_fetch_add_explicit(__int64* obj, __int64 val, int ord) {
        switch(ord) {
        case ulatomic_memory_order_relaxed:
          return ul_static_cast(__int64, _InterlockedExchangeAdd64_nf(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_consume:
        case ulatomic_memory_order_acquire:
          return ul_static_cast(__int64, _InterlockedExchangeAdd64_acq(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_release:
          return ul_static_cast(__int64, _InterlockedExchangeAdd64_rel(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_acq_rel:
        case ulatomic_memory_order_seq_cst:
        default:
          return ul_static_cast(__int64, _InterlockedExchangeAdd64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        }
      }
      ul_hapi __int64 ulatomic64_fetch_sub_explicit(__int64* obj, __int64 val, int ord) {
        switch(ord) {
        case ulatomic_memory_order_relaxed:
          return ul_static_cast(__int64, _InterlockedExchangeAdd64_nf(ul_reinterpret_cast(__int64*, obj), -ul_static_cast(__int64, val)));
        case ulatomic_memory_order_consume:
        case ulatomic_memory_order_acquire:
          return ul_static_cast(__int64, _InterlockedExchangeAdd64_acq(ul_reinterpret_cast(__int64*, obj), -ul_static_cast(__int64, val)));
        case ulatomic_memory_order_release:
          return ul_static_cast(__int64, _InterlockedExchangeAdd64_rel(ul_reinterpret_cast(__int64*, obj), -ul_static_cast(__int64, val)));
        case ulatomic_memory_order_acq_rel:
        case ulatomic_memory_order_seq_cst:
        default:
          return ul_static_cast(__int64, _InterlockedExchangeAdd64(ul_reinterpret_cast(__int64*, obj), -ul_static_cast(__int64, val)));
        }
      }
      ul_hapi __int64 ulatomic64_fetch_or_explicit(__int64* obj, __int64 val, int ord) {
        switch(ord) {
        case ulatomic_memory_order_relaxed:
          return ul_static_cast(__int64, _InterlockedOr64_nf(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_consume:
        case ulatomic_memory_order_acquire:
          return ul_static_cast(__int64, _InterlockedOr64_acq(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_release:
          return ul_static_cast(__int64, _InterlockedOr64_rel(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_acq_rel:
        case ulatomic_memory_order_seq_cst:
        default:
          return ul_static_cast(__int64, _InterlockedOr64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        }
      }
      ul_hapi __int64 ulatomic64_fetch_xor_explicit(__int64* obj, __int64 val, int ord) {
        switch(ord) {
        case ulatomic_memory_order_relaxed:
          return ul_static_cast(__int64, _InterlockedXor64_nf(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_consume:
        case ulatomic_memory_order_acquire:
          return ul_static_cast(__int64, _InterlockedXor64_acq(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_release:
          return ul_static_cast(__int64, _InterlockedXor64_rel(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_acq_rel:
        case ulatomic_memory_order_seq_cst:
        default:
          return ul_static_cast(__int64, _InterlockedXor64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        }
      }
      ul_hapi __int64 ulatomic64_fetch_and_explicit(__int64* obj, __int64 val, int ord) {
        switch(ord) {
        case ulatomic_memory_order_relaxed:
          return ul_static_cast(__int64, _InterlockedAnd64_nf(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_consume:
        case ulatomic_memory_order_acquire:
          return ul_static_cast(__int64, _InterlockedAnd64_acq(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_release:
          return ul_static_cast(__int64, _InterlockedAnd64_rel(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        case ulatomic_memory_order_acq_rel:
        case ulatomic_memory_order_seq_cst:
        default:
          return ul_static_cast(__int64, _InterlockedAnd64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
        }
      }
    #else /* !(defined(__ULATOMIC_API_MSVC_ARM32) || defined(__ULATOMIC_API_MSVC_ARM64)) */
      ul_hapi __int64 ulatomic64_load_explicit(__int64* obj, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedCompareExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, 0), ul_static_cast(__int64, 0)));
      }
      ul_hapi __int64 ulatomic64_exchange_explicit(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic64_compare_exchange_strong_explicit(__int64* obj, __int64* expected, __int64 val, int ord) {
        __int64 old = *expected; (void)ord;
        return (*expected = ul_static_cast(__int64, InterlockedCompareExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val), ul_static_cast(__int64, old)))) == old;
      }
      ul_hapi __int64 ulatomic64_fetch_add_explicit(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedExchangeAdd64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic64_fetch_sub_explicit(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedExchangeAdd64(ul_reinterpret_cast(__int64*, obj), -ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic64_fetch_or_explicit(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedOr64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic64_fetch_xor_explicit(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedXor64(ul_reinterpret_cast(__int64*, obj), -ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic64_fetch_and_explicit(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedAnd64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
      }
    #endif
    #define ulatomic64_store_explicit(obj, val, ord)                           (void)(ulatomic64_exchange_explicit(obj, val, ord))
    #define ulatomic64_compare_exchange_weak_explicit(obj, expected, val, ord) ulatomic64_compare_exchange_strong_explicit(obj, expected, val, ord)

    #define ulatomic64_store(obj, val)                             ulatomic64_store_explicit(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic64_load(obj)                                   ulatomic64_load_explicit(obj, ulatomic_memory_order_seq_cst)
    #define ulatomic64_exchange(obj, val)                          ulatomic64_exchange_explicit(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic64_compare_exchange_strong(obj, expected, val) ulatomic64_compare_exchange_strong_explicit(obj, expected, val, ulatomic_memory_order_seq_cst)
    #define ulatomic64_compare_exchange_weak(obj, expected, val)   ulatomic64_compare_exchange_weak_explicit(obj, expected, val, ulatomic_memory_order_seq_cst)
    #define ulatomic64_fetch_add(obj, val)                         ulatomic64_fetch_add_explicit(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic64_fetch_sub(obj, val)                         ulatomic64_fetch_sub_explicit(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic64_fetch_or(obj, val)                          ulatomic64_fetch_or_explicit(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic64_fetch_xor(obj, val)                         ulatomic64_fetch_xor_explicit(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic64_fetch_and(obj, val)                         ulatomic64_fetch_and_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #endif
#elif defined(ULATOMIC_API_GNUC)
  #include <limits.h>
  #include <stdint.h>

  typedef int ulatomic_memory_order_t;
  #define ulatomic_memory_order_relaxed __ATOMIC_RELAXED
  #define ulatomic_memory_order_consume __ATOMIC_CONSUME
  #define ulatomic_memory_order_acquire __ATOMIC_ACQUIRE
  #define ulatomic_memory_order_release __ATOMIC_RELEASE
  #define ulatomic_memory_order_acq_rel __ATOMIC_ACQ_REL
  #define ulatomic_memory_order_seq_cst __ATOMIC_SEQ_CST

  #define ulatomic_thread_fence(ord) __atomic_thread_fence(ord)
  #define ulatomic_signal_fence(ord) __atomic_signal_fence(ord)
  #define ulatomic_compiler_fence()  __asm__ __volatile__("":::"memory")

  typedef int32_t ulatomic32_t;
  typedef int32_t ulatomic32_raw_t;
  #define ULATOMIC32_INIT (0)
  #define ulatomic32_store_explicit(obj, val, ord)                             __atomic_store_n(obj, val, ord)
  #define ulatomic32_load_explicit(obj, ord)                                   __atomic_load_n(obj, ord)
  #define ulatomic32_exchange_explicit(obj, val, ord)                          __atomic_exchange_n(obj, val, ord)
  #define ulatomic32_compare_exchange_strong_explicit(obj, expected, val, ord) \
    ul_static_cast(int, __atomic_compare_exchange_n(obj, expected, val, 0, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic32_compare_exchange_weak_explicit(obj, expected, val, ord) \
    ul_static_cast(int, __atomic_compare_exchange_n(obj, expected, val, 1, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic32_fetch_add_explicit(obj, val, ord)                         __atomic_fetch_add(obj, val, ord)
  #define ulatomic32_fetch_sub_explicit(obj, val, ord)                         __atomic_fetch_sub(obj, val, ord)
  #define ulatomic32_fetch_or_explicit(obj, val, ord)                          __atomic_fetch_or(obj, val, ord)
  #define ulatomic32_fetch_xor_explicit(obj, val, ord)                         __atomic_fetch_xor(obj, val, ord)
  #define ulatomic32_fetch_and_explicit(obj, val, ord)                         __atomic_fetch_and(obj, val, ord)
  #define ulatomic32_store(obj, val)                             ulatomic32_store_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_load(obj)                                   ulatomic32_load_explicit(obj, ulatomic_memory_order_seq_cst)
  #define ulatomic32_exchange(obj, val)                          ulatomic32_exchange_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_compare_exchange_strong(obj, expected, val) ulatomic32_compare_exchange_strong_explicit(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_compare_exchange_weak(obj, expected, val)   ulatomic32_compare_exchange_weak_explicit(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_fetch_add(obj, val)                         ulatomic32_fetch_add_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_fetch_sub(obj, val)                         ulatomic32_fetch_sub_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_fetch_or(obj, val)                          ulatomic32_fetch_or_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_fetch_xor(obj, val)                         ulatomic32_fetch_xor_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic32_fetch_and(obj, val)                         ulatomic32_fetch_and_explicit(obj, val, ulatomic_memory_order_seq_cst)

  typedef int64_t ulatomic64_t;
  typedef int64_t ulatomic64_raw_t;
  #define ULATOMIC64_INIT (0)
  #define ulatomic64_store_explicit(obj, val, ord)                             __atomic_store_n(obj, val, ord)
  #define ulatomic64_load_explicit(obj, ord)                                   __atomic_load_n(obj, ord)
  #define ulatomic64_exchange_explicit(obj, val, ord)                          __atomic_exchange_n(obj, val, ord)
  #define ulatomic64_compare_exchange_strong_explicit(obj, expected, val, ord) \
    ul_static_cast(int, __atomic_compare_exchange_n(obj, expected, val, 0, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic64_compare_exchange_weak_explicit(obj, expected, val, ord) \
    ul_static_cast(int, __atomic_compare_exchange_n(obj, expected, val, 1, ord, _ulatomic_memory_order_of_fail(ord)))
  #define ulatomic64_fetch_add_explicit(obj, val, ord)                         __atomic_fetch_add(obj, val, ord)
  #define ulatomic64_fetch_sub_explicit(obj, val, ord)                         __atomic_fetch_sub(obj, val, ord)
  #define ulatomic64_fetch_or_explicit(obj, val, ord)                          __atomic_fetch_or(obj, val, ord)
  #define ulatomic64_fetch_xor_explicit(obj, val, ord)                         __atomic_fetch_xor(obj, val, ord)
  #define ulatomic64_fetch_and_explicit(obj, val, ord)                         __atomic_fetch_and(obj, val, ord)
  #define ulatomic64_store(obj, val)                             ulatomic64_store_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic64_load(obj)                                   ulatomic64_load_explicit(obj, ulatomic_memory_order_seq_cst)
  #define ulatomic64_exchange(obj, val)                          ulatomic64_exchange_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic64_compare_exchange_strong(obj, expected, val) ulatomic64_compare_exchange_strong_explicit(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic64_compare_exchange_weak(obj, expected, val)   ulatomic64_compare_exchange_weak_explicit(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic64_fetch_add(obj, val)                         ulatomic64_fetch_add_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic64_fetch_sub(obj, val)                         ulatomic64_fetch_sub_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic64_fetch_or(obj, val)                          ulatomic64_fetch_or_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic64_fetch_xor(obj, val)                         ulatomic64_fetch_xor_explicit(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic64_fetch_and(obj, val)                         ulatomic64_fetch_and_explicit(obj, val, ulatomic_memory_order_seq_cst)

  typedef int ulatomic_flag_t;
  #define ULATOMIC_FLAG_INIT (0)
  #define ulatomic_flag_test_and_set_explicit(obj, ord) ul_static_cast(int, __atomic_test_and_set(obj, ord))
  #define ulatomic_flag_clear_explicit(obj, ord)        __atomic_clear(obj, ord)
  #define ulatomic_flag_test_and_set(obj)               ul_static_cast(int, ulatomic_flag_test_and_set_explicit(obj, ulatomic_memory_order_seq_cst))
  #define ulatomic_flag_clear(obj)                      ulatomic_flag_clear_explicit(obj, ulatomic_memory_order_seq_cst)
#elif defined(ULATOMIC_NEEDED)
  #include <limits.h>

  #if INT_MAX == 0x7FFFFFFF
    typedef int _ulatomic_i32_t;
  #elif LONG_MAX == 0x7FFFFFFF
    typedef long _ulatomic_i32_t;
  #else
    #error "ulatomic.h: neither `int` nor `long` is 32-bit"
  #endif

  #if defined(LLONG_MAX) && (LLONG_MAX >> 62) >= 1
    #if LLONG_MAX == 0x7FFFFFFFFFFFFFFF
      typedef long long _ulatomic_i64_t;
      #define _ULATOMIC_I64_DEFINED
    #endif
  #endif
  #if !defined(_ULATOMIC_I64_DEFINED) && defined(LONG_MAX) && (LONG_MAX >> 62) >= 1
    #if LONG_MAX == 0x7FFFFFFFFFFFFFFF
      typedef long _ulatomic_i64_t;
      #define _ULATOMIC_I64_DEFINED
    #endif
  #endif
  #if !defined(_ULATOMIC_I64_DEFINED) && defined(_WIN32)
    typedef __int64 _ulatomic_i64_t;
    #define _ULATOMIC_I64_DEFINED
  #endif

  #define ulatomic_memory_order_relaxed 0
  #define ulatomic_memory_order_consume 1
  #define ulatomic_memory_order_acquire 2
  #define ulatomic_memory_order_release 3
  #define ulatomic_memory_order_acq_rel 4
  #define ulatomic_memory_order_seq_cst 5

  #define ulatomic_thread_fence(ord) ((void)0)
  #define ulatomic_signal_fence(ord) ((void)0)

  typedef _ulatomic_i32_t ulatomic32_t;
  typedef _ulatomic_i32_t ulatomic32_raw_t;
  #define ULATOMIC32_INIT (0)
  #define ulatomic32_store(obj, val) (void)(*(obj) = (val))
  #define ulatomic32_load(obj) (*(obj))
  ul_hapi _ulatomic32_i32_t ulatomic_exchange(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj = val; return old; }
  ul_hapi int ulatomic32_compare_exchange_strong(ulatomic32_t* obj, _ulatomic_i32_t* expected, _ulatomic_i32_t val) {
    if(*obj == *expected) { *obj = val; return 1; }
    else { *expected = *obj; return 0; }
  }
  ul_hapi int ulatomic32_compare_exchange_weak(ulatomic32_t* obj, _ulatomic_i32_t* expected, _ulatomic_i32_t val) {
    if(*obj == *expected) { *obj = val; return 1; }
    else { *expected = *obj; return 0; }
  }
  ul_hapi _ulatomic32_i32_t ulatomic_fetch_add(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj += val; return old; }
  ul_hapi _ulatomic32_i32_t ulatomic_fetch_sub(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj -= val; return old; }
  ul_hapi _ulatomic32_i32_t ulatomic_fetch_or(ulatomic32_t* obj, _ulatomic_i32_t val)  { _ulatomic_i32_t old = *obj; *obj |= val; return old; }
  ul_hapi _ulatomic32_i32_t ulatomic_fetch_xor(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj ^= val; return old; }
  ul_hapi _ulatomic32_i32_t ulatomic_fetch_and(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj &= val; return old; }
  #define ulatomic32_store_explicit(obj, val, ord)                             ulatomic32_store(obj, val)
  #define ulatomic32_load_explicit(obj, ord)                                   ulatomic32_load(obj)
  #define ulatomic32_exchange_explicit(obj, val, ord)                          ulatomic32_exchange(obj, val)
  #define ulatomic32_compare_exchange_strong_explicit(obj, expected, val, ord) ulatomic32_compare_exchange_strong(obj, expected, val)
  #define ulatomic32_compare_exchange_weak_explicit(obj, expected, val, ord)   ulatomic32_compare_exchange_weak(obj, expected, val)
  #define ulatomic32_fetch_add_explicit(obj, val, ord)                         ulatomic32_fetch_add(obj, val)
  #define ulatomic32_fetch_sub_explicit(obj, val, ord)                         ulatomic32_fetch_sub(obj, val)
  #define ulatomic32_fetch_or_explicit(obj, val, ord)                          ulatomic32_fetch_or(obj, val)
  #define ulatomic32_fetch_xor_explicit(obj, val, ord)                         ulatomic32_fetch_xor(obj, val)
  #define ulatomic32_fetch_and_explicit(obj, val, ord)                         ulatomic32_fetch_and(obj, val)

  #ifdef _ULATOMIC_I64_DEFINED
    typedef _ulatomic_i64_t ulatomic64_t;
    typedef _ulatomic_i64_t ulatomic64_raw_t;
    #define ULATOMIC64_INIT (0)
    #define ulatomic64_store(obj, val) (void)(*(obj) = (val))
    #define ulatomic64_load(obj) (*(obj))
    ul_hapi _ulatomic64_i64_t ulatomic_exchange(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj = val; return old; }
    ul_hapi int ulatomic64_compare_exchange_strong(ulatomic64_t* obj, _ulatomic_i64_t* expected, _ulatomic_i64_t val) {
      if(*obj == *expected) { *obj = val; return 1; }
      else { *expected = *obj; return 0; }
    }
    ul_hapi int ulatomic64_compare_exchange_weak(ulatomic64_t* obj, _ulatomic_i64_t* expected, _ulatomic_i64_t val) {
      if(*obj == *expected) { *obj = val; return 1; }
      else { *expected = *obj; return 0; }
    }
    ul_hapi _ulatomic64_i64_t ulatomic_fetch_add(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj += val; return old; }
    ul_hapi _ulatomic64_i64_t ulatomic_fetch_sub(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj -= val; return old; }
    ul_hapi _ulatomic64_i64_t ulatomic_fetch_or(ulatomic64_t* obj, _ulatomic_i64_t val)  { _ulatomic_i64_t old = *obj; *obj |= val; return old; }
    ul_hapi _ulatomic64_i64_t ulatomic_fetch_xor(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj ^= val; return old; }
    ul_hapi _ulatomic64_i64_t ulatomic_fetch_and(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj &= val; return old; }
    #define ulatomic64_store_explicit(obj, val, ord)                             ulatomic64_store(obj, val)
    #define ulatomic64_load_explicit(obj, ord)                                   ulatomic64_load(obj)
    #define ulatomic64_exchange_explicit(obj, val, ord)                          ulatomic64_exchange(obj, val)
    #define ulatomic64_compare_exchange_strong_explicit(obj, expected, val, ord) ulatomic64_compare_exchange_strong(obj, expected, val)
    #define ulatomic64_compare_exchange_weak_explicit(obj, expected, val, ord)   ulatomic64_compare_exchange_weak(obj, expected, val)
    #define ulatomic64_fetch_add_explicit(obj, val, ord)                         ulatomic64_fetch_add(obj, val)
    #define ulatomic64_fetch_sub_explicit(obj, val, ord)                         ulatomic64_fetch_sub(obj, val)
    #define ulatomic64_fetch_or_explicit(obj, val, ord)                          ulatomic64_fetch_or(obj, val)
    #define ulatomic64_fetch_xor_explicit(obj, val, ord)                         ulatomic64_fetch_xor(obj, val)
    #define ulatomic64_fetch_and_explicit(obj, val, ord)                         ulatomic64_fetch_and(obj, val)
  #endif

  #undef _ULATOMIC_I64_DEFINED
#endif

#if !defined(ULATOMIC_FLAG_INIT) && defined(ULATOMIC32_INIT)
  typedef ulatomic32_t ulatomic_flag_t;
  #define ULATOMIC_FLAG_INIT ULATOMIC32_INIT
  #define ulatomic_flag_test_and_set(obj)               ulatomic32_exchange(obj, 1)
  #define ulatomic_flag_clear(obj)                      ulatomic32_store(obj, 0)
  #define ulatomic_flag_test_and_set_explicit(obj, ord) ulatomic32_exchange_explicit(obj, 1, ord)
  #define ulatomic_flag_clear_explicit(obj, ord)        ulatomic32_store_explicit(obj, 0, ord)
#endif /* ULATOMIC_FLAG_INIT */

/* ulatomic_yield */
#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #if _WIN32_WINNT >= 0x0502
    #define ulatomic_yield() ((void)(SwitchToThread() ? 0u : SleepEx(0, FALSE)))
  #else
    #define ulatomic_yield() ((void)SleepEx(0, FALSE))
  #endif
#endif
#ifndef ulatomic_yield
  #if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
    #include <unistd.h>
    #if 0 /* defined(_POSIX_PRIORITY_SCHEDULING) || (_POSIX_C_SOURCE+0) >= 200809L */
      #define ulatomic_yield() ((void)sched_yield())
    #elif defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE+0) >= 199309L
      #include <time.h>
      ul_hapi void ulatomic_yield() {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 1000;
        nanosleep(&ts, NULL);
      }
      #define ulatomic_yield() (ulatomic_yield)()
    #elif defined(_BSD_SOURCE) || (defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE+0) >= 500)
      #define ulatomic_yield() ((void)usleep(1))
    #endif
  #endif
#endif
#ifndef ulatomic_yield
  #define ulatomic_yield() ((void)0)
#endif

ul_hapi void ulatomic_pause(void) {
#if defined(_MSC_VER)
  #if defined(_M_AMD64) || defined(_M_IX86)
    __asm pause;
  #elif defined(_M_ARM)
    __asm yield;
  #elif defined(_M_ARM64)
    __asm isb SY;
  #endif
#elif defined(__GNUC__)
  #if (defined(__i386__) || defined(__x86_64__))
    __asm__ __volatile__("pause":::"memory");
  #elif (defined(__ARM_ARCH) && __ARM_ARCH >= 8) || defined(__ARM_ARCH_8A__)
    __asm__ __volatile__("yield":::"memory");
  #elif defined(__aarch64__)
    __asm__ __volatile__("isb SY":::"memory");
  #endif
#endif
}

/* ulatomic32_wait / ulatomic32_notify_one / ulatomic32_notify_all */
#ifdef ULATOMIC32_INIT
  #undef _ULATOMIC32_WAIT_DEFINED

  /* C++20 */
  #ifdef ULATOMIC_API_CXX20
    ul_hapi void ulatomic32_wait(ulatomic32_t* obj, ulatomic32_raw_t expected) {
      obj->wait(expected);
    }
    ul_hapi void ulatomic32_notify_one(ulatomic32_t* obj) {
      obj->notify_one();
    }
    ul_hapi void ulatomic32_notify_all(ulatomic32_t* obj) {
      obj->notify_all();
    }
    #define _ULATOMIC32_WAIT_DEFINED
  #endif

  /* Windows 8 */
  #if !defined(_ULATOMIC32_WAIT_DEFINED) && defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #ifdef _MSC_VER
      #pragma comment(lib, "Synchronization.lib")
    #endif
    #if _WIN32_WINNT >= 0x0602
      ul_hapi void ulatomic32_wait(ulatomic32_t* obj, ulatomic32_raw_t expected) {
        WaitOnAddress(ul_reinterpret_cast(void*, obj), &expected, 4, INFINITE);
      }
      ul_hapi void ulatomic32_notify_one(ulatomic32_t* obj) {
        WakeByAddressSingle(ul_reinterpret_cast(void*, obj));
      }
      ul_hapi void ulatomic32_notify_all(ulatomic32_t* obj) {
        WakeByAddressAll(ul_reinterpret_cast(void*, obj));
      }
      #define _ULATOMIC32_WAIT_DEFINED
    #endif
  #endif

  /* Linux 2.6.0: use futex + table */
  #if !defined(_ULATOMIC32_WAIT_DEFINED) && defined(__linux__)
    #include <linux/version.h>
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
      #include <unistd.h>
      #include <sys/syscall.h>
      #include <linux/futex.h>
      #include <stdint.h>
      #include <limits.h>

      ul_hapi void _ulatomic32_wait(ulatomic32_t* obj, ulatomic32_raw_t expected) {
        syscall(SYS_futex, obj, FUTEX_WAIT_PRIVATE, expected, NULL, NULL, 0);
      }
      ul_hapi void _ulatomic32_notify_one(ulatomic32_t* obj) {
        syscall(SYS_futex, obj, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
      }
      ul_hapi void _ulatomic32_notify_all(ulatomic32_t* obj) {
        syscall(SYS_futex, obj, FUTEX_WAKE_PRIVATE, INT_MAX, NULL, NULL, 0);
      }

      struct alignas(64) _ulatomic32_table_t {
        ulatomic32_t waiters;
        ulatomic32_t version;
      };
      ul_hapi struct _ulatomic32_table_t* _ulatomic32_table(volatile const void* p) {
        static _ulatomic32_table_t table[256] = { };
        return table + (((uintptr_t)p >> 2) & 255);
      }
      ul_hapi void ulatomic32_wait(ulatomic32_t* obj, ulatomic32_raw_t expected) {
        int i;
        if(ulatomic32_load(obj) != expected) return;
        for(i = 0; i < 16; ++i) {
          if(ulatomic32_load(obj) != expected) return;
          ulatomic_pause();
        }
        do {
          struct _ulatomic32_table_t* c = _ulatomic32_table(obj);
          ulatomic32_raw_t version;
          ulatomic32_store(&c->waiters, 1);
          version = ulatomic32_load(&c->version);
          if(ulatomic32_load(obj) != expected) return;
          _ulatomic32_wait(&c->version, version);
        } while(0);
      }
      ul_hapi void ulatomic32_notify_all(ulatomic32_t* obj) {
        struct _ulatomic32_table_t* c = _ulatomic32_table(obj);
        ulatomic32_fetch_add_explicit(&c->version, 1, ulatomic_memory_order_seq_cst);
        if(ulatomic32_exchange_explicit(&c->waiters, 0, ulatomic_memory_order_relaxed))
          _ulatomic32_notify_all(&c->version);
      }
      ul_hapi void ulatomic32_notify_one(ulatomic32_t* obj) {
        ulatomic32_notify_all(obj);
      }
      #define _ULATOMIC32_WAIT_DEFINED
    #endif
  #endif

  #ifndef _ULATOMIC32_WAIT_DEFINED
    ul_hapi void ulatomic32_wait(ulatomic32_t* obj, ulatomic32_raw_t expected) {
      int i;
      for(i = 0; i < 16; ++i) {
        if(ulatomic32_load(obj) != expected) return;
        ulatomic_pause();
      }
      while(ulatomic32_load(obj) == expected) ulatomic_yield();
    }
    ul_hapi void ulatomic32_notify_one(ulatomic32_t* obj) { (void)obj; }
    ul_hapi void ulatomic32_notify_all(ulatomic32_t* obj) { (void)obj; }
    #define _ULATOMIC32_WAIT_DEFINED
  #endif

  #undef _ULATOMIC32_WAIT_DEFINED
#endif

/* ulatomic64_wait / ulatomic64_notify_one / ulatomic64_notify_all */
#ifdef ULATOMIC64_INIT
  #undef _ULATOMIC64_WAIT_DEFINED

  /* C++20 */
  #ifdef ULATOMIC_API_CXX20
    ul_hapi void ulatomic64_wait(ulatomic64_t* obj, ulatomic64_raw_t expected) {
      obj->wait(expected);
    }
    ul_hapi void ulatomic64_notify_one(ulatomic64_t* obj) {
      obj->notify_one();
    }
    ul_hapi void ulatomic64_notify_all(ulatomic64_t* obj) {
      obj->notify_all();
    }
    #define _ULATOMIC64_WAIT_DEFINED
  #endif

  /* Windows 8 */
  #if !defined(_ULATOMIC64_WAIT_DEFINED) && defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #ifdef _MSC_VER
      #pragma comment(lib, "Synchronization.lib")
    #endif
    #if _WIN32_WINNT >= 0x0602
      ul_hapi void ulatomic64_wait(ulatomic64_t* obj, ulatomic64_raw_t expected) {
        WaitOnAddress(ul_reinterpret_cast(void*, obj), &expected, 8, INFINITE);
      }
      ul_hapi void ulatomic64_notify_one(ulatomic64_t* obj) {
        WakeByAddressSingle(ul_reinterpret_cast(void*, obj));
      }
      ul_hapi void ulatomic64_notify_all(ulatomic64_t* obj) {
        WakeByAddressAll(ul_reinterpret_cast(void*, obj));
      }
      #define _ULATOMIC64_WAIT_DEFINED
    #endif
  #endif

  /* Linux 2.6.0: use futex + table */
  #if !defined(_ULATOMIC64_WAIT_DEFINED) && defined(__linux__)
    #include <linux/version.h>
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
      #include <unistd.h>
      #include <sys/syscall.h>
      #include <linux/futex.h>
      #include <stdint.h>
      #include <limits.h>

      ul_hapi void ulatomic64_wait(ulatomic64_t* obj, ulatomic64_raw_t expected) {
        int i;
        if(ulatomic64_load(obj) != expected) return;
        for(i = 0; i < 16; ++i) {
          if(ulatomic64_load(obj) != expected) return;
          ulatomic_pause();
        }
        do {
          struct _ulatomic32_table_t* c = _ulatomic32_table(obj);
          ulatomic32_raw_t version;
          ulatomic32_store(&c->waiters, 1);
          version = ulatomic32_load(&c->version);
          if(ulatomic64_load(obj) != expected) return;
          _ulatomic32_wait(&c->version, version);
        } while(0);
      }
      ul_hapi void ulatomic64_notify_all(ulatomic64_t* obj) {
        struct _ulatomic32_table_t* c = _ulatomic32_table(obj);
        ulatomic32_fetch_add_explicit(&c->version, 1, ulatomic_memory_order_seq_cst);
        if(ulatomic32_exchange_explicit(&c->waiters, 0, ulatomic_memory_order_relaxed))
          _ulatomic32_notify_all(&c->version);
      }
      ul_hapi void ulatomic64_notify_one(ulatomic64_t* obj) {
        ulatomic64_notify_all(obj);
      }
      #define _ULATOMIC64_WAIT_DEFINED
    #endif
  #endif

  #ifndef _ULATOMIC64_WAIT_DEFINED
    ul_hapi void ulatomic64_wait(ulatomic64_t* obj, ulatomic64_raw_t expected) {
      int i;
      for(i = 0; i < 16; ++i) {
        if(ulatomic64_load(obj) != expected) return;
        ulatomic_pause();
      }
      while(ulatomic64_load(obj) == expected) ulatomic_yield();
    }
    ul_hapi void ulatomic64_notify_one(ulatomic64_t* obj) { (void)obj; }
    ul_hapi void ulatomic64_notify_all(ulatomic64_t* obj) { (void)obj; }
    #define _ULATOMIC64_WAIT_DEFINED
  #endif

  #undef _ULATOMIC64_WAIT_DEFINED
#endif

#ifndef UL_HAS_STDINT_H
  #if defined(__GLIBC__) && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 1))
    #if defined(__GNUC__) || ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 5)))
      #define UL_HAS_STDINT_H
    #endif
  #endif
  #if defined(__MINGW32__) && (__MINGW32_MAJOR_VERSION > 2 || \
      (__MINGW32_MAJOR_VERSION == 2 && __MINGW32_MINOR_VERSION >= 0))
    #define UL_HAS_STDINT_H
  #endif
  #if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
    #include <unistd.h>
    #if defined(_POSIX_VERSION) && ((_POSIX_VERSION+0) >= 200100L)
      #define UL_HAS_STDINT_H
    #endif
  #endif
  #if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
      || (defined(__cplusplus) && __cplusplus >= 201103L)
    #define UL_HAS_STDINT_H
  #endif
  #if (defined(_MSC_VER) && _MSC_VER >= 1600) || (defined(__CYGWIN__) && defined(_STDINT_H))
    #define UL_HAS_STDINT_H
  #endif
  #if defined(__has_include)
    #if __has_include(<stdint.h>)
      #define UL_HAS_STDINT_H
    #endif
  #endif
#endif /* UL_HAS_STDINT_H */
#ifdef UL_HAS_STDINT_H
  #include <stdint.h>
  #if INTPTR_MAX == INT64_MAX
    #define _ULATOMIC_INTPTR_64BIT
  #elif INTPTR_MAX == INT32_MAX
    #define _ULATOMIC_INTPTR_32BIT
  #endif
#elif defined(INTPTR_MAX)
  #if INTPTR_MAX == INT64_MAX
    #define _ULATOMIC_INTPTR_64BIT
  #elif INTPTR_MAX == INT32_MAX
    #define _ULATOMIC_INTPTR_32BIT
  #endif
#else
  #include <limits.h>
  #if defined(_WIN32) || defined(_WIN64)
    #if _WIN64
      #define _ULATOMIC_INTPTR_64BIT
    #else
      #define _ULATOMIC_INTPTR_32BIT
    #endif
  #elif defined(__SIZEOF_POINTER__)
    #if __SIZEOF_POINTER__ == 8
      #define _ULATOMIC_INTPTR_64BIT
    #elif __SIZEOF_POINTER__ == 4
      #define _ULATOMIC_INTPTR_32BIT
    #endif
  #endif
  #if !defined(_ULATOMIC_INTPTR_32BIT) && !defined(_ULATOMIC_INTPTR_64BIT)
    #ifdef LLONG_MAX
      #define _ULATOMIC_INTPTR_64BIT
    #endif
  #endif
  #if !defined(_ULATOMIC_INTPTR_32BIT) && !defined(_ULATOMIC_INTPTR_64BIT)
    #if (LONG_MAX >> 61) == 3
      #if LONG_MAX == 0x7FFFFFFFFFFFFFFF
        #define _ULATOMIC_INTPTR_64BIT
      #endif
    #endif
  #endif
  #if !defined(_ULATOMIC_INTPTR_32BIT) && !defined(_ULATOMIC_INTPTR_64BIT)
    #if LONG_MAX == 0x7FFFFFFF
      #define _ULATOMIC_INTPTR_32BIT
    #endif
  #endif
#endif

/* ulatomiciptr_t */
#if defined(_ULATOMIC_INTPTR_64BIT) && defined(ULATOMIC64_INIT)
  typedef ulatomic64_t ulatomiciptr_t;
  typedef ulatomic64_raw_t ulatomiciptr_raw_t;
  #define ULATOMICIPTR_INIT ULATOMIC64_INIT
  #define ulatomiciptr_store(obj, val)                             ulatomic64_store(obj, val)
  #define ulatomiciptr_load(obj)                                   ulatomic64_load(obj)
  #define ulatomiciptr_exchange(obj, val)                          ulatomic64_exchange(obj, val)
  #define ulatomiciptr_compare_exchange_strong(obj, expected, val) ulatomic64_compare_exchange_strong(obj, expected, val)
  #define ulatomiciptr_compare_exchange_weak(obj, expected, val)   ulatomic64_compare_exchange_weak(obj, expected, val)
  #define ulatomiciptr_fetch_add(obj, val)                         ulatomic64_fetch_add(obj, val)
  #define ulatomiciptr_fetch_sub(obj, val)                         ulatomic64_fetch_sub(obj, val)
  #define ulatomiciptr_fetch_or(obj, val)                          ulatomic64_fetch_or(obj, val)
  #define ulatomiciptr_fetch_xor(obj, val)                         ulatomic64_fetch_xor(obj, val)
  #define ulatomiciptr_fetch_and(obj, val)                         ulatomic64_fetch_and(obj, val)
  #define ulatomiciptr_store_explicit(obj, val, ord)                             ulatomic64_store_explicit(obj, val, ord)
  #define ulatomiciptr_load_explicit(obj, ord)                                   ulatomic64_load_explicit(obj, ord)
  #define ulatomiciptr_exchange_explicit(obj, val, ord)                          ulatomic64_exchange_explicit(obj, val, ord)
  #define ulatomiciptr_compare_exchange_strong_explicit(obj, expected, val, ord) ulatomic64_compare_exchange_strong_explicit(obj, expected, val, ord)
  #define ulatomiciptr_compare_exchange_weak_explicit(obj, expected, val, ord)   ulatomic64_compare_exchange_weak_explicit(obj, expected, val, ord)
  #define ulatomiciptr_fetch_add_explicit(obj, val, ord)                         ulatomic64_fetch_add_explicit(obj, val, ord)
  #define ulatomiciptr_fetch_sub_explicit(obj, val, ord)                         ulatomic64_fetch_sub_explicit(obj, val, ord)
  #define ulatomiciptr_fetch_or_explicit(obj, val, ord)                          ulatomic64_fetch_or_explicit(obj, val, ord)
  #define ulatomiciptr_fetch_xor_explicit(obj, val, ord)                         ulatomic64_fetch_xor_explicit(obj, val, ord)
  #define ulatomiciptr_fetch_and_explicit(obj, val, ord)                         ulatomic64_fetch_and_explicit(obj, val, ord)

  #define ulatomiciptr_wait(obj, expected) ulatomic64_wait(obj, expected)
  #define ulatomiciptr_notify_one(obj)     ulatomic64_notify_one(obj)
  #define ulatomiciptr_notify_all(obj)     ulatomic64_notify_all(obj)
#elif defined(_ULATOMIC_INTPTR_32BIT) && defined(ULATOMIC32_INIT)
  typedef ulatomic32_t ulatomiciptr_t;
  typedef ulatomic32_raw_t ulatomiciptr_raw_t;
  #define ULATOMICIPTR_INIT ULATOMIC32_INIT
  #define ulatomiciptr_store(obj, val)                             ulatomic32_store(obj, val)
  #define ulatomiciptr_load(obj)                                   ulatomic32_load(obj)
  #define ulatomiciptr_exchange(obj, val)                          ulatomic32_exchange(obj, val)
  #define ulatomiciptr_compare_exchange_strong(obj, expected, val) ulatomic32_compare_exchange_strong(obj, expected, val)
  #define ulatomiciptr_compare_exchange_weak(obj, expected, val)   ulatomic32_compare_exchange_weak(obj, expected, val)
  #define ulatomiciptr_fetch_add(obj, val)                         ulatomic32_fetch_add(obj, val)
  #define ulatomiciptr_fetch_sub(obj, val)                         ulatomic32_fetch_sub(obj, val)
  #define ulatomiciptr_fetch_or(obj, val)                          ulatomic32_fetch_or(obj, val)
  #define ulatomiciptr_fetch_xor(obj, val)                         ulatomic32_fetch_xor(obj, val)
  #define ulatomiciptr_fetch_and(obj, val)                         ulatomic32_fetch_and(obj, val)
  #define ulatomiciptr_store_explicit(obj, val, ord)                             ulatomic32_store_explicit(obj, val, ord)
  #define ulatomiciptr_load_explicit(obj, ord)                                   ulatomic32_load_explicit(obj, ord)
  #define ulatomiciptr_exchange_explicit(obj, val, ord)                          ulatomic32_exchange_explicit(obj, val, ord)
  #define ulatomiciptr_compare_exchange_strong_explicit(obj, expected, val, ord) ulatomic32_compare_exchange_strong_explicit(obj, expected, val, ord)
  #define ulatomiciptr_compare_exchange_weak_explicit(obj, expected, val, ord)   ulatomic32_compare_exchange_weak_explicit(obj, expected, val, ord)
  #define ulatomiciptr_fetch_add_explicit(obj, val, ord)                         ulatomic32_fetch_add_explicit(obj, val, ord)
  #define ulatomiciptr_fetch_sub_explicit(obj, val, ord)                         ulatomic32_fetch_sub_explicit(obj, val, ord)
  #define ulatomiciptr_fetch_or_explicit(obj, val, ord)                          ulatomic32_fetch_or_explicit(obj, val, ord)
  #define ulatomiciptr_fetch_xor_explicit(obj, val, ord)                         ulatomic32_fetch_xor_explicit(obj, val, ord)
  #define ulatomiciptr_fetch_and_explicit(obj, val, ord)                         ulatomic32_fetch_and_explicit(obj, val, ord)

  #define ulatomiciptr_wait(obj, expected) ulatomic32_wait(obj, expected)
  #define ulatomiciptr_notify_one(obj)     ulatomic32_notify_one(obj)
  #define ulatomiciptr_notify_all(obj)     ulatomic32_notify_all(obj)
#else
  #if defined(ULATOMIC32_INIT) || defined(ULATOMIC64_INIT)
    #error "ulatomic.h: atomic intptr_t defined failed"
  #endif
#endif
#undef _ULATOMIC_INTPTR_64BIT
#undef _ULATOMIC_INTPTR_32BIT


/* ulatomic_spinlock_t */
#ifdef ULATOMIC_FLAG_INIT
  typedef ulatomic_flag_t ulatomic_spinlock_t;
  #define ULATOMIC_SPINLOCK_INIT ULATOMIC_FLAG_INIT
  ul_hapi void ulatomic_spin_init(ulatomic_spinlock_t* lck) {
    ulatomic_flag_clear_explicit(lck, ulatomic_memory_order_release);
  }
  ul_hapi void ulatomic_spin_lock(ulatomic_spinlock_t* lck) {
    while(ulatomic_flag_test_and_set_explicit(lck, ulatomic_memory_order_acquire)) ulatomic_pause();
  }
  ul_hapi int ulatomic_spin_trylock(ulatomic_spinlock_t* lck) {
    return !ulatomic_flag_test_and_set_explicit(lck, ulatomic_memory_order_acquire);
  }
  ul_hapi void ulatomic_spin_unlock(ulatomic_spinlock_t* lck) {
    ulatomic_flag_clear_explicit(lck, ulatomic_memory_order_release);
  }
#endif

/* ulatomic_rwlock_t */
#ifdef ULATOMIC32_INIT
  typedef struct ulatomic_rwlock_t {
    ulatomic32_t rwait;
    ulatomic32_t wlock;
  } ulatomic_rwlock_t;
  #define ULATOMIC_RWLOCK_INIT { ULATOMIC32_INIT, ULATOMIC32_INIT }
  ul_hapi void ulatomic_rw_init(ulatomic_rwlock_t* lck) {
    ulatomic32_store_explicit(&lck->rwait, 0, ulatomic_memory_order_relaxed);
    ulatomic32_store_explicit(&lck->wlock, 0, ulatomic_memory_order_relaxed);
  }
  ul_hapi void ulatomic_rw_rlock(ulatomic_rwlock_t* lck) {
    int r;
    while(ulatomic32_load_explicit(&lck->rwait, ulatomic_memory_order_acquire)) ulatomic_pause();
    r = ulatomic32_fetch_add_explicit(&lck->wlock, 2, ulatomic_memory_order_acq_rel);
    if((r & 1) == 0) return;
    while(ulatomic32_load_explicit(&lck->wlock, ulatomic_memory_order_acquire) & 1) ulatomic_pause();
  }
  ul_hapi void ulatomic_rw_wlock(ulatomic_rwlock_t* lck) {
    ulatomic32_fetch_add_explicit(&lck->rwait, 1, ulatomic_memory_order_acq_rel);
    for(;;) {
      int r = ulatomic32_load_explicit(&lck->wlock, ulatomic_memory_order_acquire);
      if(r == 0 && ulatomic32_compare_exchange_weak_explicit(&lck->wlock, &r, r | 1, ulatomic_memory_order_acq_rel))
        break;
      ulatomic_pause();
    }
  }
  ul_hapi int ulatomic_rw_tryrlock(ulatomic_rwlock_t* lck) {
    int r;
    while(ulatomic32_load_explicit(&lck->rwait, ulatomic_memory_order_acquire));
    r = ulatomic32_fetch_add_explicit(&lck->wlock, 2, ulatomic_memory_order_acq_rel);
    if((r & 1) == 0) return 1;
    else {
      ulatomic32_fetch_sub_explicit(&lck->wlock, 2, ulatomic_memory_order_acq_rel);
      return 0;
    }
  }
  ul_hapi int ulatomic_rw_trywlock(ulatomic_rwlock_t* lck) {
    int r;
    ulatomic32_fetch_add_explicit(&lck->rwait, 1, ulatomic_memory_order_acq_rel);
    r = ulatomic32_load_explicit(&lck->wlock, ulatomic_memory_order_acquire);
    if(r == 0 && ulatomic32_compare_exchange_weak_explicit(&lck->wlock, &r, r | 1, ulatomic_memory_order_acq_rel))
      return 1;
    else {
      ulatomic32_fetch_sub_explicit(&lck->rwait, 1, ulatomic_memory_order_acq_rel);
      return 0;
    }
  }
  ul_hapi void ulatomic_rw_unrlock(ulatomic_rwlock_t* lck) {
    ulatomic32_fetch_sub_explicit(&lck->wlock, 2, ulatomic_memory_order_release);
  }
  ul_hapi void ulatomic_rw_unwlock(ulatomic_rwlock_t* lck) {
    ulatomic32_fetch_and_explicit(&lck->wlock, ~1, ulatomic_memory_order_release);
    ulatomic32_fetch_sub_explicit(&lck->rwait, 1, ulatomic_memory_order_release);
  }
#endif

#endif /* ULATOMIC_H */
