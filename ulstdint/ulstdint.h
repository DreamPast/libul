/*
Polyfill for C99 <stdint.h>


# Dependencies
  The platform is 8 bits per byte.


# Note
  The file shouldn't be included if <stdint.h> is supported.
  Macros related to these types is unable to be defined: size_t, ptrdiff_t, wint_t, wchar_t.


# License
  The MIT License (MIT)

  Copyright (C) 2023-2025 Jin Cai

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

#ifndef ULSTDINT_H
#define ULSTDINT_H

#include <stddef.h>
#include <limits.h>

#ifndef UL_HAS_STDINT_H
  #if defined(__GLIBC__) && (__GLIBC__ * 100 + __GLIBC_MINOR__) >= 201
    #if defined(__GNUC__) || (__GLIBC__ * 100 + __GLIBC_MINOR__) >= 205
      #define UL_HAS_STDINT_H
    #endif
  #endif
  #if defined(__MINGW32__)
    #include <_mingw.h>
    #if (__MINGW32_MAJOR_VERSION > 2 || (__MINGW32_MAJOR_VERSION == 2 && __MINGW32_MINOR_VERSION >= 0))
      #define UL_HAS_STDINT_H
    #endif
  #endif
  #if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
    #include <unistd.h>
    #if defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200100L)
      #define UL_HAS_STDINT_H
    #endif
  #endif
  #if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(__cplusplus) && __cplusplus >= 201103L)
    #define UL_HAS_STDINT_H
  #endif
  #if (defined(_MSC_VER) && _MSC_VER >= 1600 /* Visual Studio 2010 */) || (defined(__CYGWIN__) && defined(_STDINT_H))
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
#endif

#if CHAR_BIT != 8
  #error "ulstdint.h: the platform is not 8 bits per byte"
#endif

#if !defined(UINT8_MAX)
  #if defined(SCHAR_MAX) && SCHAR_MAX == 0x7F
typedef signed char int8_t;
typedef unsigned char uint8_t;
    #define UINT8_MAX UCHAR_MAX
    #define INT8_MAX SCHAR_MAX
    #define INT8_MIN SCHAR_MIN
  #endif
#endif


#if !defined(UINT16_MAX)
  #if defined(SHRT_MAX) && SHRT_MAX == 0x7FFF
typedef signed short int16_t;
typedef unsigned short uint16_t;
    #define UINT16_MAX USHRT_MAX
    #define INT16_MAX SHRT_MAX
    #define INT16_MIN SHRT_MIN
  #endif
#endif

#if !defined(UINT32_MAX)
  #if defined(SHRT_MAX) && SHRT_MAX == 0x7FFFFFFF
typedef signed short int32_t;
typedef unsigned short uint32_t;
    #define UINT32_MAX USHRT_MAX
    #define INT32_MAX SHRT_MAX
    #define INT32_MIN SHRT_MIN
  #elif defined(INT_MAX) && INT_MAX == 0x7FFFFFFF
typedef signed int int32_t;
typedef unsigned int uint32_t;
    #define UINT32_MAX UINT_MAX
    #define INT32_MAX INT_MAX
    #define INT32_MIN INT_MIN
  #elif defined(LONG_MAX) && LONG_MAX == 0x7FFFFFFF
typedef signed long int32_t;
typedef unsigned long uint32_t;
    #define UINT32_MAX ULONG_MAX
    #define INT32_MAX LONG_MAX
    #define INT32_MIN LONG_MIN
  #elif defined(LLONG_MAX) && LLONG_MAX == 0x7FFFFFFF
typedef signed long long int32_t;
typedef unsigned long long uint32_t;
    #define UINT32_MAX ULLONG_MAX
    #define INT32_MAX LLONG_MAX
    #define INT32_MIN LLONG_MIN
  #endif
#endif

#if !defined(UINT64_MAX)
  #if !defined(UINT64_MAX) && defined(SHRT_MAX) && ((SHRT_MAX >> 61) == 3)
    #if SHRT_MAX == 0x7FFFFFFFFFFFFFFF
typedef signed short int64_t;
typedef unsigned short uint64_t;
      #define UINT64_MAX USHRT_MAX
      #define INT64_MAX SHRT_MAX
      #define INT64_MIN SHRT_MIN
    #endif
  #endif
  #if !defined(UINT64_MAX) && defined(INT_MAX) && ((INT_MAX >> 61) == 3)
    #if INT_MAX == 0x7FFFFFFFFFFFFFFF
