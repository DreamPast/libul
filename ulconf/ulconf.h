#ifndef ULCONF_H
#define ULCONF_H

/**
 * @def UL_PEDANTIC
 * @brief Enables pedantic mode (disable all non-standard features).
 */
/* #define UL_PEDANTIC */



/**
 * @def ul_likely
 * @brief Hints to the compiler that the condition is more likely to be true.
 */
/**
 * @def ul_unlikely
 * @brief Hints to the compiler that the condition is more likely to be false.
 */
#if defined(__has_builtin) && !defined(UL_PEDANTIC)
  #if __has_builtin(__builtin_expect)
    #ifndef ul_likely
      #define ul_likely(x) __builtin_expect(!!(x), 1)
    #endif
    #ifndef ul_unlikely
      #define ul_unlikely(x) __builtin_expect(!!(x), 0)
    #endif
  #endif
#endif /* ul_likely + ul_unlikely */
#ifndef ul_likely
  #define ul_likely(x) (x)
#endif /* ul_likely */
#ifndef ul_unlikely
  #define ul_unlikely(x) (x)
#endif /* ul_unlikely */

/**
 * @def ul_label_cold
 * @brief Marks a label as unlikely to be taken.
 * @example ```c
 *         int foo(int x) {
 *           if(x < 0) goto fallback;
 *           return 0;
 *         fallback:
 *           ul_label_cold;
 *           return 1;
 *         }
 *         ```
 */
#if !defined(ul_label_cold) && defined(__has_attribute) && !defined(UL_PEDANTIC)
  #if __has_attribute(cold)
    #define ul_label_cold __attribute__((cold))
  #endif
#endif
#ifndef ul_label_cold
  #define ul_label_cold
#endif

/**
 * @def ul_label_hot
 * @brief Marks a label as unlikely to be taken.
 * @example ```c
 *         int foo(int x) {
 *           if(x < 0) goto fallback;
 *           return 0;
 *         fallback:
 *           ul_label_hot;
 *           return 1;
 *         }
 *         ```
 */
#if !defined(ul_label_hot) && defined(__has_attribute) && !defined(UL_PEDANTIC)
  #if __has_attribute(hot)
    #define ul_label_hot __attribute__((hot))
  #endif
#endif
#ifndef ul_label_hot
  #define ul_label_hot
#endif

/**
 * @def ul_unreachable
 * @brief Hints to the compiler that the code is unreachable.
 */
#ifndef ul_unreachable
  #if !defined(UL_PEDANTIC) && defined(__GNUC__) || defined(__clang__)
    #define ul_unreachable() __builtin_unreachable()
  #elif !defined(UL_PEDANTIC) && defined(_MSC_VER) /* Visual Studio 6 */
    #define ul_unreachable() __assume(0)
  #else
    #define ul_unreachable() ((void)0)
  #endif
#endif /* ul_unreachable */

/**
 * @def ul_assume
 * @brief Hints to the compiler that the condition is always true.
 */
#if !defined(ul_assume) & !defined(UL_PEDANTIC) && defined(_MSC_VER) /* Visual Studio 6 */
  #define ul_assume(cond) __assume(cond)
#endif /* ul_assume */
#if !defined(ul_assume) && !defined(UL_PEDANTIC) && defined(__has_builtin)
  #if __has_builtin(__builtin_assume)
    #define ul_assume(cond) __builtin_assume(cond)
  #elif __has_builtin(__builtin_unreachable) && __has_builtin(__builtin_expect)
    #define ul_assume(cond) (__builtin_expect(!(cond), 0) ? __builtin_unreachable() : (void)0)
  #elif __has_builtin(__builtin_unreachable)
    #define ul_assume(cond) (!(cond) ? __builtin_unreachable() : (void)0)
  #endif
#endif /* ul_assume */
#ifndef ul_assume
  #define ul_assume(cond) ((void)(cond))
#endif /* ul_assume */

/**
 * @def ul_assume_aligned
 * @brief Hints to the compiler that the pointer is aligned.
 */
#if !defined(ul_assume_aligned) && defined(__cplusplus) && __cplusplus >= 202002L
  #include <memory>
  #ifdef __cpp_lib_assume_aligned
    #define ul_assume_aligned(exp, align) ::std::assume_aligned<align>(exp)
  #endif
