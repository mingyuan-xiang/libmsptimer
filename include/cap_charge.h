#ifndef INCLUDE_TIMEKEEPER_CAP_CHARGE_H
#define INCLUDE_TIMEKEEPER_CAP_CHARGE_H
#include <stdint.h>
#include <stdbool.h>

/* This library provides capacitor charging capabilties using a switch.
 * It functions as follows, assuming each switch has a pole, input,
 *    and output.
 *    1. POWER SWITCH: Used for charging the capacitor.
 *          Connect the output of the switch to the capacitor directly.
 *          This switch concerns 3 macros,
 *             CHARGE_CYCLES : # of DELAY_CYCLES for charging
 *             CHARGE_EN: the port, pin that controls the switch pole
 *             CHARGE_PIN: the port, pin connected to switch input
 *
 *    2. ADC SWITCH:  Used for sampling and discharging the cap.
 *          Connect the output of the switch to the cap w/ 100 resistor.
 *          This switch concerns 3 macros,
 *             DISCHARGE_CYCLES : # of DELAY_CYCLES for discharging
 *             ADC_EN: the port, pin the controls the switch pole
 *             ADC_PIN: the port, pin connected to switch input
 *             ADC_Ax : the analog pin value for ADC_PIN
 *
 *    Other important macros:
 *          DELAY_CYCLES: # of cycles to delay for discharge and charge
 *                functions.
 *
 * NOTE: CHANGE THESE DEFINITIONS HERE ATM. IF YOU WISH TO REPLACE WITH YOURS,
 *       DEFINE CHARGE_NODEFUALT AT COMPILE TIME.
 */

#ifndef CHARGE_NODEFUALT
/* Number of cycles to delay inside delay loops. */
#define DELAY_CYCLES 15

#define CHARGE_CYCLES 1000
#define DISCHARGE_CYCLES 912

#define CHARGE_EN 4, 1
#define CHARGE_PIN 8, 1

#define ADC_EN 1, 4
#define ADC_PIN 1, 5
#define ADC_Ax 5

#endif

#define CAPVREF 25
#define MINDISCHARGE 5

/* initilizes charging. */
void cap_charge_init();

/* for setting discharge rate. */
void cap_set_discharge(uint16_t discharge);

/* for setting charge rate. */
void cap_set_charge(uint16_t charge);

/* samples the capacitor */
void cap_sample(uint16_t *s, bool debug);

/* discharges the capacitor based on discharge count. */
void cap_discharge(bool debug);

/* discharges the capacitor based on discharge count & THEN charges
 * it up based on charge count.
 */
void cap_charge(bool discharge, bool debug);

// // uint8_t start_discharge(bool debug);
// // uint8_t stop_discharge(bool debug);
// // uint32_t time(bool debug);
// #define TABLE_LEN 4096
// #define resolution 1

#endif /* INCLUDE_TIMEKEEPER_CAP_CHARGE_H */