/*
Endian order


# Dependences
  <stdint.h>/"ulstdint.h"


# APIs
  - UL_ENDIAN_BIG/UL_ENDIAN_LITTLE
    Indicates the platform is big endian or little endian.
  - uint16_t ul_bswap16(uint16_t v);
  - uint32_t ul_bswap32(uint32_t v);
  - uint64_t ul_bswap64(uint64_t v);
  - void ul_bswap16_multi(void* dest, const void* src, size_t num);
  - void ul_bswap32_multi(void* dest, const void* src, size_t num);
  - void ul_bswap64_multi(void* dest, const void* src, size_t num);
  - T ul_bswap_*(T val);
  - T ul_trans_*_le(T val);
  - T ul_trans_*_be(T val);


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

#ifndef ULENDIAN_H
#define ULENDIAN_H

#if defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)
  #if defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
    #define UL_ENDIAN_BIG
  #elif defined(BYTE_ORDER) && BYTE_ORDER == LITTLE_ENDIAN
    #define UL_ENDIAN_LITTLE
  #endif
#elif defined(BIG_ENDIAN)
  #define UL_ENDIAN_BIG
#elif defined(LITTLE_ENDIAN)
  #define UL_ENDIAN_LITTLE
#endif

#if defined(_BIG_ENDIAN) && defined(_LITTLE_ENDIAN)
  #if defined(_BYTE_ORDER) && _BYTE_ORDER == _BIG_ENDIAN
    #define UL_ENDIAN_BIG
  #elif defined(_BYTE_ORDER) && _BYTE_ORDER == _LITTLE_ENDIAN
    #define UL_ENDIAN_LITTLE
  #endif
#elif defined(_BIG_ENDIAN)
  #define UL_ENDIAN_BIG
#elif defined(_LITTLE_ENDIAN)
  #define UL_ENDIAN_LITTLE
#endif

#if defined(__BIG_ENDIAN) && defined(__LITTLE_ENDIAN)
  #if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
    #define UL_ENDIAN_BIG
  #elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN
    #define UL_ENDIAN_LITTLE
  #endif
#elif defined(__BIG_ENDIAN)
  #define UL_ENDIAN_BIG
#elif defined(__LITTLE_ENDIAN)
  #define UL_ENDIAN_LITTLE
#endif

#if defined(__BIG_ENDIAN__) && defined(__LITTLE_ENDIAN__)
  #if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __BIG_ENDIAN__
    #define UL_ENDIAN_BIG
  #elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __LITTLE_ENDIAN__
    #define UL_ENDIAN_LITTLE
  #endif
#elif defined(__BIG_ENDIAN__)
  #define UL_ENDIAN_BIG
#elif defined(__LITTLE_ENDIAN__)
  #define UL_ENDIAN_LITTLE
#endif

#if !defined(UL_ENDIAN_BIG) && !defined(UL_ENDIAN_LITTLE)
  #if defined(__alpha__) || defined(__alpha) || defined(i386)       || \
      defined(__i386__)  || defined(_M_I86)  || defined(_M_IX86)    || \
      defined(__OS2__)   || defined(sun386)  || defined(__TURBOC__) || \
      defined(vax)       || defined(vms)     || defined(VMS)        || \
      defined(__VMS)     || defined(_M_X64)
    #define UL_ENDIAN_LITTLE
  #elif defined(AMIGA) || defined(applec) || defined(__AS400__) || \
        defined(_CRAY)    || defined(__hppa)    || defined(__hp9000)   || \
        defined(ibm370)   || defined(mc68000)   || defined(m68k)       || \
        defined(__MRC__)  || defined(__MVS__)   || defined(__MWERKS__) || \
        defined(sparc)    || defined(__sparc)   || defined(SYMANTEC_C) || \
        defined(__VOS__)  || defined(__TIGCC__) || defined(__TANDEM)   || \
        defined(THINK_C)  || defined(__VMCMS__) || defined(_AIX)       || \
        defined(__s390__) || defined(__s390x__) || defined(__zarch__)
    #define UL_ENDIAN_BIG
  #elif defined(__arm__)
    #ifdef __BIG_ENDIAN
      #define UL_ENDIAN_BIG
    #else
      #define UL_ENDIAN_LITTLE
    #endif
  #endif
#endif

#if !defined(UL_ENDIAN_BIG) && !defined(UL_ENDIAN_LITTLE)
  #error "ulendian.h: undetected endian order"
#endif
#if defined(UL_ENDIAN_BIG) && defined(UL_ENDIAN_LITTLE)
  #error "ulendian.h: big-endian and little-endian cannot coexist"
#endif

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
    #if defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200100L)
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
#else
  #include "ulstdint.h" /* polyfill */
