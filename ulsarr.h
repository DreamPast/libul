/*
Shared array


# Introduce
Provide a read-only shared array, caching operations such as slicing and concatenation
to prevent time and space issues caused by excessive operations.


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
#ifndef ULSARR_H
#define ULSARR_H

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

#ifndef ul_unreachable
  #if defined(__GNUC__) || defined(__clang__)
    #define ul_unreachable() __builtin_unreachable()
  #elif defined(_MSC_VER)
    #define ul_unreachable() __assume(0)
  #else
    #define ul_unreachable() ((void)0)
  #endif
#endif /* ul_unreachable */

#ifndef ul_reinterpret_cast
  #ifdef __cplusplus
    #define ul_reinterpret_cast(T, val) reinterpret_cast<T>(val)
  #else
    #define ul_reinterpret_cast(T, val) ((T)(val))
  #endif
#endif /* ul_reinterpret_cast */

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char ulsarr_byte_t;
typedef size_t ulsarr_len_t;
typedef unsigned ulsarr_cnt_t;
#define ULSARR_CONCAT_NUM 4

typedef struct _ulsarr_head_t {
  signed char tag;
  ulsarr_cnt_t cnt;
  ulsarr_len_t len;
} _ulsarr_head_t;

typedef struct ulsarr_alloc_t {
  void* opaque;
  void* (*fn)(void* opaque, void* ptr, size_t on, size_t nn);
} ulsarr_alloc_t;

union ulsarr_t;
typedef union ulsarr_t ulsarr_t;

ul_hapi ulsarr_t* ulsarr_new_copy(ulsarr_alloc_t* alloc, const void* src, ulsarr_len_t len);
ul_hapi ulsarr_t* ulsarr_new_span_ex(ulsarr_alloc_t* alloc, const void* span, size_t len, void* opaque, void (*finalizer)(void* opaque));
ul_hapi ulsarr_t* ulsarr_new_span(ulsarr_alloc_t* alloc, const void* span, size_t len);
ul_hapi ulsarr_t* ulsarr_new_reserve(ulsarr_alloc_t* alloc, ulsarr_len_t len, void* pptr);
ul_hapi ulsarr_t* ulsarr_copy(ulsarr_t* obj);

ul_hapi void ulsarr_destroy(ulsarr_alloc_t* alloc, ulsarr_t* obj);
ul_hapi ulsarr_t* ulsarr_slice(ulsarr_alloc_t* alloc, ulsarr_t* obj, size_t off, size_t len);
ul_hapi ulsarr_t* ulsarr_concat(ulsarr_alloc_t* alloc, ulsarr_t* lhs, ulsarr_t* rhs);

ul_hapi void ulsarr_slice_copy_to(void* dest, const ulsarr_t* obj, size_t off, size_t len);
ul_hapi void ulsarr_copy_to(void* dest, const ulsarr_t* obj);
ul_hapi ulsarr_byte_t* ulsarr_slice_copy_to_alloc(ulsarr_alloc_t* alloc, const ulsarr_t* obj, size_t off, size_t len);
ul_hapi ulsarr_byte_t* ulsarr_copy_to_alloc(ulsarr_alloc_t* alloc, const ulsarr_t* obj);
ul_hapi ulsarr_t* ulsarr_slice_copy_consecutive(ulsarr_alloc_t* alloc, ulsarr_t* obj, size_t off, size_t len);
ul_hapi ulsarr_t* ulsarr_copy_consecutive(ulsarr_alloc_t* alloc, ulsarr_t* obj);

/* UB if `off` isn't available */
ul_hapi ulsarr_byte_t ulsarr_get(const ulsarr_t* obj, size_t off);
/* available if `obj` is consecutive */
ul_hapi const ulsarr_byte_t* ulsarr_const_ref(const ulsarr_t* obj);

/* available if `obj` isn't shared with other */
ul_hapi ulsarr_byte_t* ulsarr_ref(ulsarr_t* obj);
/* available if `obj` isn't shared with other */
ul_hapi ulsarr_t* ulsarr_resize(ulsarr_alloc_t* alloc, ulsarr_t* obj, size_t new_len);


/**************
 * IMPLEMENTS *
 **************/

#define ulsarr_assert(cond) assert(cond)

