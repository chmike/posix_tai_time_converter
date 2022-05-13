# posix tai time converter

C functions for conversion between POSIX time and TAI time.

POSIX time is the system time returned by the functions time(),
gettimeofday() or clock_gettime(CLOCK_REALTIME,...). It is the
number of seconds elapsed since 1970-01-01T00:00:00 UTC
subtracted by the number of leap seconds.

TAI time is the time value returned by clock_gettime(CLOCK_TAI,...).
It is the number of seconds elapsed since 1972-01-01T00:00:00 UTC
plus 10 seconds without leap seconds subtracted.

This library can't convert time before 1972-01-01T00:00:00 UTC.

## Leap seconds

In order to convert time between UTC and TAI, the library needs the 
list of leap seconds. In the initial version of the library, this 
list was hard coded. It had the disadvantage that the library needed
to be updated when new leap seconds were added. 

To remove this problem, the library now loads the leap second list
from a file whose name is given as argument of an initialization 
function. 

The latest version of this file may be obtained from 
https://www.ietf.org/timezones/data/leap-seconds.list with a simple 
`wget https://www.ietf.org/timezones/data/leap-seconds.list`. 

On Debian and derived systems, this file is already available as
`/usr/share/timezone/leap-second.list`, and it is updated as needed.
Users of other operating systems will have to set up a cron job to 
get an equivalent updated file. Updating it every June and December
is enough.

## API

**`int paconv_load_leap_seconds(const char *leap_second_list_file_name)`**

The `paconv_load_leap_seconds` function loads the leap second list 
from the file whose name is given as argument. It returns 0 if it succeeds
and -1 in case of failure. It is required to call this function before any
of the conversion functions. 

On Debian and derived systems, the path to the file is 
`/usr/share/timezone/leap-second.list`. It it updated by the operating
system. On other systems, the user is responsible to download the file
with the command `wget https://www.ietf.org/timezones/data/leap-seconds.list`.
It is suggested to use a cron job to automatically update this file
in June and December.

**`const time_t paconv_invalid`**

The `paconv_invalid` time_t value is returned by the conversion functions
when the input value is out of valid range.

**`time_t paconv_posix_min()`**

The `paconv_posix_min` function returns the smallest valid posix time 
value.

**`time_t paconv_posix_max()`**

The `paconv_posix_max` function returns the bigest valid posix time 
value.

**`time_t paconv_tai_min()`**

The `paconv_tai_min` function returns the smallest valid tai time value.

**`time_t paconv_tai_max()`**

The `paconv_tai_max` function returns the bigest valid tai time value.


**`time_t paconv_posix2tai(time_t posix)`**

The `paconv_posix2tai` function returns the tai time corresponding to the
given posix time, or the `paconv_invalid` time_t value if the given posix 
time value is out of valid range or a leap second file has not been loaded.

**`time_t paconv_tai2posix(time_t tai)`**

The `paconv_tai2posix` function returns the poisx time corresponding to the
given tai time, or the `paconv_invalid` time_t value if the given tai 
time value is out of valid range or a leap second file has not been loaded.

## Compilation

To compile and execute the unit test, run the following command

    gcc -O3 main.c posix_tai.c && ./a.out
    
## Security

The code has been extensively tested. There is a small risk to get invalid
results if the input file is invalid.


