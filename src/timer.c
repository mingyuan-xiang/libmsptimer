#include <timer.h>
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <mspprintf.h>

uint32_t time_h = 0;

// Measures how long a function takes.
uint32_t measure_fn_us(timer_fn fn, void* arg, int freq) {
   time_h = 0;
   
   uint16_t old_CTL1 = CSCTL1;
   uint16_t old_CTL2 = CSCTL2;
   uint16_t old_CTL3 = CSCTL3;
   
   /*
   // set the timer clock speed in accordance with the DCO speed
   // Goal is to match it with 1MHz-ish
   uint16_t idiv = ID__1;
   uint16_t eidiv = TAIDEX__1;
   float multiplier = MULT100;

   // if set, they will be equal. If not, they won't be.
   bool dcorsel = (CSCTL1 & DCORSEL) == DCORSEL;


   switch(CSCTL1 & 0xE) {
      case DCOFSEL_0:   // 1 MHz either way
         break;
      case DCOFSEL_1:   // 2.67 MHz or 5.33 MHz
         if (dcorsel) {                  // 1.33 MHz
            eidiv = TAIDEX__2;
            multiplier = MULT133;
         }
         else {                          // 1.07 MHz
            eidiv = TAIDEX__5;
            multiplier = MULT107;
         }
         break;
      case DCOFSEL_2:  // 3.5 MHz or 7 MHz
         if (dcorsel) {                 // 1.16 MHz
            eidiv = TAIDEX__3; 
            multiplier = MULT116;
         }
         else eidiv = TAIDEX__7;        // 1.00 MHz
         break;
      case DCOFSEL_3:  // 4 MHz or 8 MHz
         if (dcorsel) eidiv = TAIDEX__4;// 1.00 MHz
         else eidiv = TAIDEX__8;        // 1.00 MHz
         break;
      case DCOFSEL_4:  // 5.33 MHz or 16 MHz
         if (dcorsel) {                 // 1.07 MHz
            eidiv = TAIDEX__5;
            multiplier = MULT107;
         }
         else {                         // 1.00 MHz
            eidiv = TAIDEX__8;        
            idiv = ID__2;
         }
         break;
      case DCOFSEL_5:  // 7 MHz or 21 MHz
         if (dcorsel) eidiv = TAIDEX__7;// 1.00  MHz
         else {                         // 0.875 MHz
            eidiv = TAIDEX__3;        
            idiv = ID__8;
            multiplier = MULT0875;
         }
         break;
      case DCOFSEL_6:  // 8 MHz or 24 MHz
      case DCOFSEL_7:  // 8 MHz or 24 MHz
         if (dcorsel) eidiv = TAIDEX__8;// 1.00 MHz
         else {                         // 1.00 MHz
            eidiv = TAIDEX__3;        
            idiv = ID__8;
         }
         break;
      default:                         // Should not Happen
         break;
   }
   */

   // Set up MCLK AND SMCLK
   CSCTL0_H = CSKEY_H;
   switch(freq) {
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
   switch(freq) {
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


// __attribute__((interrupt(TIMER0_A0_VECTOR))) void TIMER0_A0_ISR2(void) {
//   switch(__even_in_range(TA0IV, TAIV__TAIFG)) {
//      case TAIV__TAIFG: 
//         time_h++;
//       default:
//          break;
//   }
// }
