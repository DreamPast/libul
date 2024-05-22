/*
Date and time (like `Date` in Javascript)


# Dependences
  - C89 => milliseconds is always set to 0
  - Windows API => full support
  - POSIX API => full support


# Config macro
  - ULDATE_IGNORE_JULIAN_CALENDAR
    Ignore julian Calendar.
    If it's not defined, will use following rules:
    - If year > 1582, use gregorian calendar.
      In the calendar, a leap year is a year that is exactly divisible by 4,
      except for years that are exactly divisible by 100 but not exactly divisible by 400.
    - If year is 1582, 1582-10-5 ~ 1582-10-14 are removed.
    - If year < 1582, use julian calendar.
      In the calendar, a leap year is a year that is exactly divisible by 4.
  - ULDATE_USE_USEC / ULDATE_USE_MSEC(default) / ULDATE_USE_SEC
    Use 1 microsecond/millisecond(default)/second as "1" unit in `uldate_t`.
    If 32-bit integer is using, select millisecond will make `uldate_t` not hold a year.


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

#ifndef ULDATE_H
#define ULDATE_H

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

#ifndef ul_static_cast
  #ifdef __cplusplus
    #define ul_static_cast(T, val) static_cast<T>(val)
  #else
    #define ul_static_cast(T, val) ((T)(val))
  #endif
#endif /* ul_static_cast */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L /* C99 */
  #define _uldate_trunc(v) trunc(v)
#elif defined(__cplusplus) && __cplusplus >= 201103L /* C++11 */
  #include <cmath>
  #define _uldate_trunc(v) std::trunc(v)
#elif defined(_MSC_VER) && _MSC_VER >= 1800 /* Visual Studio 2013 */
  #define _uldate_trunc(v) trunc(v)
#else
  #define _uldate_trunc(v) ((v) > 0 ? floor(v) : ceil(v))
#endif

#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#if defined(LLONG_MAX)
  typedef long long uldate_int_t;
  typedef unsigned long long uldate_uint_t;
  #define ULDATE_INT_MIN LLONG_MIN
  #define ULDATE_INT_MAX LLONG_MAX
  #define ULDATE_UINT_MAX ULLONG_MAX
  #define ULDATE_INT_C(val) val ## ll
  #define ULDATE_UINT_C(val) val ## ull
#elif defined(_WIN32)
  typedef __int64 uldate_int_t;
  typedef unsigned __int64 uldate_uint_t;
  #define ULDATE_INT_MIN 0x8000000000000000i64
  #define ULDATE_INT_MAX 0x7FFFFFFFFFFFFFFFi64
  #define ULDATE_UINT_MAX 0xFFFFFFFFFFFFFFFFui64
  #define ULDATE_INT_C(val) val ## i64
  #define ULDATE_UINT_C(val) val ## ui64
#else
  typedef long uldate_int_t;
  typedef unsigned long uldate_uint_t;
  #define ULDATE_INT_MIN LONG_MIN
  #define ULDATE_INT_MAX LONG_MAX
  #define ULDATE_UINT_MAX ULONG_MAX
  #define ULDATE_INT_C(val) val ## l
  #define ULDATE_UINT_C(val) val ## ul
#endif

/* starts from "1970-01-01T00:00:00.000Z" */
typedef uldate_int_t uldate_t;
#define ULDATE_INVALID ULDATE_INT_MIN

#if defined(ULDATE_USE_USEC) + defined(ULDATE_USE_MSEC) + defined(ULDATE_USE_SEC) != 1
  #if defined(ULDATE_USE_USEC) + defined(ULDATE_USE_MSEC) + defined(ULDATE_USE_SEC) == 0
    #define ULDATE_USE_MSEC
  #else
    #error "uldate.h: more than 2 macros of `ULDATE_USE_USEC`, `ULDATE_USE_MSEC`, `ULDATE_USE_SEC` are defined"
  #endif
#endif

#if defined(ULDATE_USE_USEC) /* 64-bit: about 292277 years, 32-bit: error */
  #define ULDATE_FROM_NANOSECOND(x)  ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000))
  #define ULDATE_FROM_MICROSECOND(x) ul_static_cast(uldate_int_t, (x))
  #define ULDATE_FROM_MILLISECOND(x) ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000))
  #define ULDATE_FROM_SECOND(x)      ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000000))
  #define ULDATE_FROM_MINUTE(x)      ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(60000000))
  #define ULDATE_FROM_HOUR(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(3600000000))
  #define ULDATE_FROM_DAY(x)         ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(86400000000))
  #define ULDATE_FROM_WEEK(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(604800000000))
  #define ULDATE_FROM_MONTH(x)       ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(2629746000000))
  #define ULDATE_FROM_YEAR(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(31556952000000))

  #define ULDATE_TO_NANOSECOND(x)  ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000))
  #define ULDATE_TO_MICROSECOND(x) ul_static_cast(uldate_int_t, (x))
  #define ULDATE_TO_MILLISECOND(x) ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000))
  #define ULDATE_TO_SECOND(x)      ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000000))
  #define ULDATE_TO_MINUTE(x)      ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(60000000))
  #define ULDATE_TO_HOUR(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(3600000000))
  #define ULDATE_TO_DAY(x)         ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(86400000000))
  #define ULDATE_TO_WEEK(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(604800000000))
  #define ULDATE_TO_MONTH(x)       ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(2629746000000))
  #define ULDATE_TO_YEAR(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(31556952000000))

  #define ULDATE_MILLISECOND_DOUBLE (1000.0)
#elif defined(ULDATE_USE_MSEC) /* 64-bit: about 292277024 years, 32-bit: error */
  #define ULDATE_FROM_NANOSECOND(x)  ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000000))
  #define ULDATE_FROM_MICROSECOND(x) ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000))
  #define ULDATE_FROM_MILLISECOND(x) ul_static_cast(uldate_int_t, (x))
  #define ULDATE_FROM_SECOND(x)      ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000))
  #define ULDATE_FROM_MINUTE(x)      ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(60000))
  #define ULDATE_FROM_HOUR(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(3600000))
  #define ULDATE_FROM_DAY(x)         ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(86400000))
  #define ULDATE_FROM_WEEK(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(604800000))
  #define ULDATE_FROM_MONTH(x)       ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(2629746000))
  #define ULDATE_FROM_YEAR(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(31556952000))

  #define ULDATE_TO_NANOSECOND(x)  ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000000))
  #define ULDATE_TO_MICROSECOND(x) ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000))
  #define ULDATE_TO_MILLISECOND(x) ul_static_cast(uldate_int_t, (x))
  #define ULDATE_TO_SECOND(x)      ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000))
  #define ULDATE_TO_MINUTE(x)      ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(60000))
  #define ULDATE_TO_HOUR(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(3600000))
  #define ULDATE_TO_DAY(x)         ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(86400000))
  #define ULDATE_TO_WEEK(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(604800000))
  #define ULDATE_TO_MONTH(x)       ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(2629746000))
  #define ULDATE_TO_YEAR(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(31556952000))

  #define ULDATE_MILLISECOND_DOUBLE (1.0)
#elif defined(ULDATE_USE_SEC) /* 64-bit: about 292277024626 years, 32-bit: about 68 years(the biggest year is 2038) */
  #define ULDATE_FROM_NANOSECOND(x)  ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000000000))
  #define ULDATE_FROM_MICROSECOND(x) ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000000))
  #define ULDATE_FROM_MILLISECOND(x) ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(1000000))
  #define ULDATE_FROM_SECOND(x)      ul_static_cast(uldate_int_t, (x))
  #define ULDATE_FROM_MINUTE(x)      ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(60))
  #define ULDATE_FROM_HOUR(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(3600))
  #define ULDATE_FROM_DAY(x)         ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(86400))
  #define ULDATE_FROM_WEEK(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(604800))
  #define ULDATE_FROM_MONTH(x)       ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(2629746))
  #define ULDATE_FROM_YEAR(x)        ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(31556952))

  #define ULDATE_TO_NANOSECOND(x)  ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000000000))
  #define ULDATE_TO_MICROSECOND(x) ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000000))
  #define ULDATE_TO_MILLISECOND(x) ul_static_cast(uldate_int_t, (x) * ULDATE_INT_C(1000000))
  #define ULDATE_TO_SECOND(x)      ul_static_cast(uldate_int_t, (x))
  #define ULDATE_TO_MINUTE(x)      ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(60))
  #define ULDATE_TO_HOUR(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(3600))
  #define ULDATE_TO_DAY(x)         ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(86400))
  #define ULDATE_TO_WEEK(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(604800))
  #define ULDATE_TO_MONTH(x)       ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(2629746))
  #define ULDATE_TO_YEAR(x)        ul_static_cast(uldate_int_t, (x) / ULDATE_INT_C(31556952))

  #define ULDATE_MILLISECOND_DOUBLE (0.001)
