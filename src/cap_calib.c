#include <msp430.h>
#include <mspbase.h>
#include <mspprintf.h>

#include <cap_charge.h>
#include <cap_calib.h>
#include <timekeepers.h>

/* Discharges the cap continously until it reaches the discharge goal */
static void cap_calib_discharge_all(uint16_t discharge_goal, bool debug)
{
   uint16_t s;
   cap_sample(&s, debug);
   while (s > discharge_goal)
   {
      cap_discharge(debug);
      cap_sample(&s, debug);
   }
}

/* Charges the cap continously until it reaches the charge goal */
static void cap_calib_charge_all(uint16_t charge_goal, bool debug)
{
   uint16_t s;
   cap_sample(&s, debug);
   while (s < charge_goal)
   {
      cap_charge(false, debug);
      cap_sample(&s, debug);
   }
}

void cap_calib_dis_charge(uint16_t discharge_goal,
                          uint16_t charge_goal,
                          uint16_t *discharge_time,
                          uint16_t *charge_time,
                          bool debug)
{
   uint16_t s;

   // Start by calibrating a good enough discharge
   // NOTE: this sets a discharge time that is larger than needed.
   s = 0xFFF;
   (*discharge_time)--;
   while (s > discharge_goal)
   {
      (*discharge_time)++;
      cap_set_discharge(*discharge_time);

      if (debug)
      {
         uint16_t v = *discharge_time;
         msp_printf(0, "Attempting discharge with: %u\n\r", v);
      }
      // charge it all the way up, then attempt to discharge
      cap_calib_charge_all(charge_goal, false);
      cap_discharge(false);
      cap_sample(&s, debug);
   }

   // Calibrate the charge, given the discharge
   s = 0;
   (*charge_time)--;
   while (s < charge_goal)
   {
      (*charge_time)++;
      cap_set_charge(*charge_time);

      if (debug)
      {
         uint16_t v = *charge_time;
         msp_printf(0, "Attempting charge with: %u\n\r", v);
      }

      // Since discharge is calibrated, charge it normally
      cap_charge(true, false);
      cap_sample(&s, debug);
   }

   // Recalibrate the discharge
   s = 0;
   (*discharge_time)++;
   while (s < discharge_goal)
   {
      (*discharge_time)--;
      cap_set_discharge(*discharge_time);

      if (debug)
      {
         uint16_t v = *discharge_time;
         msp_printf(0, "Attempting discharge with: %u\n\r", v);
      }

      // Charge normally
      cap_charge(true, false);
      cap_discharge(false);
      cap_sample(&s, debug);
   }

   // Recalibrate the charge
   s = 0XFFF;
   (*charge_time)++;
   while (s >= charge_goal)
   {
      (*charge_time)--;
      cap_set_charge(*charge_time);

      if (debug)
      {
         uint16_t v = *charge_time;
         msp_printf(0, "Attempting charge with: %u\n\r", v);
      }

      // Charge normally
      cap_charge(true, false);
      cap_sample(&s, debug);
   }
}

void cap_calib_cover_range(uint32_t start,
                           uint32_t end,
                           uint32_t step,
                           bool debug)
{
   if (start == 0 || end == 0 || step == 0)
   {
      msp_printf(0, "Invalid calibration settings\n\r");
   }
   msp_printf(0, "\n\r");

   timers_init();
   uint32_t decay_ms = start;

   msp_printf(0, "InitialVoltage,RecordVoltage,VoltageDiff,Duration\n\r");

   cap_charge(true, false);
   while (decay_ms <= end)
   {
      uint16_t v_i, v_e;
      cap_charge(true, false);
      cap_sample(&v_i, debug);

      // start_discharge(false);

      sleep_timer_ms(decay_ms);

      // stop_discharge(false);

      cap_sample(&v_e, debug);

      // Prevents a caching issue
      __delay_cycles(10);

      int16_t diff = v_i - v_e;
      msp_printf(0, "%u, %u, %i, %n\n\r", v_i, v_e, diff, decay_ms);

      decay_ms += step;

      __delay_cycles(128);
   }
}

void cap_calib_cover_duration(uint32_t start,
                              uint32_t step,
                              uint16_t end_adc,
                              bool debug)
{
   if (start == 0 || end_adc <= 0xF || step == 0)
   {
      msp_printf(0, "Invalid calibration settings\n\r");
   }
   msp_printf(0, "\n\r");

   uint32_t decay_ms = 0;
   uint16_t v_i, v_e;
   timers_init();

   cap_charge(true, false);
   cap_charge(true, false);
   cap_charge(true, false);

   // initial sampling
   cap_sample(&v_i, debug);
   msp_printf(0, "%u, 0, %n\n\r", v_i, decay_ms);

   // start_discharge(false);

   // initial sleep
   sleep_timer_ms(start);
   cap_sample(&v_e, debug);
   decay_ms += start;
   msp_printf(0, "%u, %u, %n\n\r", v_e, v_i - v_e, decay_ms);

   // rest of the duration
   while (v_e >= end_adc)
   {
      v_i = v_e;
      sleep_timer_ms(step);
      cap_sample(&v_e, debug);
      decay_ms += step;
      msp_printf(0, "%u, %u, %n\n\r", v_e, v_i - v_e, decay_ms);
   }
}

// uint8_t test_repeatability(uint32_t time_ms, uint8_t repeats, bool debug)
// {
//    init_sleep_timer();

//    uint32_t end = ((uint32_t)time_ms * 1000 / US_PER_TICK);

//    for (uint16_t i = 0; i < repeats; i++)
//    {
//       charge(debug);
//       charge(debug);
//       start_discharge(debug);

//       start_sleep_timer(end);

//       uint32_t time_meas = time(debug);

//       uint32_t time_meas_ms = (uint32_t)time_meas;
//       msp_printf("%n, %n\n\r", time_meas_ms, time_ms);
//       stop_discharge(debug);
//    }
// }

// uint8_t test_repeatability_range(uint32_t start, uint32_t end,
//                                  uint16_t step, uint8_t repeats, bool debug)
// {
//    msp_printf("\n\r");
//    if (start == 0)
//    {
//       msp_printf("Invalid start time, 0 is not acceptable");
//       return -1;
//    }

//    uint32_t time_ms = start;

//    init_sleep_timer();

//    // for some reason helps over charging
//    charge(debug);
//    charge(debug);
//    charge(debug);
//    msp_printf("CalcT,RealT\n\r");
//    while (time_ms <= end)
//    {
//       test_repeatability(time_ms, repeats, debug);
//       time_ms += step;
//    }
// }

//    msp_printf("Running calibration with charge/discharge: %u, %u\n\r", charge_time, discharge_time);
// }