#define _ulsarr_malloc(alloc, num) (ul_unlikely(alloc) ? (alloc)->fn((alloc)->opaque, NULL, 0, (num)) : malloc(num))
#define _ulsarr_realloc(alloc, ptr, on, nn) (ul_unlikely(alloc) ? (alloc)->fn((alloc)->opaque, (ptr), (on), (nn)) : realloc((ptr), (nn)))
#define _ulsarr_free(alloc, ptr, on) (ul_unlikely(alloc) ? (alloc)->fn((alloc)->opaque, (ptr), (on), 0) : (free((ptr)), ul_reinterpret_cast(void*, 0)))

enum {
  ULSARR_TAG_SEQ,
  ULSARR_TAG_SLICE,
  ULSARR_TAG_CONCAT,
  ULSARR_TAG_SPAN
};

typedef struct _ulsarr_slice_t {
  _ulsarr_head_t head;
  const ulsarr_byte_t* ptr;
  ulsarr_t* raw;
} _ulsarr_slice_t;

typedef struct _ulsarr_concat_t {
  _ulsarr_head_t head;
  ulsarr_t* hold[ULSARR_CONCAT_NUM];
} _ulsarr_concat_t;

typedef struct _ulsarr_span_t {
  _ulsarr_head_t head;
  const ulsarr_byte_t* ptr;
  void* opaque;
  void (*finalizer)(void* opaque);
} _ulsarr_span_t;

union ulsarr_t {
  _ulsarr_head_t head;
  struct {
    _ulsarr_head_t head;
    ulsarr_byte_t b[8];
  } seq;
  _ulsarr_slice_t slice;
  _ulsarr_concat_t concat;
  _ulsarr_span_t span;
};

#define _ULSARR_NEW_SEQ(VAR, LEN) \
  (VAR) = ul_reinterpret_cast(ulsarr_t*, _ulsarr_malloc(alloc, sizeof(_ulsarr_head_t) + (LEN))); \
  if(ul_unlikely(!(VAR))) return NULL; \
  (VAR)->head.cnt = 1; (VAR)->head.len = (LEN); (VAR)->head.tag = ULSARR_TAG_SEQ
#define _ULSARR_NEW_SLICE(VAR, LEN) \
  (VAR) = ul_reinterpret_cast(ulsarr_t*, _ulsarr_malloc(alloc, sizeof(_ulsarr_slice_t))); \
  if(ul_unlikely(!(VAR))) return NULL; \
  (VAR)->head.cnt = 1; (VAR)->head.len = (LEN); (VAR)->head.tag = ULSARR_TAG_SLICE
#define _ULSARR_NEW_CONCAT(VAR, LEN) \
  (VAR) = ul_reinterpret_cast(ulsarr_t*, _ulsarr_malloc(alloc, sizeof(_ulsarr_concat_t))); \
  if(ul_unlikely(!(VAR))) return NULL; \
  (VAR)->head.cnt = 1; (VAR)->head.len = (LEN); (VAR)->head.tag = ULSARR_TAG_CONCAT
#define _ULSARR_NEW_SPAN(VAR, LEN) \
  (VAR) = ul_reinterpret_cast(ulsarr_t*, _ulsarr_malloc(alloc, sizeof(_ulsarr_span_t))); \
  if(ul_unlikely(!(VAR))) return NULL; \
  (VAR)->head.cnt = 1; (VAR)->head.len = (LEN); (VAR)->head.tag = ULSARR_TAG_SPAN

ul_hapi ulsarr_t* ulsarr_new_copy(ulsarr_alloc_t* alloc, const void* src, ulsarr_len_t len) {
  ulsarr_t* obj;
  _ULSARR_NEW_SEQ(obj, len);
  memcpy(obj->seq.b, src, len);
  return obj;
}
ul_hapi ulsarr_t* ulsarr_new_span_ex(ulsarr_alloc_t* alloc, const void* span, size_t len, void* opaque, void (*finalizer)(void* opaque)) {
  ulsarr_t* obj;
  _ULSARR_NEW_SPAN(obj, len);
  obj->span.ptr = ul_reinterpret_cast(const ulsarr_byte_t*, span);
  obj->span.opaque = opaque;
  obj->span.finalizer = finalizer;
  return obj;
}
ul_hapi ulsarr_t* ulsarr_new_span(ulsarr_alloc_t* alloc, const void* span, size_t len) {
  return ulsarr_new_span_ex(alloc, span, len, NULL, 0);
}
ul_hapi ulsarr_t* ulsarr_new_reserve(ulsarr_alloc_t* alloc, ulsarr_len_t len, void* pptr) {
  ulsarr_t* obj;
  _ULSARR_NEW_SEQ(obj, len);
  if(pptr) *ul_reinterpret_cast(void**, pptr) = obj->seq.b;
  return obj;
}

