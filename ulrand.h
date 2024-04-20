/*
Random number generator


# Introduction
  This file uses [PCG Random Number Generators](https://www.pcg-random.org/).


# Dependence
  <stdint.h>/"ulstdint.h"
  - 32-bit number generator needs 64-bit integer support.
  - 64-bit number generator needs 128-bit integer support.


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
#ifndef ULRAND_H
#define ULRAND_H

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

#ifndef ul_static_cast
  #ifdef __cplusplus
    #define ul_static_cast(T, val) static_cast<T>(val)
  #else
    #define ul_static_cast(T, val) ((T)(val))
  #endif
#endif /* ul_static_cast */

typedef struct ulrand8_t {
  uint16_t state;
  uint16_t inc;
} ulrand8_t;
#define _ULRAND8_DEFAULT_MULTIPLIER 0x8Du
#define _ULRAND8_DEFAULT_INCREMENT  0x4Du
ul_hapi uint8_t _ulrand8_rotr(uint8_t val, unsigned rot) {
  return ul_static_cast(uint8_t, (val >> rot) | (val << ((~rot + 1) & 7)));
}
ul_hapi uint8_t _ulrand8_output(uint16_t state) {
  return _ulrand8_rotr(
    ul_static_cast(uint8_t, ((state >> 5u) ^ state) >> 5u),
    ul_static_cast(unsigned, state >> 13u)
  );
}
ul_hapi void ulrand8_step(ulrand8_t* rng) {
  rng->state = rng->state * _ULRAND8_DEFAULT_MULTIPLIER + rng->inc;
}
ul_hapi void ulrand8_init2(ulrand8_t* rng, uint16_t initstate, uint16_t initseq) {
  rng->state = 0u; rng->inc = ul_static_cast(uint16_t, (initseq << 1) | 1);
  ulrand8_step(rng);
  rng->state += initstate;
  ulrand8_step(rng);
}
ul_hapi void ulrand8_init(ulrand8_t* rng, uint16_t initstate) {
  rng->state = 0u; rng->inc = _ULRAND8_DEFAULT_INCREMENT;
  ulrand8_step(rng);
  rng->state += initstate;
  ulrand8_step(rng);
}
ul_hapi uint8_t ulrand8_gen(ulrand8_t* rng) {
  uint16_t oldstate = rng->state;
  ulrand8_step(rng);
  return _ulrand8_output(oldstate);
}
ul_hapi uint8_t ulrand8_boundage(ulrand8_t* rng, uint8_t bound) {
  uint8_t threshold = (~bound + 1u) % bound;
  for (;;) {
    uint8_t r = ulrand8_gen(rng);
    if(r >= threshold) return r % bound;
  }
}
ul_hapi void ulrand8_advance(ulrand8_t* rng, uint16_t delta) {
  uint16_t state = rng->state;
  uint16_t cur_mult = _ULRAND8_DEFAULT_MULTIPLIER;
  uint16_t cur_plus = rng->inc;
  uint16_t acc_mult = 1u;
  uint16_t acc_plus = 0u;
  while(delta > 0) {
    if(delta & 1) {
      acc_mult *= cur_mult;
      acc_plus = acc_plus * cur_mult + cur_plus;
    }
    cur_plus = (cur_mult + 1) * cur_plus;
    cur_mult *= cur_mult;
    delta >>= 1;
  }
  rng->state = acc_mult * state + acc_plus;
}