typedef signed int int64_t;
typedef unsigned int uint64_t;
      #define UINT64_MAX UINT_MAX
      #define INT64_MAX INT_MAX
      #define INT64_MIN INT_MIN
    #endif
  #endif
  #if !defined(UINT64_MAX) && defined(LONG_MAX) && ((LONG_MAX >> 61) == 3)
    #if LONG_MAX == 0x7FFFFFFFFFFFFFFF
typedef signed long int64_t;
typedef unsigned long uint64_t;
      #define UINT64_MAX ULONG_MAX
      #define INT64_MAX LONG_MAX
      #define INT64_MIN LONG_MIN
    #endif
  #endif
  #if !defined(UINT64_MAX) && defined(LLONG_MAX) && ((LLONG_MAX >> 61) == 3)
    #if LLONG_MAX == 0x7FFFFFFFFFFFFFFF
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
      #define UINT64_MAX ULLONG_MAX
      #define INT64_MAX LLONG_MAX
      #define INT64_MIN LLONG_MIN
    #endif
  #endif
  #if !defined(UINT64_MAX) && defined(_WIN32)
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
    #define UINT64_MAX 0xFFFFFFFFFFFFFFFFui64
    #define INT64_MAX 0x7FFFFFFFFFFFFFFFi64
    #define INT64_MIN 0x8000000000000000i64
  #endif
#endif



#if !defined(UINT_LEAST8_MAX)
  #if !defined(SCHAR_MAX) || SCHAR_MAX < 0x7F
    #error "ulstdint.h: not ASCI/ISO C"
  #endif
typedef signed char int_least8_t;
typedef unsigned char uint_least8_t;
  #define UINT_LEAST8_MAX UCHAR_MAX
  #define INT_LEAST8_MAX SCHAR_MAX
  #define INT_LEAST8_MIN SCHAR_MIN
  #define INT8_C(v) (v)
  #define UINT8_C(v) (v##U)
#endif

#if !defined(UINT_LEAST16_MAX)
  #if !defined(SHRT_MAX) || SHRT_MAX < 0x7FFF
    #error "ulstdint.h: not ASCI/ISO C"
  #endif
typedef signed short int_least16_t;
typedef unsigned short uint_least16_t;
  #define UINT_LEAST16_MAX USHRT_MAX
  #define INT_LEAST16_MAX SHRT_MAX
  #define INT_LEAST16_MIN SHRT_MIN
  #define INT16_C(v) (v)
  #define UINT16_C(v) (v##U)
#endif

#if !defined(UINT_LEAST32_MAX)
  #if !defined(LONG_MAX) || LONG_MAX < 0x7FFFFFFF
    #error "ulstdint.h: not ASCI/ISO C"
  #endif
  #if defined(SHRT_MAX) && SHRT_MAX >= 0x7FFFFFFF
typedef signed short int_least32_t;
typedef unsigned short uint_least32_t;
    #define UINT_LEAST32_MAX USHRT_MAX
    #define INT_LEAST32_MAX SHRT_MAX
    #define INT_LEAST32_MIN SHRT_MIN
    #define INT32_C(v) (v)
    #define UINT32_C(v) (v##U)
  #elif defined(INT_MAX) && INT_MAX >= 0x7FFFFFFF
typedef signed int int_least32_t;
typedef unsigned int uint_least32_t;
    #define UINT_LEAST32_MAX UINT_MAX
    #define INT_LEAST32_MAX INT_MAX
    #define INT_LEAST32_MIN INT_MIN
    #define INT32_C(v) (v)
    #define UINT32_C(v) (v##U)
  #else
typedef signed long int_least32_t;
typedef unsigned long uint_least32_t;
    #define UINT_LEAST32_MAX ULONG_MAX
    #define INT_LEAST32_MAX LONG_MAX
    #define INT_LEAST32_MIN LONG_MIN
    #define INT32_C(v) (v##L)
    #define UINT32_C(v) (v##UL)
  #endif
#endif

#if !defined(UINT_LEAST64_MAX)
  #if !defined(UINT_LEAST64_MAX) && defined(SHRT_MAX) && ((SHRT_MAX >> 61) >= 3)
    #if SHRT_MAX >= 0x7FFFFFFFFFFFFFFF