ul_hapi ulsarr_t* ulsarr_copy(ulsarr_t* obj) { ++obj->head.cnt; return obj; }

ul_hapi void ulsarr_destroy(ulsarr_alloc_t* alloc, ulsarr_t* obj) {
  if(ul_unlikely(!obj)) return;
  if(--obj->head.cnt != 0) return;
  switch(obj->head.tag) {
  case ULSARR_TAG_SEQ:
    _ulsarr_free(alloc, obj, sizeof(_ulsarr_head_t) + obj->head.len);
    break;
  case ULSARR_TAG_SLICE:
    obj = obj->slice.raw;
    if(--obj->head.cnt != 0) break;
    if(obj->head.tag == ULSARR_TAG_SEQ) _ulsarr_free(alloc, obj, sizeof(_ulsarr_head_t) + obj->head.len);
    else {
      if(obj->span.finalizer) obj->span.finalizer(obj->span.opaque);
      _ulsarr_free(alloc, obj, sizeof(_ulsarr_span_t));
    }
    break;
  case ULSARR_TAG_CONCAT:
    do {
      size_t i = 0;
      for(; i < ULSARR_CONCAT_NUM; ++i) ulsarr_destroy(alloc, obj->concat.hold[i]);
    } while(0);
    break;
  case ULSARR_TAG_SPAN:
    if(obj->span.finalizer) obj->span.finalizer(obj->span.opaque);
    _ulsarr_free(alloc, obj, sizeof(_ulsarr_span_t));
    break;
  }
}

ul_hapi ulsarr_t* _ulsarr_slice_internal(ulsarr_alloc_t* alloc, ulsarr_t* obj, size_t off, size_t len) {
  ulsarr_t* ret;
  ulsarr_assert(obj->head.tag != ULSARR_TAG_CONCAT);
  _ULSARR_NEW_SLICE(ret, len);
  switch(obj->head.tag) {
  case ULSARR_TAG_SEQ: ret->slice.raw = obj; ret->slice.ptr = obj->seq.b + off; break;
  case ULSARR_TAG_SPAN: ret->slice.raw = obj; ret->slice.ptr = obj->span.ptr + off; break;
  case ULSARR_TAG_SLICE: ret->slice.raw = obj->slice.raw; ret->slice.ptr = obj->slice.ptr + off; break;
  }
  ++ret->slice.raw->head.cnt;
  return ret;
}
ul_hapi ulsarr_t* ulsarr_slice(ulsarr_alloc_t* alloc, ulsarr_t* obj, size_t off, size_t len) {
  if(ul_unlikely(off == 0 && len >= obj->head.len)) { ++obj->head.cnt; return obj; }
  if(ul_unlikely(off >= obj->head.len)) return NULL;
  len = (obj->head.len - off >= len) ? len : obj->head.len - off;
  if(obj->head.tag != ULSARR_TAG_CONCAT) return _ulsarr_slice_internal(alloc, obj, off, len);
  else {
    size_t i;
    for(i = 0; i < ULSARR_CONCAT_NUM; ++i) {
      const size_t sub_len = obj->concat.hold[i]->head.len;
      if(off >= sub_len) off -= sub_len;
      else break;
    }
    if(off + len > obj->concat.hold[i]->head.len) return _ulsarr_slice_internal(alloc, obj->concat.hold[i], off, len);
    else {
      size_t j = 1;
      ulsarr_t* ret;
      _ULSARR_NEW_CONCAT(ret, len);
      ++(ret->concat.hold[0] = obj->concat.hold[i++])->head.cnt;
      len -= (ret->concat.hold[0]->head.len - off);
      while(i < ULSARR_CONCAT_NUM)
        if(len >= ret->concat.hold[i]->head.len) {
          ++(ret->concat.hold[j++] = obj->concat.hold[i])->head.cnt;
          len -= ret->concat.hold[i++]->head.len;
        } else {
          ret->concat.hold[j] = _ulsarr_slice_internal(alloc, obj->concat.hold[i], 0, len);
          if(ul_unlikely(!ret->concat.hold[j])) { _ulsarr_free(alloc, ret, sizeof(_ulsarr_concat_t)); return NULL; }
          break;
        }
      return ret;
    }
  }
}