#endif /* ul_assume_aligned */
#if !defined(ul_assume_aligned) && !defined(UL_PEDANTIC) && defined(__has_builtin)
  #if __has_builtin(__builtin_assume_aligned)
    #define ul_assume_aligned(exp, align) __builtin_assume_aligned(exp, align)
  #endif
#endif /* ul_assume_aligned */
#ifndef ul_assume_aligned
  #define ul_assume_aligned(exp, align) ((void)0)
#endif /* ul_assume_aligned */

/**
 * @def ul_restrict
 * @brief Restrict qualifier (C99/C++ extensions). Marks pointers with same types not to overlap.
 */
#ifndef ul_restrict
  #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #define ul_restrict restrict
  #elif !defined(UL_PEDANTIC)
    #if defined(_MSC_VER) && _MSC_VER >= 1400 /* Visual Studio 2005 */
      #define ul_restrict __restrict
    #elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 304
      #define ul_restrict __restrict__
    #endif
  #endif
  #ifndef ul_restrict
    #define ul_restrict
  #endif
#endif /* ul_restrict */

/**
 * @def ul_may_alias
 * @brief Disables strict aliasing rules for given type (fallback: not defined).
 */
#ifndef ul_may_alias
  #if !defined(UL_PEDANTIC) && defined(__has_attribute)
    #if __has_attribute(__may_alias__)
      #define ul_may_alias __attribute__((__may_alias__))
    #endif
  #endif
#endif /* ul_may_alias */

/**
 * @def ul_noexcept
 * @brief Marks a function not to throw exceptions.
 */
#ifndef ul_noexcept
  #ifdef __cplusplus
    #if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900 /* Visual Studio 2015 */)
      #define ul_noexcept noexcept
    #else
      #define ul_noexcept throw()
    #endif
  #else
    #define ul_noexcept
  #endif
#endif /* ul_noexcept */

/**
 * @def ul_constant_p
 * @brief Checks if the expression is a constant.
 * @example ul_constant_p(1 + 2) -> 1
 */
#if !defined(ul_constant_p) && !defined(UL_PEDANTIC) && defined(__has_builtin)
  #if __has_builtin(__builtin_constant_p)
    #define ul_constant_p(exp) __builtin_constant_p(exp)
  #endif
#endif /* ul_constant_p */
#ifndef ul_constant_p
  #define ul_constant_p(exp) (0)
#endif /* ul_constant_p */

/**
 * @def ul_prefetch
 * @brief Prefetches the data into the cache.
 * @details `rw` is the read/write type (0 = read, 1 = write, 2 = shard read), dfault is 0.
 * @details `locality` is the locality hint (0 = no locality, 1 = low locality,
 *          2 = moderate locality, 3 = high locality), default is 3.
 */
#if !defined(ul_prefetch) && !defined(UL_PEDANTIC) && defined(__has_builtin)
  #if __has_builtin(__builtin_prefetch)
    #define ul_prefetch(addr, rw, locality) __builtin_prefetch(addr, rw, locality)
  #endif
#endif /* ul_prefetch */
#ifndef ul_prefetch
  #define ul_prefetch(addr, rw, locality) ((void)0)
#endif /* ul_prefetch */



/**
 * @def ul_static_cast
 * @brief Converts between types using a combination of implicit and user-defined conversions
 * (same as C++ `static_cast`).
 */
#ifndef ul_static_cast
  #ifdef __cplusplus
    #define ul_static_cast(T, val) (static_cast<T>(val))
  #else
    #define ul_static_cast(T, val) ((T)(val))
  #endif
#endif /* ul_static_cast */

/**
 * @def ul_reinterpret_cast
 * @brief Converts between types by reinterpreting the underlying bit pattern (same as C++ `reinterpret_cast`).
 */
#ifndef ul_reinterpret_cast
  #ifdef __cplusplus
    #define ul_reinterpret_cast(T, val) (reinterpret_cast<T>(val))
  #else
    #define ul_reinterpret_cast(T, val) ((T)(val))
  #endif
