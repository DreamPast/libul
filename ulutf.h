/*
UTF support


# Dependences:
  8-bit integer, 16-bit integer, 32-bit integer


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
#ifndef ULUTF_H
#define ULUTF_H

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
  #if defined(__cplusplus) && __cplusplus >= 201103L && defined(__has_cpp_attribute)
    #if __has_cpp_attribute(nodiscard)
      #define ul_nodiscard [[nodiscard]]
    #endif
  #endif
  #if !defined(ul_nodiscard) && defined(__has_attribute)
    #if __has_attribute(warn_unused_result)
      #define ul_nodiscard __attribute__((warn_unused_result))
    #endif
  #endif
  #if !defined(ul_nodiscard) && defined(_MSC_VER) && _MSC_VER >= 1700
    #define ul_nodiscard _Check_return_
  #endif
  #if !defined(ul_nodiscard) && defined(_HAS_NODISCARD)
    #define ul_nodiscard _NODISCARD
  #endif
  #ifndef ul_nodiscard
    #define ul_nodiscard
  #endif
#endif /* ul_nodiscard */

#if !defined(ul_fallthrough) && defined(__has_attribute)
  #if __has_attribute(fallthrough)
    #define ul_fallthrough __attribute__((__fallthrough__))
  #endif
#endif
#if !defined(ul_fallthrough) && defined(__GNUC__) && __GNUC__ >= 7
  #define ul_fallthrough __attribute__((__fallthrough__))
#endif
#if !defined(ul_fallthrough) && defined(__has_cpp_attribute)
  #if __has_cpp_attribute(fallthrough)
    #define ul_fallthrough [[fallthrough]]
  #endif
#endif
#ifndef ul_fallthrough
  #define ul_fallthrough ((void)0)
#endif /* ul_fallthrough */

#ifndef ul_static_cast
  #ifdef __cplusplus
    #define ul_static_cast(T, val) static_cast<T>(val)
  #else
    #define ul_static_cast(T, val) ((T)(val))
  #endif
#endif /* ul_static_cast */

#include <limits.h>

#if CHAR_BIT != 8 || CHAR_MAX != 0x7F
  #error "uldecode.h: `char` is not 8-bit"
#endif
typedef unsigned char ulutf_u8_t;

#if SHRT_MAX != 0x7FFF
  #error "uldecode.h: `short` is not 16-bit"
#endif
typedef unsigned short ulutf_u16_t;

#if INT_MAX == 0x7FFFFFFF
  typedef unsigned ulutf_u32_t;
#elif LONG_MAX == 0x7FFFFFFF
  typedef unsigned long ulutf_u32_t;
#else
  #error "uldecode.h: neither `int` nor `long` is 32-bit"
#endif

ul_hapi int ulutf16_is_first_surrogate(ulutf_u16_t c) {
	return 0xD800u <= c && c <= 0xDBFFu;
}
ul_hapi int ulutf16_is_second_surrogate(ulutf_u16_t c) {
	return 0xDC00u <= c && c <= 0xDFFFu;
}
ul_hapi ulutf_u32_t ulutf16_combine_surrogate(ulutf_u16_t c0, ulutf_u16_t c1) {
	return (ul_static_cast(ulutf_u32_t, c0 & 0x3FFu) << 10 | (c1 & 0x3FFu)) + 0x10000u;
}
ul_hapi int ulutf32_is_valid(ulutf_u32_t u) {
	return !(u > 0x10FFFFu || (0xD800 <= u && u <= 0xDFFFu));
}
ul_hapi int ulutf16_width(ulutf_u32_t c) {
  if(ul_unlikely(c >= 0xD800 && c <= 0xDFFF)) return -1;
  if(ul_likely(c < 0x10000u)) return 1;
  if(c <= 0x10FFFFu) return 2;
  return -1;
}
ul_hapi ulutf_u16_t ulutf16_make_first_surrogate(ulutf_u32_t u) {
  return ul_static_cast(ulutf_u16_t, 0xD800 | ((u - 0x10000u) >> 10));
}
ul_hapi ulutf_u16_t ulutf16_make_second_surrogate(ulutf_u32_t u) {
  return ul_static_cast(ulutf_u16_t, 0xDC00 | (u & 0x3FFu));
}