typedef signed short int_least64_t;
typedef unsigned short uint_least64_t;
      #define UINT_LEAST64_MAX USHRT_MAX
      #define INT_LEAST64_MAX SHRT_MAX
      #define INT_LEAST64_MIN SHRT_MIN
      #define INT64_C(v) (v)
      #define UINT64_C(v) (v##U)
    #endif
  #endif
  #if !defined(UINT_LEAST64_MAX) && defined(INT_MAX) && ((INT_MAX >> 61) >= 3)
    #if INT_MAX >= 0x7FFFFFFFFFFFFFFF
typedef signed int int_least64_t;
typedef unsigned int uint_least64_t;
      #define UINT_LEAST64_MAX UINT_MAX
      #define INT_LEAST64_MAX INT_MAX
      #define INT_LEAST64_MIN INT_MIN
      #define INT64_C(v) (v)
      #define UINT64_C(v) (v##U)
    #endif
  #endif
  #if !defined(UINT_LEAST64_MAX) && defined(LONG_MAX) && ((LONG_MAX >> 61) >= 3)
    #if LONG_MAX >= 0x7FFFFFFFFFFFFFFF
typedef signed long int_least64_t;
typedef unsigned long uint_least64_t;
      #define UINT_LEAST64_MAX ULONG_MAX
      #define INT_LEAST64_MAX LONG_MAX
      #define INT_LEAST64_MIN LONG_MIN
      #define INT64_C(v) (v##L)
      #define UINT64_C(v) (v##UL)
    #endif
  #endif
  #if !defined(UINT_LEAST64_MAX) && defined(LLONG_MAX) && ((LLONG_MAX >> 61) >= 3)
    #if LLONG_MAX >= 0x7FFFFFFFFFFFFFFF
typedef signed long long int_least64_t;
typedef unsigned long long uint_least64_t;
      #define UINT_LEAST64_MAX ULLONG_MAX
      #define INT_LEAST64_MAX LLONG_MAX
      #define INT_LEAST64_MIN LLONG_MIN
      #define INT64_C(v) (v##LL)
      #define UINT64_C(v) (v##ULL)
    #endif
  #endif
  #if !defined(UINT_LEAST64_MAX) && defined(_WIN32)
typedef __int64 int_least64_t;
typedef unsigned __int64 uint_least64_t;
    #define UINT_LEAST64_MAX 0xFFFFFFFFFFFFFFFFui64
    #define INT_LEAST64_MAX 0x7FFFFFFFFFFFFFFFi64
    #define INT_LEAST64_MIN 0x8000000000000000i64
    #define INT64_C(v) (v##i64)
    #define UINT64_C(v) (v##ui64)
  #endif
#endif



#if !defined(UINT_FAST8_MAX)
  #if !defined(SCHAR_MAX) || SCHAR_MAX < 0x7F
    #error "ulstdint.h: not ASCI/ISO C"
  #endif
typedef signed char int_fast8_t;
typedef unsigned char uint_fast8_t;
  #define UINT_FAST8_MAX UCHAR_MAX
  #define INT_FAST8_MAX SCHAR_MAX
  #define INT_FAST8_MIN SCHAR_MIN
#endif

#if !defined(UINT_FAST16_MAX)
  #if !defined(SHRT_MAX) || SHRT_MAX < 0x7FFF
    #error "ulstdint.h: not ASCI/ISO C"
  #endif
typedef signed short int_fast16_t;
typedef unsigned short uint_fast16_t;
  #define UINT_FAST16_MAX USHRT_MAX
  #define INT_FAST16_MAX SHRT_MAX
  #define INT_FAST16_MIN SHRT_MIN
#endif

#if !defined(UINT_FAST32_MAX)
  #if defined(SHRT_MAX) && SHRT_MAX >= 0x7FFFFFFF
typedef signed short int_fast32_t;
typedef unsigned short uint_fast32_t;
    #define UINT_FAST32_MAX USHRT_MAX
    #define INT_FAST32_MAX SHRT_MAX
    #define INT_FAST32_MIN SHRT_MIN
  #elif defined(INT_MAX) && INT_MAX >= 0x7FFFFFFF