#endif /* ul_reinterpret_cast */

/**
 * @def UL_JOIN
 * @brief Concatenates two identifiers.
 * @example UL_JOIN(foo, bar) -> foobar
 */
#ifndef UL_JOIN
  #define __UL_JOIN2(X, Y) X##Y
  #define __UL_JOIN(X, Y) __UL_JOIN2(X, Y)
  #define UL_JOIN(X, Y) __UL_JOIN(X, Y)
#endif /* UL_JOIN */

/**
 * @def ul_static_assert
 * @brief Static assertion checking.
 */
#ifndef ul_static_assert
  #if defined(__cplusplus) && __cplusplus >= 201103L
    #define ul_static_assert(cond, msg) static_assert(cond, msg)
  #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #define ul_static_assert(cond, msg) static_assert(cond, msg)
  #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define ul_static_assert(cond, msg) _Static_assert(cond, msg)
  #elif defined(__cplusplus) /* clang-format off */
    namespace __ul_static_assert {
      template<bool x> struct __UL_STATIC_CAST_FAILURE;
      template<> struct __UL_STATIC_CAST_FAILURE<true> { enum { value = 1 }; };
    }
    #define ul_static_assert(cond, msg) enum { UL_JOIN(___UL_STATIC_ASSERT_, __LINE__) = \
        sizeof(::__ul_static_assert::__UL_STATIC_CAST_FAILURE<static_cast<bool>(cond)>::value) } /* clang-format on */
  #else

    #define ul_static_assert(cond, msg) /* clang-format off */ \
      typedef struct { int __error_if_negative: (cond) ? 1 : -1; } UL_JOIN(___UL_STATIC_ASSERT_, __LINE__)
    /* clang-format on */
  #endif
#endif /* ul_static_assert */

/**
 * @def UL_STRINGIZE
 * @brief Converts the argument to a string.
 * @example UL_STRINGIZE(foo) -> "foo"
 */
#ifndef UL_STRINGIZE
  #define __UL_STRINGIZE(X) #X
  #define UL_STRINGIZE(X) __UL_STRINGIZE(X)
#endif /* UL_STRINGIZE */

/**
 * @def ul_nullptr
 * @brief Null pointer constant.
 */
#ifndef ul_nullptr
  #if defined(__cplusplus) && __cplusplus >= 201103L
    #define ul_nullptr nullptr
  #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #define ul_nullptr nullptr
  #else
    #include <stddef.h>
    #define ul_nullptr NULL
  #endif
#endif /* ul_nullptr */



/**
 * @def UL_HAS_STDINT_H
 * @brief Checks if `<stdint.h>` exists.
 */
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

/**
 * @def ul_pragma
 * @brief Pragma directive (useful in macros).
 * @example ul_pragma(GCC diagnostic ignored "-Wdeprecated-declarations")
 */
#ifndef ul_pragma
  #if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
    #define ul_pragma(val) _Pragma(#val)
  #endif
  #if !defined(ul_pragma) && !defined(UL_PEDANTIC)
    #if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 3)
      #define ul_pragma(val) _Pragma(#val)
    #elif defined(_MSC_VER) && _MSC_VER >= 1500 /* Visual Studio 2008 */
      #define ul_pragma(val) __pragma(val)
    #endif
  #endif
  /* #define ul_pragma(val) */
#endif /* ul_pragma */

/**
 * @def ul_fallthrough
 * @brief Marks a fallthrough in a switch statement (it's used to suppress warnings).
 */
#if !defined(ul_fallthrough) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L && defined(__has_c_attribute)
  #if __has_c_attribute(fallthrough)
    #define ul_fallthrough [[fallthrough]]
  #endif
#endif /* ul_fallthrough */
#if !defined(ul_fallthrough) && (defined(__cplusplus) && __cplusplus >= 201103L && defined(__has_cpp_attribute))
  #if __has_cpp_attribute(fallthrough)
    #define ul_fallthrough [[fallthrough]]
  #endif
#endif /* ul_fallthrough */
#if !defined(ul_fallthrough) && !defined(UL_PEDANTIC)
  #if defined(__has_attribute)
    #if __has_attribute(fallthrough)
      #define ul_fallthrough __attribute__((__fallthrough__))
    #endif
  #endif
