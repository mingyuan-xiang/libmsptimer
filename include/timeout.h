#ifndef INCLUDE_TIMEKEEPER_TIMEOUT_H
#define INCLUDE_TIMEKEEPER_TIMEOUT_H

#include <timekeepers.h>
#include <stdlib.h>

/* Timeout provides an efficient method for keeping track of min time
 * passed without using the internal timer and works even if the MSP loses
 * power. The following library provides the following funcionality,
 *    timeout invocation: Starts the timeout mechanism based on the requested
 *                         wait time. Tries to be accurate as possible.
 *
 *    timeout calibration: various functionality is provided for timeout
 *                         calibration.
 */

#define _TIMEOUT_INTPIN_ 3
#define _TIMEOUT_INTPORT_ 6
#define TIMEOUT_INT _TIMEOUT_INTPORT_, _TIMEOUT_INTPIN_

/* Initilizes timeout. Note that it needs to use cap_charge_init, so make sure 
 *    that it setup properly (pins and such).
 */
void init_timeout();


/* Enters sleep mode until awakened by pin
 */
void timeout_sleep();


/* Tests how long a timeout can last for a range of charge values.
 * Prints outputs to uart in the following format,
 *    charge_time, timeout delay
 */
void timeout_range(uint16_t *range, size_t count);


/* Finds the minimmum charge time such that the status on the INPORT.INTPIN
 * changes.
 */
uint16_t timeout_min_charge_finder(uint16_t min, uint16_t max, uint16_t step);

#endif /* INCLUDE_TIMEKEEPER_TIMEOUT_H */