ul_hapi void _ulsarr_slice_copy_to_direct(void* dest, const ulsarr_t* obj, size_t off, size_t len) {
  ulsarr_assert(obj->head.tag != ULSARR_TAG_CONCAT);
  ulsarr_assert(off < obj->head.len && obj->head.len - off >= len);
  switch(obj->head.tag) {
  case ULSARR_TAG_SEQ: memcpy(dest, obj->seq.b + off, len); break;
  case ULSARR_TAG_SLICE: memcpy(dest, obj->slice.ptr + off, len); break;
  case ULSARR_TAG_SPAN: memcpy(dest, obj->span.ptr + off, len); break;
  }
}
ul_hapi void _ulsarr_slice_copy_to_internal(void* dest, const ulsarr_t* obj, size_t off, size_t len) {
  ulsarr_assert(off < obj->head.len && obj->head.len - off >= len);
  if(obj->head.tag != ULSARR_TAG_CONCAT) { _ulsarr_slice_copy_to_direct(dest, obj, off, len); return; }
  else {
    size_t i;
    for(i = 0; i < ULSARR_CONCAT_NUM; ++i) {
      const size_t sub_len = obj->concat.hold[i]->head.len;
      if(off >= sub_len) off -= sub_len;
      else break;
    }
    do {
      const size_t len2 = obj->concat.hold[i]->head.len - off;
      _ulsarr_slice_copy_to_direct(dest, obj->concat.hold[i], off, len2);
      len -= len2;
      if(len == 0) return;
      dest = ul_reinterpret_cast(ulsarr_byte_t*, dest) + len2;
    } while(0);
    for(++i; i < ULSARR_CONCAT_NUM; ++i) {
      const size_t sub_len = obj->concat.hold[i]->head.len;
      if(len <= sub_len) {
        _ulsarr_slice_copy_to_direct(dest, obj->concat.hold[i], 0, len);
        break;
      } else {
        _ulsarr_slice_copy_to_direct(dest, obj->concat.hold[i], 0, sub_len);
        len -= sub_len;
        dest = ul_reinterpret_cast(ulsarr_byte_t*, dest) + sub_len;
      }
    }
  }
}

ul_hapi void ulsarr_slice_copy_to(void* dest, const ulsarr_t* obj, size_t off, size_t len) {
  if(ul_unlikely(off >= obj->head.len)) return;
  len = (obj->head.len - off >= len) ? len : obj->head.len - off;
  _ulsarr_slice_copy_to_internal(dest, obj, off, len);
}
ul_hapi void ulsarr_copy_to(void* dest, const ulsarr_t* obj) { ulsarr_slice_copy_to(dest, obj, 0, obj->head.len); }
ul_hapi ulsarr_byte_t* ulsarr_slice_copy_to_alloc(ulsarr_alloc_t* alloc, const ulsarr_t* obj, size_t off, size_t len) {
  ulsarr_byte_t* ret;
  if(ul_unlikely(off >= obj->head.len)) return NULL;
  len = (obj->head.len - off >= len) ? len : obj->head.len - off;
  ret = ul_reinterpret_cast(ulsarr_byte_t*, _ulsarr_malloc(alloc, len));
  if(ul_unlikely(!ret)) return NULL;
  _ulsarr_slice_copy_to_internal(ret, obj, off, len);
  return ret;
}
ul_hapi ulsarr_byte_t* ulsarr_copy_to_alloc(ulsarr_alloc_t* alloc, const ulsarr_t* obj) {
  ulsarr_byte_t* ret = ul_reinterpret_cast(ulsarr_byte_t*, _ulsarr_malloc(alloc, obj->head.len));
  if(ul_unlikely(!ret)) return NULL;
  _ulsarr_slice_copy_to_internal(ret, obj, 0, obj->head.len);
  return ret;
}

ul_hapi ulsarr_t* ulsarr_slice_copy_consecutive(ulsarr_alloc_t* alloc, ulsarr_t* obj, size_t off, size_t len) {
  ulsarr_t* ret;
  if(off >= obj->head.len) return NULL;
  len = (obj->head.len - off >= len) ? len : obj->head.len - off;
  _ULSARR_NEW_SEQ(ret, len);
  _ulsarr_slice_copy_to_internal(ret->seq.b, obj, off, len);
  return ret;
}
ul_hapi ulsarr_t* ulsarr_copy_consecutive(ulsarr_alloc_t* alloc, ulsarr_t* obj) {
  ulsarr_t* ret;
  /* if(obj->head.tag == ULSARR_TAG_SEQ) { ++obj->head.cnt; return obj; } */
  _ULSARR_NEW_SEQ(ret, obj->head.len);
  _ulsarr_slice_copy_to_internal(ret->seq.b, obj, 0, obj->head.len);
  return ret;
}

