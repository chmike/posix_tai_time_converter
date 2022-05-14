#include "paconv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


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

// parse_integer returns a pointer on the first non digit in
// the [beg,end[ range, or end if the end is reached. If the
// range contains no digits, beg is returned.
const char* parse_integer(const char *beg, const char *end) {
  while(beg != end && *beg >= '0' && *beg <= '9')
    beg++;
  return beg;
}

// decode_ulong decodes the unsigned long integer between beg 
// and end. It is assumed that the range has been obtained
// with parse_integer.
unsigned long decode_ulong(const char *beg, const char *end) {
  unsigned long val = 0;
  while(beg != end) 
    val = val*10 + *beg++ - '0';
  return val;
}

bool decode_time(const char *beg, const char *end, int *hour, int *minute, int *second){
  *hour = *minute = *second = 0;
  const char *tkBeg = beg, *tkEnd;
  // decode hours
  tkEnd = parse_integer(tkBeg, end);
  if(tkEnd == tkBeg || tkEnd == end || *tkEnd != ':')
    return false;
  *hour = decode_ulong(tkBeg, tkEnd);
  if(*hour > 23)
    return false;
  // decode minutes
  tkBeg = tkEnd+1;
  tkEnd = parse_integer(tkBeg, end);
  if(tkEnd == tkBeg || tkEnd == end || *tkEnd != ':')
    return false;
  *minute = decode_ulong(tkBeg, tkEnd);
  if(*minute > 59)
    return false;
  // decode seconds
  tkBeg = tkEnd+1;
  tkEnd = parse_integer(tkBeg, end);
  if(tkEnd == tkBeg || tkEnd != end)
    return false;
  *second = decode_ulong(tkBeg, tkEnd);
  return *second <= 60;
}

// skip_white_space returns a pointer on the first non-white space
// in the [beg,end[ range, or end if none is found.
const char *skip_white_space(const char *beg, const char *end){
  while(beg != end && (*beg == ' ' || *beg == '\t'))
    beg++;
  return beg;
}

// parse_token return a pointer to the first white space character
// in the [beg,end[ range, or end if none is found.
const char* parse_token(const char *beg, const char *end) {
  while(beg != end && *beg != ' ' && *beg != '\t')
    beg++;
  return beg;
}

// token_equals return true if the range [beg, end[ is equal to
// token.
bool token_equals(const char *beg, const char *end, const char *token){
  int l = strlen(token);
  return (end-beg) >= l && strncmp(beg, token, l) == 0;
}