#endif

#define ULDATE_FROM_NSEC(x) ULDATE_FROM_NANOSECOND(x)
#define ULDATE_FROM_USEC(x) ULDATE_FROM_MICROSECOND(x)
#define ULDATE_FROM_MSEC(x) ULDATE_FROM_MILLISECOND(x)
#define ULDATE_FROM_SEC(x)  ULDATE_FROM_SECOND(x)
#define ULDATE_FROM_MIN(x)  ULDATE_FROM_MINUTE(x)

#define ULDATE_TO_NSEC(x) ULDATE_TO_NANOSECOND(x)
#define ULDATE_TO_USEC(x) ULDATE_TO_MICROSECOND(x)
#define ULDATE_TO_MSEC(x) ULDATE_TO_MILLISECOND(x)
#define ULDATE_TO_SEC(x)  ULDATE_TO_SECOND(x)
#define ULDATE_TO_MIN(x)  ULDATE_TO_MINUTE(x)

/* Return GMT offset minutes of timezone. For example, UTC+8 will return +480 minutes. */
ul_hapi int uldate_get_gmtoff_minutes(void);
ul_hapi uldate_t uldate_utc_to_locale(uldate_t utc);
ul_hapi uldate_t uldate_locale_to_utc(uldate_t loc);
ul_hapi uldate_t uldate_now_utc(void);
ul_hapi uldate_t uldate_now_locale(void);

ul_hapi uldate_t uldate_from_ms(uldate_int_t ms_since_epoch);
ul_hapi uldate_t uldate_from_mday(uldate_int_t year, uldate_int_t mon, uldate_int_t mday);
ul_hapi uldate_t uldate_from_yday(uldate_int_t year, uldate_int_t yday);
ul_hapi uldate_t uldate_from_wday_sunday(uldate_int_t year, uldate_int_t week, uldate_int_t wday);
ul_hapi uldate_t uldate_from_wday_monday(uldate_int_t year, uldate_int_t week, uldate_int_t wday);
ul_hapi uldate_t uldate_from_time(uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec);

ul_hapi uldate_t uldate_from_mday_time(
  uldate_int_t year, uldate_int_t mon, uldate_int_t mday,
  uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec
);
ul_hapi uldate_t uldate_from_yday_time(
  uldate_int_t year, uldate_int_t yday,
  uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec
);
ul_hapi uldate_t uldate_from_wday_sunday_time(
  uldate_int_t year, uldate_int_t week, uldate_int_t wday,
  uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec
);
ul_hapi uldate_t uldate_from_wday_monday_time(
  uldate_int_t year, uldate_int_t week, uldate_int_t wday,
  uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec
);

ul_hapi uldate_t uldate_from_mday_time_double(
  double year, double mon, double mday,
  double hour, double min, double sec, double msec
);
ul_hapi uldate_t uldate_from_yday_time_double(
  double year, double yday,
  double hour, double min, double sec, double msec
);

/*
  Structure holding date and time broken down into its components.
  Note:
  - `year` is different from `tm_year`, it's since year zero.
  - `mday` is different from `tm_mday`, it counts from 0.
*/
typedef struct uldate_tm_t {
  uldate_int_t year; /* years since year zero */
  int mon; /* months since January, range [0, 11] */
  int mday; /* day of the month, range [0, 30] */
  int hour; /* hours since midnight, range [0, 23] */
  int min; /* minutes after the hour, range [0, 59] */
  int sec; /* seconds after the minute, range [0, 59] (leap second not supported) */
  int msec; /* milliseconds after the second, range [0, 999] */
  int usec; /* microseconds after the millisecond, range [0, 999] */

  int wday; /* days since Sunday, range [0, 6] */
  int yday; /* days since January 1, range [0, 365] */
} uldate_tm_t;

ul_hapi void uldate_to_tm(uldate_t date, uldate_tm_t* tm);
ul_hapi uldate_t uldate_from_tm(const uldate_tm_t* tm);
ul_hapi uldate_t uldate_from_tm_normalized(uldate_tm_t* tm);

ul_hapi int uldate_tm_get_week_sunday(const uldate_tm_t* tm);
ul_hapi int uldate_tm_get_week_monday(const uldate_tm_t* tm);
/* return -1 if failed, otherwise return the week index */
ul_hapi int uldate_tm_get_iso8601_week(const uldate_tm_t* tm, uldate_int_t* p_year);

ul_hapi int uldate_get_week_monday(uldate_t date);
ul_hapi int uldate_get_week_sunday(uldate_t date);
/* return -1 if failed, otherwise return the week index */
ul_hapi int uldate_get_iso8601_week(uldate_t date, uldate_int_t* p_year);

/* gets the length needed for the buffer (including tail '\0') to format `tm`. returns 0 if failed. */
ul_hapi size_t uldate_tm_format_len(const char* fmt, const uldate_tm_t* tm);
/* format `tm` into `dest`. returns length (including tail '\0'), or 0 if failed. */
ul_hapi size_t uldate_tm_format(char* dest, size_t len, const char* fmt, const uldate_tm_t* tm);
/* parse `src` to get `tm`. return the end position of `src`, or NULL if failed. */
ul_hapi const char* uldate_tm_parse(const char* src, const char* fmt, uldate_tm_t* tm);

ul_hapi size_t uldate_format_len(const char* fmt, uldate_t date);
ul_hapi size_t uldate_format(char* dest, size_t len, const char* fmt, uldate_t date);
ul_hapi const char* uldate_parse(const char* src, const char* fmt, uldate_t* date);

/* equivalent to "%a, %d %b %Y %T GMT" (30 bytes), for example, "Tue, 16 Apr 2024 07:16:47 GMT" */
ul_hapi size_t uldate_to_utc_string(char* dest, size_t len, uldate_t utc);
/*  equivalent to "%a %b %d %Y %T GMT%z" (34 bytes), for example, "Tue Apr 16 2024 15:16:47 GMT+0800" */
ul_hapi size_t uldate_to_string(char* dest, size_t len, uldate_t utc);
/* equivalent to "%FT%T.%+Z" (25 bytes), for example, "2024-04-16T07:16:47.000Z" */
ul_hapi size_t uldate_to_iso_string(char* dest, size_t len, uldate_t utc);
/* equivalent to "%D, %T %p" (22 bytes), for example, "04/16/24, 07:16:47 AM" */
ul_hapi size_t uldate_to_locale_string(char* dest, size_t len, uldate_t utc);

ul_hapi time_t uldate_to_ctime(uldate_t date);
ul_hapi uldate_t uldate_from_ctime(time_t ctime);

ul_hapi void uldate_tm_to_ctm(const uldate_tm_t* tm, struct tm* ctm);
ul_hapi void uldate_tm_from_ctm(const struct tm* ctm, uldate_tm_t* tm);

ul_hapi void uldate_to_ctm(uldate_t date, struct tm* ctm);
ul_hapi uldate_t uldate_from_ctm(const struct tm* ctm);



/**************
 * IMPLEMENTS *
 **************/



#if defined(_WIN32)
  #include <Windows.h>
#endif
#if (defined(_DEFAULT_SOURCE) && (_DEFAULT_SOURCE+0)) || (defined(_BSD_SOURCE) && (_BSD_SOURCE+0))
  #include <sys/time.h>
#endif

ul_hapi int uldate_get_gmtoff_minutes(void) {
#if defined(_WIN32)
  TIME_ZONE_INFORMATION info;
  if(ul_unlikely(GetTimeZoneInformation(&info) == TIME_ZONE_ID_INVALID)) return 0;
  return -info.Bias;
#elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
    (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE+0) >= 1) || \
    (defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE+0)) || \
    (defined(_BSD_SOURCE) && (_BSD_SOURCE+0)) || \
    (defined(_SVID_SOURCE) && (_SVID_SOURCE+0)) || \
    (defined(_POSIX_SOURCE) && (_POSIX_SOURCE+0))
  time_t ti = 100000;
  struct tm tm;
  if(ul_unlikely(localtime_r(&ti, &tm) == NULL)) return 0;
  return tm.tm_gmtoff / 60;
#else
  time_t ti = 100000;
  time_t gm_ti, loc_ti;
  struct tm* tm;

  tm = gmtime(&ti);
  if(ul_unlikely(tm == NULL)) return 0;
  gm_ti = mktime(tm);
  if(ul_unlikely(gm_ti == -1)) return 0;


  tm = localtime(&ti);
  if(ul_unlikely(tm == NULL)) return 0;
  loc_ti = mktime(tm);
  if(ul_unlikely(loc_ti == -1)) return 0;

  return (loc_ti - gm_ti) / 60;
