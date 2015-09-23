#include <time.h>

/* POSIX time is the system time returned by the functions time(),
 * gettimeofday() or clock_gettime(CLOCK_REALTIME,...). It is the
 * number of seconds elapsed since 1970-01-01T00:00:00 UTC
 * subtracted by the number of leap seconds.
 *
 * TAI time is the time value returned by clock_gettime(CLOCK_TAI,...).
 * It is the number of seconds elapsed since 1970-01-01T00:00:00 UTC
 * without leap seconds subtracted.
 *
 * The following conversion can be used to convert time between POSIX
 * and TAI.
 *
 * The conversion uses a table of leap seconds obtained from
 * https://www.ietf.org/timezones/data/leap-seconds.list.
 * This file is updated as new leap seconds are added.
 *
 * The table is currently hard code but should ideally be loaded from
 * the referenced source when possible. It should also be reloaded at
 * runtime when its validity limit is reached so that programs running
 * for a very long duration get a properly updated leap second table.
 * Dynamic loading and updating of the leap second table might be
 * implemented in future version of this files.
 */


/* Converts POSIX time to TAI time */
time_t posix2tai(time_t posix_time);

/* Converts TAI time to POSIX time */
time_t tai2posix(time_t tai_time);

/* Return 1 if posix_time doesn't exceed the leap second table validity
 * limit, return 0 otherwise.
 */
int is_valid_posix_time(time_t posix_time);

/* Return 1 if tai_time doesn't exceed the leap second table validity
 * limit, return 0 otherwise.
 */
int is_valid_tai_time(time_t tai_time);

/* Generate source code for leap second table using file from
 * https://www.ietf.org/timezones/data/leap-seconds.list.
 * If leap_seconds_list is NULL try to open the local file
 * leap-seconds.list.
 */
void generate_leap_second_tbl(const char *leap_seconds_list);

/* Execute a unit test. Return 0 on success and -1 on failure */
int posix_tai_unit_test();
