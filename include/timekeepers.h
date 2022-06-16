#ifndef INCLUDE_TIMEKEEPER_H
#define INCLUDE_TIMEKEEPER_H
#include <stdint.h>

/* This library provides timekeeper functionlity as follows,
 *    stop_watch : measures how long a function takes using lfxt
 *                 accurate, but cannot be run intermittently
 *    time_out   : processes a timeout using the timeout circuitry.
 *                 for more info, refer to appropriate header.
 *    sleep_timer: sleeps for a certain duration
 *
 * Each module/functionality in this library uses a different timer
 * to prevent clashes. If using other libraries that use timers,
 * change the timers used below.
 */

#define STOPWATCH_TIMER 1
#define TIMEOUT_CALIB_TIMER 2
#define SLEEP_TIMER 3

typedef void (*stop_watch_fn)(void *);

/* If using the timers in this library, initilize them as follows
 * Similarly, you can stop the initilized processes.
 */
void timers_init();

void timers_stop();

/* Measures how long a function takes with LFXTCLK.
 * fn should not use ACLK. ACLK & PJ.5-6 settings are altered
 * in this function.
 *
 * The return value is in milli-seconds or cycles
 */
uint32_t stop_watch_cycle(stop_watch_fn fn, void *arg);

uint32_t stop_watch_ms(stop_watch_fn fn, void *arg);


/* Uses the LFXTCLK to sleep for a certain duration in ms
 * Once the sleep is over, the function will return. ACLK & PJ.5-6 
 *    settings are altered in this function.
 *
 * end is either in milli-seconds or cycles.
 */
void sleep_timer_cycle(uint32_t end);

void sleep_timer_ms(uint32_t end);


#endif /* INCLUDE_TIMEKEEPER_H */