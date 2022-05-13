#ifndef PACONV_H
#define PACONV_H

#include <time.h>
#include <stdbool.h>

// paconv_load_leap_seconds loads the leap second list from the file
// whose name is given as argument. The latest file may be downloaded
// from https://www.ietf.org/timezones/data/leap-seconds.list.
// On Debian and derived OS the latest file version is available as
// "/usr/share/timezone/leap-second.list". 
int paconv_load_leap_seconds(const char *leap_second_list_file_name);

// paconv_invalid is an invalid time_t value.
extern const time_t paconv_invalid;

// paconv_posix_max returns the maximum posix value.
time_t paconv_posix_max();

// paconv_posix_max returns the minimum posix value.
time_t paconv_posix_min();

// paconv_posix_max returns the maximum tai value.
time_t paconv_tai_max();

// paconv_posix_max returns the minimum tai value.
time_t paconv_tai_min();

// paconv_posix2tai returns the tai time corresponding to the given posix time,
// or the paconv_invalid time_t value if the posix time is out of valid range.
time_t paconv_posix2tai(time_t posix);

// paconv_tai2posix returns the posix time corresponding to the given tai time,
// or the paconv_invalid time_t value if the tai time is out of valid range.
time_t paconv_tai2posix(time_t tai);

#endif // PACONV_H