#endif
}

ul_hapi uldate_t uldate_utc_to_locale(uldate_t utc) {
  return utc + ULDATE_FROM_MINUTE(uldate_get_gmtoff_minutes());
}
ul_hapi uldate_t uldate_locale_to_utc(uldate_t loc) {
  return loc - ULDATE_FROM_MINUTE(uldate_get_gmtoff_minutes());
}

ul_hapi uldate_t uldate_now_utc(void) {
#if defined(_WIN32)
  SYSTEMTIME system_time;
  FILETIME file_time;
  BOOL success;
  uldate_uint_t time;

  GetSystemTime(&system_time);
  success = SystemTimeToFileTime(&system_time, &file_time); (void)success;
  if(ul_unlikely(!success)) return ULDATE_INVALID;
  time = ul_static_cast(uldate_uint_t, file_time.dwLowDateTime)
    | (ul_static_cast(uldate_uint_t, file_time.dwHighDateTime) << 32);
  return ULDATE_FROM_MICROSECOND((time - ULDATE_INT_C(116444736000000000)) / 10);
#elif (defined(_DEFAULT_SOURCE) && (_DEFAULT_SOURCE+0)) || (defined(_BSD_SOURCE) && (_BSD_SOURCE+0))
  struct timeval tv;
  if(gettimeofday(&tv, NULL) == -1) return ULDATE_INVALID;
  return ULDATE_FROM_SECOND(ul_static_cast(uldate_int_t, tv.tv_sec)) + ULDATE_FROM_MICROSECOND(tv.tv_usec);
#else
  time_t sec = time(NULL);
  if(sec == ul_static_cast(time_t, -1)) return ULDATE_INVALID;
  return ULDATE_FROM_SECOND(ul_static_cast(uldate_int_t, sec));
#endif
}
ul_hapi uldate_t uldate_now_locale(void) {
  const uldate_t utc = uldate_now_utc();
  return ul_likely(utc != ULDATE_INVALID) ? uldate_utc_to_locale(utc) : ULDATE_INVALID;
}

