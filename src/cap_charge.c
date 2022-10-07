// #include <msp430.h>
// #include <mspbase.h>
// #include <mspprintf.h>

// #include <cap_charge.h>
// #include <timekeepers.h>

// static uint16_t sample_voltage;
// static uint16_t charge_time;
// static uint16_t discharge_time = MINDISCHARGE;

// void cap_set_discharge(uint16_t discharge)
// {
//    if (discharge < MINDISCHARGE)
//    {
//       discharge_time = MINDISCHARGE;
//    }
//    else
//    {
//       discharge_time = discharge;
//    }
// }

// void cap_set_charge(uint16_t charge)
// {
//    charge_time = charge;
// }

// void cap_charge_init()
// {
//    cap_set_charge(CHARGE_CYCLES);
//    cap_set_discharge(DISCHARGE_CYCLES);

//    gpio_clear_(CHARGE_EN);
//    gpio_clear_(CHARGE_PIN);
//    gpio_clear_(ADC_EN);
//    gpio_clear_(ADC_PIN);

//    gpio_dir_out_(CHARGE_EN);
//    gpio_dir_out_(CHARGE_PIN);
//    gpio_dir_out_(ADC_EN);
//    gpio_dir_in_(ADC_PIN);
// }

// void cap_sample(uint16_t *s, bool debug)
// {
//    // enable adc_pin and set to analog mode
//    // adc_gpio_(ADC_PIN);
   

//    // Configure internal REFerence module at 1.2V for ADC.
//    ref_busy();
//    ref_start(CAPVREF);

//    // Configure adc to ADC_Ax
//    adc_setup_single_conv(ADC_Ax, 0);

//    // Wait for reference to settle.
//    ref_ready();

//    // Turn on the adc and sleep
//    _enable_interrupts();
//    gpio_set_(ADC_EN);
//    adc_begin();
//    LPM1;

//    // clean up and set disable adc
//    adc_clean_single_conv(ADC_Ax, 0);
//    ref_stop();

//    gpio_clear_(ADC_EN);
//    gpio_unsel0_(ADC_PIN);
//    gpio_unsel1_(ADC_PIN);

//    if (debug)
//    {
//       msp_printf(0, "Sampled %u\n\r", sample_voltage);
//    }

//    *s = sample_voltage;
// }

// void cap_discharge(bool debug)
// {
//    // set the adc_pin to output LOW
//    gpio_clear_(ADC_PIN);
//    gpio_dir_out_(ADC_PIN);
//    gpio_set_(ADC_EN);

//    for (uint16_t i = 0; i < discharge_time; i++)
//    {
//       __delay_cycles(DELAY_CYCLES);
//    }

//    gpio_clear_(ADC_EN);
//    gpio_dir_in_(ADC_PIN);

//    if (debug)
//    {
//       uint16_t res;
//       cap_sample(&res, false);
//       msp_printf(0, "Discharged %u\n\r", sample_voltage);
//    }
// }

// void cap_charge(bool discharge, bool debug)
// {
//    // Discharge the timekeeper
//    if (discharge)
//    {
//       cap_discharge(debug);
//    }

//    gpio_set_(CHARGE_PIN);
//    gpio_set_(CHARGE_EN);

//    for (uint16_t i = 0; i < charge_time; i++)
//    {
//       __delay_cycles(DELAY_CYCLES);
//    }
//    gpio_clear_(CHARGE_EN);
//    gpio_clear_(CHARGE_PIN);

//    if (debug)
//    {
//       uint16_t res;
//       cap_sample(&res, false);
//       msp_printf(0, "Charged %u\n\r", sample_voltage);
//    }
// }

// // uint16_t wakeup_voltage;
// // extern uint16_t log_lut[TABLE_LEN];

// // uint8_t start_discharge(bool debug)
// // {
// //    gpio_set(DISCHARGE_PORT, DISCHARGE_PIN);
// // }

// // uint8_t stop_discharge(bool debug)
// // {
// //    gpio_clear(DISCHARGE_PORT, DISCHARGE_PIN);
// // }

// // uint32_t time(bool debug)
// // {
// //    uint16_t s;
// //    sample(&s, debug);
// //    return log_lut[s] * resolution;
// // }

// __attribute__((interrupt(ADC12_B_VECTOR))) void ADC12_ISR(void)
// {
//    sample_voltage = ADC12MEM0;
//    LPM1_EXIT;
// }