typedef struct ulrand16_t {
  uint32_t state;
  uint32_t inc;
} ulrand16_t;
#define _ULRAND16_DEFAULT_MULTIPLIER 0x321Du
#define _ULRAND16_DEFAULT_INCREMENT  0xBB75u
ul_hapi uint16_t _ulrand16_rotr(uint16_t val, unsigned rot) {
  return ul_static_cast(uint16_t, (val >> rot) | (val << ((~rot + 1) & 15)));
}
ul_hapi uint16_t _ulrand16_output(uint32_t state) {
  return _ulrand16_rotr(
    ul_static_cast(uint16_t, ((state >> 10u) ^ state) >> 12u),
    ul_static_cast(unsigned, state >> 28u)
  );
}
ul_hapi void ulrand16_step(ulrand16_t* rng) {
  rng->state = rng->state * _ULRAND16_DEFAULT_MULTIPLIER + rng->inc;
}
ul_hapi void ulrand16_init2(ulrand16_t* rng, uint32_t initstate, uint32_t initseq) {
  rng->state = 0u; rng->inc = (initseq << 1u) | 1u;
  ulrand16_step(rng);
  rng->state += initstate;
  ulrand16_step(rng);
}
ul_hapi void ulrand16_init(ulrand16_t* rng, uint32_t initstate) {
  rng->state = 0u; rng->inc = _ULRAND16_DEFAULT_INCREMENT;
  ulrand16_step(rng);
  rng->state += initstate;
  ulrand16_step(rng);
}
ul_hapi uint16_t ulrand16_gen(ulrand16_t* rng) {
  uint32_t oldstate = rng->state;
  ulrand16_step(rng);
  return _ulrand16_output(oldstate);
}
ul_hapi uint16_t ulrand16_boundage(ulrand16_t* rng, uint16_t bound) {
  uint16_t threshold = (~bound + 1u) % bound;
  for (;;) {
    uint16_t r = ulrand16_gen(rng);
    if(r >= threshold) return r % bound;
  }
}
ul_hapi void ulrand16_advance(ulrand16_t* rng, uint32_t delta) {
  uint32_t state = rng->state;
  uint32_t cur_mult = _ULRAND16_DEFAULT_MULTIPLIER;
  uint32_t cur_plus = rng->inc;
  uint32_t acc_mult = 1u;
  uint32_t acc_plus = 0u;
  while(delta > 0) {
    if(delta & 1) {
      acc_mult *= cur_mult;
      acc_plus = acc_plus * cur_mult + cur_plus;
    }
    cur_plus = (cur_mult + 1) * cur_plus;
    cur_mult *= cur_mult;
    delta >>= 1;
  }
  rng->state = acc_mult * state + acc_plus;
}



#ifdef UINT64_MAX
  typedef struct ulrand32_t {
    uint64_t state;
    uint64_t inc;
  } ulrand32_t;
  #define _ULRAND32_DEFAULT_MULTIPLIER 0x2C9277B5u
  #define _ULRAND32_DEFAULT_INCREMENT  0xAC564B05u
  ul_hapi uint32_t _ulrand32_rotr(uint32_t val, unsigned rot) {
    return (val >> rot) | (val << ((~rot + 1u) & 31));
  }
  ul_hapi uint32_t _ulrand32_output(uint64_t state) {
    return _ulrand32_rotr(
      ul_static_cast(uint32_t, ((state >> 18u) ^ state) >> 27u),
      ul_static_cast(unsigned, state >> 59u)
    );
  }
  ul_hapi void ulrand32_step(ulrand32_t* rng) {
    rng->state = rng->state * _ULRAND32_DEFAULT_MULTIPLIER + rng->inc;
  }
  ul_hapi void ulrand32_init2(ulrand32_t* rng, uint64_t initstate, uint64_t initseq) {
    rng->state = 0u; rng->inc = (initseq << 1u) | 1u;
    ulrand32_step(rng);
    rng->state += initstate;
    ulrand32_step(rng);
  }
  ul_hapi void ulrand32_init(ulrand32_t* rng, uint64_t initstate) {
    rng->state = 0u; rng->inc = _ULRAND32_DEFAULT_INCREMENT;
    ulrand32_step(rng);
    rng->state += initstate;
    ulrand32_step(rng);
  }
  ul_hapi uint32_t ulrand32_gen(ulrand32_t* rng) {
    uint64_t oldstate = rng->state;
    ulrand32_step(rng);
    return _ulrand32_output(oldstate);
  }
  ul_hapi uint32_t ulrand32_boundage(ulrand32_t* rng, uint32_t bound) {
    uint32_t threshold = (~bound + 1u) % bound;
    for (;;) {
      uint32_t r = ulrand32_gen(rng);
      if(r >= threshold) return r % bound;
    }
  }
  ul_hapi void ulrand32_advance(ulrand32_t* rng, uint64_t delta) {
    uint64_t state = rng->state;
    uint64_t cur_mult = _ULRAND32_DEFAULT_MULTIPLIER;
    uint64_t cur_plus = rng->inc;
    uint64_t acc_mult = 1u;
    uint64_t acc_plus = 0u;
    while(delta > 0) {
      if(delta & 1) {
        acc_mult *= cur_mult;
        acc_plus = acc_plus * cur_mult + cur_plus;
      }
      cur_plus = (cur_mult + 1) * cur_plus;
      cur_mult *= cur_mult;
      delta >>= 1;
    }
    rng->state = acc_mult * state + acc_plus;
  }