#ifdef ULDATE_IGNORE_JULIAN_CALENDAR
  /* return days of the first day of the given year from '1970', negative value is accepted */
  ul_hapi uldate_int_t _uldate_days_from_year(uldate_int_t y) {
    if(ul_likely(y > 1970))
      return 365 * (y - 1970) + (y - 1969) / 4 - (y - 1901) / 100 + (y - 1601) / 400;
    else
      return 365 * (y - 1970) + (y - 1972) / 4 - (y - 2000) / 100 + (y - 2000) / 400;
  }
  ul_hapi int _uldate_days_in_year(uldate_int_t y) {
    return 365 + !(y % 4) - !(y % 100) + !(y % 400);
  }
  ul_hapi int _uldate_days_in_month(uldate_int_t y, int mi) {
    static const int TABLE[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    return mi != 1 ? TABLE[mi] : TABLE[mi] + !(y % 4) - !(y % 100) + !(y % 400);
  }
  ul_hapi int _uldate_yday_from_mon_day(uldate_int_t year, int mon, int day) {
    int i;
    int di = day;
    for(i = 0; i < mon; ++i) di += _uldate_days_in_month(year, i);
    return di;
  }
  ul_hapi int _uldate_mon_day_from_yday(uldate_int_t year, int* yday) {
    int i, md, yd = *yday;
    for(i = 0; i < 12; ++i) {
      md = _uldate_days_in_month(year, i);
      if(yd < md) break;
      yd -= md;
    }
    if(ul_unlikely(i == 12)) return -1;
    *yday = yd;
    return i;
  }
#else /* !defined(ULDATE_IGNORE_JULIAN_CALENDAR) */
  /* return days of the first day of the given year from '1970', negative value is accepted */
  ul_hapi uldate_int_t _uldate_days_from_year(uldate_int_t y) {
    if(ul_likely(y > 1970))
      return 365 * (y - 1970) + (y - 1969) / 4 - (y - 1901) / 100 + (y - 1601) / 400;
    else if(ul_likely(y > 1582))
      return 365 * (y - 1970) + (y - 1972) / 4 - (y - 2000) / 100 + (y - 2000) / 400;
    else
      return -141704 + 365 * (y - 1582) + (y - 1584) / 4;
  }
  ul_hapi int _uldate_days_in_year(uldate_int_t y) {
    return ul_likely(y != 1582) ? 365 + !(y % 4) - !(y % 100) + !(y % 400) : 355;
  }
  ul_hapi int _uldate_days_in_month(uldate_int_t y, int mi) {
    static const int TABLE[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if(ul_likely(y > 1582))
      return mi != 1 ? TABLE[mi] : TABLE[mi] + !(y % 4) - !(y % 100) + !(y % 400);
    else if(ul_likely(y < 1582 || mi != 9))
      return TABLE[mi] + (mi != 1 && !(y % 4));
    else return 21;
  }
  ul_hapi int _uldate_yday_from_mon_day(uldate_int_t year, int mon, int day) {
    int i;
    int di = day;
    for(i = 0; i < mon; ++i) di += _uldate_days_in_month(year, i);
    if(ul_unlikely(year == 1582 && mon == 9 && day >= 4)) {
      /* 1582-10-5 ~ 1582-10-14 are removed */
      if(day <= 13) { /* illegal, ignore it */ }
      else { di -= 10; }
    }
    return di;
  }
  ul_hapi int _uldate_mon_day_from_yday(uldate_int_t year, int* yday) {
    int i, md, yd = *yday;
    for(i = 0; i < 12; ++i) {
      md = _uldate_days_in_month(year, i);
      if(yd < md) break;
      yd -= md;
    }
    if(ul_unlikely(i == 12)) return -1;
    /* 1582-10-5 ~ 1582-10-14 are removed */
    if(ul_unlikely(year == 1582 && i == 9 && yd >= 4)) yd += 10;
    *yday = yd;
    return i;
  }
#endif

static const int _uldate_week_sun_fix[] = { 0, 1, 2, 3, 4, 5, 6 };
static const int _uldate_week_mon_fix[] = { 6, 0, 1, 2, 3, 4, 5 };
ul_hapi uldate_int_t _uldate_year_from_days(uldate_int_t* days) {
  uldate_int_t d = *days, nd;
  uldate_int_t y = d / 365 + 1970;
  for(;;) {
    nd = _uldate_days_from_year(y);
    if(d < nd) --y;
    else {
      if(d < nd + _uldate_days_in_year(y)) break;
      ++y;
    }
  }
  *days = d - nd;
  return y;
}
ul_hapi uldate_int_t _uldate_days_from_year_mon(uldate_int_t year, int mon) {
  int i;
  uldate_int_t di;
  di = _uldate_days_from_year(year);
  for(i = 0; i < mon; ++i) di += _uldate_days_in_month(year, i);
  return di;
}
ul_hapi int _uldate_wday_from_days(uldate_int_t days) {
  int x = ul_static_cast(int, (days + 4) % 7);
  return ul_unlikely(x < 0) ? x + 7 : x;
}
ul_hapi uldate_int_t _uldate_yday_from_wday_sunday(uldate_int_t year, uldate_int_t week, uldate_int_t wday) {
  return week * 7 + wday - _uldate_week_sun_fix[_uldate_wday_from_days(_uldate_days_from_year(year))];
}
ul_hapi uldate_int_t _uldate_yday_from_wday_monday(uldate_int_t year, uldate_int_t week, uldate_int_t wday) {
  return week * 7 + wday - _uldate_week_mon_fix[_uldate_wday_from_days(_uldate_days_from_year(year))];
}

ul_hapi uldate_t uldate_from_ms(uldate_int_t ms_since_epoch) { return ULDATE_FROM_MILLISECOND(ms_since_epoch); }

ul_hapi uldate_t uldate_from_mday(uldate_int_t year, uldate_int_t mon, uldate_int_t mday) {
  uldate_int_t yi = mon / 12;

  mon %= 12;
  if(ul_unlikely(mon < 0)) { mon += 12; --yi; }
  yi += year;

  return ULDATE_FROM_DAY(mday + _uldate_days_from_year_mon(yi, ul_static_cast(int, mon)));
}
ul_hapi uldate_t uldate_from_yday(uldate_int_t year, uldate_int_t yday) {
  return ULDATE_FROM_DAY(yday + _uldate_days_from_year(year));
}
ul_hapi uldate_t uldate_from_wday_sunday(uldate_int_t year, uldate_int_t week, uldate_int_t wday) {
  return ULDATE_FROM_DAY(_uldate_days_from_year(year) + _uldate_yday_from_wday_sunday(year, week, wday));
}
ul_hapi uldate_t uldate_from_wday_monday(uldate_int_t year, uldate_int_t week, uldate_int_t wday) {
  return ULDATE_FROM_DAY(_uldate_days_from_year(year) + _uldate_yday_from_wday_monday(year, week, wday));
}
ul_hapi uldate_t uldate_from_time(uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec) {
  return ULDATE_FROM_HOUR(hour) + ULDATE_FROM_MINUTE(min) + ULDATE_FROM_SECOND(sec) + ULDATE_FROM_MILLISECOND(msec);
}

ul_hapi uldate_t uldate_from_mday_time(
  uldate_int_t year, uldate_int_t mon, uldate_int_t mday,
  uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec
) { return uldate_from_mday(year, mon, mday) + uldate_from_time(hour, min, sec, msec); }
ul_hapi uldate_t uldate_from_yday_time(
  uldate_int_t year, uldate_int_t yday,
  uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec
) { return uldate_from_yday(year, yday) + uldate_from_time(hour, min, sec, msec); }
ul_hapi uldate_t uldate_from_wday_sunday_time(
  uldate_int_t year, uldate_int_t week, uldate_int_t wday,
  uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec
) { return uldate_from_wday_sunday(year, week, wday) + uldate_from_time(hour, min, sec, msec); }
ul_hapi uldate_t uldate_from_wday_monday_time(
  uldate_int_t year, uldate_int_t week, uldate_int_t wday,
  uldate_int_t hour, uldate_int_t min, uldate_int_t sec, uldate_int_t msec
) { return uldate_from_wday_monday(year, week, wday) + uldate_from_time(hour, min, sec, msec); }

ul_hapi uldate_t uldate_from_mday_time_double(
  double year, double mon, double mday,
  double hour, double min, double sec, double msec
) {
  int m;
  double di, x;
  volatile double ret;

  m = ul_static_cast(int, fmod(mon, 12));
  if(ul_unlikely(m < 0)) m += 12;
  di = ul_static_cast(double,
    _uldate_days_from_year_mon(ul_static_cast(uldate_int_t, year + floor(mon / 12)), m)) + mday;
  x = (hour * 3600000 + min * 60000 + sec * 1000 + msec) * ULDATE_MILLISECOND_DOUBLE;
  ret = di * 86400000 * ULDATE_MILLISECOND_DOUBLE;
  ret += x;
  return ul_static_cast(uldate_int_t, _uldate_trunc(ret));
}
ul_hapi uldate_t uldate_from_yday_time_double(
  double year, double yday,
  double hour, double min, double sec, double msec
) {
  double di, x;
  volatile double ret;
  di = ul_static_cast(double, _uldate_days_from_year(ul_static_cast(uldate_int_t, year))) + yday;
  x = (hour * 3600000 + min * 60000 + sec * 1000 + msec) * ULDATE_MILLISECOND_DOUBLE;
  ret = di * 86400000 * ULDATE_MILLISECOND_DOUBLE;
  ret += x;
  return ul_static_cast(uldate_int_t, _uldate_trunc(ret));
}


ul_hapi void uldate_to_tm(uldate_t date, uldate_tm_t* tm) {
  uldate_int_t h, days, y;
  int ms, s, m, i, md;

  days = ULDATE_TO_DAY(date);
  date -= ULDATE_FROM_DAY(days);
  h = ULDATE_TO_MILLISECOND(date);
  tm->usec = ul_static_cast(int, ULDATE_TO_MICROSECOND(date - ULDATE_FROM_MILLISECOND(h)));

  y = _uldate_year_from_days(&days);
  for(i = 0; i < 11; ++i) {
    md = _uldate_days_in_month(y, i);
    if(days < md) break;
    days -= md;
  }
  tm->year = y;
  tm->mon = i;
  tm->mday = ul_static_cast(int, days);
  tm->wday = _uldate_wday_from_days(days);
  tm->yday = _uldate_yday_from_mon_day(tm->year, i, ul_static_cast(int, days));

  ms = ul_static_cast(int, h % 1000); h = (h - ms) / 1000;
  s = ul_static_cast(int, h % 60); h = (h - s) / 60;
  m = ul_static_cast(int, h % 60); h = (h - m) / 60;
  tm->hour = ul_static_cast(int, h);
  tm->min = m;
  tm->sec = s;
  tm->msec = ms;
}
ul_hapi uldate_t uldate_from_tm(const uldate_tm_t* tm) {
  return uldate_from_mday_time(tm->year, tm->mon, tm->mday,
    tm->hour, tm->min, tm->sec, tm->msec) + ULDATE_FROM_MICROSECOND(tm->usec);
}
ul_hapi uldate_t uldate_from_tm_normalized(uldate_tm_t* tm) {
  uldate_t date;
  date = uldate_from_tm(tm);
  uldate_to_tm(date, tm);
  return date;
}

ul_hapi int uldate_tm_get_week_sunday(const uldate_tm_t* tm) {
  int x;
  if(ul_unlikely(tm->yday < 0 || tm->yday >= 366)) return -1;
  x = _uldate_wday_from_days(_uldate_days_from_year(tm->year));
  return ul_static_cast(int, (tm->yday + _uldate_week_sun_fix[x]) / 7);
}
ul_hapi int uldate_tm_get_week_monday(const uldate_tm_t* tm) {
  int x;
  if(ul_unlikely(tm->yday < 0 || tm->yday >= 366)) return -1;
  x = _uldate_wday_from_days(_uldate_days_from_year(tm->year));
  return ul_static_cast(int, (tm->yday + _uldate_week_mon_fix[x]) / 7);
}
ul_hapi int uldate_tm_get_iso8601_week(const uldate_tm_t* tm, uldate_int_t* p_year) {
  static int iso8601_fix[] = { -1, 0, 1, 2, 3, -3, -2 };
  uldate_int_t year = tm->year;
  int yday = tm->yday;
  if(ul_unlikely(yday < 0 || yday >= 366)) return -1;
  switch(_uldate_wday_from_days(_uldate_days_from_year(year))) {
  case 0: if(yday == 0) yday += _uldate_days_in_year(--year); break;
  case 5: if(yday <= 2) yday += _uldate_days_in_year(--year); break;
  case 6: if(yday <= 1) yday += _uldate_days_in_year(--year); break;
  default:
    do {
      const int d = yday - _uldate_days_in_year(year)
        + iso8601_fix[_uldate_wday_from_days(_uldate_days_from_year(year + 1))];
      if(ul_unlikely(d > 0)) {
        if(ul_likely(p_year)) *p_year = year + 1;
        return d / 7;
      }
    } while(0);
  }
  if(ul_likely(p_year)) *p_year = year;
  return (yday + iso8601_fix[_uldate_wday_from_days(_uldate_days_from_year(year))]) / 7;
}

ul_hapi int uldate_get_week_monday(uldate_t date) {
  uldate_tm_t tm;
  uldate_to_tm(date, &tm);
  return uldate_tm_get_week_monday(&tm);
}
ul_hapi int uldate_get_week_sunday(uldate_t date) {
  uldate_tm_t tm;
  uldate_to_tm(date, &tm);
  return uldate_tm_get_week_sunday(&tm);
}
ul_hapi int uldate_get_iso8601_week(uldate_t date, uldate_int_t* p_year) {
  uldate_tm_t tm;
  uldate_to_tm(date, &tm);
  return uldate_tm_get_iso8601_week(&tm, p_year);
}


static const char _uldate_abbr_month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
static const char _uldate_month_names[] =
  "JanuaryFebruaryMarchAprilMayJuneJulyAugustSeptemberOctoberNovemberDecember";
static const int _uldate_month_names_len[] = { 0, 7, 15, 20, 25, 28, 32, 36, 42, 51, 58, 66, 74 };

static const char _uldate_abbr_wday_names[] = "SunMonTueWedThuFriSat";
static const char _uldate_wday_names[] = "SundayMondayTuesdayWednesdayThursdayFridaySaturday";
static const int _uldate_wday_names_len[] = { 0, 6, 12, 19, 28, 36, 42, 50 };

ul_hapi size_t uldate_tm_format_len(const char* fmt, const uldate_tm_t* tm) {
  size_t len = 0;
  while(*fmt) {
    if(*fmt++ != '%') { ++len; continue; }
    if(*fmt == 'E' || *fmt == 'O') ++fmt; /* ignore */
    switch(*fmt++) {
    case '%': ++len; break;
    case 'n': ++len; break;
    case 't': ++len; break;

    /* Extension */
    case '+': /* 3 digits of milliseconds as a decimal number */
      len += 3; break;

    /* Year */
    case 'Y': /* 4 digits of year as a decimal number, range [0000, 9999] */
    case 'G': /* 4 digits of ISO 8601 week-based year as a decimal number, range [0000, 9999] */
      len += 4; break;
    case 'y': /* last 2 digits of year as a decimal number, range [00, 99]  */
    case 'C': /* first 2 digits of year as a deciaml number, range [00, 99] */
    case 'g': /* last 2 digits of ISO 8601 week-based year as a decimal number, range [00, 99] */
      len += 2; break;

    /* Month */
    case 'b': case 'h': /* abbreviated month name */
      len += 3; break;
    case 'B': /* full month name */
      len += ul_static_cast(size_t,
        _uldate_month_names_len[tm->mon + 1] - _uldate_month_names_len[tm->mon]);
      break;
    case 'm': /* month as a decimal number, range [01, 12] */
      len += 2; break;

    /* Week */
    case 'U': /* week of the year as a decimal number(week starts in Sunday), range [00, 53]. */
    case 'W': /* week of the year as a decimal number(week starts in Monday), range [00, 53]. */
    case 'V': /* ISO 8601 week of the year as a decimal number, range [00, 53] */
      len += 2; break;

    /* Day of the year/month */
    case 'j': /* day of the year as a decimal number, range [001, 366] */
      len += 3; break;
    case 'd': /* day of the month as a decimal number, range [01, 31] */
    case 'e': /* day of the month as a decimal number, range [ 1, 31] */
      len += 2; break;

    /* Day of the week */
    case 'a': /* abbreviated weekday name */
      len += 3; break;
    case 'A': /* full weekday name */
      len += ul_static_cast(size_t,
        _uldate_wday_names_len[tm->wday + 1] - _uldate_wday_names_len[tm->wday]);
      break;
    case 'w': /* weekday as a decimal number, range [0, 6] */
      ++len; break;
    case 'u': /* weekday as a decimal number, range [1, 7] */
      ++len; break;

    /* Hour, minute, second */
    case 'H': /* hour as a decimal number, 24 hour clock, range [00, 23] */
    case 'I': /* hour as a decimal number, 12 hour clock, range [01, 12] */
    case 'M': /* minute as a decimal number, range [00, 59] */
    case 'S': /* second as a decimal number, range [00, 59](NOTE: leap second not supported) */
      len += 2; break;

    /* Other */
    case 'c': /* standard date and time string (equivalent to "%a %b %d %H:%M:%S %Y") */
      len += 24; break;
    case 'x': /* localized date representation (equivalent to "%m/%d/%y" there) */
    case 'D': /* equivalent to "%m/%d/%y" */
      len += 8; break;
    case 'F': /* equivalent to "%Y-%m-%d" */
      len += 10; break;
    case 'r': /* equivalent to "%I:%M:%S" */
      len += 8; break;
    case 'R': /* equivalent to "%H:%M" */
      len += 5; break;
    case 'X': /* localized time representation (equivalent to "%H:%M:%S" there) */
    case 'T': /* equivalent to "%H:%M:%S" */
      len += 8; break;
    case 'p': /* "AM" or "PM" */
      len += 2; break;
    case 'z': /* offset from UTC in the ISO 8601 format */
      len += 5; break;
    case 'Z': /* time zone name or abbreviation */
      return 0; /* TODO: make a list? */

    default:
      return 0;
    }
  }
  return len + 1;
}
ul_hapi size_t uldate_tm_format(char* dest, size_t len, const char* fmt, const uldate_tm_t* tm) {
  const size_t len_raw = len;
  int x = 0;

  while(*fmt) {
    if(*fmt++ != '%') { if(!len) return 0; *dest++ = fmt[-1]; --len; continue; }
    if(*fmt == 'E' || *fmt == 'O') ++fmt; /* ignore */
    switch(*fmt++) {
    case '%': if(!len) return 0; *dest++ = '%'; --len; break;
    case 'n': if(!len) return 0; *dest++ = '\n'; --len; break;
    case 't': if(!len) return 0; *dest++ = '\t'; --len; break;

    /* Extension */
    case '+': /* 3 digits of milliseconds as a decimal number */
      if(len < 3) return 0;
      if(ul_unlikely(tm->msec < 0 || tm->msec > 999)) return 0;
      x = tm->msec;
      dest[2] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[1] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[0] = ul_static_cast(char, x + '0');
      len -= 3; dest += 3; break;

    /* Year */
    case 'Y': /* 4 digits of year as a decimal number, range [0000, 9999] */
      if(len < 4) return 0;
      if(ul_unlikely(tm->year < 0 || tm->year > 9999)) return 0;
      x = ul_static_cast(int, tm->year);
      dest[3] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[2] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[1] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[0] = ul_static_cast(char, x + '0');
      len -= 4; dest += 4; break;
    case 'y': /* last 2 digits of year as a decimal number, range [00, 99]  */
      if(len < 2) return 0;
      if(ul_unlikely(tm->year < 0 || tm->year > 9999)) return 0;
      x = ul_static_cast(int, tm->year % 100);
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      len -= 2; dest += 2; break;
    case 'C': /* first 2 digits of year as a deciaml number, range [00, 99] */
      if(len < 2) return 0;
      if(ul_unlikely(tm->year < 0 || tm->year > 9999)) return 0;
      x = ul_static_cast(int, tm->year / 100);
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      len -= 2; dest += 2; break;
    case 'G': /* 4 digits of ISO 8601 week-based year as a decimal number, range [0000, 9999] */
      if(len < 4) return 0;
      if(ul_unlikely(tm->year < 0 || tm->year > 9999)) return 0;
      do {
        uldate_int_t year;
        if(uldate_tm_get_iso8601_week(tm, &year) < 0) return 0;
        if(ul_unlikely(year < 0 || year > 9999)) return 0;
        dest[3] = ul_static_cast(char, year % 10 + '0'); year /= 10;
        dest[2] = ul_static_cast(char, year % 10 + '0'); year /= 10;
        dest[1] = ul_static_cast(char, year % 10 + '0'); year /= 10;
        dest[0] = ul_static_cast(char, year + '0');
      } while(0);
      len -= 4; dest += 4; break;
    case 'g': /* last 2 digits of ISO 8601 week-based year as a decimal number, range [00, 99] */
      if(len < 2) return 0;
      if(ul_unlikely(tm->year < 0 || tm->year > 9999)) return 0;
      do {
        uldate_int_t year;
        if(uldate_tm_get_iso8601_week(tm, &year) < 0) return 0;
        if(ul_unlikely(year < 0 || year > 9999)) return 0;
        year %= 100;
        dest[0] = ul_static_cast(char, year / 10 + '0');
        dest[1] = ul_static_cast(char, year % 10 + '0');
      } while(0);
      len -= 2; dest += 2; break;

    /* Month */
    case 'b': case 'h': /* abbreviated month name */
      if(len < 3) return 0;
      if(ul_unlikely(tm->mon < 0 || tm->mon >= 12)) return 0;
      x = tm->mon * 3;
      dest[0] = _uldate_abbr_month_names[x];
      dest[1] = _uldate_abbr_month_names[x + 1];
      dest[2] = _uldate_abbr_month_names[x + 2];
      len -= 3; dest += 3; break;
    case 'B': /* full month name */
      do {
        size_t l = ul_static_cast(size_t,
          _uldate_month_names_len[tm->mon + 1] - _uldate_month_names_len[tm->mon]);
        if(len < l) return 0;
        if(ul_unlikely(tm->mon < 0 || tm->mon >= 12)) return 0;
        memcpy(dest, _uldate_month_names + _uldate_month_names_len[tm->mon], l);
        dest += l; len -= l;
      } while(0); break;
    case 'm': /* month as a decimal number, range [01, 12] */
      if(len < 2) return 0;
      if(ul_unlikely(tm->mon < 0 || tm->mon >= 12)) return 0;
      x = tm->mon + 1;
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      len -= 2; dest += 2; break;

    /* Week */
    case 'U': /* week of the year as a decimal number(week starts in Sunday), range [00, 53]. */
      if(len < 2) return 0;
      if(ul_unlikely(tm->yday < 0 || tm->yday >= 366)) return 0;
      x = uldate_tm_get_week_sunday(tm);
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      len -= 2; dest += 2; break;
    case 'W': /* week of the year as a decimal number(week starts in Monday), range [00, 53]. */
      if(len < 2) return 0;
      if(ul_unlikely(tm->yday < 0 || tm->yday >= 366)) return 0;
      x = uldate_tm_get_week_monday(tm);
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      len -= 2; dest += 2; break;
    case 'V': /* ISO 8601 week of the year as a decimal number, range [00, 53] */
      if(len < 2) return 0;
      x = uldate_tm_get_iso8601_week(tm, NULL);
      if(ul_unlikely(x < 0)) return 0;
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      len -= 2; dest += 2; break;

    /* Day of the year/month */
    case 'j': /* day of the year as a decimal number, range [001, 366] */
      if(len < 3) return 0;
      if(ul_unlikely(tm->yday < 0 || tm->yday >= 366)) return 0;
      x = tm->yday + 1;
      dest[2] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[1] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[0] = ul_static_cast(char, x + '0');
      dest += 3; len -= 3; break;
    case 'd': /* day of the month as a decimal number, range [01, 31] */
      if(len < 2) return 0;
      if(ul_unlikely(tm->mday < 0 || tm->mday >= 31)) return 0;
      x = tm->mday + 1;
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      dest += 2; len -= 2; break;
    case 'e': /* day of the month as a decimal number, range [ 1, 31] */
      if(len < 2) return 0;
      if(ul_unlikely(tm->mday < 0 || tm->mday >= 31)) return 0;
      x = tm->mday + 1;
      dest[1] = ul_static_cast(char, x % 10 + '0');
      if(x > 9) dest[0] = ul_static_cast(char, x / 10 + '0');
      else dest[0] = ' ';
      dest += 2; len -= 2; break;

    /* Day of the week */
    case 'a': /* abbreviated weekday name */
      if(len < 3) return 0;
      if(ul_unlikely(tm->wday < 0 || tm->wday >= 7)) return 0;
      x = tm->wday * 3;
      dest[0] = _uldate_abbr_wday_names[x];
      dest[1] = _uldate_abbr_wday_names[x + 1];
      dest[2] = _uldate_abbr_wday_names[x + 2];
      dest += 3; len -= 3; break;
    case 'A': /* full weekday name */
      if(ul_unlikely(tm->wday < 0 || tm->wday >= 7)) return 0;
      do {
        size_t l = ul_static_cast(size_t,
          _uldate_wday_names_len[tm->wday + 1] - _uldate_wday_names_len[tm->wday]);
        if(len < l) return 0;
        memcpy(dest, _uldate_wday_names + _uldate_wday_names_len[tm->wday], l);
        dest += l; len -= l;
      } while(0); break;
    case 'w': /* weekday as a decimal number, range [0, 6] */
      if(len < 1) return 0;
      if(ul_unlikely(tm->wday < 0 || tm->wday >= 7)) return 0;
      *dest++ = ul_static_cast(char, tm->wday + '0');
      --len; break;
    case 'u': /* weekday as a decimal number, range [1, 7] */
      if(len < 1) return 0;
      if(ul_unlikely(tm->wday < 0 || tm->wday >= 7)) return 0;
      *dest++ = ul_static_cast(char, tm->wday ? tm->wday + '0' : '7');
      --len; break;

    /* Hour, minute, second */
    case 'H': /* hour as a decimal number, 24 hour clock, range [00, 23] */
      if(len < 2) return 0;
      if(ul_unlikely(tm->hour < 0 || tm->hour >= 24)) return 0;
      x = tm->hour;
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      dest += 2; len -= 2; break;
    case 'I': /* hour as a decimal number, 12 hour clock, range [01, 12] */
      if(len < 2) return 0;
      if(ul_unlikely(tm->hour < 0 || tm->hour >= 24)) return 0;
      x = tm->hour % 12;
      if(ul_unlikely(x < 0 || x >= 24)) return 0;
      if(x == 0) x = 12; else ++x;
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      dest += 2; len -= 2; break;
    case 'M': /* minute as a decimal number, range [00, 59] */
      if(len < 2) return 0;
      if(ul_unlikely(tm->min < 0 || tm->min >= 60)) return 0;
      x = tm->min;
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      dest += 2; len -= 2; break;
    case 'S': /* second as a decimal number, range [00, 59](NOTE: leap second not supported) */
      if(len < 2) return 0;
      if(ul_unlikely(tm->sec < 0 || tm->sec >= 60)) return 0;
      x = tm->sec;
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[0] = ul_static_cast(char, x / 10 + '0');
      dest += 2; len -= 2; break;

    /* Other */
    case 'c': /* standard date and time string (equivalent to "%a %b %d %H:%M:%S %Y") */
      if(len < 24) return 0;
      if(ul_unlikely(tm->wday < 0 || tm->wday >= 7)) return 0;
      if(ul_unlikely(tm->mon < 0 || tm->mon >= 12)) return 0;
      if(ul_unlikely(tm->mday < 0 || tm->mday >= 31)) return 0;
      if(ul_unlikely(tm->hour < 0 || tm->hour >= 24)) return 0;
      if(ul_unlikely(tm->min < 0 || tm->min >= 60)) return 0;
      if(ul_unlikely(tm->sec < 0 || tm->sec >= 60)) return 0;
      if(ul_unlikely(tm->year < 0 || tm->year > 9999)) return 0;

      x = tm->wday * 3;
      dest[0] = _uldate_abbr_wday_names[x];
      dest[1] = _uldate_abbr_wday_names[x + 1];
      dest[2] = _uldate_abbr_wday_names[x + 2];
      dest[3] = ' ';
      dest += 4;

      x = tm->mon * 3;
      dest[0] = _uldate_abbr_month_names[x];
      dest[1] = _uldate_abbr_month_names[x + 1];
      dest[2] = _uldate_abbr_month_names[x + 2];
      dest[3] = ' ';
      dest += 4;

      x = tm->mday + 1;
      dest[0] = ul_static_cast(char, x / 10 + '0');
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[2] = ' ';
      dest += 3;

      dest[0] = ul_static_cast(char, tm->hour / 10 + '0');
      dest[1] = ul_static_cast(char, tm->hour % 10 + '0');
      dest[2] = ':';
      dest += 3;

      dest[0] = ul_static_cast(char, tm->min / 10 + '0');
      dest[1] = ul_static_cast(char, tm->min % 10 + '0');
      dest[2] = ':';
      dest += 3;

      dest[0] = ul_static_cast(char, tm->sec / 10 + '0');
      dest[1] = ul_static_cast(char, tm->sec % 10 + '0');
      dest[2] = ' ';
      dest += 3;

      x = ul_static_cast(int, tm->year);
      dest[3] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[2] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[1] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[0] = ul_static_cast(char, x + '0');
      dest += 4; len -= 24; break;
    case 'x': /* localized date representation (equivalent to "%m/%d/%y" there) */
    case 'D': /* equivalent to "%m/%d/%y" */
      if(len < 8) return 0;
      if(ul_unlikely(tm->mon < 0 || tm->mon >= 12)) return 0;
      if(ul_unlikely(tm->mday < 0 || tm->mday >= 31)) return 0;
      if(ul_unlikely(tm->year < 0 || tm->year > 9999)) return 0;

      x = tm->mon + 1;
      dest[0] = ul_static_cast(char, x / 10 + '0');
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[2] = '/';

      x = tm->mday + 1;
      dest[3] = ul_static_cast(char, x / 10 + '0');
      dest[4] = ul_static_cast(char, x % 10 + '0');
      dest[5] = '/';

      x = ul_static_cast(int, tm->year % 100);
      dest[6] = ul_static_cast(char, x / 10 + '0');
      dest[7] = ul_static_cast(char, x % 10 + '0');
      dest += 8; len -= 8; break;
    case 'F': /* equivalent to "%Y-%m-%d" */
      if(len < 10) return 0;
      if(ul_unlikely(tm->mon < 0 || tm->mon >= 12)) return 0;
      if(ul_unlikely(tm->mday < 0 || tm->mday >= 31)) return 0;
      if(ul_unlikely(tm->year < 0 || tm->year > 9999)) return 0;

      x = ul_static_cast(int, tm->year);
      dest[3] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[2] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[1] = ul_static_cast(char, x % 10 + '0'); x /= 10;
      dest[0] = ul_static_cast(char, x + '0');
      dest[4] = '-';

      x = tm->mon + 1;
      dest[5] = ul_static_cast(char, x / 10 + '0');
      dest[6] = ul_static_cast(char, x % 10 + '0');
      dest[7] = '-';

      x = tm->mday + 1;
      dest[8] = ul_static_cast(char, x / 10 + '0');
      dest[9] = ul_static_cast(char, x % 10 + '0');
      dest += 10; len -= 10; break;
    case 'r': /* equivalent to "%I:%M:%S" */
      if(len < 8) return 0;
      if(ul_unlikely(tm->hour < 0 || tm->hour >= 24)) return 0;
      if(ul_unlikely(tm->min < 0 || tm->min >= 60)) return 0;
      if(ul_unlikely(tm->sec < 0 || tm->sec >= 60)) return 0;

      x = tm->hour % 12;
      if(x == 0) x = 12; else ++x;
      dest[0] = ul_static_cast(char, x / 10 + '0');
      dest[1] = ul_static_cast(char, x % 10 + '0');
      dest[2] = ':';
      dest[3] = ul_static_cast(char, tm->min / 10 + '0');
      dest[4] = ul_static_cast(char, tm->min % 10 + '0');
      dest[5] = ':';
      dest[6] = ul_static_cast(char, tm->sec / 10 + '0');
      dest[7] = ul_static_cast(char, tm->sec % 10 + '0');
      dest += 8; len -= 8; break;
    case 'R': /* equivalent to "%H:%M" */
      if(len < 5) return 0;
      if(ul_unlikely(tm->hour < 0 || tm->hour >= 24)) return 0;
      if(ul_unlikely(tm->min < 0 || tm->min >= 60)) return 0;

      dest[0] = ul_static_cast(char, tm->hour / 10 + '0');
      dest[1] = ul_static_cast(char, tm->hour % 10 + '0');
      dest[2] = ':';
      dest[3] = ul_static_cast(char, tm->min / 10 + '0');
      dest[4] = ul_static_cast(char, tm->min % 10 + '0');
      dest += 5; len -= 5; break;
    case 'X': /* localized time representation (equivalent to "%H:%M:%S" there) */
    case 'T': /* equivalent to "%H:%M:%S" */
      if(len < 8) return 0;
      if(ul_unlikely(tm->hour < 0 || tm->hour >= 24)) return 0;
      if(ul_unlikely(tm->min < 0 || tm->min >= 60)) return 0;
      if(ul_unlikely(tm->sec < 0 || tm->sec >= 60)) return 0;

      dest[0] = ul_static_cast(char, tm->hour / 10 + '0');
      dest[1] = ul_static_cast(char, tm->hour % 10 + '0');
      dest[2] = ':';
      dest[3] = ul_static_cast(char, tm->min / 10 + '0');
      dest[4] = ul_static_cast(char, tm->min % 10 + '0');
      dest[5] = ':';
      dest[6] = ul_static_cast(char, tm->sec / 10 + '0');
      dest[7] = ul_static_cast(char, tm->sec % 10 + '0');
      dest += 8; len -= 8; break;

    case 'p': /* "AM" or "PM" */
      if(len < 2) return 0;
      if(tm->hour < 12) dest[0] = 'A';
      else dest[0] = 'P';
      dest[1] = 'M';
      dest += 2; len -= 2; break;
    case 'z': /* offset from UTC in the ISO 8601 format */
      if(len < 5) return 0;
      x = uldate_get_gmtoff_minutes();
      if(x >= 0) dest[0] = '+';
      else { dest[0] = '-'; x = -x; }
      do {
        int y = x / 60; x %= 60;
        dest[1] = ul_static_cast(char, y / 10 + '0');
        dest[2] = ul_static_cast(char, y % 10 + '0');
        dest[4] = ul_static_cast(char, x / 10 + '0');
        dest[3] = ul_static_cast(char, x % 10 + '0');
      } while(0);
      dest += 5; len -= 5; break;
    case 'Z': /* time zone name or abbreviation */
      return 0; /* TODO: make a list? */

    default:
      return 0;
    }
  }
  if(!len) return 0;
  *dest++ = 0; --len;
  return len_raw - len;
}
ul_hapi const char* uldate_tm_parse(const char* src, const char* fmt, uldate_tm_t* tm) {
  int i, j, x;
  int year = 0, mon = -1, mday = -1, yday = -1, wday = -1;
  int week = -1, week_start_sun = 1;
  int hour = 0, min = 0, sec = 0, msec = 0;
  const char* fmt_rec = NULL;

do_again:
  while(*fmt) {
    if(*fmt++ != '%') {
      if(isspace(fmt[-1])) { while(isspace(*src)) { ++src; } }
      else { if(*src != fmt[-1]) return NULL; else ++src; }
      continue;
    }
    if(*fmt == 'E' || *fmt == 'O') ++fmt; /* ignore */
    switch(*fmt++) {
    case '%':
      if(*src++ != '%') return NULL;
      break;
    case 't': case 'n':
      while(isspace(*src)) { ++src; } break;

    /* Extension */
    case '.': /* match all letter */
      ++src; break;
    case '+':
      /* millisecond as a decimal number, range [000, 999],
      leading zeroes permitted but not required */
      for(i = 0, x = 0; i < 3 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x > 999) return NULL;
      msec = x;
      break;
    case 'F': /* equivalent to "%Y-%m-%d" */
      fmt_rec = fmt; fmt = "%Y-%m-%d"; break;


    /* Year */
    case 'Y':
      /* full year as 4 digit decimal number, leading zeroes permitted but not required */
      for(i = 0, x = 0; i < 4 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      year = x; break;
    case 'y': /* last 2 digits of year as a decimal number */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      year = (x >= 69 ? x + 1900 : x + 2000); break;
    case 'C': /* first 2 digits of year as a decimal number */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      year = year % 100 + x * 100; break;

    /* Month */
    case 'b': case 'h': case 'B': /* full or abbreviated month name */
      for(i = 0; i < 12; ++i) {
        for(j = _uldate_month_names_len[i], x = 0;
          j < _uldate_month_names_len[i + 1];
          ++j, ++x
        )
          if(src[x] != _uldate_month_names[j]) break;
        if(j == _uldate_month_names_len[i + 1]) { src += x; break; }
      }
      if(i != 12) { mon = i; break; }
      for(i = 0; i < 36; i += 3)
        if(src[0] == _uldate_abbr_month_names[i]
          && src[1] == _uldate_abbr_month_names[i + 1]
          && src[2] == _uldate_abbr_month_names[i + 2]
        ) { src += 3; break; }
      if(i != 36) { mon = i / 3; break; }
      return NULL;
    case 'm':
      /* month as a decimal number, range [01, 12], leading zero permitted but not required */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x == 0 || x > 12) return NULL;
      mon = x - 1; break;

    /* Week */
    case 'U':
      /* the week of the year (a week starts on Sunday) as a decimal number, range [00, 53],
      leading zero permitted but not require. */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
          x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x < 0 || x > 53) return NULL;
      week = x; week_start_sun = 1; break;
    case 'W':
      /* the week of the year (a week starts on Monday) as a decimal number, range [00, 53],
      leading zero permitted but not require. */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
          x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x < 0 || x > 53) return NULL;
      week = x; week_start_sun = 0; break;

    /* Day of the year/month */
    case 'j':
      /* day of the year as a decimal number, range [001, 365],
      leading zeroes permitted but not required */
      for(i = 0, x = 0; i < 3 && *src >= '0' && *src <= '9'; ++i, ++src)
          x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x <= 0 || x > 366) return NULL;
      yday = x - 1;
      break;
    case 'd': case 'e':
      /* day of the month as a decimal number, range [01, 31],
      leading zeroes permitted but not required */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
          x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x <= 0 || x > 31) return NULL;
      mday = x - 1;
      break;

    /* Day of the week */
    case 'a': case 'A': /* full or abbreviated name of the day of the week */
      for(i = 0; i < 12; ++i) {
        for(j = _uldate_wday_names_len[i], x = 0; j < _uldate_wday_names_len[i + 1]; ++j, ++x)
          if(src[x] != _uldate_wday_names[j]) break;
        if(j == _uldate_wday_names_len[i + 1]) { src += x; break; }
      }
      if(i != 12) { wday = i; break; }
      for(i = 0; i < 36; i += 3)
        if(src[0] == _uldate_abbr_wday_names[i]
          && src[1] == _uldate_abbr_wday_names[i + 1]
          && src[2] == _uldate_abbr_wday_names[i + 2]
        ) { src += 3; break; }
      if(i != 36) { mon = i / 3; break; }
      return NULL;
    case 'w':
      /* weekday as a decimal number, range [0, 6], where Sunday is 0 */
      if(*src < '0' || *src >= '7') return NULL;
      wday = *src++ - '0';
      break;

    /* Hour, minute, second */
    case 'H':
      /* hour(24 hour clock) as a decimal number, range [00, 23],
      leading zeroes permitted but not required */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x >= 24) return NULL;
      hour = x;
      break;
    case 'I':
      /* hour(12 hour clock) as a decimal number, range [01, 12],
      leading zeroes permitted but not required */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x == 0 || x > 12) return NULL;
      hour = (x == 12 ? 0 : x);
      break;
    case 'M':
      /* minute as a decimal number, range [00, 59],
      leading zeroes permitted but not required */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x >= 60) return NULL;
      min = x;
      break;
    case 'S':
      /* second as a decimal number, range [00, 59],
      leading zeroes permitted but not required */
      for(i = 0, x = 0; i < 2 && *src >= '0' && *src <= '9'; ++i, ++src)
        x = x * 10 + *src - '0';
      if(i == 0) return NULL;
      if(x >= 60) return NULL;
      sec = x;
      break;

    /* Other */
    /* Note: the following characters shouldn't appear in `fmt` if `fmt_rec` is not NULL */
    case 'c': /* standard date and time string (equivalent to "%a %b %d %H:%M:%S %Y") */
      fmt_rec = fmt; fmt = "%a %b %d %H:%M:%S %Y"; break;
    case 'x': /* localized date representation (equivalent to "%m/%d/%y" there) */
      fmt_rec = fmt; fmt = "%m/%d/%y"; break;
    case 'X': /* localized time representation (equivalent to "%H:%M:%S" there) */
      fmt_rec = fmt; fmt = "%H:%M:%S"; break;
    case 'D': /* equivalent to "%m/%d/%y" */
      fmt_rec = fmt; fmt = "%m/%d/%y"; break;
    case 'r': /* equivalent to "%I:%M:%S" */
      fmt_rec = fmt; fmt = "%I:%M:%S"; break;
    case 'R': /* equivalent to "%H:%M" */
      fmt_rec = fmt; fmt = "%H:%M"; break;
    case 'T': /* equivalent to "%H:%M:%S" */
      fmt_rec = fmt; fmt = "%H:%M:%S"; break;

    case 'p': /* "AM" or "PM" */
      if(src[0] == 'A' || src[1] == 'a') { /* do nothing */ }
      else if(src[0] == 'P' || src[1] == 'p') hour = ul_likely(hour < 12) ? hour + 12 : hour;
      else return NULL;
      if(src[1] != 'M' && src[1] != 'm') return NULL;
      src += 2; break;

    default: return NULL;
    }
  }
  if(ul_unlikely(fmt_rec)) { fmt = fmt_rec; fmt_rec = NULL; goto do_again; }

  if(year == -1) goto fillback; /* don't need fix date part */
  if(week != -1 && wday != -1 && !week_start_sun) {
    if(_uldate_wday_from_days(_uldate_days_from_year(year)) == 0)
      --week;
  }

  if(yday != -1) {
    int _mday = yday, _mon, _week, _wday;

    _mon = _uldate_mon_day_from_yday(year, &_mday);
    if(ul_unlikely(_mon == -1)) return NULL;
    if(mon != -1 && _mon != mon) return NULL;
    if(mday != -1 && _mday != mday) return NULL;

    _wday = _uldate_wday_from_days(_uldate_days_from_year(year));
    _week = (yday + _uldate_week_sun_fix[_wday]) / 7;
    _wday = (_wday + yday) % 7;
    if(week != -1 && _week != week) return NULL;
    if(wday != -1 && _wday != wday) return NULL;

    goto fillback;
  }
  if(mon != -1 && mday != -1) {
    int _week, _wday, _yday;

    _yday = _uldate_yday_from_mon_day(year, mon, mday);
    if(yday != -1 && _yday != mday) return NULL;

    _wday = _uldate_wday_from_days(_uldate_days_from_year(year));
    _week = (_yday + _uldate_week_sun_fix[yday]) / 7;
    _wday = (_wday + yday) % 7;
    if(week != -1 && _week != week) return NULL;
    if(wday != -1 && _wday != wday) return NULL;

    goto fillback;
  }
  if(week != -1 && wday != -1) {
    int _mon, _yday, _mday;

    _yday = ul_static_cast(int, _uldate_yday_from_wday_sunday(year, week, wday));
    if(yday != -1 && _yday != mday) return NULL;

    _mday = _yday;
    _mon = _uldate_mon_day_from_yday(year, &_mday);
    if(ul_unlikely(_mon == -1)) return NULL;
    if(mon != -1 && _mon != week) return NULL;
    if(mday != -1 && _mday != wday) return NULL;

    goto fillback;
  }
  return NULL;

