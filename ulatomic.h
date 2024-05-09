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
    | void ulatomic_store_32(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic_load_32(ulatomic32_t* obj);
    | ulatomic32_raw_t ulatomic_exchange_32(ulatomic32_t* obj, ulatomic32_raw_t val);
    | void ulatomic_store_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic_load_explicit_32(ulatomic32_t* obj, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic_exchange_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    |
    | int ulatomic_compare_exchange_strong_32(ulatomic32_t* obj, ulatomic32_raw_t* expected, ulatomic32_raw_t val);
    | int ulatomic_compare_exchange_weak_32(ulatomic32_t* obj, ulatomic32_raw_t* expected, ulatomic32_raw_t val);
    | int ulatomic_compare_exchange_strong_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t* expected, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | int ulatomic_compare_exchange_weak_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t* expected, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomic32_raw_t ulatomic_fetch_add_32(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic_fetch_sub_32(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic_fetch_add_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic_fetch_sub_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomic32_raw_t ulatomic_fetch_or_32(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic_fetch_xor_32(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic_fetch_and_32(ulatomic32_t* obj, ulatomic32_raw_t val);
    | ulatomic32_raw_t ulatomic_fetch_or_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic_fetch_xor_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic32_raw_t ulatomic_fetch_and_explicit_32(ulatomic32_t* obj, ulatomic32_raw_t val, ulatomic_memory_order_t ord);
    +----------------------------------------------------------------
    | typedef ... ulatomic64_t;
    | typedef ... ulatomic64_raw_t;
    | ulatomic64_t ULATOMIC64_INIT = ...;
    |
    | void ulatomic_store_64(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic_load_64(ulatomic64_t* obj);
    | ulatomic64_raw_t ulatomic_exchange_64(ulatomic64_t* obj, ulatomic64_raw_t val);
    | void ulatomic_store_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic_load_explicit_64(ulatomic64_t* obj, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic_exchange_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    |
    | int ulatomic_compare_exchange_strong_64(ulatomic64_t* obj, ulatomic64_raw_t* expected, ulatomic64_raw_t val);
    | int ulatomic_compare_exchange_weak_64(ulatomic64_t* obj, ulatomic64_raw_t* expected, ulatomic64_raw_t val);
    | int ulatomic_compare_exchange_strong_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t* expected, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | int ulatomic_compare_exchange_weak_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t* expected, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomic64_raw_t ulatomic_fetch_add_64(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic_fetch_sub_64(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic_fetch_add_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic_fetch_sub_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    |
    | ulatomic64_raw_t ulatomic_fetch_or_64(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic_fetch_xor_64(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic_fetch_and_64(ulatomic64_t* obj, ulatomic64_raw_t val);
    | ulatomic64_raw_t ulatomic_fetch_or_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic_fetch_xor_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
    | ulatomic64_raw_t ulatomic_fetch_and_explicit_64(ulatomic64_t* obj, ulatomic64_raw_t val, ulatomic_memory_order_t ord);
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
    | void ulatomic_spinlock_lock(ulatomic_spinlock_t* lck);
    | int ulatomic_spinlock_trylock(ulatomic_spinlock_t* lck);
    | void ulatomic_spinlock_unlock(ulatomic_spinlock_t* lck);
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
#elif __STDC_VERSION__ >= 201112L && (!defined(__STDC_NO_ATOMICS__) || !__STDC_NO_ATOMICS__) /* C11 and above */
  #define ULATOMIC_API_C11
#endif
#if defined(_MSC_VER)
  #define ULATOMIC_API_MSVC
#endif
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) /* gcc 4.7 and above */
  #define ULATOMIC_API_GNUC
#endif


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
  #define ulatomic_store_32(obj, val)                             atomic_store(obj, val)
  #define ulatomic_load_32(obj)                                   atomic_load(obj)
  #define ulatomic_exchange_32(obj, val)                          atomic_exchange(obj, val)
  #define ulatomic_compare_exchange_strong_32(obj, expected, val) ul_static_cast(int, atomic_compare_exchange_strong(obj, expected, val))
  #define ulatomic_compare_exchange_weak_32(obj, expected, val)   ul_static_cast(int, atomic_compare_exchange_weak(obj, expected, val))
  #define ulatomic_fetch_add_32(obj, val)                         atomic_fetch_add(obj, val)
  #define ulatomic_fetch_sub_32(obj, val)                         atomic_fetch_sub(obj, val)
  #define ulatomic_fetch_or_32(obj, val)                          atomic_fetch_or(obj, val)
  #define ulatomic_fetch_xor_32(obj, val)                         atomic_fetch_xor(obj, val)
  #define ulatomic_fetch_and_32(obj, val)                         atomic_fetch_and(obj, val)
  #define ulatomic_store_explicit_32(obj, val, ord)                             atomic_store_explicit(obj, val, ord)
  #define ulatomic_load_explicit_32(obj, ord)                                   atomic_load_explicit(obj, ord)
  #define ulatomic_exchange_explicit_32(obj, val, ord)                          atomic_exchange_explicit(obj, val, ord)
  #define ulatomic_compare_exchange_strong_explicit_32(obj, expected, val, ord) ul_static_cast(int, atomic_compare_exchange_strong_explicit(obj, expected, val, ord))
  #define ulatomic_compare_exchange_weak_explicit_32(obj, expected, val, ord)   ul_static_cast(int, atomic_compare_exchange_weak_explicit(obj, expected, val, ord))
  #define ulatomic_fetch_add_explicit_32(obj, val, ord)                         atomic_fetch_add_explicit(obj, val, ord)
  #define ulatomic_fetch_sub_explicit_32(obj, val, ord)                         atomic_fetch_sub_explicit(obj, val, ord)
  #define ulatomic_fetch_or_explicit_32(obj, val, ord)                          atomic_fetch_or_explicit(obj, val, ord)
  #define ulatomic_fetch_xor_explicit_32(obj, val, ord)                         atomic_fetch_xor_explicit(obj, val, ord)
  #define ulatomic_fetch_and_explicit_32(obj, val, ord)                         atomic_fetch_and_explicit(obj, val, ord)

  typedef _Atomic int64_t ulatomic64_t;
  typedef int64_t ulatomic64_raw_t;
  #if __STDC_VERSION__ >= 201710L
    #define ULATOMIC64_INIT (0)
  #else
    #define ULATOMIC64_INIT ATOMIC_VAR_INIT(0)
  #endif
  #define ulatomic_store_64(obj, val)                             atomic_store(obj, val)
  #define ulatomic_load_64(obj)                                   atomic_load(obj)
  #define ulatomic_exchange_64(obj, val)                          atomic_exchange(obj, val)
  #define ulatomic_compare_exchange_strong_64(obj, expected, val) ul_static_cast(int, atomic_compare_exchange_strong(obj, expected, val))
  #define ulatomic_compare_exchange_weak_64(obj, expected, val)   ul_static_cast(int, atomic_compare_exchange_weak(obj, expected, val))
  #define ulatomic_fetch_add_64(obj, val)                         atomic_fetch_add(obj, val)
  #define ulatomic_fetch_sub_64(obj, val)                         atomic_fetch_sub(obj, val)
  #define ulatomic_fetch_or_64(obj, val)                          atomic_fetch_or(obj, val)
  #define ulatomic_fetch_xor_64(obj, val)                         atomic_fetch_xor(obj, val)
  #define ulatomic_fetch_and_64(obj, val)                         atomic_fetch_and(obj, val)
  #define ulatomic_store_explicit_64(obj, val, ord)                             atomic_store_explicit(obj, val, ord)
  #define ulatomic_load_explicit_64(obj, ord)                                   atomic_load_explicit(obj, ord)
  #define ulatomic_exchange_explicit_64(obj, val, ord)                          atomic_exchange_explicit(obj, val, ord)
  #define ulatomic_compare_exchange_strong_explicit_64(obj, expected, val, ord) ul_static_cast(int, atomic_compare_exchange_strong_explicit(obj, expected, val, ord))
  #define ulatomic_compare_exchange_weak_explicit_64(obj, expected, val, ord)   ul_static_cast(int, atomic_compare_exchange_weak_explicit(obj, expected, val, ord))
  #define ulatomic_fetch_add_explicit_64(obj, val, ord)                         atomic_fetch_add_explicit(obj, val, ord)
  #define ulatomic_fetch_sub_explicit_64(obj, val, ord)                         atomic_fetch_sub_explicit(obj, val, ord)
  #define ulatomic_fetch_or_explicit_64(obj, val, ord)                          atomic_fetch_or_explicit(obj, val, ord)
  #define ulatomic_fetch_xor_explicit_64(obj, val, ord)                         atomic_fetch_xor_explicit(obj, val, ord)
  #define ulatomic_fetch_and_explicit_64(obj, val, ord)                         atomic_fetch_and_explicit(obj, val, ord)
#elif defined(ULATOMIC_API_CXX11)
  #include <atomic>
  #include <cstdint>

  typedef ::std::memory_order ulatomic_memory_order_t;
  #define ulatomic_memory_order_relaxed ::std::memory_order::memory_order_relaxed
  #define ulatomic_memory_order_consume ::std::memory_order::memory_order_consume
  #define ulatomic_memory_order_acquire ::std::memory_order::memory_order_acquire
  #define ulatomic_memory_order_release ::std::memory_order::memory_order_release
  #define ulatomic_memory_order_acq_rel ::std::memory_order::memory_order_acq_rel
  #define ulatomic_memory_order_seq_cst ::std::memory_order::memory_order_seq_cst

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
  #define ulatomic_store_32(obj, val)                             ::std::atomic_store(obj, val)
  #define ulatomic_load_32(obj)                                   ::std::atomic_load(obj)
  #define ulatomic_exchange_32(obj, val)                          ::std::atomic_exchange(obj, val)
  #define ulatomic_compare_exchange_strong_32(obj, expected, val) ul_static_cast(int, ::std::atomic_compare_exchange_strong(obj, expected, val))
  #define ulatomic_compare_exchange_weak_32(obj, expected, val)   ul_static_cast(int, ::std::atomic_compare_exchange_weak(obj, expected, val))
  #define ulatomic_fetch_add_32(obj, val)                         ::std::atomic_fetch_add(obj, val)
  #define ulatomic_fetch_sub_32(obj, val)                         ::std::atomic_fetch_sub(obj, val)
  #define ulatomic_fetch_or_32(obj, val)                          ::std::atomic_fetch_or(obj, val)
  #define ulatomic_fetch_xor_32(obj, val)                         ::std::atomic_fetch_xor(obj, val)
  #define ulatomic_fetch_and_32(obj, val)                         ::std::atomic_fetch_and(obj, val)
  #define ulatomic_store_explicit_32(obj, val, ord)                             ::std::atomic_store_explicit(obj, val, ord)
  #define ulatomic_load_explicit_32(obj, ord)                                   ::std::atomic_load_explicit(obj, ord)
  #define ulatomic_exchange_explicit_32(obj, val, ord)                          ::std::atomic_exchange_explicit(obj, val, ord)
  #define ulatomic_compare_exchange_strong_explicit_32(obj, expected, val, ord) ul_static_cast(int, ::std::atomic_compare_exchange_strong_explicit(obj, expected, val, ord))
  #define ulatomic_compare_exchange_weak_explicit_32(obj, expected, val, ord)   ul_static_cast(int, ::std::atomic_compare_exchange_weak_explicit(obj, expected, val, ord))
  #define ulatomic_fetch_add_explicit_32(obj, val, ord)                         ::std::atomic_fetch_add_explicit(obj, val, ord)
  #define ulatomic_fetch_sub_explicit_32(obj, val, ord)                         ::std::atomic_fetch_sub_explicit(obj, val, ord)
  #define ulatomic_fetch_or_explicit_32(obj, val, ord)                          ::std::atomic_fetch_or_explicit(obj, val, ord)
  #define ulatomic_fetch_xor_explicit_32(obj, val, ord)                         ::std::atomic_fetch_xor_explicit(obj, val, ord)
  #define ulatomic_fetch_and_explicit_32(obj, val, ord)                         ::std::atomic_fetch_and_explicit(obj, val, ord)

  typedef ::std::atomic<int64_t> ulatomic64_t;
  typedef int64_t ulatomic64_raw_t;
  #define ULATOMIC64_INIT { 0 }
  #define ulatomic_store_64(obj, val)                             ::std::atomic_store(obj, val)
  #define ulatomic_load_64(obj)                                   ::std::atomic_load(obj)
  #define ulatomic_exchange_64(obj, val)                          ::std::atomic_exchange(obj, val)
  #define ulatomic_compare_exchange_strong_64(obj, expected, val) ul_static_cast(int, ::std::atomic_compare_exchange_strong(obj, expected, val))
  #define ulatomic_compare_exchange_weak_64(obj, expected, val)   ul_static_cast(int, ::std::atomic_compare_exchange_weak(obj, expected, val))
  #define ulatomic_fetch_add_64(obj, val)                         ::std::atomic_fetch_add(obj, val)
  #define ulatomic_fetch_sub_64(obj, val)                         ::std::atomic_fetch_sub(obj, val)
  #define ulatomic_fetch_or_64(obj, val)                          ::std::atomic_fetch_or(obj, val)
  #define ulatomic_fetch_xor_64(obj, val)                         ::std::atomic_fetch_xor(obj, val)
  #define ulatomic_fetch_and_64(obj, val)                         ::std::atomic_fetch_and(obj, val)
  #define ulatomic_store_explicit_64(obj, val, ord)                             ::std::atomic_store_explicit(obj, val, ord)
  #define ulatomic_load_explicit_64(obj, ord)                                   ::std::atomic_load_explicit(obj, ord)
  #define ulatomic_exchange_explicit_64(obj, val, ord)                          ::std::atomic_exchange_explicit(obj, val, ord)
  #define ulatomic_compare_exchange_strong_explicit_64(obj, expected, val, ord) ul_static_cast(int, ::std::atomic_compare_exchange_strong_explicit(obj, expected, val, ord))
  #define ulatomic_compare_exchange_weak_explicit_64(obj, expected, val, ord)   ul_static_cast(int, ::std::atomic_compare_exchange_weak_explicit(obj, expected, val, ord))
  #define ulatomic_fetch_add_explicit_64(obj, val, ord)                         ::std::atomic_fetch_add_explicit(obj, val, ord)
  #define ulatomic_fetch_sub_explicit_64(obj, val, ord)                         ::std::atomic_fetch_sub_explicit(obj, val, ord)
  #define ulatomic_fetch_or_explicit_64(obj, val, ord)                          ::std::atomic_fetch_or_explicit(obj, val, ord)
  #define ulatomic_fetch_xor_explicit_64(obj, val, ord)                         ::std::atomic_fetch_xor_explicit(obj, val, ord)
  #define ulatomic_fetch_and_explicit_64(obj, val, ord)                         ::std::atomic_fetch_and_explicit(obj, val, ord)
#elif defined(ULATOMIC_API_MSVC)
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
    ul_hapi int ulatomic_load_explicit_32(int* obj, int ord) {
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
    ul_hapi int ulatomic_exchange_explicit_32(int* obj, int val, int ord) {
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
    ul_hapi int ulatomic_compare_exchange_strong_explicit_32(int* obj, int* expected, int val, int ord) {
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
    ul_hapi int ulatomic_fetch_add_explicit_32(int* obj, int val, int ord) {
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
    ul_hapi int ulatomic_fetch_sub_explicit_32(int* obj, int val, int ord) {
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
    ul_hapi int ulatomic_fetch_or_explicit_32(int* obj, int val, int ord) {
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
    ul_hapi int ulatomic_fetch_xor_explicit_32(int* obj, int val, int ord) {
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
    ul_hapi int ulatomic_fetch_and_explicit_32(int* obj, int val, int ord) {
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
    ul_hapi int ulatomic_exchange_explicit_32(int* obj, int val, int ord) {
      (void)ord; return ul_static_cast(int, InterlockedExchange(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
    }
    #if _MSC_VER >= 1300
      ul_hapi int ulatomic_load_explicit_32(int* obj, int ord) {
        (void)ord; return ul_static_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), 0, 0));
      }
      ul_hapi int ulatomic_compare_exchange_strong_explicit_32(int* obj, int* expected, int val, int ord) {
        int old = *expected; (void)ord;
        return (*expected = ul_static_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val), ul_static_cast(long, old))) == old);
      }
    #else
      ul_hapi int ulatomic_load_explicit_32(int* obj, int ord) {
        (void)ord; return ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), 0, 0));
      }
      ul_hapi int ulatomic_compare_exchange_strong_explicit_32(int* obj, int* expected, int val, int ord) {
        int old = *expected; (void)ord;
        return (*expected = ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), ul_reinterpret_cast(void*, val), ul_reinterpret_cast(void*, old))) == old);
      }
    #endif
    ul_hapi int ulatomic_fetch_add_explicit_32(int* obj, int val, int ord) {
      (void)ord; return ul_static_cast(int, InterlockedExchangeAdd(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
    }
    ul_hapi int ulatomic_fetch_sub_explicit_32(int* obj, int val, int ord) {
      (void)ord; return ul_static_cast(int, InterlockedExchangeAdd(ul_reinterpret_cast(long*, obj), -ul_static_cast(long, val)));
    }
    #if _MSC_VER >= 1500 || defined(_AMD64_) /* older MSVC don't obtain these APIs */
      ul_hapi int ulatomic_fetch_or_explicit_32(int* obj, int val, int ord) {
        (void)ord; return ul_static_cast(int, _InterlockedOr(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }
      ul_hapi int ulatomic_fetch_xor_explicit_32(int* obj, int val, int ord) {
        (void)ord; return ul_static_cast(int, _InterlockedXor(ul_reinterpret_cast(long*, obj), -ul_static_cast(long, val)));
      }
      ul_hapi int ulatomic_fetch_and_explicit_32(int* obj, int val, int ord) {
        (void)ord; return ul_static_cast(int, _InterlockedAnd(ul_reinterpret_cast(long*, obj), ul_static_cast(long, val)));
      }
      #define ulatomic_fetch_or_32(obj, val)  ulatomic_fetch_or_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
      #define ulatomic_fetch_xor_32(obj, val) ulatomic_fetch_xor_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
      #define ulatomic_fetch_and_32(obj, val) ulatomic_fetch_and_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
    #else
      #if _MSC_VER >= 1300
        ul_hapi int ulatomic_fetch_or_32(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov | val);
          } while(InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), nv, ov) != ov);
          return ov;
        }
        ul_hapi int ulatomic_fetch_xor_32(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov ^ val);
          } while(InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), nv, ov) != ov);
          return ov;
        }
        ul_hapi int ulatomic_fetch_and_32(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov & val);
          } while(InterlockedCompareExchange(ul_reinterpret_cast(long*, obj), nv, ov) != ov);
          return ov;
        }
      #else
        ul_hapi int ulatomic_fetch_or_32(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov | val);
          } while(ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), ul_reinterpret_cast(void*, nv), ul_reinterpret_cast(void*, ov))) != ov);
          return ov;
        }
        ul_hapi int ulatomic_fetch_xor_32(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov ^ val);
          } while(ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), ul_reinterpret_cast(void*, nv), ul_reinterpret_cast(void*, ov))) != ov);
          return ov;
        }
        ul_hapi int ulatomic_fetch_and_32(int* obj, int val) {
          long ov, nv;
          do {
            ov = ul_static_cast(long, *obj);
            nv = ul_static_cast(long, ov & val);
          } while(ul_reinterpret_cast(int, InterlockedCompareExchange(ul_reinterpret_cast(void**, obj), ul_reinterpret_cast(void*, nv), ul_reinterpret_cast(void*, ov))) != ov);
          return ov;
        }
      #endif
      #define ulatomic_fetch_or_explicit_32(obj, val, ord)  ulatomic_fetch_or_32(obj, val)
      #define ulatomic_fetch_xor_explicit_32(obj, val, ord) ulatomic_fetch_xor_32(obj, val)
      #define ulatomic_fetch_and_explicit_32(obj, val, ord) ulatomic_fetch_and_32(obj, val)
    #endif
  #endif
  #define ulatomic_store_explicit_32(obj, val, ord)                           (void)(ulatomic_exchange_explicit_32(obj, val, ord))
  #define ulatomic_compare_exchange_weak_explicit_32(obj, expected, val, ord) ulatomic_compare_exchange_strong_explicit_32(obj, expected, val, ord)

  #define ulatomic_store_32(obj, val)                             ulatomic_store_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_load_32(obj)                                   ulatomic_load_explicit_32(obj, ulatomic_memory_order_seq_cst)
  #define ulatomic_exchange_32(obj, val)                          ulatomic_exchange_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_compare_exchange_strong_32(obj, expected, val) ulatomic_compare_exchange_strong_explicit_32(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_compare_exchange_weak_32(obj, expected, val)   ulatomic_compare_exchange_weak_explicit_32(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_add_32(obj, val)                         ulatomic_fetch_add_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_sub_32(obj, val)                         ulatomic_fetch_sub_explicit_32(obj, val, ulatomic_memory_order_seq_cst)

  #if defined(__ULATOMIC_API_MSVC_X64) || defined(__ULATOMIC_API_MSVC_ARM32) || defined(__ULATOMIC_API_MSVC_ARM64)
    typedef __int64 ulatomic64_t;
    typedef __int64 ulatomic64_raw_t;
    #define ULATOMIC64_INIT (0)
    #if defined(__ULATOMIC_API_MSVC_ARM32) || defined(__ULATOMIC_API_MSVC_ARM64)
      ul_hapi __int64 ulatomic_load_explicit_64(__int64* obj, int ord) {
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
      ul_hapi __int64 ulatomic_exchange_explicit_64(__int64* obj, __int64 val, int ord) {
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
      ul_hapi __int64 ulatomic_compare_exchange_strong_explicit_64(__int64* obj, __int64* expected, __int64 val, int ord) {
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
      ul_hapi __int64 ulatomic_fetch_add_explicit_64(__int64* obj, __int64 val, int ord) {
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
      ul_hapi __int64 ulatomic_fetch_sub_explicit_64(__int64* obj, __int64 val, int ord) {
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
      ul_hapi __int64 ulatomic_fetch_or_explicit_64(__int64* obj, __int64 val, int ord) {
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
      ul_hapi __int64 ulatomic_fetch_xor_explicit_64(__int64* obj, __int64 val, int ord) {
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
      ul_hapi __int64 ulatomic_fetch_and_explicit_64(__int64* obj, __int64 val, int ord) {
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
      ul_hapi __int64 ulatomic_load_explicit_64(__int64* obj, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedCompareExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, 0), ul_static_cast(__int64, 0)));
      }
      ul_hapi __int64 ulatomic_exchange_explicit_64(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic_compare_exchange_strong_explicit_64(__int64* obj, __int64* expected, __int64 val, int ord) {
        __int64 old = *expected; (void)ord;
        return (*expected = ul_static_cast(__int64, InterlockedCompareExchange64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val), ul_static_cast(__int64, old)))) == old;
      }
      ul_hapi __int64 ulatomic_fetch_add_explicit_64(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedExchangeAdd64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic_fetch_sub_explicit_64(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedExchangeAdd64(ul_reinterpret_cast(__int64*, obj), -ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic_fetch_or_explicit_64(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedOr64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic_fetch_xor_explicit_64(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedXor64(ul_reinterpret_cast(__int64*, obj), -ul_static_cast(__int64, val)));
      }
      ul_hapi __int64 ulatomic_fetch_and_explicit_64(__int64* obj, __int64 val, int ord) {
        (void)ord; return ul_static_cast(__int64, InterlockedAnd64(ul_reinterpret_cast(__int64*, obj), ul_static_cast(__int64, val)));
      }
    #endif
    #define ulatomic_store_explicit_64(obj, val, ord)                           (void)(ulatomic_exchange_explicit_64(obj, val, ord))
    #define ulatomic_compare_exchange_weak_explicit_64(obj, expected, val, ord) ulatomic_compare_exchange_strong_explicit_64(obj, expected, val, ord)

    #define ulatomic_store_64(obj, val)                             ulatomic_store_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic_load_64(obj)                                   ulatomic_load_explicit_64(obj, ulatomic_memory_order_seq_cst)
    #define ulatomic_exchange_64(obj, val)                          ulatomic_exchange_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic_compare_exchange_strong_64(obj, expected, val) ulatomic_compare_exchange_strong_explicit_64(obj, expected, val, ulatomic_memory_order_seq_cst)
    #define ulatomic_compare_exchange_weak_64(obj, expected, val)   ulatomic_compare_exchange_weak_explicit_64(obj, expected, val, ulatomic_memory_order_seq_cst)
    #define ulatomic_fetch_add_64(obj, val)                         ulatomic_fetch_add_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic_fetch_sub_64(obj, val)                         ulatomic_fetch_sub_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic_fetch_or_64(obj, val)                          ulatomic_fetch_or_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic_fetch_xor_64(obj, val)                         ulatomic_fetch_xor_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
    #define ulatomic_fetch_and_64(obj, val)                         ulatomic_fetch_and_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
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
  #define ulatomic_store_explicit_32(obj, val, ord)                             __atomic_store_n(obj, val, ord)
  #define ulatomic_load_explicit_32(obj, ord)                                   __atomic_load_n(obj, ord)
  #define ulatomic_exchange_explicit_32(obj, val, ord)                          __atomic_exchange_n(obj, val, ord)
  #define ulatomic_compare_exchange_strong_explicit_32(obj, expected, val, ord) ul_static_cast(int, __atomic_compare_exchange_n(obj, expected, val, 0, ord, ord))
  #define ulatomic_compare_exchange_weak_explicit_32(obj, expected, val, ord)   ul_static_cast(int, __atomic_compare_exchange_n(obj, expected, val, 1, ord, ord))
  #define ulatomic_fetch_add_explicit_32(obj, val, ord)                         __atomic_fetch_add(obj, val, ord)
  #define ulatomic_fetch_sub_explicit_32(obj, val, ord)                         __atomic_fetch_sub(obj, val, ord)
  #define ulatomic_fetch_or_explicit_32(obj, val, ord)                          __atomic_fetch_or(obj, val, ord)
  #define ulatomic_fetch_xor_explicit_32(obj, val, ord)                         __atomic_fetch_xor(obj, val, ord)
  #define ulatomic_fetch_and_explicit_32(obj, val, ord)                         __atomic_fetch_and(obj, val, ord)
  #define ulatomic_store_32(obj, val)                             ulatomic_store_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_load_32(obj)                                   ulatomic_load_explicit_32(obj, ulatomic_memory_order_seq_cst)
  #define ulatomic_exchange_32(obj, val)                          ulatomic_exchange_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_compare_exchange_strong_32(obj, expected, val) ulatomic_compare_exchange_strong_explicit_32(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_compare_exchange_weak_32(obj, expected, val)   ulatomic_compare_exchange_weak_explicit_32(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_add_32(obj, val)                         ulatomic_fetch_add_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_sub_32(obj, val)                         ulatomic_fetch_sub_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_or_32(obj, val)                          ulatomic_fetch_or_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_xor_32(obj, val)                         ulatomic_fetch_xor_explicit_32(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_and_32(obj, val)                         ulatomic_fetch_and_explicit_32(obj, val, ulatomic_memory_order_seq_cst)

  typedef int64_t ulatomic64_t;
  typedef int64_t ulatomic64_raw_t;
  #define ULATOMIC64_INIT (0)
  #define ulatomic_store_explicit_64(obj, val, ord)                             __atomic_store_n(obj, val, ord)
  #define ulatomic_load_explicit_64(obj, ord)                                   __atomic_load_n(obj, ord)
  #define ulatomic_exchange_explicit_64(obj, val, ord)                          __atomic_exchange_n(obj, val, ord)
  #define ulatomic_compare_exchange_strong_explicit_64(obj, expected, val, ord) ul_static_cast(int, __atomic_compare_exchange_n(obj, expected, val, 0, ord, ord))
  #define ulatomic_compare_exchange_weak_explicit_64(obj, expected, val, ord)   ul_static_cast(int, __atomic_compare_exchange_n(obj, expected, val, 1, ord, ord))
  #define ulatomic_fetch_add_explicit_64(obj, val, ord)                         __atomic_fetch_add(obj, val, ord)
  #define ulatomic_fetch_sub_explicit_64(obj, val, ord)                         __atomic_fetch_sub(obj, val, ord)
  #define ulatomic_fetch_or_explicit_64(obj, val, ord)                          __atomic_fetch_or(obj, val, ord)
  #define ulatomic_fetch_xor_explicit_64(obj, val, ord)                         __atomic_fetch_xor(obj, val, ord)
  #define ulatomic_fetch_and_explicit_64(obj, val, ord)                         __atomic_fetch_and(obj, val, ord)
  #define ulatomic_store_64(obj, val)                             ulatomic_store_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_load_64(obj)                                   ulatomic_load_explicit_64(obj, ulatomic_memory_order_seq_cst)
  #define ulatomic_exchange_64(obj, val)                          ulatomic_exchange_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_compare_exchange_strong_64(obj, expected, val) ulatomic_compare_exchange_strong_explicit_64(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_compare_exchange_weak_64(obj, expected, val)   ulatomic_compare_exchange_weak_explicit_64(obj, expected, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_add_64(obj, val)                         ulatomic_fetch_add_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_sub_64(obj, val)                         ulatomic_fetch_sub_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_or_64(obj, val)                          ulatomic_fetch_or_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_xor_64(obj, val)                         ulatomic_fetch_xor_explicit_64(obj, val, ulatomic_memory_order_seq_cst)
  #define ulatomic_fetch_and_64(obj, val)                         ulatomic_fetch_and_explicit_64(obj, val, ulatomic_memory_order_seq_cst)

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
  #define ulatomic_store_32(obj, val) (void)(*(obj) = (val))
  #define ulatomic_load_32(obj) (*(obj))
  ul_hapi _ulatomic_i32_t ulatomic_exchange_32(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj = val; return old; }
  ul_hapi int ulatomic_compare_exchange_strong_32(ulatomic32_t* obj, _ulatomic_i32_t* expected, _ulatomic_i32_t val) {
    if(*obj == *expected) { *obj = val; return 1; }
    else { *expected = *obj; return 0; }
  }
  ul_hapi int ulatomic_compare_exchange_weak_32(ulatomic32_t* obj, _ulatomic_i32_t* expected, _ulatomic_i32_t val) {
    if(*obj == *expected) { *obj = val; return 1; }
    else { *expected = *obj; return 0; }
  }
  ul_hapi _ulatomic_i32_t ulatomic_fetch_add_32(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj += val; return old; }
  ul_hapi _ulatomic_i32_t ulatomic_fetch_sub_32(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj -= val; return old; }
  ul_hapi _ulatomic_i32_t ulatomic_fetch_or_32(ulatomic32_t* obj, _ulatomic_i32_t val)  { _ulatomic_i32_t old = *obj; *obj |= val; return old; }
  ul_hapi _ulatomic_i32_t ulatomic_fetch_xor_32(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj ^= val; return old; }
  ul_hapi _ulatomic_i32_t ulatomic_fetch_and_32(ulatomic32_t* obj, _ulatomic_i32_t val) { _ulatomic_i32_t old = *obj; *obj &= val; return old; }
  #define ulatomic_store_explicit_32(obj, val, ord)                             ulatomic_store_32(obj, val)
  #define ulatomic_load_explicit_32(obj, ord)                                   ulatomic_load_32(obj)
  #define ulatomic_exchange_explicit_32(obj, val, ord)                          ulatomic_exchange_32(obj, val)
  #define ulatomic_compare_exchange_strong_explicit_32(obj, expected, val, ord) ulatomic_compare_exchange_strong_32(obj, expected, val)
  #define ulatomic_compare_exchange_weak_explicit_32(obj, expected, val, ord)   ulatomic_compare_exchange_weak_32(obj, expected, val)
  #define ulatomic_fetch_add_explicit_32(obj, val, ord)                         ulatomic_fetch_add_32(obj, val)
  #define ulatomic_fetch_sub_explicit_32(obj, val, ord)                         ulatomic_fetch_sub_32(obj, val)
  #define ulatomic_fetch_or_explicit_32(obj, val, ord)                          ulatomic_fetch_or_32(obj, val)
  #define ulatomic_fetch_xor_explicit_32(obj, val, ord)                         ulatomic_fetch_xor_32(obj, val)
  #define ulatomic_fetch_and_explicit_32(obj, val, ord)                         ulatomic_fetch_and_32(obj, val)

  #ifdef _ULATOMIC_I64_DEFINED
    typedef _ulatomic_i64_t ulatomic64_t;
    typedef _ulatomic_i64_t ulatomic64_raw_t;
    #define ULATOMIC64_INIT (0)
    #define ulatomic_store_64(obj, val) (void)(*(obj) = (val))
    #define ulatomic_load_64(obj) (*(obj))
    ul_hapi _ulatomic_i64_t ulatomic_exchange_64(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj = val; return old; }
    ul_hapi int ulatomic_compare_exchange_strong_64(ulatomic64_t* obj, _ulatomic_i64_t* expected, _ulatomic_i64_t val) {
      if(*obj == *expected) { *obj = val; return 1; }
      else { *expected = *obj; return 0; }
    }
    ul_hapi int ulatomic_compare_exchange_weak_64(ulatomic64_t* obj, _ulatomic_i64_t* expected, _ulatomic_i64_t val) {
      if(*obj == *expected) { *obj = val; return 1; }
      else { *expected = *obj; return 0; }
    }
    ul_hapi _ulatomic_i64_t ulatomic_fetch_add_64(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj += val; return old; }
    ul_hapi _ulatomic_i64_t ulatomic_fetch_sub_64(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj -= val; return old; }
    ul_hapi _ulatomic_i64_t ulatomic_fetch_or_64(ulatomic64_t* obj, _ulatomic_i64_t val)  { _ulatomic_i64_t old = *obj; *obj |= val; return old; }
    ul_hapi _ulatomic_i64_t ulatomic_fetch_xor_64(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj ^= val; return old; }
    ul_hapi _ulatomic_i64_t ulatomic_fetch_and_64(ulatomic64_t* obj, _ulatomic_i64_t val) { _ulatomic_i64_t old = *obj; *obj &= val; return old; }
    #define ulatomic_store_explicit_64(obj, val, ord)                             ulatomic_store_64(obj, val)
    #define ulatomic_load_explicit_64(obj, ord)                                   ulatomic_load_64(obj)
    #define ulatomic_exchange_explicit_64(obj, val, ord)                          ulatomic_exchange_64(obj, val)
    #define ulatomic_compare_exchange_strong_explicit_64(obj, expected, val, ord) ulatomic_compare_exchange_strong_64(obj, expected, val)
    #define ulatomic_compare_exchange_weak_explicit_64(obj, expected, val, ord)   ulatomic_compare_exchange_weak_64(obj, expected, val)
    #define ulatomic_fetch_add_explicit_64(obj, val, ord)                         ulatomic_fetch_add_64(obj, val)
    #define ulatomic_fetch_sub_explicit_64(obj, val, ord)                         ulatomic_fetch_sub_64(obj, val)
    #define ulatomic_fetch_or_explicit_64(obj, val, ord)                          ulatomic_fetch_or_64(obj, val)
    #define ulatomic_fetch_xor_explicit_64(obj, val, ord)                         ulatomic_fetch_xor_64(obj, val)
    #define ulatomic_fetch_and_explicit_64(obj, val, ord)                         ulatomic_fetch_and_64(obj, val)
  #endif

  #undef _ULATOMIC_I64_DEFINED
#endif

#if !defined(ULATOMIC_FLAG_INIT) && defined(ULATOMIC32_INIT)
  typedef ulatomic32_t ulatomic_flag_t;
  #define ULATOMIC_FLAG_INIT ULATOMIC32_INIT
  #define ulatomic_flag_test_and_set(obj)               ulatomic_exchange_32(obj, 1)
  #define ulatomic_flag_clear(obj)                      ulatomic_store_32(obj, 0)
  #define ulatomic_flag_test_and_set_explicit(obj, ord) ulatomic_exchange_explicit_32(obj, 1, ord)
  #define ulatomic_flag_clear_explicit(obj, ord)        ulatomic_store_explicit_32(obj, 0, ord)
#endif /* ULATOMIC_FLAG_INIT */

/* ulatomiciptr_t */
#if INTPTR_MAX == INT64_MAX && defined(ULATOMIC64_INIT)
  typedef ulatomic64_t ulatomiciptr_t;
  typedef ulatomic64_raw_t ulatomiciptr_raw_t;
  #define ULATOMICIPTR_INIT ULATOMIC64_INIT
  #define ulatomic_store_iptr(obj, val)                             ulatomic_store_64(obj, val)
  #define ulatomic_load_iptr(obj)                                   ulatomic_load_64(obj)
  #define ulatomic_exchange_iptr(obj, val)                          ulatomic_exchange_64(obj, val)
  #define ulatomic_compare_exchange_strong_iptr(obj, expected, val) ulatomic_compare_exchange_strong_64(obj, expected, val)
  #define ulatomic_compare_exchange_weak_iptr(obj, expected, val)   ulatomic_compare_exchange_weak_64(obj, expected, val)
  #define ulatomic_fetch_add_iptr(obj, val)                         ulatomic_fetch_add_64(obj, val)
  #define ulatomic_fetch_sub_iptr(obj, val)                         ulatomic_fetch_sub_64(obj, val)
  #define ulatomic_fetch_or_iptr(obj, val)                          ulatomic_fetch_or_64(obj, val)
  #define ulatomic_fetch_xor_iptr(obj, val)                         ulatomic_fetch_xor_64(obj, val)
  #define ulatomic_fetch_and_iptr(obj, val)                         ulatomic_fetch_and_64(obj, val)
  #define ulatomic_store_explicit_iptr(obj, val, ord)                             ulatomic_store_explicit_64(obj, val, ord)
  #define ulatomic_load_explicit_iptr(obj, ord)                                   ulatomic_load_explicit_64(obj, ord)
  #define ulatomic_exchange_explicit_iptr(obj, val, ord)                          ulatomic_exchange_explicit_64(obj, val, ord)
  #define ulatomic_compare_exchange_strong_explicit_iptr(obj, expected, val, ord) ulatomic_compare_exchange_strong_explicit_64(obj, expected, val, ord)
  #define ulatomic_compare_exchange_weak_explicit_iptr(obj, expected, val, ord)   ulatomic_compare_exchange_weak_explicit_64(obj, expected, val, ord)
  #define ulatomic_fetch_add_explicit_iptr(obj, val, ord)                         ulatomic_fetch_add_explicit_64(obj, val, ord)
  #define ulatomic_fetch_sub_explicit_iptr(obj, val, ord)                         ulatomic_fetch_sub_explicit_64(obj, val, ord)
  #define ulatomic_fetch_or_explicit_iptr(obj, val, ord)                          ulatomic_fetch_or_explicit_64(obj, val, ord)
  #define ulatomic_fetch_xor_explicit_iptr(obj, val, ord)                         ulatomic_fetch_xor_explicit_64(obj, val, ord)
  #define ulatomic_fetch_and_explicit_iptr(obj, val, ord)                         ulatomic_fetch_and_explicit_64(obj, val, ord)
#elif INTPTR_MAX == INT32_MAX && defined(ULATOMIC32_INIT)
  typedef ulatomic32_t ulatomiciptr_t;
  typedef ulatomic32_raw_t ulatomiciptr_raw_t;
  #define ULATOMICIPTR_INIT ULATOMIC32_INIT
  #define ulatomic_store_iptr(obj, val)                             ulatomic_store_32(obj, val)
  #define ulatomic_load_iptr(obj)                                   ulatomic_load_32(obj)
  #define ulatomic_exchange_iptr(obj, val)                          ulatomic_exchange_32(obj, val)
  #define ulatomic_compare_exchange_strong_iptr(obj, expected, val) ulatomic_compare_exchange_strong_32(obj, expected, val)
  #define ulatomic_compare_exchange_weak_iptr(obj, expected, val)   ulatomic_compare_exchange_weak_32(obj, expected, val)
  #define ulatomic_fetch_add_iptr(obj, val)                         ulatomic_fetch_add_32(obj, val)
  #define ulatomic_fetch_sub_iptr(obj, val)                         ulatomic_fetch_sub_32(obj, val)
  #define ulatomic_fetch_or_iptr(obj, val)                          ulatomic_fetch_or_32(obj, val)
  #define ulatomic_fetch_xor_iptr(obj, val)                         ulatomic_fetch_xor_32(obj, val)
  #define ulatomic_fetch_and_iptr(obj, val)                         ulatomic_fetch_and_32(obj, val)
  #define ulatomic_store_explicit_iptr(obj, val, ord)                             ulatomic_store_explicit_32(obj, val, ord)
  #define ulatomic_load_explicit_iptr(obj, ord)                                   ulatomic_load_explicit_32(obj, ord)
  #define ulatomic_exchange_explicit_iptr(obj, val, ord)                          ulatomic_exchange_explicit_32(obj, val, ord)
  #define ulatomic_compare_exchange_strong_explicit_iptr(obj, expected, val, ord) ulatomic_compare_exchange_strong_explicit_32(obj, expected, val, ord)
  #define ulatomic_compare_exchange_weak_explicit_iptr(obj, expected, val, ord)   ulatomic_compare_exchange_weak_explicit_32(obj, expected, val, ord)
  #define ulatomic_fetch_add_explicit_iptr(obj, val, ord)                         ulatomic_fetch_add_explicit_32(obj, val, ord)
  #define ulatomic_fetch_sub_explicit_iptr(obj, val, ord)                         ulatomic_fetch_sub_explicit_32(obj, val, ord)
  #define ulatomic_fetch_or_explicit_iptr(obj, val, ord)                          ulatomic_fetch_or_explicit_32(obj, val, ord)
  #define ulatomic_fetch_xor_explicit_iptr(obj, val, ord)                         ulatomic_fetch_xor_explicit_32(obj, val, ord)
  #define ulatomic_fetch_and_explicit_iptr(obj, val, ord)                         ulatomic_fetch_and_explicit_32(obj, val, ord)
#else
  #if defined(ULATOMIC32_INIT) || defined(ULATOMIC64_INIT)
    #error "ulatomic.h: atomic intptr_t defined failed"
  #endif
#endif

/* ulatomic_spinlock_t */
#ifdef ULATOMIC_FLAG_INIT
  typedef ulatomic_flag_t ulatomic_spinlock_t;
  #define ULATOMIC_SPINLOCK_INIT ULATOMIC_FLAG_INIT
  ul_hapi void ulatomic_spinlock_lock(ulatomic_spinlock_t* lck) {
    while(ulatomic_flag_test_and_set_explicit(lck, ulatomic_memory_order_acquire));
  }
  ul_hapi int ulatomic_spinlock_trylock(ulatomic_spinlock_t* lck) {
    return !ulatomic_flag_test_and_set_explicit(lck, ulatomic_memory_order_acquire);
  }
  ul_hapi void ulatomic_spinlock_unlock(ulatomic_spinlock_t* lck) {
    ulatomic_flag_clear_explicit(lck, ulatomic_memory_order_release);
  }
#endif

#endif /* ULATOMIC_H */
