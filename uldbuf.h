/*
Dynamic Buffer


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
#ifndef ULDBUF_H
#define ULDBUF_H

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

#ifndef ul_reinterpret_cast
  #ifdef __cplusplus
    #define ul_reinterpret_cast(T, val) reinterpret_cast<T>(val)
  #else
    #define ul_reinterpret_cast(T, val) (T)(val)
  #endif
#endif /* ul_reinterpret_cast */

#ifndef ul_static_cast
  #ifdef __cplusplus
    #define ul_static_cast(T, val) static_cast<T>(val)
  #else
    #define ul_static_cast(T, val) (T)(val)
  #endif
#endif /* ul_static_cast */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

/**
 * memory-allocation function
 *
 * When `ptr` is empty(it's guaranteed that `on` is 0), try to allocate a new memory.
 * When `ptr` isn't empty and `nn` isn't 0, try to resize the memory.
 * The above two operations can return NULL if the operation fails.
 *
 * When `ptr` isn't empty but `nn` is 0, free the memory(the operation shouldn't fail).
 *
 * @param opaque user's opaque pointer
 * @param ptr old pointer
 * @param on size of old pointer
 * @param nn size desired
*/
typedef void* (*uldbuf_realloc_fn_t)(void* opaque, void* ptr, size_t on, size_t nn);
typedef struct uldbuf_t {
  unsigned char* data;
  size_t len;
  size_t alloced;
  uldbuf_realloc_fn_t alloc_fn;
  void* opaque;
} uldbuf_t;

ul_hapi void* _uldbuf_default_alloc(void* opaque, void* ptr, size_t on, size_t nn) {
  (void)opaque; (void)on;
  return nn ? realloc(ptr, nn) : (free(ptr), ul_reinterpret_cast(void*, 0));
}

#define ULDBUF_INIT_CUSTOM(data, len, alloced_bytes, alloc_function, opaque) \
  { ul_reinterpret_cast(unsigned char*, (data)), (len), (alloced_bytes), (alloc_function), (opaque) }
ul_hapi void uldbuf_init_custom(
  uldbuf_t* b, void* data, size_t len, size_t alloced_bytes,
  uldbuf_realloc_fn_t alloc_function, void* opaque
) {
  b->data = ul_reinterpret_cast(unsigned char*, data);
  b->len = len;
  b->alloced = alloced_bytes;
  b->alloc_fn = alloc_function;
  b->opaque = opaque;
}

#define ULDBUF_INIT2(alloc, opaque) { NULL, 0, 0, (alloc), (opaque) }
ul_hapi void uldbuf_init2(uldbuf_t* b, uldbuf_realloc_fn_t alloc, void* opaque) {
  b->data = NULL;
  b->len = 0;
  b->alloced = 0;
  b->alloc_fn = alloc;
  b->opaque = opaque;
}

#define ULDBUF_INIT { NULL, 0, 0, _uldbuf_default_alloc, NULL }
ul_hapi void uldbuf_init(uldbuf_t* b) {
  uldbuf_init2(b, _uldbuf_default_alloc, NULL);
}

#define ULDBUF_INIT_FIXED_MEMORY(buf, len) { ul_reinterpret_cast(unsigned char*, buf), 0, (len), NULL, NULL }
ul_hapi void uldbuf_init_fixed_memory(uldbuf_t* b, void* buf, size_t len) {
  b->data = ul_reinterpret_cast(unsigned char*, buf);
  b->len = 0;
  b->alloced = len;
  b->alloc_fn = NULL;
  b->opaque = NULL;
}

ul_hapi unsigned char* uldbuf_release(uldbuf_t* b) {
  unsigned char* data = b->data;
  b->data = NULL;
  b->len = 0;
  b->alloced = 0;
  return data;
}
ul_hapi void uldbuf_free(uldbuf_t* b) {
  if(b->data && ul_likely(b->alloc_fn))
    b->alloc_fn(b->opaque, b->data, b->alloced, 0);
  b->data = NULL;
  b->len = 0;
  b->alloced = 0;
}

ul_hapi int _uldbuf_realloc(uldbuf_t* b, size_t target_alloced) {
  unsigned char* new_data;
  size_t new_alloced;
  if(target_alloced > b->alloced) {
    new_alloced = b->alloced + (b->alloced >> 1) + 1;
    new_alloced = new_alloced < target_alloced ? target_alloced : new_alloced;
    if(ul_unlikely(!b->alloc_fn)) return -1;
    new_data = ul_reinterpret_cast(unsigned char*, b->alloc_fn(b->opaque, b->data, b->alloced, new_alloced));
    if(ul_unlikely(!new_data)) return -1;
    b->data = new_data;
    b->alloced = new_alloced;
  }
  return 0;
}
#define _uldbuf_may_realloc(b, n) (ul_unlikely((n) > (b)->alloced) \
  && ul_unlikely(_uldbuf_realloc((b), (n))))

ul_hapi int uldbuf_resize(uldbuf_t* b, size_t target_len) {
  if(_uldbuf_may_realloc(b, target_len)) return -1;
  b->len = target_len; return 0;
}
ul_hapi void uldbuf_clear(uldbuf_t* b) { b->len = 0; }

