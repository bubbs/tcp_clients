// Bubbs.  06/June/2004.
// Side product of 'httpclient', which requires the ability to
// parse cookie expiry dates using a number of variable formats.

// Two functions: parse_time() and mktime().
// parse_time() takes a date/time string and a date format (a regex including custom date
// related patterns), and returns a mapping of parsed values (or zero for failure).
// mktime() takes a mapping of date/time values (as returned by parse_date() or gmtime())
// and converts it into an integer compatible with time() and usable in ctime().

// Bubbs, 28/Feb/2007.
// Added PATTERN_LIST regexp.
// Tweaked preparation phase to avoid subst() usage.

// 02/June/2007.
// Fixed mktime().

// 02/Feb/2008.
// Added mktime(int *) to support mktime(gmtime()) usage.

// 13/Feb/2009.
// Fixed typo in CENTURY_START and corrected its' value.  Made use of CENTURY_START in mktime().
// Recoded pattern handling and month length calculation to remove a number of ugly definitions.


#if defined(TIMEWARP)
#include <time.h>
#else
#include <sys/time.h>
#endif


// Defines how to expand 2-digit years into 4-digit years.
// If the (2-digit) year is less than this, the century is '20', '19' otherwise.
// (70 is chosen since it's at the start of the epoch).
#define CENTURY_START    70

// Default date format, matches ctime().
#define CTIME_FORMAT     "WEEKDAY MONTH DATE HOUR:MINUTE:SECOND YEAR"


// Date token regular expressions.

#define MONTH_LIST       "jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec"
#define DAY_LIST         "sun|mon|tue|wed|thu|fri|sat"

#if defined(TIMEWARP) || defined(DUNE)
#define ZONE_LIST        "|ast|yst|pst|mst|cst|est|ast|nst|wet|pwt|bst|gmt|utc|met|cet|cest|sast|eet|awt|act|aet|nzst|nzt"
#else
#define ZONE_LIST        ""
#endif

#define YEAR             "([0-9][0-9][0-9][0-9]|[0-9][0-9])"
#define MONTH            "(0[1-9]|1[0-2]|[1-9]|" MONTH_LIST ")"
#define DATE             "(0[1-9]|[12][0-9]|3[01]|[1-9])"
#define WEEKDAY          "(" DAY_LIST ")"
#define HOUR             "([01][0-9]|2[0-3]|[0-9])"
#define AMPM             "([ap]m)"
#define MINUTE           "([0-5][0-9]|[0-9])"
#define SECOND           "([0-5][0-9]|[0-9])"
#define ZONE             "([-+][0-9]+\\.[0-9]*|[-+][0-9]+" ZONE_LIST ")"


#if defined(TIMEWARP) || defined(DUNE)
#if __FILE__ == "secure/simul/parse_time.c"
// If installed as simul-efuns, this need to be here.
float query_zone_offset(mixed arg);
#endif
#endif


