#include <stdio.h>
#include "paconv.h"


int paconv_test() {
  if(paconv_posix2tai(0) != paconv_invalid) {
    return -1;
  }
  if(paconv_posix2tai(100000) != paconv_invalid) {
    return -2;
  }
  if (paconv_posix2tai(63072000) != 63072010) {
    return -3;
  }
  if (paconv_posix2tai(63073000) != 63073010) {
    return -4;
  }
  if (paconv_posix2tai(78796799) != 78796809) {
    return -5;
  }
  if (paconv_posix2tai(78796800) != 78796811) {
    return -6;
  }
  if (paconv_posix2tai(78797800) != 78797811) {
    return -7;
  }
  if (paconv_tai2posix(0) != paconv_invalid) {
    return -8;
  }
  if (paconv_tai2posix(1010) != paconv_invalid) {
    return -9;
  }
  if (paconv_tai2posix(63072010) != 63072000) {
    return -10;
  }
  if (paconv_tai2posix(63073010) != 63073000) {
    return -11;
  }
  if (paconv_tai2posix(78796809) != 78796799) {
    return -12;
  }
  if (paconv_tai2posix(78796810) != 78796800) {
    return -13;
  }
  if (paconv_tai2posix(78796811) != 78796800) {
    return -14;
  }
  if (paconv_tai2posix(78797811) != 78797800) {
    return -15;
  }
  if (paconv_posix2tai(paconv_posix_max()) != paconv_tai_max()) {
    return -16;
  }
  if (paconv_tai2posix(paconv_tai_max()) != paconv_posix_max()) {
    return -17;
  }
  if (paconv_posix2tai(paconv_posix_max()+1) != paconv_invalid) {
    return -18;
  }
  if (paconv_tai2posix(paconv_tai_max()+1) != paconv_invalid) {
    return -19;
  }
  if (paconv_posix2tai(paconv_posix_min()) != paconv_tai_min()) {
    return -20;
  }

  if (paconv_tai2posix(paconv_tai_min()) != paconv_posix_min()) {
    return -21;
  }
  if (paconv_posix2tai(paconv_posix_min()-1) != paconv_invalid) {
    return -22;
  }

  if (paconv_tai2posix(paconv_tai_min()-1) != paconv_invalid) {
    return -23;
  }

  time_t min = paconv_posix_min(), max = paconv_posix_max();
  int k = 10000, k2 = 100;
   printf("Executing unit test\n");
  for(int i = 0; i < k; i++) {
    time_t posix = min + i*((max-min)/k);
    if (i%k2 == 0)
      printf("Unit test: %d%%\n", i/k2);
    if (paconv_tai2posix(paconv_posix2tai(posix)) != posix) {
      fprintf(stderr, "posix tai conversion error with posix time %lld\n", (long long)posix);
      return -100;
    }
  }
  return 0;
}


int main(void){
  int res = paconv_init(NULL);
  if (res < 0) {
    fprintf(stderr, "Failed initializing paconv: %d\n", res);
    return 1;
  }
  res = paconv_test();
  if (res < 0) {
    fprintf(stderr, "Unit test failure: %d\n", res);
    return 1;
  }
  puts("Unit test succeeded");
  return 0;
}

