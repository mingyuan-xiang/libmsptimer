#ifndef INCLUDE_TIMEKEEPER_CAP_CALIB_H
#define INCLUDE_TIMEKEEPER_CAP_CALIB_H
#include <stdint.h>
#include <stdbool.h>

/* This library provides calibration capabilities for the cap_charge library.
 * Possible calibrations are,
 *    1. Charge/Discharge Time calibration:
 *          These calibrate the charging time such that a full charge or
 *             discharge results at least (for discharge) or at most (for
 *             charge) in the requested adc value.
 *          Note that charging in dependent on dicharge and discharge is
 *             dependent on the initial voltage of the cap. The
 *             calibration tries to tune both in tandem.
 *
 *          NOTE: The calibration WILL alter the charge/discharge times.
 * 
 *    2. Cover Range:
 *          This function helps with calibrating the timekeepers. It takes 
 *             a start, an end, and a step in ms. It then prints out the 
 *             adc value of the timekeeper/cap every step ms in the 
 *             start-end range. The printout is of the format 
 *                ChargedVoltage, DecayedVoltaged, Diff, Duration
 */

void cap_calib_dis_charge(uint16_t discharge_goal,
                          uint16_t charge_goal,
                          uint16_t *discharge_time,
                          uint16_t *charge_time,
                          bool debug);


void cap_calib_cover_range(uint32_t start,
                           uint32_t end,
                           uint32_t step,
                           bool debug);

void cap_calib_cover_duration(uint32_t start,
                              uint32_t step,
                              uint16_t end_adc,
                              bool debug);


// uint8_t test_repeatability(uint32_t time_ms, uint8_t repeats, bool debug);
// uint8_t test_repeatability_range(uint32_t start, uint32_t end,
//                                  uint16_t step, uint8_t repeats, bool debug);

#endif /* INCLUDE_TIMEKEEPER_CAP_CHARGE_H */