ul_hapi ulsarr_t* _ulsarr_concat_multi(ulsarr_alloc_t* alloc, ulsarr_t** hold) {
  size_t i;
  size_t len = 0;
  ulsarr_t* ret;
  ulsarr_byte_t* ptr;

  for(i = 0; hold[i]; ++i) { len += hold[i]->head.len; ulsarr_assert(hold[i]->head.tag != ULSARR_TAG_CONCAT); }
  _ULSARR_NEW_SEQ(ret, len);
  len = 0; ptr = ret->seq.b;
  for(i = 0; hold[i]; ++i) {
    _ulsarr_slice_copy_to_direct(ptr, hold[i], 0, hold[i]->head.len);
    ptr += hold[i]->head.len;
  }
  return ret;
}
ul_hapi ulsarr_t* ulsarr_concat(ulsarr_alloc_t* alloc, ulsarr_t* lhs, ulsarr_t* rhs) {
  ulsarr_t* hold[ULSARR_CONCAT_NUM * 2 + 1];
  size_t hold_num = 0;

  if(lhs->head.tag != ULSARR_TAG_CONCAT) hold[hold_num++] = lhs;
  else {
    for(; hold_num < ULSARR_CONCAT_NUM && lhs->concat.hold[hold_num]; ++hold_num)
      hold[hold_num] = lhs->concat.hold[hold_num];
  }
  if(rhs->head.tag != ULSARR_TAG_CONCAT) hold[hold_num++] = rhs;
  else {
    size_t i;
    for(i = 0; i < ULSARR_CONCAT_NUM && rhs->concat.hold[i]; ++i)
      hold[hold_num++] = rhs->concat.hold[i];
  }

  if(hold_num < ULSARR_CONCAT_NUM) {
    size_t i;
    ulsarr_t* ret;
    _ULSARR_NEW_CONCAT(ret, lhs->head.len + rhs->head.len);
    if(ul_unlikely(!ret)) return NULL;
    for(i = 0; i < ULSARR_CONCAT_NUM; ++i) ret->concat.hold[i] = hold[i];
    if(i != ULSARR_CONCAT_NUM) ret->concat.hold[i] = NULL;
    return ret;
  } else {
    hold[hold_num] = NULL;
    return _ulsarr_concat_multi(alloc, hold);
  }
}

ul_hapi ulsarr_byte_t ulsarr_get(const ulsarr_t* obj, size_t off) {
  ulsarr_assert(off < obj->head.len);
  switch(obj->head.tag) {
  case ULSARR_TAG_SEQ: return obj->seq.b[off];
  case ULSARR_TAG_SLICE: return obj->slice.ptr[off];
  case ULSARR_TAG_SPAN: return obj->span.ptr[off];
  }
  do {
    size_t i;
    const ulsarr_t* obj2;
    for(i = 0; i < ULSARR_CONCAT_NUM; ++i) {
      obj2 = obj->concat.hold[i];
      if(off >= obj2->head.len) { off -= obj2->head.len; continue; }
      switch(obj2->head.tag) {
      case ULSARR_TAG_SEQ: return obj2->seq.b[off];
      case ULSARR_TAG_SLICE: return obj2->slice.ptr[off];
      case ULSARR_TAG_SPAN: return obj2->span.ptr[off];
      }
    }
    ul_unreachable();
    return 0;
  } while(0);
}
ul_hapi const ulsarr_byte_t* ulsarr_const_ref(const ulsarr_t* obj) {
  switch(obj->head.tag) {
  case ULSARR_TAG_SEQ: return obj->seq.b;
  case ULSARR_TAG_SLICE: return obj->slice.ptr;
  case ULSARR_TAG_SPAN: return obj->span.ptr;
  }
  return NULL;
}

ul_hapi ulsarr_byte_t* ulsarr_ref(ulsarr_t* obj) {
  if(obj->head.cnt == 1 && obj->head.tag == ULSARR_TAG_SEQ) return obj->seq.b;
  else return NULL;
}
ul_hapi ulsarr_t* ulsarr_resize(ulsarr_alloc_t* alloc, ulsarr_t* obj, size_t new_len) {
  ulsarr_t* nobj;
  if(obj->head.tag != ULSARR_TAG_SEQ) return NULL;
  if(obj->head.cnt != 1) return NULL;
  nobj = ul_reinterpret_cast(ulsarr_t*, _ulsarr_realloc(alloc, obj, sizeof(_ulsarr_head_t) + obj->head.len, sizeof(_ulsarr_head_t) + new_len));
  if(ul_unlikely(!nobj)) return NULL;
  nobj->head.len = new_len;
  return nobj;
}

#endif /* ULSARR_H */