varargs mapping parse_time(string date_text, string date_fmt) {
  if (!date_text)
    date_text = ctime();
  if (!stringp(date_text))
    raise_error("parse_time(): Bad type to argument 1.\n");
  if (!date_fmt)
   date_fmt = CTIME_FORMAT;
  if (!stringp(date_fmt))
    raise_error("parse_time(): Bad type to argument 2.\n");

  // preparation.
  date_text = lower_case(date_text);
  mapping patterns =
    ([
      "YEAR"   : YEAR,
      "MONTH"  : MONTH,
      "DATE"   : DATE,
      "WEEKDAY": WEEKDAY,
      "HOUR"   : HOUR,
      "AMPM"   : AMPM,
      "MINUTE" : MINUTE,
      "SECOND" : SECOND,
      "ZONE"   : ZONE,
    ]);
  string *fmt_bits = regexplode(date_fmt, implode(m_indices(patterns), "|"));
  string  fmt      = "^" +implode(map(fmt_bits, (: $2[$1] || $1 :), patterns), "") +"$";
  // check format match.
  if (!strlen(date_text) || strlen(regreplace(date_text, fmt, "", 0)))
    return 0;

  // parse for individual tokens.
  mapping date_values = ([ ]);
  int     size        = sizeof(fmt_bits);
  for(int index = 1; index < size; index += 2)
    m_add(date_values, fmt_bits[index], regreplace(date_text, fmt, "\\" +((index +1) /2), 0));

  // normalise and sanity checks.
  if (member(date_values, "YEAR")) {
    int num = to_int(date_values["YEAR"]);
    if (strlen(date_values["YEAR"]) == 2)
      num += (num < CENTURY_START) ? 2000 : 1900;
    date_values["YEAR"] = num;
  }
  if (member(date_values, "MONTH")) {
    int num = to_int(date_values["MONTH"]);
    if (!num)
      num = member(explode(MONTH_LIST, "|"), date_values["MONTH"]) +1;
    if ((num < 1) || (num > 12))
      return 0;
    date_values["MONTH"] = num;
  }
  if (member(date_values, "DATE")) {
    int num = to_int(date_values["DATE"]);
    int ml  = 31;
//    if ((num < 1) || (num > max(MONTH_LENGTHS)))
    if ((num < 1) || (num > ml))
      return 0;
    if (member(date_values, "MONTH")) {
      int ly = 1;
      if (member(date_values, "YEAR"))
        ly = (!(date_values["YEAR"] %4) && (date_values["YEAR"] %100)) || !(date_values["YEAR"] %400);
//      int ml = (MONTH_LENGTHS)[date_values["MONTH"] -1];
//      if (ly && (date_values["MONTH"] == 2))
//        ml++;
      switch(date_values["MONTH"]) {
        case  2:
          ml = 28 +(ly && 1);
        break;
        case  4:
        case  6:
        case  9:
        case 11:
          ml = 30;
        break;
      }
      if (num > ml)
        return 0;
    }
    date_values["DATE"] = num;
  }
  if (member(date_values, "WEEKDAY")) {
    int num = member(explode(DAY_LIST, "|"), date_values["WEEKDAY"]);
    if (num == -1)
      return 0;
    if (member(date_values, "YEAR") && member(date_values, "MONTH") && member(date_values, "DATE")) {
      // check it matches the given date...
      int y = date_values["YEAR"];
      int m = date_values["MONTH"];
      int d = date_values["DATE"];
      // now, watch carefully.
      int m2 = (m +9) %12;
      int y2 = y -(m2 /10);
      while(y2 < 0)
        y2 += 400;
      int w  = (d +2 +(((m2 *13) +2) /5) +y2 +(y2 /4) -(y2 /100) +(y2 /400)) %7;
      // and that's magic!
      if (num != w)
        return 0;
    }
    date_values["WEEKDAY"] = num;
  }
  if (member(date_values, "HOUR")) {
    int num = to_int(date_values["HOUR"]);
    if (member(date_values, "AMPM")) {
      if ((num < 1) || (num > 12))
        return 0;
      if (date_values["AMPM"] == "pm")
        num += 12;
      if (num == 24)
        num = 0;
    }
    else if ((num < 0) || (num > 23))
      return 0;
    date_values["HOUR"] = num;
  }
  if (member(date_values, "MINUTE")) {
    int num = to_int(date_values["MINUTE"]);
    if ((num < 0) || (num > 59))
      return 0;
    date_values["MINUTE"] = num;
  }
  if (member(date_values, "SECOND")) {
    int num = to_int(date_values["SECOND"]);
    if ((num < 0) || (num > 59))
      return 0;
    date_values["SECOND"] = num;
  }
#if defined(TIMEWARP) || defined(DUNE)
  float utc_offset = query_zone_offset("UTC");
  date_values["ZONE"] = (member(date_values, "ZONE") ? query_zone_offset(date_values["ZONE"]) : 0.0) -utc_offset;
#else
  date_values["ZONE"] = (member(date_values, "ZONE") ? to_float(date_values["ZONE"]) : 0.0);
#endif

  return date_values;
}


int mktime(mixed date_values) {
#if defined(LPC_TIME_H_)
  if (pointerp(date_values) && (sizeof(date_values) == TM_MAX) && (sizeof(filter(date_values, #'intp)) == TM_MAX))
    date_values =
      ([
        "YEAR"  : date_values[TM_YEAR],
        "MONTH" : date_values[TM_MON] +1,
        "DATE"  : date_values[TM_MDAY],
        "HOUR"  : date_values[TM_HOUR],
        "MINUTE": date_values[TM_MIN],
        "SECOND": date_values[TM_SEC],
        "ZONE"  : 0,
      ]);
#endif

  if (!mappingp(date_values))
    raise_error("mktime(): Bad type to argument 1.\n");

  int total = (date_values["YEAR"] -(1900 +CENTURY_START)) *365 *24 *60 *60;

  for(int i = 1900 +CENTURY_START; i < date_values["YEAR"]; i++)
    if ((!(i %4) && (i %100)) || !(i %400))
      total += 24 *60 *60;

//  int ly = (!(date_values["YEAR"] %4) && (date_values["YEAR"] %100)) || !(date_values["YEAR"] %400);
//  for(int i = 0; i < date_values["MONTH"] -1; i++)
//    total += (MONTH_LENGTHS[i] +(ly && (i == 1) && 1)) *24 *60 *60;
  for(int i = 1; i < date_values["MONTH"]; i++)
    switch(i) {
      case  2:
        int ly = (!(date_values["YEAR"] %4) && (date_values["YEAR"] %100)) || !(date_values["YEAR"] %400);
        total += (28 +(ly && 1)) *24 *60 *60;
      break;
      case  4:
      case  6:
      case  9:
      case 11:
        total += 30 *24 *60 *60;
      break;
      default:
        total += 31 *24 *60 *60;
      break;
    }

  total += ((date_values["DATE"] -1) *24 *60 *60) +(date_values["HOUR"] *60 *60) +(date_values["MINUTE"] *60) +date_values["SECOND"];
  total -= to_int(date_values["ZONE"] *60 *60);

  return total;
}