#endif

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

#include <limits.h>
#include <assert.h>

ul_hapi uint16_t (ul_bswap16)(uint16_t v) {
  return ul_static_cast(uint16_t, (v >> 8) | (v << 8));
}
ul_hapi uint32_t (ul_bswap32)(uint32_t v) {
  return (v >> 24)
    | ((v & 0x00FF0000u) >> 8)
    | ((v & 0x0000FF00u) << 8)
    | (v << 24);
}
#ifdef UINT64_MAX
  ul_hapi uint64_t (ul_bswap64)(uint64_t v) {
    return (v >> 56)
      | ((v & (ul_static_cast(uint64_t, 0xFFu) << 48)) >> 40)
      | ((v & (ul_static_cast(uint64_t, 0xFFu) << 40)) >> 24)
      | ((v & (ul_static_cast(uint64_t, 0xFFu) << 32)) >> 8)
      | ((v & 0xFF000000u) << 8)
      | ((v & 0x00FF0000u) << 24)
      | ((v & 0x0000FF00u) << 40)
      | (v << 56);
  }
#endif

#ifdef __has_builtin
  #if __has_builtin(__builtin_bswap16)
    #define ul_bswap16(x) __builtin_bswap16(x)
  #endif
  #if __has_builtin(__builtin_bswap32)
    #define ul_bswap32(x) __builtin_bswap32(x)
  #endif
  #if __has_builtin(__builtin_bswap64)
    #define ul_bswap64(x) __builtin_bswap64(x)
  #endif
#endif

ul_hapi void ul_bswap16_multi(void* dest, const void* src, size_t num) {
  uint16_t* rp = ul_reinterpret_cast(uint16_t*, dest);
  const uint16_t* sp = ul_reinterpret_cast(const uint16_t*, src);
  assert((ul_reinterpret_cast(intptr_t, dest) & 0x1) == 0);
  assert((ul_reinterpret_cast(intptr_t, src) & 0x1) == 0);
  while(num--) { *rp++ = ul_bswap16(*sp); ++sp; }
}
ul_hapi void ul_bswap32_multi(void* dest, const void* src, size_t num) {
  uint32_t* rp = ul_reinterpret_cast(uint32_t*, dest);
  const uint32_t* sp = ul_reinterpret_cast(const uint32_t*, src);
  assert((ul_reinterpret_cast(intptr_t, dest) & 0x3) == 0);
  assert((ul_reinterpret_cast(intptr_t, src) & 0x3) == 0);
  while(num--) { *rp++ = ul_bswap32(*sp); ++sp; }
}
#ifdef UINT64_MAX
  ul_hapi void ul_bswap64_multi(void* dest, const void* src, size_t num) {
    uint64_t* rp = ul_reinterpret_cast(uint64_t*, dest);
    const uint64_t* sp = ul_reinterpret_cast(const uint64_t*, src);
    assert((ul_reinterpret_cast(intptr_t, dest) & 0x7) == 0);
    assert((ul_reinterpret_cast(intptr_t, src) & 0x7) == 0);
    while(num--) { *rp++ = ul_bswap64(*sp); ++sp; }
  }
#endif

ul_hapi char ul_bswap_char(char c) { return c; }
ul_hapi signed char ul_bswap_schar(signed char c) { return c; }
ul_hapi unsigned char ul_bswap_uchar(unsigned char c) { return c; }

#if SHRT_MAX == INT16_MAX
  ul_hapi signed short ul_bswap_short(signed short val) {
    return ul_static_cast(signed short, ul_bswap16(ul_static_cast(uint16_t, val)));
  }
  ul_hapi unsigned short ul_bswap_ushort(unsigned short val) {
    return ul_static_cast(unsigned short, ul_bswap16(ul_static_cast(uint16_t, val)));
  }
#elif SHRT_MAX == INT32_MAX
  ul_hapi signed short ul_bswap_short(signed short val) {
    return ul_static_cast(signed short, ul_bswap32(ul_static_cast(uint32_t, val)));
  }
  ul_hapi unsigned short ul_bswap_ushort(unsigned short val) {
    return ul_static_cast(unsigned short, ul_bswap32(ul_static_cast(uint32_t, val)));
  }