// decode_month returns the month indice or -1 if not a valid month.
int decode_month(const char *beg, const char *end) {
  const char *months[] = {"Jan", "Fev", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  if(end - beg == 3)
    for(int i = 0; i < 12; i++)
      if(strncmp(beg, months[i], 3) == 0)
        return i;
  return -1;
}


// parse_leap_second parses the line for a leap second. It returns a value
// with 0 posix time if the line is not a valid leap second line.
leap_second_t parse_leap_second(const char *line, int max_len, int delta) {
  struct tm t;
  memset(&t, 0, sizeof(struct tm));
  leap_second_t res;
  res.posix = res.tai = 0;
  res.delta = 0;
  // set end to end of line or return zero leap second if not found
  const char *end = line;
  while(end != line + max_len && *end != '\n')
    end++;
  if(end == line + max_len)
    return res; // line has been truncated
  // parse "Leap" token
  const char *tkBeg = line, *tkEnd = parse_token(tkBeg, end);
  res.delta = 1;
  if(!token_equals(tkBeg, tkEnd, "Leap"))
    return res; // line doesn't start with Leap
  // parse and decode year
  tkBeg = skip_white_space(tkEnd, end);
  tkEnd = parse_integer(tkBeg, end);
  res.delta = 2;
  if(tkBeg == tkEnd || tkEnd - tkBeg > 14)
    return res;
  t.tm_year = decode_ulong(tkBeg, tkEnd);
  res.delta = 3;
  if(t.tm_year < 1972 || t.tm_year > 3000)
    return res;
  t.tm_year -= 1900;
  // parse and decode month
  tkBeg = skip_white_space(tkEnd, end);
  tkEnd = parse_token(tkBeg, end);
  t.tm_mon = decode_month(tkBeg, tkEnd);
  res.delta = 4;
  if(t.tm_mon == -1)
    return res;
  // parse and decode day
  tkBeg = skip_white_space(tkEnd, end);
  tkEnd = parse_token(tkBeg, end);
  t.tm_mday = decode_ulong(tkBeg, tkEnd);
  const int max_days[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
  res.delta = 5;
  if(t.tm_mday < 1 || t.tm_mday > max_days[t.tm_mon])
    return res;
  // parse and decode time
  tkBeg = skip_white_space(tkEnd, end);
  tkEnd = parse_token(tkBeg, end);
  res.delta = 6;
  if(!decode_time(tkBeg, tkEnd, &t.tm_hour, &t.tm_min, &t.tm_sec))
    return res;
  // parse and decode sign
  tkBeg = skip_white_space(tkEnd, end);
  tkEnd = parse_token(tkBeg, end);
  res.delta = 7;
  if(tkEnd - tkBeg != 1 || (*tkBeg != '+' && *tkBeg != '-' ))
    return res;
  delta += *tkBeg == '+'?1:-1;
  // parse and check presence of "S"
  tkBeg = skip_white_space(tkEnd, end);
  tkEnd = parse_token(tkBeg, end);
  res.delta = 8;
  if(tkEnd - tkBeg != 1 || *tkBeg != 'S' || tkEnd != end)
    return res;
  // compute posix time_t value
  res.posix = timegm(&t);
  res.delta = delta;
  res.tai = res.posix + res.delta;
  return res;
}

void dump_leap_seconds(leap_second_t *tbl, int n){
  printf("posix_max: %lu tai_max: %lu\n", paconv_posix_max(), paconv_tai_max());
  for(int i = 0; i < n; i++) {
    printf("%12lu %12lu %ld\n", tbl[i].posix, tbl[i].tai, tbl[i].delta);
  }
}

// load_leapseconds_B loads leap seconds from the /usr/share/zoneinfo/leapseconds file.
int load_leapseconds_B(const char *file_name) {
  if(file_name == NULL)
    file_name = "/usr/share/zoneinfo/leapseconds";
  if(leap_seconds != NULL)
    free(leap_seconds);
  leap_seconds = NULL;
  posix_max = (time_t)63072000;
  tai_max = (time_t)63072010;
  nbr_leap_seconds = 0;
  time_t expires = 0;
  int max_vals = 50, n_vals = 0, n_leap = 10;
  leap_second_t *tbl = calloc(max_vals, sizeof(leap_second_t));
  FILE *f = fopen(file_name, "r");
  if (f == NULL)
    return -1; // failed opening file for reading
  char buf[128];
  int delta = 10; // initial number of lead seconds
  tbl[0].posix = 63072000;
  tbl[0].tai = tbl[0].posix + delta;
  tbl[0].delta = delta;
  n_vals++;
  while (fgets(buf, sizeof(buf), f)) {
    if(buf[0] == '\n')
      continue;
    buf[sizeof(buf)-1] = 0;
    if (buf[0] == '#') {
      if(strncmp(buf, "#expires ", 9) == 0)
        if(sscanf(buf, "#expires %lu", &expires) != 1)
          return -2; // failed reading expires date
      continue;
    }
    leap_second_t val = parse_leap_second(buf, sizeof(buf), delta);
    if(val.posix == 0)
      return -4-val.delta; // failed parsing leap second line
    delta = val.delta;
    if(n_vals == max_vals) {
      // grow buffer
      int new_max_vals = max_vals*2;
      leap_second_t *tmp = calloc(new_max_vals, sizeof(leap_second_t));
      memcpy(tmp, tbl, max_vals*sizeof(leap_second_t));
      free(tbl);
      tbl = tmp;
      max_vals = new_max_vals;
    }
    tbl[n_vals++] = val;
  }
  if(n_vals == 0)
    return -3; // no leap seconds found in file
  nbr_leap_seconds = n_vals;
  leap_seconds = tbl;
  posix_max = expires;
  tai_max = expires + tbl[n_vals-1].delta;
  dump_leap_seconds(leap_seconds, nbr_leap_seconds);
  return 0;
}

// load_leapseconds_A loads leap seconds from the
// /usr/share/zoneinfo/leap-seconds.list file or the file downloaded
// from https://www.ietf.org/timezones/data/leap-seconds.list.
int load_leapseconds_A(const char *file_name){
  static const long long offset = 2208988800LL;
  if(file_name == NULL)
    file_name = "/usr/share/zoneinfo/leap-seconds.list";
  if(leap_seconds != NULL)
    free(leap_seconds);
  leap_seconds = NULL;
  posix_max = (time_t)63072000;
  tai_max = (time_t)63072010;
  nbr_leap_seconds = 0;
  time_t expires = 0;
  int max_vals = 50, n_vals = 0, n_leap = 10;
  leap_second_t *tbl = calloc(max_vals, sizeof(leap_second_t));
  FILE *f = fopen(file_name, "r");
  if (f == NULL) {
    free(tbl);
    return -1; // failed opening file for reading
  }
  char buf[128];
  while (fgets(buf, sizeof(buf), f)) {
    if (buf[0] == '#') {
      if (buf[1] == '@') {
        long long posix;
        if (sscanf(buf + 2, "%lld", &posix) != 1) {
          free(tbl);
          return -2; // failed decoding the validity limit of table 
        }
        expires = (time_t)(posix - offset);
      }
    } else {
      long long delta, posix;
      if (sscanf(buf, "%lld%lld", &posix, &delta) != 2) {
        free(tbl);
        return -3; // failed decoding the posix time value
      }
      if(n_vals == max_vals) {
        // grow buffer
        int new_max_vals = max_vals*2;
        leap_second_t *tmp = calloc(new_max_vals, sizeof(leap_second_t));
        if(tmp == NULL) {
          free(tbl);
          return -4; // faile allocating storage
        }
        memcpy(tmp, tbl, max_vals*sizeof(leap_second_t));
        free(tbl);
        tbl = tmp;
        max_vals = new_max_vals;
      }
      posix -= offset;
      tbl[n_vals].posix = (time_t)posix;
      tbl[n_vals].tai = (time_t)posix + delta;
      tbl[n_vals].delta = delta;
      n_vals++;
    }
  }
  if(expires == (time_t)0) {
    free(tbl);
    return -6; // failed finding the validity limit value in file
  }
  if(n_vals < 25) {
    free(tbl);
    return -7; // not enough leap seconds
  }
  nbr_leap_seconds = n_vals;
  leap_seconds = tbl;
  posix_max = expires;
  tai_max = expires+tbl[n_vals-1].delta;
  dump_leap_seconds(leap_seconds, nbr_leap_seconds);
  return 0;
}

// paconv_init initializes paconv by loading the leap second list.
// If file_name is NULL, it tries to load them from the file
// "/usr/share/zoneinfo/leap-seconds.list" or "/usr/share/zoneinfo/leapseconds".
// If the OS doesn't have one of these files, you'll have to download the file
// from https://www.ietf.org/timezones/data/leap-seconds.list with a simple wget.
// A cron job executed every six months may keep it up to date.
int paconv_init(const char *file_name) {
  int res = load_leapseconds_A(file_name);
  if(res == 0)
    return 0;
  return load_leapseconds_B(file_name);  
}

