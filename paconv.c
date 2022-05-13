#include "paconv.h"
#include <stdio.h>
//#include <errno.h>
#include <stdlib.h>
//#include <string.h>


typedef struct {
  time_t posix, tai, delta;
} leap_second_t ;

static leap_second_t *leap_seconds;
static int nbr_leap_seconds;
const time_t paconv_invalid = (time_t)(-1);
static time_t posix_max = (time_t)63072000;
static time_t tai_max = (time_t)63072010;
time_t paconv_posix_max() { return posix_max;}
time_t paconv_posix_min() { return (time_t)63072000;}
time_t paconv_tai_max() { return tai_max;}
time_t paconv_tai_min() { return (time_t)63072010;}


// paconv_load_leap_seconds loads the leap second list from the file
// whose name is given as argument. The latest file may be downloaded
// from https://www.ietf.org/timezones/data/leap-seconds.list.
// On Debian and derived OS the latest file version is available as
// "/usr/share/timezone/leap-second.list". 
int paconv_load_leap_seconds(const char *leap_seconds_list_file_name){
  const int max_val = 512;
  long long val[max_val];
  nbr_leap_seconds = 0;
  long long expDelta = 10;
  static const long long offset = 2208988800LL;
  time_t l = (time_t)0; 
  char buf[128];
  if(leap_seconds != NULL)
    free(leap_seconds);
  leap_seconds = NULL;
  posix_max = (time_t)63072000;
  tai_max = (time_t)63072010;
  FILE *f = fopen(leap_seconds_list_file_name, "r");
  if (f == NULL)
    return -1; // failed opening file for reading
  while (fgets(buf, sizeof(buf), f)) {
    long long tmp;
    if (buf[0] == '#') {
      if (buf[1] == '@') {
        if (sscanf(buf + 2, "%lld", &tmp) != 1) {
          return -2; // failed decoding the validity limit of table 
        }
        l = (time_t)(tmp - offset);
      }
    } else {
      long long gotDelta;
      if (sscanf(buf, "%lld%lld", &tmp, &gotDelta) != 2) {
        return -3; // failed decoding the posix time value
      }
      if(gotDelta != expDelta)
        return -4; // didn't found the expected delta value
      expDelta++;
      if(nbr_leap_seconds >= max_val)
        return -5; // val table overflow
      val[nbr_leap_seconds++] = (time_t)(tmp - offset);
    }
  }
  if(l == (time_t)0)
    return -6; // failed finding the validity limit value in file
  if(nbr_leap_seconds < 25)
    return -7; // not enough leap seconds
  leap_seconds = calloc(nbr_leap_seconds, sizeof(leap_second_t));
  for(int i = 0; i < nbr_leap_seconds; i++) {
    leap_seconds[i].posix = val[i];
    leap_seconds[i].tai = val[i] + 10 + i;
    leap_seconds[i].delta = 10 + i;
  }
  posix_max = l;
  tai_max = l+expDelta-1;
  return 0;
}

// paconv_posix2tai returns the tai time corresponding to the given posix time,
// or the paconv_invalid time_t value if the posix time is out of valid range.
time_t paconv_posix2tai(time_t posix) {
  if(leap_seconds == NULL)
    return paconv_invalid; // leap seconds are not loaded
  if(posix < paconv_posix_min() || posix > paconv_posix_max()) {
    return paconv_invalid; // posix time value is out of range
  }
  int i = 0, j = nbr_leap_seconds;
  while(i < j) {
    int m = (i+j)/2;
    if(leap_seconds[m].posix < posix){
      i = m + 1;
    } else {
      j = m;
    }
  }
  time_t res = leap_seconds[i].posix == posix ? leap_seconds[i].tai : posix + leap_seconds[i-1].delta;
  return res;
}

// paconv_tai2posix returns the posix time corresponding to the given tai time,
// or the paconv_invalid time_t value if the tai time is out of valid range.
time_t paconv_tai2posix(time_t tai) {
  if(leap_seconds == NULL)
    return -2; // leap seconds are not loaded
  if(tai < paconv_tai_min() || tai > paconv_tai_max()) {
    return -1; // posix time value is out of range
  }
  int i = 0, j = nbr_leap_seconds;
  while(i < j){
    int m = (i+j)/2;
    if(leap_seconds[m].tai < tai){
      i = m + 1;
    } else {
      j = m;
    }
  }
  time_t res = leap_seconds[i].tai == tai ? leap_seconds[i].posix : tai - leap_seconds[i-1].delta;
  return res;
}



/*  Example leap second table:

    {  63072000,   63072010,  10}, // 2272060800    10  # 1 Jan 1972
    {  78796800,   78796811,  11}, // 2287785600    11  # 1 Jul 1972
    {  94694400,   94694412,  12}, // 2303683200    12  # 1 Jan 1973
    { 126230400,  126230413,  13}, // 2335219200    13  # 1 Jan 1974
    { 157766400,  157766414,  14}, // 2366755200    14  # 1 Jan 1975
    { 189302400,  189302415,  15}, // 2398291200    15  # 1 Jan 1976
    { 220924800,  220924816,  16}, // 2429913600    16  # 1 Jan 1977
    { 252460800,  252460817,  17}, // 2461449600    17  # 1 Jan 1978
    { 283996800,  283996818,  18}, // 2492985600    18  # 1 Jan 1979
    { 315532800,  315532819,  19}, // 2524521600    19  # 1 Jan 1980
    { 362793600,  362793620,  20}, // 2571782400    20  # 1 Jul 1981
    { 394329600,  394329621,  21}, // 2603318400    21  # 1 Jul 1982
    { 425865600,  425865622,  22}, // 2634854400    22  # 1 Jul 1983
    { 489024000,  489024023,  23}, // 2698012800    23  # 1 Jul 1985
    { 567993600,  567993624,  24}, // 2776982400    24  # 1 Jan 1988
    { 631152000,  631152025,  25}, // 2840140800    25  # 1 Jan 1990
    { 662688000,  662688026,  26}, // 2871676800    26  # 1 Jan 1991
    { 709948800,  709948827,  27}, // 2918937600    27  # 1 Jul 1992
    { 741484800,  741484828,  28}, // 2950473600    28  # 1 Jul 1993
    { 773020800,  773020829,  29}, // 2982009600    29  # 1 Jul 1994
    { 820454400,  820454430,  30}, // 3029443200    30  # 1 Jan 1996
    { 867715200,  867715231,  31}, // 3076704000    31  # 1 Jul 1997
    { 915148800,  915148832,  32}, // 3124137600    32  # 1 Jan 1999
    {1136073600, 1136073633,  33}, // 3345062400    33  # 1 Jan 2006
    {1230768000, 1230768034,  34}, // 3439756800    34  # 1 Jan 2009
    {1341100800, 1341100835,  35}, // 3550089600    35  # 1 Jul 2012
    {1435708800, 1435708836,  36}, // 3644697600    36  # 1 Jul 2015
    {1483228800, 1483228837,  37}  // 3692217600    37  # 1 Jan 2017
*/