ul_hapi int uldbuf_write(uldbuf_t* b, size_t offset, const void* data, size_t len) {
  size_t end;
  end = offset + len;
  if(_uldbuf_may_realloc(b, end)) return -1;
  memcpy(b->data + offset, data, len);
  if(end > b->len) b->len = end;
  return 0;
}
ul_hapi int uldbuf_put(uldbuf_t* b, const void* data, size_t len) {
  size_t end;
  end = b->len + len;
  if(_uldbuf_may_realloc(b, end)) return -1;
  memcpy(b->data + b->len, data, len);
  b->len = end;
  return 0;
}
ul_hapi int uldbuf_put_self(uldbuf_t* b, size_t offset, size_t len) {
  size_t end;
  if(ul_unlikely(offset > b->len)) return -1;
  if(offset + len > b->len) len = b->len - offset;
  end = b->len + len;
  if(_uldbuf_may_realloc(b, end)) return -1;
  memmove(b->data + b->len, b->data + offset, len);
  b->len = end;
  return 0;
}

ul_hapi int uldbuf_put0(uldbuf_t* b, size_t num) {
  if(_uldbuf_may_realloc(b, b->len + num)) return -1;
  memset(b->data + b->len, 0, num);
  b->len += num;
  return 0;
}
ul_hapi int uldbuf_align(uldbuf_t* b, size_t align) {
  size_t rest = b->len % align;
  if(rest == 0) return 0;
  rest = align - rest;
  if(_uldbuf_may_realloc(b, b->len + rest)) return -1;
  memset(b->data + b->len, 0, rest);
  b->len += rest;
  return 0;
}

ul_hapi int uldbuf_put_char(uldbuf_t* b, char c) {
  if(_uldbuf_may_realloc(b, b->len + 1)) return -1;
  b->data[b->len++] = ul_static_cast(unsigned char, c); return 0;
}
ul_hapi int uldbuf_put_schar(uldbuf_t* b, signed char sc) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(sc))) return -1;
  *ul_reinterpret_cast(signed char*, b->data + b->len) = sc;
  b->len += sizeof(sc); return 0;
}
ul_hapi int uldbuf_put_uchar(uldbuf_t* b, unsigned char uc) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(uc))) return -1;
  *ul_reinterpret_cast(unsigned char*, b->data + b->len) = uc;
  b->len += sizeof(uc); return 0;
}
ul_hapi int uldbuf_put_string(uldbuf_t* b, const char* str) {
  return uldbuf_put(b, str, strlen(str));
}

ul_hapi int uldbuf_put_wchar(uldbuf_t* b, wchar_t wc) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(wc))) return -1;
  memcpy(b->data + b->len, &wc, sizeof(wc));
  b->len += sizeof(wc); return 0;
}
ul_hapi int uldbuf_put_wstr(uldbuf_t* b, const wchar_t* wstr) {
  size_t len;
  for(len = 0; wstr[len]; ++len) { }
  return uldbuf_put(b, wstr, len * sizeof(wchar_t));
}

ul_hapi int uldbuf_put_short(uldbuf_t* b, signed short val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}
ul_hapi int uldbuf_put_int(uldbuf_t* b, signed int val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}
ul_hapi int uldbuf_put_long(uldbuf_t* b, signed long val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}
#ifdef LLONG_MAX
  ul_hapi int uldbuf_put_llong(uldbuf_t* b, signed long long val) {
    if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
    memcpy(b->data + b->len, &val, sizeof(val));
    b->len += sizeof(val); return 0;
  }
#endif
ul_hapi int uldbuf_put_ushort(uldbuf_t* b, unsigned short val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}
ul_hapi int uldbuf_put_uint(uldbuf_t* b, unsigned int val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}
ul_hapi int uldbuf_put_ulong(uldbuf_t* b, unsigned long val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}
#ifdef ULLONG_MAX
  ul_hapi int uldbuf_put_ullong(uldbuf_t* b, unsigned long long val) {
    if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
    memcpy(b->data + b->len, &val, sizeof(val));
    b->len += sizeof(val); return 0;
  }
#endif

ul_hapi int uldbuf_put_float(uldbuf_t* b, float val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}
ul_hapi int uldbuf_put_double(uldbuf_t* b, double val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}
ul_hapi int uldbuf_put_ldouble(uldbuf_t* b, long double val) {
  if(_uldbuf_may_realloc(b, b->len + sizeof(val))) return -1;
  memcpy(b->data + b->len, &val, sizeof(val));
  b->len += sizeof(val); return 0;
}

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
    || (defined(__cplusplus) && __cplusplus >= 201103L) \
    || (defined(_MSC_VER) && _MSC_VER >= 1800)
  /* TODO: for C89, maybe write it in file, and then read it out? */
  #include <stdarg.h>
  #include <stdio.h>

  #ifndef ul_printf_like
    #if (defined(__GNUC__) && __GNUC__ >= 3) || defined(__clang__)
      #define ul_printf_like(f, x) __attribute__((format(printf, f, x)))
    #else
      #define ul_printf_like(f, x)
    #endif
  #endif /* ul_printf_like */

  ul_hapi ul_printf_like(2, 3) int uldbuf_printf(uldbuf_t* s, const char* fmt, ...) {
    va_list vl;
    char buf[128];
    int len;

    va_start(vl, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, vl);
    va_end(vl);

    if(ul_unlikely(len < 0)) return -1;
    if(ul_static_cast(unsigned, len) < sizeof(buf)) {
      return uldbuf_put(s, buf, ul_static_cast(unsigned, len));
    } else {
      if(_uldbuf_may_realloc(s, s->len + ul_static_cast(unsigned, len) + 1)) return -1;
      va_start(vl, fmt);
      len = vsnprintf(ul_reinterpret_cast(char*, s->data + s->len), s->alloced - s->len, fmt, vl);
      va_end(vl);
      s->len += ul_static_cast(unsigned, len);
      return 0;
    }
  }
#endif

#endif /* ULDBUF_H */
