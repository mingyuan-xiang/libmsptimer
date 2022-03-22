#include <timer.h>
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t time_h = 0;

// Measures how long a function takes.
uint32_t measure_fn_us(timer_fn fn, void *arg, int freq)
{
   time_h = 0;

   uint16_t old_CTL1 = CSCTL1;
   uint16_t old_CTL2 = CSCTL2;
   uint16_t old_CTL3 = CSCTL3;

   // Set up MCLK AND SMCLK
   CSCTL0_H = CSKEY_H;
   switch (freq)
   {
   case 1:
      CSCTL1 = DCOFSEL_0;
      break;
   case 4:
      CSCTL1 = DCOFSEL_3;
      break;
   case 8:
      CSCTL1 = DCOFSEL_6;
      break;
   case 16:
      CSCTL1 = DCORSEL | DCOFSEL_4;
   default:
      // should not happen
      return -1;
   }
   CSCTL2 &= ~(SELS | SELM);
   CSCTL2 |= SELS__DCOCLK | SELM__DCOCLK;
   CSCTL3 &= ~(DIVS | DIVM);
   CSCTL3 |= DIVS__1 | DIVM__1;
   CSCTL0_H = 0;

   // Setup the Timer

   // sourced by small clock and generate overflow interrupt
   TA0CTL = TASSEL__SMCLK | TAIE;
   switch (freq)
   {
   case 1:
      break;
   case 4:
      TA0CTL |= ID__4;
      break;
   case 8:
      TA0CTL |= ID__8;
      break;
   case 16:
      TA0CTL |= ID__8;
      TA0EX0 = TAIDEX__2;
   default:
      // should not happen
      return -1;
   }

   __enable_interrupt();

   // start the timer on continuous mode & run
   TA0CTL |= MC__CONTINUOUS | TACLR;

   fn(arg);

   // disable the timer
   TA0CTL &= ~(MC);

   // read the last count
   uint32_t time = TA0R;

   // combine
   time |= (time_h << 16);

   // Set up MCLK AND SMCLK
   CSCTL0_H = CSKEY_H;
   CSCTL1 = old_CTL1;
   CSCTL2 = old_CTL2;
   CSCTL3 = old_CTL3;
   CSCTL0_H = 0;

   return time;
}

// __attribute__((interrupt(TIMER0_A0_VECTOR))) void TIMER0_A0_ISR2(void)
// {
//    switch (__even_in_range(TA0IV, TAIV__TAIFG))
//    {
//    case TAIV__TAIFG:
//       time_h++;
//    default:
//       break;
//    }
// }