ul_hapi int ulutf8_is_trail(ulutf_u8_t c) { return (c & 0xC0u) == 0x80u; }
ul_hapi int ulutf8_is_lead(ulutf_u8_t c) { return (c & 0xC0u) != 0x80u; }

ul_hapi int ulutf8_trail_length(ulutf_u8_t c) {
  if(ul_likely(c <= 0x7Fu)) return 0;
	switch(c) {
  case 0xC0: case 0xC1: case 0xC2: case 0xC3:
  case 0xC4: case 0xC5: case 0xC6: case 0xC7:
  case 0xC8: case 0xC9: case 0xCA: case 0xCB:
  case 0xCC: case 0xCD: case 0xCE: case 0xCF:
  case 0xD0: case 0xD1: case 0xD2: case 0xD3:
  case 0xD4: case 0xD5: case 0xD6: case 0xD7:
  case 0xD8: case 0xD9: case 0xDA: case 0xDB:
  case 0xDC: case 0xDD: case 0xDE: case 0xDF:
    return 1;
  case 0xE0: case 0xE1: case 0xE2: case 0xE3:
  case 0xE4: case 0xE5: case 0xE6: case 0xE7:
  case 0xE8: case 0xE9: case 0xEA: case 0xEB:
  case 0xEC: case 0xED: case 0xEE: case 0xEF:
    return 2;
  case 0xF0: case 0xF1: case 0xF2: case 0xF3:
  case 0xF4: case 0xF5: case 0xF6: case 0xF7:
	  return 3;
  case 0xF8: case 0xF9: case 0xFA: case 0xFB:
	  return 4;
  case 0xFC: case 0xFD:
	  return 5;
  default:
    return -1;
	}
}
ul_hapi int ulutf8_width(ulutf_u32_t u) {
  if(ul_likely(u <= 0x7Fu)) return 1;
  if(u <= 0x7FFu) return 2;
  if(u <= 0xFFFFu) return 3;
  if(ul_likely(u <= 0x1FFFFF)) return 4;
  if(u <= 0x3FFFFFFu) return 5;
  if(u <= 0x7FFFFFFFu) return 6;
  return -1;
}