#elif SHRT_MAX == INT64_MAX
  ul_hapi signed short ul_bswap_short(signed short val) {
    return ul_static_cast(signed short, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
  ul_hapi unsigned short ul_bswap_ushort(unsigned short val) {
    return ul_static_cast(unsigned short, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
#else
  #error "ulendian.h: `short` should be 16-bit, 32-bit or 64-bit"
#endif

#if INT_MAX == INT16_MAX
  ul_hapi signed int ul_bswap_int(signed int val) {
    return ul_static_cast(signed int, ul_bswap16(ul_static_cast(uint16_t, val)));
  }
  ul_hapi unsigned int ul_bswap_uint(unsigned int val) {
    return ul_static_cast(unsigned int, ul_bswap16(ul_static_cast(uint16_t, val)));
  }
#elif INT_MAX == INT32_MAX
  ul_hapi signed int ul_bswap_int(signed int val) {
    return ul_static_cast(signed int, ul_bswap32(ul_static_cast(uint32_t, val)));
  }
  ul_hapi unsigned int ul_bswap_uint(unsigned int val) {
    return ul_static_cast(unsigned int, ul_bswap32(ul_static_cast(uint32_t, val)));
  }
#elif INT_MAX == INT64_MAX
  ul_hapi signed int ul_bswap_int(signed int val) {
    return ul_static_cast(signed int, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
  ul_hapi unsigned int ul_bswap_uint(unsigned int val) {
    return ul_static_cast(unsigned int, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
#else
  #error "ulendian.h: `int` should be 16-bit, 32-bit or 64-bit"
#endif

#if LONG_MAX == INT32_MAX
  ul_hapi signed long ul_bswap_long(signed long val) {
    return ul_static_cast(signed long, ul_bswap32(ul_static_cast(uint32_t, val)));
  }
  ul_hapi unsigned long ul_bswap_ulong(unsigned long val) {
    return ul_static_cast(unsigned long, ul_bswap32(ul_static_cast(uint32_t, val)));
  }
#elif LONG_MAX == INT64_MAX
  ul_hapi signed long ul_bswap_long(signed long val) {
    return ul_static_cast(signed long, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
  ul_hapi unsigned long ul_bswap_ulong(unsigned long val) {
    return ul_static_cast(unsigned long, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
#else
  #error "ulendian.h: `long` should be 32-bit or 64-bit"
#endif

#if !defined(LLONG_MAX)
  /* define nothing */
#elif defined(INT64_MAX) && LLONG_MAX == INT64_MAX
  ul_hapi signed long long ul_bswap_llong(signed long long val) {
    return ul_static_cast(signed long long, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
  ul_hapi unsigned long long ul_bswap_ullong(unsigned long long val) {
    return ul_static_cast(unsigned long long, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
#else
  #error "ulendian.h: `long long` should be 64-bit"
#endif

ul_hapi int8_t ul_bswap_i8(int8_t val) { return val; }
ul_hapi int16_t ul_bswap_i16(int16_t val) {
  return ul_static_cast(int16_t, ul_bswap16(ul_static_cast(uint16_t, val)));
}
ul_hapi int32_t ul_bswap_i32(int32_t val) {
  return ul_static_cast(int32_t, ul_bswap32(ul_static_cast(uint32_t, val)));
}

ul_hapi uint8_t ul_bswap_u8(uint8_t val) { return val; }
ul_hapi uint16_t ul_bswap_u16(uint16_t val) { return ul_bswap16(val); }
ul_hapi uint32_t ul_bswap_u32(uint32_t val) { return ul_bswap32(val); }

#ifdef INT64_MAX
  ul_hapi int64_t ul_bswap_i64(int64_t val) {
    return ul_static_cast(int64_t, ul_bswap64(ul_static_cast(uint64_t, val)));
  }
  ul_hapi uint64_t ul_bswap_u64(uint64_t val) { return ul_bswap64(val); }
#endif

ul_hapi float ul_bswap_float(float val) {
  union { float f; uint32_t i; } x;
  x.f = val;
  x.i = ul_bswap32(x.i);
  return x.f;
}
ul_hapi double ul_bswap_double(double val) {
#ifdef UINT64_MAX
  union { double f; uint64_t i; } x;
  x.f = val;
  x.i = ul_bswap64(x.i);
  return x.f;
#else
  union { double f; uint32_t i[2]; } x;
  uint32_t t;
  x.f = val;
  t = ul_bswap32(x.i[0]);
  x.i[0] = ul_bswap32(x.i[1]);
  x.i[1] = t;
  return x.f;
#endif
}

#define ul_trans_char_le(c)  (c)
#define ul_trans_schar_le(c) (c)
#define ul_trans_uchar_le(c) (c)
#define ul_trans_i8_le(c)    (c)
#define ul_trans_u8_le(c)    (c)

#define ul_trans_char_be(c)  (c)
#define ul_trans_schar_be(c) (c)
#define ul_trans_uchar_be(c) (c)
#define ul_trans_i8_be(c)    (c)
#define ul_trans_u8_be(c)    (c)

#ifdef UL_ENDIAN_LITTLE
  #define ul_trans_short_le(val) (val)
  #define ul_trans_int_le(val)   (val)
  #define ul_trans_long_le(val)  (val)

  #define ul_trans_ushort_le(val) (val)
  #define ul_trans_uint_le(val)   (val)
  #define ul_trans_ulong_le(val)  (val)

  #define ul_trans_float_le(val)  (val)
  #define ul_trans_double_le(val) (val)

  #define ul_trans_i16_le(val) (val)
  #define ul_trans_i32_le(val) (val)

  #define ul_trans_u16_le(val) (val)
  #define ul_trans_u32_le(val) (val)

  #ifdef INT64_MAX
    #define ul_trans_i64_le(val) (val)
    #define ul_trans_u64_le(val) (val)
  #endif

  #ifdef LLONG_MAX
    #define ul_trans_llong_le(val)  (val)
    #define ul_trans_ullong_le(val) (val)
  #endif


  #define ul_trans_short_be(val) ul_bswap_short(val)
  #define ul_trans_int_be(val)   ul_bswap_int(val)
  #define ul_trans_long_be(val)  ul_bswap_long(val)

  #define ul_trans_ushort_be(val) ul_bswap_ushort(val)
  #define ul_trans_uint_be(val)   ul_bswap_uint(val)
  #define ul_trans_ulong_be(val)  ul_bswap_ulong(val)

  #define ul_trans_float_be(val)  ul_bswap_float(val)
  #define ul_trans_double_be(val) ul_bswap_double(val)

  #define ul_trans_i16_be(val) ul_bswap_i16(val)
  #define ul_trans_i32_be(val) ul_bswap_i32(val)

  #define ul_trans_u16_be(val) ul_bswap_u16(val)
  #define ul_trans_u32_be(val) ul_bswap_u32(val)

  #ifdef INT64_MAX
    #define ul_trans_i64_be(val) ul_bswap_i64(val)
    #define ul_trans_u64_be(val) ul_bswap_u64(val)
  #endif

  #ifdef LLONG_MAX
    #define ul_trans_llong_be(val)  ul_bswap_llong(val)
    #define ul_trans_ullong_be(val) ul_bswap_ullong(val)
  #endif
#else
  #define ul_trans_short_be(val) (val)
  #define ul_trans_int_be(val)   (val)
  #define ul_trans_long_be(val)  (val)

  #define ul_trans_ushort_be(val) (val)
  #define ul_trans_uint_be(val)   (val)
  #define ul_trans_ulong_be(val)  (val)

  #define ul_trans_float_be(val)  (val)
  #define ul_trans_double_be(val) (val)

  #define ul_trans_i16_be(val) (val)
  #define ul_trans_i32_be(val) (val)

  #define ul_trans_u16_be(val) (val)
  #define ul_trans_u32_be(val) (val)

  #ifdef INT64_MAX
    #define ul_trans_i64_be(val) (val)
    #define ul_trans_u64_be(val) (val)
  #endif

  #ifdef LLONG_MAX
    #define ul_trans_llong_be(val)  (val)
    #define ul_trans_ullong_be(val) (val)
  #endif


  #define ul_trans_short_le(val) ul_bswap_short(val)
  #define ul_trans_int_le(val)   ul_bswap_int(val)
  #define ul_trans_long_le(val)  ul_bswap_long(val)

  #define ul_trans_ushort_le(val) ul_bswap_ushort(val)
  #define ul_trans_uint_le(val)   ul_bswap_uint(val)
  #define ul_trans_ulong_le(val)  ul_bswap_ulong(val)

  #define ul_trans_float_le(val)  ul_bswap_float(val)
  #define ul_trans_double_le(val) ul_bswap_double(val)

  #define ul_trans_i16_le(val) ul_bswap_i16(val)
  #define ul_trans_i32_le(val) ul_bswap_i32(val)

  #define ul_trans_u16_le(val) ul_bswap_u16(val)
  #define ul_trans_u32_le(val) ul_bswap_u32(val)

  #ifdef INT64_MAX
    #define ul_trans_i64_le(val) ul_bswap_i64(val)
    #define ul_trans_u64_le(val) ul_bswap_u64(val)
  #endif

  #ifdef LLONG_MAX
    #define ul_trans_llong_le(val)  ul_bswap_llong(val)
    #define ul_trans_ullong_le(val) ul_bswap_ullong(val)
  #endif
#endif

#endif /* ULENDIAN_H */
