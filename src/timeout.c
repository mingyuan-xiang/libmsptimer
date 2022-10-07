// #include <msp430.h>
// #include <gpio.h>
// #include <mspprintf.h>
// #include <macro_basics.h>

// #include <cap_charge.h>
// #include <timeout.h>
// #include <timekeepers.h>

// void init_timeout()
// {
//    cap_charge_init();
//    gpio_dir_in_(TIMEOUT_INT);
//    gpio_pulldown_(TIMEOUT_INT);
//    gpio_intr_rising_(TIMEOUT_INT);
// }

// void timeout_sleep()
// {
//    STIC3(P, _TIMEOUT_INTPORT_, IFG) = ~STICH(BIT, _TIMEOUT_INTPIN_);
//    __enable_interrupt();
//    gpio_intr_en_(TIMEOUT_INT);
//    __bis_SR_register(LPM4_bits);
//    __disable_interrupt();
// }

// static void fizz(void *count)
// {
//    timeout_sleep();
// }

// void timeout_range(uint16_t *range, size_t len)
// {
//    timers_init();
//    for (size_t i = 0; i < len; i++)
//    {
//       cap_set_charge(range[i]);
//       // prevent hotness issues
//       __delay_cycles(2000);
//       cap_charge(true, false);
//       uint32_t t = stop_watch_ms(fizz, &i);

//       msp_printf(0, "%u, %n\n\r", range[i], t);
//    }
// }

// uint16_t timeout_min_charge_finder(uint16_t min, uint16_t max, uint16_t step)
// {
//    uint16_t min_count = 0;
//    uint16_t i = max;

//    // if step is bigger than min value, prevent overflow
//    if (step >= min)
//       min += step;

//    for (; i >= min; i -= step)
//    {

//       cap_set_charge(i);

//       // delay a bit in case of issues
//       __delay_cycles(2000);
//       cap_charge(true, true);

//       // if min_count is undecided, keep going
//       if (min_count == 0 && (gpio_read_(TIMEOUT_INT) == STICH(BIT, _TIMEOUT_INTPIN_)))
//          min_count = i;

//       // once min_count has settled, the pin should remain high for future iterations. If not, it was erroneous or other issues persist.
//       else if (min_count != 0 && gpio_read_(TIMEOUT_INT) != STICH(BIT, _TIMEOUT_INTPIN_))
//          // issue has occured, clear out min_count in case of one time issue
//          min_count = 0;

//       msp_printf(0, "gpio: %u -- %u\n\r", gpio_read_(TIMEOUT_INT) == STICH(BIT, _TIMEOUT_INTPIN_), min_count);
//    }

//    if (min_count == 0)
//       min_count = i;

//    return min_count + 1;
// }

// __attribute__((interrupt(STIC3(PORT, _TIMEOUT_INTPORT_, _VECTOR)))) void STIC3(PORT, _TIMEOUT_INTPORT_, _ISR)(void)
// {
//    switch (__even_in_range(STIC3(P, _TIMEOUT_INTPORT_, IV), 0x10))
//    {
//    case STICH(STIC3(P, _TIMEOUT_INTPORT_, IV__P), STIC3(_TIMEOUT_INTPORT_, IFG, _TIMEOUT_INTPIN_)):
//       // disable the interrupt
//       gpio_intr_dis_(TIMEOUT_INT);
//       LPM4_EXIT;
//       break;
//    default:
//       break;
//    }
// }