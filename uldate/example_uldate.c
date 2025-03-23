#include "uldate.h"
#include <stdio.h>
#include <time.h>

int main() {
  const uldate_t utc = uldate_now_utc();
  const uldate_t date = uldate_utc_to_locale(utc);

  printf("date: %" ULDATE_PRId " (locale) / %" ULDATE_PRId " (utc)\n", date, utc);

  {
    uldate_tm_t tm;
    uldate_to_tm(date, &tm);
    printf("tm:\n");
    printf("    year, mon, mday: %" ULDATE_PRId ", %d, %d\n", tm.tm_year, tm.tm_mon, tm.tm_mday);
    printf("    hour, min, sec:  %d, %d, %d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("    msec, usec:      %d, %d\n", tm.tm_msec, tm.tm_usec);
    printf("    wday, yday:      %d, %d\n", tm.tm_wday, tm.tm_yday);
  }

  {
    char buf[128];
    printf("string:\n");
    uldate_to_string(buf, sizeof(buf), utc);
    printf("    default: %s\n", buf);
    uldate_to_iso_string(buf, sizeof(buf), utc);
    printf("    ISO:     %s\n", buf);
    uldate_to_locale_string(buf, sizeof(buf), utc);
    printf("    locale:  %s\n", buf);
    uldate_to_utc_string(buf, sizeof(buf), utc);
    printf("    UTC:     %s\n", buf);
  }

  {
    struct tm ctm;
    time_t ctime = uldate_to_ctime(date);
    uldate_to_ctm(date, &ctm);
    printf("<time.h>:\n");
    printf("    ctime: %" ULDATE_PRId "\n", ul_static_cast(uldate_int_t, ctime));
    printf("    tm:\n");
    printf("        year, mon, mday: %d, %d, %d\n", ctm.tm_year, ctm.tm_mon, ctm.tm_mday);
    printf("        hour, min, sec:  %d, %d, %d\n", ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
    printf("        wday, yday:      %d, %d\n", ctm.tm_wday, ctm.tm_yday);
  }

  return 0;
}
