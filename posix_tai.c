#include "posix_tai.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* The leap second table is obtained from
 * https://www.ietf.org/timezones/data/leap-seconds.list
 * and generated from the file with the function
 * generate_leap_second_tbl() below.
 */
static const size_t nleap_seconds = 36;
struct leap_second_t {time_t posix, tai;};
struct leap_second_t validity_limit = {1467071999, 1467072035};
static const struct leap_second_t leap_seconds[] = {
    {         0,          0},
    {         0,          0},
    {         0,          0},
    {         0,          0},
    {         0,          0},
    {         0,          0},
    {         0,          0},
    {         0,          0},
    {         0,          0},
    {  63071999,   63072009}, // 2272060800	10	# 1 Jan 1972
    {  78796799,   78796810}, // 2287785600	11	# 1 Jul 1972
    {  94694399,   94694411}, // 2303683200	12	# 1 Jan 1973
    { 126230399,  126230412}, // 2335219200	13	# 1 Jan 1974
    { 157766399,  157766413}, // 2366755200	14	# 1 Jan 1975
    { 189302399,  189302414}, // 2398291200	15	# 1 Jan 1976
    { 220924799,  220924815}, // 2429913600	16	# 1 Jan 1977
    { 252460799,  252460816}, // 2461449600	17	# 1 Jan 1978
    { 283996799,  283996817}, // 2492985600	18	# 1 Jan 1979
    { 315532799,  315532818}, // 2524521600	19	# 1 Jan 1980
    { 362793599,  362793619}, // 2571782400	20	# 1 Jul 1981
    { 394329599,  394329620}, // 2603318400	21	# 1 Jul 1982
    { 425865599,  425865621}, // 2634854400	22	# 1 Jul 1983
    { 489023999,  489024022}, // 2698012800	23	# 1 Jul 1985
    { 567993599,  567993623}, // 2776982400	24	# 1 Jan 1988
    { 631151999,  631152024}, // 2840140800	25	# 1 Jan 1990
    { 662687999,  662688025}, // 2871676800	26	# 1 Jan 1991
    { 709948799,  709948826}, // 2918937600	27	# 1 Jul 1992
    { 741484799,  741484827}, // 2950473600	28	# 1 Jul 1993
    { 773020799,  773020828}, // 2982009600	29	# 1 Jul 1994
    { 820454399,  820454429}, // 3029443200	30	# 1 Jan 1996
    { 867715199,  867715230}, // 3076704000	31	# 1 Jul 1997
    { 915148799,  915148831}, // 3124137600	32	# 1 Jan 1999
    {1136073599, 1136073632}, // 3345062400	33	# 1 Jan 2006
    {1230767999, 1230768033}, // 3439756800	34	# 1 Jan 2009
    {1341100799, 1341100834}, // 3550089600	35	# 1 Jul 2012
    {1435708799, 1435708835}  // 3644697600	36	# 1 Jul 2015
};


time_t posix2tai(time_t posix) {
    size_t n = (sizeof(leap_seconds) / sizeof(struct leap_second_t)) - 1;
    if (posix >= leap_seconds[n].posix)
        return posix < validity_limit.posix ? posix + n : 0;
    size_t l = 10;
    n += 10;
    while (n > 0) {
        size_t s = n / 2, i = l + s;
        if (leap_seconds[i].posix < posix) {
            l = ++i; n -= ++s;
        } else {
            n = s;
        }
    }
    return posix + (time_t)l;
}


time_t tai2posix(time_t tai) {
    size_t n = (sizeof(leap_seconds) / sizeof(struct leap_second_t)) - 1;
    if (tai > leap_seconds[n].tai)
        return tai < validity_limit.tai ? tai - n : 0;
    size_t l = 10;
    n -= 10;
    while (n > 0) {
        size_t s = n / 2, i = l + s;
        if (leap_seconds[i].tai < tai) {
            l = ++i; n -= ++s;
        } else {
            n = s;
        }
    }
    return tai - (time_t)l;
}

int is_valid_posix_time(time_t posix) {
    return posix < validity_limit.posix;
}

int is_valid_tai_time(time_t tai) {
    return tai < validity_limit.tai;
}


struct val_t {
    time_t p, t, d;
    char   c[128];
};

struct leap_second_tbl_t {
    size_t n;           /* number of leap seconds in table */
    struct val_t l;     /* validity limit of table */
    struct val_t v[1];  /* table of leap seconds relative to
                           1970-01-01T00:00:00 */
};

/* Return a malloc leap second table loaded from the leap-second.list
 * file obtained from https://www.ietf.org/timezones/data/leap-seconds.list.
 * Return NULL if it fails to load the data.
 * Converts the second count with NTP epoch to Unix epoch 1970-01-01T00:00:00
 * The number of seconds elapsed between the NTP epoch 1900-01-01T00:00:00
 * to the Unix epoch is 2208988800L according to the Time protocol in RFC 868.
 */