typedef signed int int_fast32_t;
typedef unsigned int uint_fast32_t;
    #define UINT_FAST32_MAX UINT_MAX
    #define INT_FAST32_MAX INT_MAX
    #define INT_FAST32_MIN INT_MIN
  #elif defined(LONG_MAX) && LONG_MAX >= 0x7FFFFFFF
typedef signed long int_fast32_t;
typedef unsigned long uint_fast32_t;
    #define UINT_FAST32_MAX ULONG_MAX
    #define INT_FAST32_MAX LONG_MAX
    #define INT_FAST32_MIN LONG_MIN
  #elif defined(LLONG_MAX) && LLONG_MAX >= 0x7FFFFFFF
typedef signed long long int_fast32_t;
typedef unsigned long long uint_fast32_t;
    #define UINT_FAST32_MAX ULLONG_MAX
    #define INT_FAST32_MAX LLONG_MAX
    #define INT_FAST32_MIN LLONG_MIN
  #endif
#endif

#if !defined(UINT_FAST64_MAX)
  #if !defined(UINT_FAST64_MAX) && defined(SHRT_MAX) && ((SHRT_MAX >> 61) >= 3)
    #if SHRT_MAX >= 0x7FFFFFFFFFFFFFFF
typedef signed short int_fast64_t;
typedef unsigned short uint_fast64_t;
      #define UINT_FAST64_MAX USHRT_MAX
      #define INT_FAST64_MAX SHRT_MAX
      #define INT_FAST64_MIN SHRT_MIN
    #endif
  #endif
  #if !defined(UINT_FAST64_MAX) && defined(INT_MAX) && ((INT_MAX >> 61) >= 3)
    #if INT_MAX >= 0x7FFFFFFFFFFFFFFF
typedef signed int int_fast64_t;
typedef unsigned int uint_fast64_t;
      #define UINT_FAST64_MAX UINT_MAX
      #define INT_FAST64_MAX INT_MAX
      #define INT_FAST64_MIN INT_MIN
    #endif
  #endif
  #if !defined(UINT_FAST64_MAX) && defined(LONG_MAX) && ((LONG_MAX >> 61) >= 3)
    #if LONG_MAX >= 0x7FFFFFFFFFFFFFFF
typedef signed long int_fast64_t;
typedef unsigned long uint_fast64_t;
      #define UINT_FAST64_MAX ULONG_MAX
      #define INT_FAST64_MAX LONG_MAX
      #define INT_FAST64_MIN LONG_MIN
    #endif
  #endif
  #if !defined(UINT_FAST64_MAX) && defined(LLONG_MAX) && ((LLONG_MAX >> 61) >= 3)
    #if LLONG_MAX >= 0x7FFFFFFFFFFFFFFF
typedef signed long long int_fast64_t;
typedef unsigned long long uint_fast64_t;
      #define UINT_FAST64_MAX ULLONG_MAX
      #define INT_FAST64_MAX LLONG_MAX
      #define INT_FAST64_MIN LLONG_MIN
    #endif
  #endif
  #if !defined(UINT_FAST64_MAX) && defined(_WIN32)
typedef __int64 int_fast64_t;
typedef unsigned __int64 uint_fast64_t;
    #define UINT_FAST64_MAX 0xFFFFFFFFFFFFFFFFui64
    #define INT_FAST64_MAX 0x7FFFFFFFFFFFFFFFi64
    #define INT_FAST64_MIN 0x8000000000000000i64
  #endif
#endif



#if !defined(UINTMAX_MAX)
  #if defined(ULLONG_MAX)
typedef signed long long intmax_t;
typedef unsigned long long uintmax_t;
    #define UINTMAX_MAX ULLONG_MAX
    #define INTMAX_MAX LLONG_MAX
    #define INTMAX_MIN LLONG_MIN
    #define INTMAX_C(v) (v##LL)
    #define UINTMAX_C(v) (v##ULL)
  #elif defined(_WIN32)
typedef __int64 intmax_t;
typedef unsigned __int64 uintmax_t;
    #define UINTMAX_MAX 0xFFFFFFFFFFFFFFFFui64
    #define INTMAX_MAX 0x7FFFFFFFFFFFFFFFi64
    #define INTMAX_MIN 0x8000000000000000i64
    #define INTMAX_C(v) (v##i64)
    #define UINTMAX_C(v) (v##ui64)
  #else
