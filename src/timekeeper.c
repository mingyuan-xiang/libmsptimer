#include <msp430.h>
#include <clock.h>
#include <timer.h>
#include <gpio.h>

#include <stdint.h>
#include <stdbool.h>

#include <timekeepers.h>

static uint32_t stop_watch_tick = 0;
static uint32_t sleep_timer_tick = 0;
static uint16_t sleep_timer_last = 0;

static uint16_t old_csctl2 = 0;
void timers_init()
{
   lfxt_start(old_csctl2);
}

void timers_stop()
{
   lfxt_stop(old_csctl2);
}

// Measures how long a function takes.
static inline uint32_t _stop_watch_(stop_watch_fn fn, void *arg, bool aclk)
{
   stop_watch_tick = 0;

   if (aclk)
   {
      // Timer sourced by AUX and interrupt setup
      timer_setup_cont(STOPWATCH_TIMER, ACLK, 1, 1);
   }
   else
   {
      timer_setup_cont(STOPWATCH_TIMER, SMCLK, 1, 1);
   }
   __enable_interrupt();

   // start the timer on continuous mode & run
   timer_start_cont(STOPWATCH_TIMER);

   fn(arg);

   // disable the timer
   timer_halt(STOPWATCH_TIMER);

   // read the last count
   // this value is in lfxt clock cycles
   uint32_t time;

   time = (stop_watch_tick << 16) | TA1R;

   timer_halt(STOPWATCH_TIMER);
   timer_reset(STOPWATCH_TIMER);
   timer_IFG_disable(STOPWATCH_TIMER);

   return time;
}

uint32_t stop_watch_cycle(stop_watch_fn fn, void *arg, bool aclk)
{
   return _stop_watch_(fn, arg, aclk);
}

uint32_t stop_watch_ms(stop_watch_fn fn, void *arg)
{
   uint32_t t = _stop_watch_(fn, arg, true);

   // turn into ms
   t *= 1000;
   t = t >> 15;
   return t;
}

static inline void _sleep_timer_(bool aclk)
{
   if (sleep_timer_last == 0 && sleep_timer_tick == 0)
      return;

   // Timer sourced by ACLK and interrupt setup
   __enable_interrupt();

   if (sleep_timer_tick == 0)
   {
      timer_setup_up(SLEEP_TIMER, ACLK, 1, 1, sleep_timer_last);
      timer_start_up(SLEEP_TIMER);
   }
   else
   {
      timer_setup_cont(SLEEP_TIMER, ACLK, 1, 1);
      timer_start_cont(SLEEP_TIMER);
   }

   LPM1;

   _disable_interrupts();
   timer_halt(STOPWATCH_TIMER);
   timer_reset(STOPWATCH_TIMER);
   timer_CCIE_disable(STOPWATCH_TIMER);

   sleep_timer_last = 0;
   sleep_timer_tick = 0;
}

void sleep_timer_cycle(uint32_t end, bool aclk)
{
   sleep_timer_tick = end >> 16;
   sleep_timer_last = end & 0xFFFF;
   _sleep_timer_(aclk);
}

void sleep_timer_ms(uint32_t end)
{
   // Turn end from ms into a lfxt clock cycles

   // each 2000 ms, the clock incurr one tick, soo..
   sleep_timer_tick = end / 2000;

   // // the rest should get translated into clock cycles
   end %= 2000;
   end = end << 15;
   end /= 1000;

   sleep_timer_last = end;
   _sleep_timer_(true);
}
void measure_freq_helper(void * arg) {
   __delay_cycles(1002900);
}

/* Measures how many cycles a clock approximately takes in 1 second.
 * c goes as follows,
 *    1: DCO
 *    2: MODCLK
 *    3: LFMODCLK
 *    4: LFXTCLK (calibrated based on this)
 *    5: VLOCLK 
 *    6: HFXTCLK
 */
uint32_t measure_freq(uint8_t c) {
   uint16_t new, old;
   uint32_t x = 0;
   switch(c) {
      case 1:
         clock_src_update(DCOCLK, DCOCLK, VLOCLK, old);
         x = stop_watch_cycle(measure_freq_helper, 0, false);
         clock_src_update_all(old, new);
         break;
      case 2:
         clock_src_update(DCOCLK, MODCLK, VLOCLK, old);
         x = stop_watch_cycle(measure_freq_helper, 0, false);
         clock_src_update_all(old, new);
         break;
      case 3:
         clock_src_update(DCOCLK, MODCLK, LFMODCLK, old);
         x = stop_watch_cycle(measure_freq_helper, 0, true);
         clock_src_update_all(old, new);
         break;
      case 4:
         timers_init();
         x = stop_watch_cycle(measure_freq_helper, 0, true);
         timers_stop();
         break;
      case 5:
         clock_src_update(DCOCLK, MODCLK, VLOCLK, old);
         x = stop_watch_cycle(measure_freq_helper, 0, true);
         clock_src_update_all(old, new);
         break;
      default:
         break;
   }
   return x;
}

void __attribute__((interrupt(STIC3(TIMER, STOPWATCH_TIMER, _A1_VECTOR)))) STIC3(TIMER, STOPWATCH_TIMER, _A1_ISR)(void)
{
   switch (__even_in_range(STIC3(TA, STOPWATCH_TIMER, IV), TAIV__TAIFG))
   {
   case TAIV__NONE:
      break; // No interrupt
   case TAIV__TACCR1:
      break; // CCR1 not used
   case TAIV__TACCR2:
      break; // CCR2 not used
   case TAIV__TACCR3:
      break; // reserved
   case TAIV__TACCR4:
      break; // reserved
   case TAIV__TACCR5:
      break; // reserved
   case TAIV__TACCR6:
      break;         // reserved
   case TAIV__TAIFG: // overflow
      stop_watch_tick++;
      break;
   default:
      break;
   }
}

void __attribute__((interrupt(STIC3(TIMER, SLEEP_TIMER, _A1_VECTOR)))) STIC3(TIMER, SLEEP_TIMER, _A1_ISR)(void)
{
   switch (__even_in_range(STIC3(TA, SLEEP_TIMER, IV), TAIV__TAIFG))
   {
   case TAIV__NONE:
      break; // No interrupt
   case TAIV__TACCR1:
      break; // CCR1 not used
   case TAIV__TACCR2:
      break; // CCR2 not used
   case TAIV__TACCR3:
      break; // reserved
   case TAIV__TACCR4:
      break; // reserved
   case TAIV__TACCR5:
      break; // reserved
   case TAIV__TACCR6:
      break;         // reserved
   case TAIV__TAIFG: // overflow
      // check to see if we need to switch from cont mode to up mode
      if (--sleep_timer_tick == 0)
      {
         // time's up
         if (sleep_timer_last == 0)
            LPM1_EXIT;
         else
         {
            timer_IFG_disable(SLEEP_TIMER);
            timer_setup_up(SLEEP_TIMER, ACLK, 1, 1, sleep_timer_last);
            timer_start_up(SLEEP_TIMER);
         }
      }
      break;
   default:
      break;
   }
}

void __attribute__((interrupt(STIC3(TIMER, SLEEP_TIMER, _A0_VECTOR)))) STIC3(TIMER, SLEEP_TIMER, _A0_ISR)(void)
{
   LPM1_EXIT;
}