fillback:
  tm->hour = hour;
  tm->min = min;
  tm->sec = sec;
  tm->msec = msec;
  tm->usec = 0;

  tm->year = year;
  tm->mon = mon;
  tm->mday = mday;
  tm->yday = yday;
  return src;
}

ul_hapi size_t uldate_format_len(const char* fmt, uldate_t date) {
  uldate_tm_t tm;
  uldate_to_tm(date, &tm);
  return uldate_tm_format_len(fmt, &tm);
}
ul_hapi size_t uldate_format(char* dest, size_t len, const char* fmt, uldate_t date) {
  uldate_tm_t tm;
  uldate_to_tm(date, &tm);
  return uldate_tm_format(dest, len, fmt, &tm);
}
ul_hapi const char* uldate_parse(const char* src, const char* fmt, uldate_t* date) {
  uldate_tm_t tm;
  src = uldate_tm_parse(src, fmt, &tm);
  if(!src) return NULL;
  *date = uldate_from_tm(&tm);
  return src;
}

ul_hapi size_t uldate_to_utc_string(char* dest, size_t len, uldate_t utc) {
  return uldate_format(dest, len, "%a, %d %b %Y %T GMT", utc);
}
ul_hapi size_t uldate_to_string(char* dest, size_t len, uldate_t utc) {
  return uldate_format(dest, len, "%a %b %d %Y %T GMT%z", utc + ULDATE_FROM_MINUTE(uldate_get_gmtoff_minutes()));
}
ul_hapi size_t uldate_to_iso_string(char* dest, size_t len, uldate_t utc) {
  return uldate_format(dest, len, "%FT%T.%+Z", utc);
}
ul_hapi size_t uldate_to_locale_string(char* dest, size_t len, uldate_t utc) {
  return uldate_format(dest, len, "%D, %T %p", utc);
}