typedef signed long intmax_t;
typedef unsigned long uintmax_t;
    #define UINTMAX_MAX ULONG_MAX
    #define INTMAX_MAX LONG_MAX
    #define INTMAX_MIN LONG_MIN
    #define INTMAX_C(v) (v##L)
    #define UINTMAX_C(v) (v##UL)
  #endif
#endif



#if !defined(UINTPTR_MAX)
  /* there's no generic way to detect pointer's size in macro */
  #if defined(_WIN32) || defined(_WIN64)
    #if _WIN64
      #ifndef _INTPTR_T_DEFINED
typedef __int64 intptr_t;
      #endif
      #ifndef _UINTPTR_T_DEFINED
typedef unsigned __int64 uintptr_t;
      #endif
      #define UINTPTR_MAX 0xFFFFFFFFFFFFFFFFui64
      #define INTPTR_MAX 0x7FFFFFFFFFFFFFFFi64
      #define INTPTR_MIN 0x8000000000000000i64
      #define __ULSTDINT_PTRMAX 0
    #else
      #ifndef _INTPTR_T_DEFINED
typedef signed int intptr_t;
      #endif
      #ifndef _UINTPTR_T_DEFINED
typedef unsigned int uintptr_t;
      #endif
      #define UINTPTR_MAX UINT_MAX
      #define INTPTR_MAX INT_MAX
      #define INTPTR_MIN INT_MIN
      #define __ULSTDINT_PTRMAX 0
    #endif
  #elif defined(__SIZEOF_POINTER__)
    #if __SIZEOF_POINTER__ == 8
      #define __ULSTDINT_PTRMAX 0x7FFFFFFFFFFFFFFF
    #elif __SIZEOF_POINTER__ == 4
      #define __ULSTDINT_PTRMAX 0x7FFFFFFF
    #elif __SIZEOF_POINTER__ == 2
      #define __ULSTDINT_PTRMAX 0x7FFF
    #elif __SIZEOF_POINTER__ == 1
      #define __ULSTDINT_PTRMAX 0x7F
    #endif
  #else
    /* can't measure pointer's size, suppose intmax_t and uintmax_t is able to store pointer */
  #endif

  #if defined(LLONG_MAX) && (!defined(__ULSTDINT_PTRMAX) || __ULSTDINT_PTRMAX == LLONG_MAX)
typedef signed long long intptr_t;
typedef unsigned long long uintptr_t;
    #define UINTPTR_MAX ULLONG_MAX
    #define INTPTR_MAX LLONG_MAX
    #define INTPTR_MIN LLONG_MIN
  #elif defined(LONG_MAX) && (!defined(__ULSTDINT_PTRMAX) || __ULSTDINT_PTRMAX == LONG_MAX)
typedef signed long intptr_t;
typedef unsigned long uintptr_t;
    #define UINTPTR_MAX ULONG_MAX
    #define INTPTR_MAX LONG_MAX
    #define INTPTR_MIN LONG_MIN
  #elif defined(INT_MAX) && (!defined(__ULSTDINT_PTRMAX) || __ULSTDINT_PTRMAX == INT_MAX)
typedef signed int intptr_t;
typedef unsigned int uintptr_t;
    #define UINTPTR_MAX UINT_MAX
    #define INTPTR_MAX INT_MAX
    #define INTPTR_MIN INT_MIN
  #elif defined(SHRT_MAX) && (!defined(__ULSTDINT_PTRMAX) || __ULSTDINT_PTRMAX == SHRT_MAX)
typedef signed short intptr_t;
typedef unsigned short uintptr_t;
    #define UINTPTR_MAX USHRT_MAX
    #define INTPTR_MAX SHRT_MAX
    #define INTPTR_MIN SHRT_MIN
  #elif defined(SCHAR_MAX) && (!defined(__ULSTDINT_PTRMAX) || __ULSTDINT_PTRMAX == SCHAR_MAX)
typedef signed char intptr_t;
typedef unsigned char uintptr_t;
    #define UINTPTR_MAX UCHAR_MAX
    #define INTPTR_MAX SCHAR_MAX
    #define INTPTR_MIN SCHAR_MIN
  #endif
  #undef __ULSTDINT_PTRMAX
#endif

#endif /* ULSTDINT_H */
