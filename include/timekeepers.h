#ifndef INCLUDE_TIMEKEEPER_H
#define INCLUDE_TIMEKEEPER_H
#include <stdint.h>

// Adjust these if using a sleeper inside a stop watch while using smclk for stop watch.
#define TIMEKEEPER_SLEEP LPM1;
#define TIMEKEEPER_EXIT_SLEEP LPM1_EXIT;

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
 *
 * Takes circa 35 ms based on my experiments.
 */
void timers_init();

void timers_stop();

/* Measures how long a function takes with LFXTCLK.
 * fn should not use ACLK. ACLK & PJ.5-6 settings are altered
 * in this function.
 *
 * The return value is in milli-seconds or cycles
 * lfxt runs the timer in with lfxtclk; if false, SMCLK is used
 */
uint32_t stop_watch_cycle(stop_watch_fn fn, void *arg, bool aclk);

uint32_t stop_watch_ms(stop_watch_fn fn, void *arg);


/* Uses the SMCLK or ACLK (LFXTCLK ideally) to sleep for a certain duration in ms
 * Once the sleep is over, the function will return. ACLK & PJ.5-6 
 *    settings are altered in this function.
 *
 * end is either in milli-seconds or cycles.
 * Note: if using SMCLK, don't use sleep_timer_ms. It DOES NOT work due to math.
 */
void sleep_timer_cycle(uint32_t end, bool aclk);

void sleep_timer_ms(uint32_t end);

/* Measures how many cycles the smclk takes in 32768 of LFXT clock cycles.
 * To work needs timers init setup and smlk setup to desired clock.
 * 
 * In essence, this function runs a sleep timer with 32768 on the LFXT
 * and uses a stop watch with the smlk. So in reality, the value returned
 * is how long it took for the 32768 LFXT cycles to run + stopwatch DCO overhead.
 * With DCO set to 1 MHz, this takes 3 LFXT cycles, approximately 0.1 ms or 
 * 90-100 cycles.
 * 
 */
uint32_t measure_smlk_cycles();

/* Measures how many cycles a clock approximately takes in 1 second.
 * To do so, it replaces the SMCLK with selected clk source and runs 
 * lfxt for 32768 cycles. Similar to above, it has some small overhead, 
 * about 0.1 ms or 90-100 DCO cycles on 1 MHz.
 * c goes as follows,
 *    1: DCO
 *    2: MODCLK
 *    3: LFMODCLK
 *    4: VLOCLK
 *    5: LFXTCLK (calibrated based on this)
 *    6: HFXTCLK
 * 
 * Assumes lfxtclk is setup by timers_init
 */
uint32_t measure_freq(uint8_t c);

#endif /* INCLUDE_TIMEKEEPER_H */