#endif /* ul_fallthrough */
#ifndef ul_fallthrough
  #define ul_fallthrough ((void)0)
#endif /* ul_fallthrough */

/**
 * @def UL_VLA
 * @brief Variable Length Array (VLA) declaration (fallback: not defined).
 * @example ```c
 *          int foo(int n) {
 *            UL_VLA(arr, int, n);
 *            printf("%p\n", arr);
 *          }
 *          ```
 */
#ifndef UL_VLA
  #if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !__STDC_NO_VLA__) /* since C11, VLA is optional */ \
    || (defined(__STDC_VERSION__) && __STDC_VERSION__ < 201112L && __STDC_VERSION__ >= 199901L)                       \
    || (defined(__GNUC__) /* GNUC 0.9 */ || defined(__clang__))
    #define UL_VLA(varname, T, size) T varname[size]
  #elif defined(_WIN32) && !defined(UL_PEDANTIC)
    #include <malloc.h>
    #ifdef __cplusplus
      #define UL_VLA(varname, T, size) T* varname = reinterpret_cast<T*>(_alloca(sizeof(T) * (size)))
    #else
      #define UL_VLA(varname, T, size) T* varname = (T*)_alloca(sizeof(T) * (size))
    #endif
  #endif
#endif /* UL_VLA */

/**
 * @def UL_FUNCTION
 * @brief Current function name.
 * @example ```c
 *          void foo(void) {
 *            printf("Function: %s\n", UL_FUNCTION);
 *          }
 *          ```
 */
#ifndef UL_FUNCTION
  #if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(__cplusplus) && __cplusplus >= 201103L) \
    || (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 295) /* GNUC 2.95 */                               \
    || (defined(_MSC_VER) && _MSC_VER >= 1310 /* Visual Studio 2013 */) || defined(__clang__)
    #define UL_FUNCTION __func__
  #elif !defined(UL_PEDANTIC) \
    && ((defined(__GNUC__) && __GNUC__ >= 2) || (defined(_MSC_VER) && _MSC_VER >= 1300 /* Visual Studio 2003 */))
    #define UL_FUNCTION __FUNCTION__
  #else
    #define UL_FUNCTION "<unknown>"
  #endif
#endif /* UL_FUNCTION */

/**
 * @def UL_PRETTY_FUNCTION
 * @brief Current pretty function name (fallback: not defined).
 * @example ```c
 *          void foo(void) {
 *            printf("Function: %s\n", UL_PRETTY_FUNCTION);
 *          }
 *          ```
 */
#if !defined(UL_PRETTY_FUNCTION) && !defined(UL_PEDANTIC)
  #if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 200)
    #define UL_PRETTY_FUNCTION __PRETTY_FUNCTION__
  #elif defined(__FUNCSIG__) || (defined(_MSC_VER) && _MSC_VER >= 1300 /* Visual Studio 2003 */)
    #define UL_PRETTY_FUNCTION __FUNCSIG__
  #endif
#endif /* UL_PRETTY_FUNCTION */



/**
 * @def ul_unused
 * @brief Marks a variable or function may be unused.
 */
#if !defined(ul_unused) && defined(__has_attribute) && !defined(UL_PEDANTIC)
  #if __has_attribute(unused)
    #define ul_unused __attribute__((unused))
  #endif
#endif /* ul_unused */
#if !defined(ul_unused) && (defined(__cplusplus) && __cplusplus >= 201103L && defined(__has_cpp_attribute))
  #if __has_cpp_attribute(maybe_unused)
    #define ul_unused [[maybe_unused]]
  #endif
#endif /* ul_unused */
#if !defined(ul_unused) && (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L && defined(__has_c_attribute))
  #if __has_c_attribute(maybe_unused)
    #define ul_unused [[maybe_unused]]
  #endif
#endif /* ul_unused */
#ifndef ul_unused
  #define ul_unused
#endif /* ul_unused */

/**
 * @def ul_inline
 * @brief Marks a function as inline.
 * @note When an inline function occurs more than once in the program, all those will be identical.
 */