/* return bytes writen */
ul_hapi int ulutf8_encode(ulutf_u8_t* p, ulutf_u32_t u) {
  ulutf_u8_t* q = p;
  if(u <= 0x7Fu) {
    *q++ = ul_static_cast(ulutf_u8_t, u);
  } else {
    if(u <= 0x7FFu) {
      *q++ = ul_static_cast(ulutf_u8_t, (u >> 6) | 0xC0);
    } else {
      if(u <= 0xFFFFu) {
        *q++ = ul_static_cast(ulutf_u8_t, (u >> 12) | 0xE0);
      } else {
        if(ul_likely(u <= 0x1FFFFF)) {
          *q++ = ul_static_cast(ulutf_u8_t, (u >> 18) | 0xF0);
        } else {
          if(u <= 0x3FFFFFFu) {
            *q++ = ul_static_cast(ulutf_u8_t, (u >> 24) | 0xF8);
          } else if(u <= 0x7FFFFFFFu) {
            *q++ = ul_static_cast(ulutf_u8_t, (u >> 30) | 0xFC);
            *q++ = ul_static_cast(ulutf_u8_t, ((u >> 24) & 0x3F) | 0x80);
          } else {
            return 0;
          }
          *q++ = ul_static_cast(ulutf_u8_t, ((u >> 18) & 0x3F) | 0x80);
        }
        *q++ = ul_static_cast(ulutf_u8_t, ((u >> 12) & 0x3F) | 0x80);
      }
      *q++ = ul_static_cast(ulutf_u8_t, ((u >> 6) & 0x3F) | 0x80);
    }
    *q++ = ul_static_cast(ulutf_u8_t, (u & 0x3F) | 0x80);
  }
  return ul_static_cast(int, q - p);
}
ul_hapi ulutf_u32_t ulutf8_decode(const ulutf_u8_t* p, size_t n, const ulutf_u8_t** pp) {
  ulutf_u32_t u, umin;
  int l;
  ulutf_u8_t c;

  u = *p++;
  if(u < 0x7Fu) {
    *pp = p;
    return u;
  }
	switch(u) {
  case 0xC0: case 0xC1: case 0xC2: case 0xC3:
  case 0xC4: case 0xC5: case 0xC6: case 0xC7:
  case 0xC8: case 0xC9: case 0xCA: case 0xCB:
  case 0xCC: case 0xCD: case 0xCE: case 0xCF:
  case 0xD0: case 0xD1: case 0xD2: case 0xD3:
  case 0xD4: case 0xD5: case 0xD6: case 0xD7:
  case 0xD8: case 0xD9: case 0xDA: case 0xDB:
  case 0xDC: case 0xDD: case 0xDE: case 0xDF:
    l = 0; umin = 0x80u; u &= 0x1F; break;
  case 0xE0: case 0xE1: case 0xE2: case 0xE3:
  case 0xE4: case 0xE5: case 0xE6: case 0xE7:
  case 0xE8: case 0xE9: case 0xEA: case 0xEB:
  case 0xEC: case 0xED: case 0xEE: case 0xEF:
    l = 1; umin = 0x800u; u &= 0xF; break;
  case 0xF0: case 0xF1: case 0xF2: case 0xF3:
  case 0xF4: case 0xF5: case 0xF6: case 0xF7:
	  l = 2; umin = 0x10000u; u &= 0x7; break;
  case 0xF8: case 0xF9: case 0xFA: case 0xFB:
	  l = 3; umin = 0x200000u; u &= 0x3; break;
  case 0xFC: case 0xFD:
	  l = 4; umin = 0x4000000u; u &= 0x1; break;
  default: return ul_static_cast(ulutf_u32_t, -1);
	}

  if(ul_unlikely(ul_static_cast(size_t, l) >= n)) return ul_static_cast(ulutf_u32_t, -1);
  switch(l) {
  case 4:
    c = *p++;
    if(c < 0x80 || c >= 0xC0) return ul_static_cast(ulutf_u32_t, -1);
    u = (u << 6) | (c & 0x3F);
    ul_fallthrough;
  case 3:
    c = *p++;
    if(c < 0x80 || c >= 0xC0) return ul_static_cast(ulutf_u32_t, -1);
    u = (u << 6) | (c & 0x3F);
    ul_fallthrough;
  case 2:
    c = *p++;
    if(c < 0x80 || c >= 0xC0) return ul_static_cast(ulutf_u32_t, -1);
    u = (u << 6) | (c & 0x3F);
    ul_fallthrough;
  case 1:
    c = *p++;
    if(c < 0x80 || c >= 0xC0) return ul_static_cast(ulutf_u32_t, -1);
    u = (u << 6) | (c & 0x3F);
    ul_fallthrough;
  case 0:
    c = *p++;
    if(c < 0x80 || c >= 0xC0) return ul_static_cast(ulutf_u32_t, -1);
    u = (u << 6) | (c & 0x3F);
  }

  if(u < umin) return ul_static_cast(ulutf_u32_t, -1);
  *pp = p;
  return u;
}

ul_hapi int ulutf_is_ascii(const char* str, size_t n) {
  while(n--) if(!(ul_static_cast(unsigned char, *str) <= 0x7Fu)) return 0;
  return 1;
}

#endif /* ULUTF_H */