struct leap_second_tbl_t*
        load_posix_leap_seconds(const char *leap_seconds_list) {
    static const long long offset = 2208988801LL;
    struct leap_second_tbl_t *tbl = NULL;
    char buf[128];
    FILE *f = fopen(leap_seconds_list, "r");
    if (f == NULL) {
        fprintf(stderr, "%s : %s\n", strerror(errno), leap_seconds_list);
        goto fail;
    }
    /* Count the number of leap seconds and get validity limit*/
    size_t n = 0;
    time_t l = 0;
    while (fgets(buf, sizeof(buf), f)) {
        if (buf[0] == '#') {
            if (buf[1] == '@') {
                long long tmp;
                if (sscanf(buf + 2, "%lld", &tmp) != 1)
                    goto fail;
                l = (time_t)(tmp - offset);
            }
        } else  if (buf[0] != '\n' && buf[0] != 'r')
            ++n;
    }
    if (n < 25)
        goto fail;
    /* Fill the table with the leap seconds */
    tbl = calloc(1, sizeof(struct leap_second_tbl_t) + n *
                 sizeof(struct val_t));
    tbl->n = n;
    tbl->l.p = l;
    size_t i = 0;
    rewind(f);
    while (fgets(buf, sizeof(buf), f)) {
        if (buf[0] != '#' && buf[0] != '\n' && buf[0] != 'r') {
            long long tmp, delta;
            if (sscanf(buf, "%lld%lld", &tmp, &delta) != 2)
                goto fail;
            struct val_t *p = tbl->v + i++;
            p->p = (time_t)(tmp - offset);
            p->t = p->p + delta;
            p->d = delta;
            strtok(buf,"\n\r"); /* remove trailing \n or \r */
            strcpy(p->c, buf);
        }
    }
    fclose(f);
    tbl->l.d = tbl->v[n - 1].d;
    tbl->l.t = tbl->l.p + tbl->l.d;
    return tbl;

fail:
    free(tbl);
    fclose(f);
    return NULL;
}

/* Generate the source code for leap seconds table */
void generate_leap_second_tbl(const char *leap_seconds_list) {
    const char *fn = leap_seconds_list ?leap_seconds_list : "leap-seconds.list";
    struct leap_second_tbl_t* tbl = load_posix_leap_seconds(fn);
    if (tbl == NULL) {
        printf("Failed loading the leap seconds from file '%s'\n", fn);
    } else {
        size_t i;
        puts("");
        printf("static const size_t nleap_seconds = %zd;\n", tbl->n);
        printf("struct leap_second_t {time_t posix, tai, delta;};\n");
        printf("struct leap_second_t validity_limit = {%lld, %lld, %lld};\n",
               (long long)tbl->l.p, (long long)tbl->l.t, (long long)tbl->l.d);
        printf("static const struct leap_second_t leap_seconds[] = {\n");
        struct val_t *p = tbl->v;
        for (i = 0; i < tbl->n - 1; ++i, ++p) {
            printf("    {%10lld, %10lld, %3lld},", (long long)p->p,
                   (long long)p->t, (long long)p->d);
            if (p->c[0])
                printf(" // %s\n", p->c);
            else
                printf("\n");
        }
        printf("    {%10lld, %10lld, %3lld}  // %s\n", (long long)p->p,
               (long long)p->t, (long long)p->d, p->c);
        printf("};\n");
        free(tbl);
    }
}


int fail_test(time_t t) {
    time_t tai = posix2tai(t);
    time_t posix = tai2posix(tai);
    if (posix != t)
        return 1;
    return 0;
}


int posix_tai_unit_test() {
    if (posix2tai(0) != 10)
        return -1;
    if (posix2tai(1000) != 1010)
        return -2;
    if (posix2tai(63072000) != 63072010)
        return -3;
    if (posix2tai(63073000) != 63073010)
        return -4;
    if (posix2tai(78796799) != 78796809)
        return -5;
    if (posix2tai(78796800) != 78796811)
        return -6;
    if (posix2tai(78797800) != 78797811)
        return -7;
    if (tai2posix(0) != -10)
        return -8;
    if (tai2posix(1010) != 1000)
        return -9;
    if (tai2posix(63072010) != 63072000)
        return -10;
    if (tai2posix(63073010) != 63073000)
        return -11;
    if (tai2posix(78796809) != 78796799)
        return -12;
    if (tai2posix(78796810) != 78796800)
        return -13;
    if (tai2posix(78796811) != 78796800)
        return -14;
    if (tai2posix(78797811) != 78797800)
        return -15;
    time_t l, b = 63072000, m = 94694412 - b;
    time_t k = (m + 1) / 100;
    printf("Executing unit test\n");
    for (l = 0; l < m; ++l) {
        if ((l % k) == 0)
            printf("Unit test: %d%%\n", (int)(l/k));
        if (fail_test(b + l))
            return -100;
    }
    if (!is_valid_posix_time(time(NULL))) {
        fputs("leap_seconds table in posix_tai.c is outdated\n", stderr);
        return -101;
    }
    if (!is_valid_posix_time(posix2tai(time(NULL)))) {
        fputs("leap_seconds table in posix_tai.c is outdated\n", stderr);
        return -102;
    }
    return l != m;
}
