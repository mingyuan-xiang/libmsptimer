#ifndef INCLUDE_MSPTIMER_H
#define INCLUDE_MSPTIMER_H
#include <stdint.h>

typedef void (*timer_fn)(void *);

// Measures how long a function takes with MCLK 1, 4, 8, or 16 MHz.
// fn should not use SMCLK. If using SMCLK, look into other functions
// SMCLK settings are restored after this function is done executing
uint32_t measure_fn_us(timer_fn fn, void *arg, int freq);

#endif /* INCLUDE_MSPTIMER_H */