#ifndef ul_inline
  #if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
    #define ul_inline inline
  #else
    #define ul_inline
  #endif
#endif /* ul_inline */

/**
 * @def ul_constexpr
 * @brief Marks a function able to be evaluated at compile time (C++11).
 */
#if !defined(ul_constexpr) && defined(__cplusplus)
  #if __cplusplus >= 201103L
    #define ul_constexpr constexpr
    #define UL_CONSTEXPR_INIT /* clang-format off */ { }                                     /* clang-format on */
  #elif defined(_MSC_VER) && _MSC_VER >= 1900 /* Visual Studio 2015 */
    #define ul_constexpr constexpr
    #define UL_CONSTEXPR_INIT /* clang-format off */ { } /* clang-format on */
  #endif
#endif /* ul_constexpr */
#ifndef ul_constexpr
  #define ul_constexpr
  #define UL_CONSTEXPR_INIT
#endif /* ul_constexpr */

/**
 * @def ul_dl_export
 * @brief Mark a symbol to be exported in dynamic library.
 */
#if !defined(ul_dl_export) && !defined(UL_PEDANTIC)
  #if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 4)
    #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
      #define ul_dl_export __attribute__((__dllexport__))
    #else
      #define ul_dl_export __attribute__((__visibility__("default")))
    #endif
  #elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define ul_dl_export __declspec(dllexport)
  #endif
#endif /* ul_dl_export */
#if !defined(ul_dl_export)
  #define ul_dl_export
#endif /* ul_dl_export */

/**
 * @def ul_dl_import
 * @brief Mark a symbol to be imported from dynamic library.
 */
#if !defined(ul_dl_import) && !defined(UL_PEDANTIC)
  #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
    #if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 4)
      #define ul_dl_import __attribute__((__dllimport__))
    #else
      #define ul_dl_import __declspec(dllimport)
    #endif
  #endif
#endif /* ul_dl_import */
#if !defined(ul_dl_import)
  #define ul_dl_import
#endif /* ul_dl_import */

/**
 * @def ul_hidden
 * @brief Mark a symbol to be hidden.
 */
#if !defined(ul_hidden) && !defined(UL_PEDANTIC)
  #if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 4)
    #define ul_hidden __attribute__((__visibility__("hidden")))
  #endif
#endif /* ul_hidden */
#if !defined(ul_hidden)
  #define ul_hidden
#endif /* ul_hidden */

/**
 * @def ul_forceinline
 * @brief Forces a function to be inlined (however, the compiler may ignore it).
 */
#if !defined(ul_forceinline) && !defined(UL_PEDANTIC) && defined(__has_attribute)
  #if __has_attribute(__always_inline__)
    #define ul_forceinline __attribute__((__always_inline__))
  #endif
#endif /* ul_forceinline */
#if !defined(ul_forceinline) && !defined(UL_PEDANTIC) && defined(_MSC_VER)
  #define ul_forceinline __forceinline
#endif /* ul_forceinline */
#if !defined(ul_forceinline)
  #if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
    #define ul_forceinline inline
  #else
    #define ul_forceinline
  #endif
#endif /* ul_forceinline */

/**
 * @def ul_noinline
 * @brief Marks a function to not be inlined.
 */
#if !defined(ul_noinline) && !defined(UL_PEDANTIC) && defined(__has_attribute)
  #if __has_attribute(noinline)
    #define ul_noinline __attribute__((noinline))
  #elif __has_attribute(__noinline__)
    #define ul_noinline __attribute__((__noinline__))
  #endif
#endif /* ul_noinline */
#ifndef ul_noinline
  #if !defined(UL_PEDANTIC) && defined(_MSC_VER) && _MSC_VER >= 1310 /* Visual Studio 2003 */
    #define ul_noinline __declspec(noinline)
  #else
    #define ul_noinline
  #endif
#endif /* ul_noinline */

/**
 * @def ul_noreturn
 * @brief Marks a function that doesn't return.
 */
#if !defined(ul_noreturn) && (defined(__cplusplus) && __cplusplus >= 201103L && defined(__has_cpp_attribute))
  #if __has_cpp_attribute(noreturn)
    #define ul_noreturn [[noreturn]]
  #endif