#endif



#ifdef __SIZEOF_INT128__
  typedef unsigned __int128 _ulrand_u128_t;
  typedef struct ulrand64_t {
    _ulrand_u128_t state;
    _ulrand_u128_t inc;
  } ulrand64_t;
  #define _ULRAND64_DEFAULT_MULTIPLIER \
    (ul_static_cast(_ulrand_u128_t, 0X4385DF649FCCF645u) << 64 | 0X2360ED051FC65DA4u)
  #define _ULRAND64_DEFAULT_INCREMENT \
    (ul_static_cast(_ulrand_u128_t, 0X14057B7EF767814Fu) << 64 | 0X5851F42D4C957F2Du)
  ul_hapi uint64_t _ulrand64_rotr(uint64_t val, unsigned rot) {
    return (val >> rot) | (val << ((~rot + 1u) & 63));
  }
  ul_hapi uint64_t _ulrand64_output(_ulrand_u128_t state) {
    return _ulrand64_rotr(
      ul_static_cast(uint64_t, ((state >> 29u) ^ state) >> 58u),
      ul_static_cast(unsigned, state >> 122u)
    );
  }
  ul_hapi void ulrand64_step(ulrand64_t* rng) {
    rng->state = rng->state * _ULRAND64_DEFAULT_MULTIPLIER + rng->inc;
  }
  ul_hapi void ulrand64_init2(ulrand64_t* rng, _ulrand_u128_t initstate, _ulrand_u128_t initseq) {
    rng->state = 0u; rng->inc = (initseq << 1u) | 1u;
    ulrand64_step(rng);
    rng->state += initstate;
    ulrand64_step(rng);
  }
  ul_hapi void ulrand64_init(ulrand64_t* rng, _ulrand_u128_t initstate) {
    rng->state = 0u; rng->inc = _ULRAND64_DEFAULT_INCREMENT;
    ulrand64_step(rng);
    rng->state += initstate;
    ulrand64_step(rng);
  }
  ul_hapi uint64_t ulrand64_gen(ulrand64_t* rng) {
    _ulrand_u128_t oldstate = rng->state;
    ulrand64_step(rng);
    return _ulrand64_output(oldstate);
  }
  ul_hapi uint64_t ulrand64_boundage(ulrand64_t* rng, uint64_t bound) {
    uint64_t threshold = (~bound + 1u) % bound;
    for (;;) {
      uint64_t r = ulrand64_gen(rng);
      if(r >= threshold) return r % bound;
    }
  }
  ul_hapi void ulrand64_advance(ulrand64_t* rng, _ulrand_u128_t delta) {
    _ulrand_u128_t state = rng->state;
    _ulrand_u128_t cur_mult = _ULRAND64_DEFAULT_MULTIPLIER;
    _ulrand_u128_t cur_plus = rng->inc;
    _ulrand_u128_t acc_mult = 1u;
    _ulrand_u128_t acc_plus = 0u;
    while(delta > 0) {
      if(delta & 1) {
        acc_mult *= cur_mult;
        acc_plus = acc_plus * cur_mult + cur_plus;
      }
      cur_plus = (cur_mult + 1) * cur_plus;
      cur_mult *= cur_mult;
      delta >>= 1;
    }
    rng->state = acc_mult * state + acc_plus;
  }
#endif

#endif /* ULRAND_H */
