#ifndef INCLUDE_TIMEKEEPER_H
#define INCLUDE_TIMEKEEPER_H
#include <libmsp/macro_basics.h>
#include <stdint.h>
#include <string.h>

#ifndef CONFIG_STOPWATCH_TIMER
#define CONFIG_STOPWATCH_TIMER 1
#endif

/* If using the timers in this library, initilize them as follows
 * Similarly, you can stop the initilized processes.
 *
 * Takes circa 35 ms experiments, i.e. don't use this library in
 * production.
 */
void timers_init();
void timers_stop();

/*
* This function starts the timer.
*/
void start_timer();

/*
* This function stops the timer and returns the number of clock cycles
* @return the number of clock cycles
*/
uint32_t stop_timer();

/*
* The following two functions can prevent the compiler from optimizing some part of the
* code so that we will get wrong timing results, such as reordering the instructions
* or removing the instructions.
* Reference:
*   + https://stackoverflow.com/a/38025837/13813036
*   + https://www.youtube.com/watch?v=nXaxk27zwlk
*/

/*
* This function prevents the compiler from removing the instructions.
*/
__inline__ void escape(void* p) {
  __asm__ volatile("" : : "g"(p) : "memory");
}

/*
* This is a memory barrier for the compiler that prevents the compiler from
* reordering the instructions.
*/
__inline__ void clobber() {
  __asm__ volatile("" : : : "memory");
}

#endif /* INCLUDE_TIMEKEEPER_H */