#endif /* ul_noreturn */
#if !defined(ul_noreturn) && (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L && defined(__has_c_attribute))
  #if __has_c_attribute(noreturn)
    #define ul_noreturn [[noreturn]]
  #endif
#endif /* ul_noreturn */
#if !defined(ul_noreturn) && (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
  #define ul_noreturn _Noreturn
#endif /* ul_noreturn */
#if !defined(ul_noreturn) && !defined(UL_PEDANTIC)
  #if defined(_MSC_VER)
    #define ul_noreturn __declspec(noreturn)
  #elif defined(__has_attribute)
    #if __has_attribute(noreturn)
      #define ul_noreturn __attribute__((noreturn))
    #elif __has_attribute(__noreturn__)
      #define ul_noreturn __attribute__((__noreturn__))
    #endif
  #endif
#endif /* ul_noreturn */
#ifndef ul_noreturn
  #define ul_noreturn
#endif /* ul_noreturn */

/**
 * @def ul_nodiscard
 * @brief Marks a function that the return value must be used.
 */
#if !defined(ul_nodiscard) && (defined(__cplusplus) && __cplusplus >= 201103L && defined(__has_cpp_attribute))
  #if __has_cpp_attribute(nodiscard)
    #define ul_nodiscard [[nodiscard]]
  #endif
#endif /* ul_nodiscard */
#if !defined(ul_nodiscard) && (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L && defined(__has_c_attribute))
  #if __has_c_attribute(nodiscard)
    #define ul_nodiscard [[nodiscard]]
  #endif
#endif /* ul_nodiscard */
#if !defined(ul_nodiscard) && !defined(UL_PEDANTIC) && defined(__has_attribute)
  #if __has_attribute(warn_unused_result)
    #define ul_nodiscard __attribute__((warn_unused_result))
  #endif
#endif /* ul_nodiscard */
#if !defined(ul_nodiscard) && !defined(UL_PEDANTIC) && defined(_HAS_NODISCARD)
  #define ul_nodiscard _NODISCARD
#endif /* ul_nodiscard */                                                                    /* */
#if !defined(ul_nodiscard) && !defined(UL_PEDANTIC) && defined(_MSC_VER) && _MSC_VER >= 1700 /* Visual Studio 2012 */
  #define ul_nodiscard _Check_return_
#endif /* ul_nodiscard */
#ifndef ul_nodiscard
  #define ul_nodiscard
#endif /* ul_nodiscard */

/**
 * @def ul_deprecated
 * @brief Marks a function as deprecated.
 */
#if !defined(ul_deprecated)
  #if !defined(ul_deprecated)
    #if (defined(__cplusplus) && __cplusplus >= 201103L && defined(__has_cpp_attribute))
      #if __has_cpp_attribute(deprecated)
        #define ul_deprecated(msg) [[deprecated(msg)]]
      #endif
    #elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L && defined(__has_c_attribute))
      #if __has_c_attribute(deprecated)
        #define ul_deprecated(msg) [[deprecated(msg)]]
      #endif
    #endif
  #endif
  #if !defined(ul_deprecated) && !defined(UL_PEDANTIC) && defined(__clang__)
    #if __has_attribute(deprecated)
      #define ul_deprecated(msg) __attribute__((deprecated(msg)))
    #endif
  #endif
  #if !defined(ul_deprecated) && !defined(UL_PEDANTIC)
    #if defined(_MSC_VER) && _MSC_VER >= 1400 /* Visual Studio 2005 */
      #define ul_deprecated(msg) __declspec(deprecated(msg))
    #elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 405
      #define ul_deprecated(msg) __declspec(deprecated(msg))
    #endif
  #endif

  #if !defined(ul_deprecated) && !defined(UL_PEDANTIC) && defined(__GNUC__)
    #define ul_deprecated(msg) __attribute__((deprecated))
  #endif
  #if !defined(ul_deprecated) && !defined(UL_PEDANTIC) \
    && (defined(_MSC_VER) && _MSC_VER >= 1310) /* Visual Studio 2003 */
    #define ul_deprecated(msg) __declspec(deprecated)
  #endif
  #ifndef ul_deprecated
    #define ul_deprecated(msg)
  #endif