ul_hapi time_t uldate_to_ctime(uldate_t date) {
  /* round to -infinity */
  if(ul_unlikely(date < 0)) {
    uldate_t r = ULDATE_TO_SECOND(date);
    return ul_static_cast(time_t, r - (ULDATE_FROM_SECOND(r) != date));
  } else return ul_static_cast(time_t, ULDATE_TO_SECOND(date));
}
ul_hapi uldate_t uldate_from_ctime(time_t ctime) {
  return ULDATE_FROM_SECOND(ul_static_cast(uldate_t, ctime));
}

ul_hapi void uldate_tm_to_ctm(const uldate_tm_t* tm, struct tm* ctm) {
  ctm->tm_year = ul_static_cast(int, tm->year);
  ctm->tm_mon = tm->mon;
  ctm->tm_mday = tm->mday + 1;
  ctm->tm_hour = tm->hour;
  ctm->tm_min = tm->min;
  ctm->tm_sec = tm->sec;
  ctm->tm_wday = tm->wday;
  ctm->tm_yday = tm->yday;
  ctm->tm_isdst = -1;
}
ul_hapi void uldate_tm_from_ctm(const struct tm* ctm, uldate_tm_t* tm) {
  tm->year = ctm->tm_year;
  tm->mon = ctm->tm_mon;
  tm->mday = ctm->tm_mday - 1;
  tm->hour = ctm->tm_hour;
  tm->min = ctm->tm_min;
  tm->sec = ctm->tm_sec;
  tm->msec = 0;
  tm->usec = 0;
  tm->wday = ctm->tm_wday;
  tm->yday = ctm->tm_yday;
}

ul_hapi void uldate_to_ctm(uldate_t date, struct tm* ctm) {
  /* ensure Thread-Safety */
  uldate_tm_t tm;
  uldate_to_tm(date, &tm);
  uldate_tm_to_ctm(&tm, ctm);
}
ul_hapi uldate_t uldate_from_ctm(const struct tm* ctm) {
  struct tm _ctm = *ctm;
  return uldate_from_ctime(mktime(&_ctm));
}

#endif /* ULDATE_H */
