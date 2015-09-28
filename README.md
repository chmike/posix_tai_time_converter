# posix tai time converter

C functions for conversion between POSIX time and TAI time.

POSIX time is the system time returned by the functions time(),
gettimeofday() or clock_gettime(CLOCK_REALTIME,...). It is the
number of seconds elapsed since 1970-01-01T00:00:00 UTC
subtracted by the number of leap seconds.

TAI time is the time value returned by clock_gettime(CLOCK_TAI,...).
It is the number of seconds elapsed since 1970-01-01T00:00:00 UTC
without leap seconds subtracted.

The following conversion functions can be used to convert time 
between POSIX and TAI time.

The conversion uses a table of leap seconds obtained from
https://www.ietf.org/timezones/data/leap-seconds.list.
This file is updated as new leap seconds are added.

In order to teest if the table is outdated use the following code

    if (!is_valid_posix_time(time(NULL))) /* table is outdated */;

The table is currently hard code but should ideally be loaded from
the referenced source when possible. It should also be reloaded at
runtime when its validity limit is reached so that programs running
for a very long duration get a properly updated leap second table.
Dynamic loading and updating of the leap second table might be
implemented in future version of this files.

### Compilation

To compile and run use the following command

    gcc -O3 main.c posix_tai.c && ./a.out
    
It will first output the C code for the hardcoded table and the run the unit
tests.

If the hardcoded leap second table is outdated, the unit test will fail. You then should download the latest version of the `leap-seconds.list` file at 
https://www.ietf.org/timezones/data/leap-seconds.list.

By running the test again, you will get as first output the new version of
the code for the table. Replace the corresponding source code in posix_tai.c
and run the test again. The unit test should then succeed.