#endif /* ul_deprecated */

/**
 * @def ul_alignas
 * @brief Aligns the variable to the specified alignment (fallback: not defined).
 */
#ifndef ul_alignas
  #if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
    #define ul_alignas(x) alignas(x)
  #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define ul_alignas(x) _Alignas(x)
  #endif
  #if !defined(UL_PEDANTIC)
    #if defined(_MSC_VER) && _MSC_VER >= 1310 /* Visual Studio 2003 */
      #define ul_alignas(x) __declspec(align(x))
    #elif defined(__has_attribute)
      #if __has_attribute(__aligned__)
        #define ul_alignas(x) __attribute__((__aligned__(x)))
      #elif __has_attribute(aligned)
        #define ul_alignas(x) __attribute__((aligned(x)))
      #endif
    #endif
  #endif
#endif

/**
 * @def ul_alignof
 * @brief Returns the alignment of the type (fallback: not defined).
 */
#ifndef ul_alignof
  #if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
    #define ul_alignof(x) alignof(x)
  #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define ul_alignof(x) _Alignof(x)
  #elif !defined(UL_PEDANTIC) && defined(_MSC_VER) && _MSC_VER >= 1310 /* Visual Studio 2003 */
    #define ul_alignof(x) __alignof(x)
  #elif !defined(UL_PEDANTIC) && defined(__GNUC__) || defined(__clang__)
    #define ul_alignof(x) __alignof__(x)
  #endif
#endif

/**
 * @def ul_pack_struct
 * @brief Packs the structure to the smallest possible size (fallback: not defined).
 * @example ```c
 *         ul_pack_struct(foo) {
 *           int a;
 *           char b;
 *         };
 *         ```
 */
#if !defined(ul_pack_struct) && !defined(UL_PEDANTIC)
  #if defined(_MSC_VER) && _MSC_VER >= 1500 /* Visual Studio 2008 */
    /* "#pragma pack" is supported since VC6, but "_Pragma" has version requirements */
    #define ul_pack_struct(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
  #elif defined(__has_attribute)
    #if __has_attribute(packed)
      #define ul_pack_struct(name) struct name __attribute__((packed))
    #endif
  #endif
#endif /* ul_pack_struct */



#if !defined(ul_aligned_alloc) || !defined(ul_aligned_free)
  #if defined(_WIN32)
    #if !defined(UL_PEDANTIC)
      #include <malloc.h>
      #define ul_aligned_alloc(align, size) _aligned_malloc((size), (align))
      #define ul_aligned_free(p) _aligned_free(p)
    #endif
  #elif defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE)          /* glibc */ \
    || (defined(__cplusplus) && __cplusplus >= 201703L)           /* C++17 */ \
    || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) /* C11 */
    /* Note that `aligned_alloc` is unavailable on Windows */
    #include <stdlib.h>
    #define ul_aligned_alloc(align, size) aligned_alloc((align), (size))
    #define ul_aligned_free(p) free(p)
  #endif
#endif /* ul_aligned_alloc + ul_aligned_free */
#if !defined(ul_aligned_alloc) || !defined(ul_aligned_free)
  #include <assert.h>
static ul_inline void* _ul_aligned_alloc(size_t align, size_t size) {
  const size_t offset = align - 1 + sizeof(void*);
  void *raw, *ret;
  assert((align & (align - 1)) == 0);
  if(ul_unlikely(!(raw = malloc(size + offset))))
    return ul_nullptr;
  ret = ul_reinterpret_cast(void*, (ul_reinterpret_cast(size_t, raw) + offset) & ~(align - 1));
  ul_reinterpret_cast(void**, ret)[-1] = raw;
  return ret;
}
static ul_inline void _ul_aligned_free(void* p) {
  if(ul_likely(p))
    free(ul_reinterpret_cast(void**, p)[-1]);
}
  #define ul_aligned_alloc(align, size) _ul_aligned_alloc((align), (size))
  #define ul_aligned_free(p) _ul_aligned_free(p)
#endif /* ul_aligned_alloc + ul_aligned_free */

#endif /* ULCONF_H */
