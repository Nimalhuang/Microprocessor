#include <msp430g2553.h>
unsigned int counter = 0;
void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT - Watchdog Timer

  // Configure LED at P1.0 as Output
  P1DIR |= BIT0;                            // P1.0 output

  // Loops Forever
  while(1)
  {
	P1OUT ^= BIT0;     // Toggle P1.0
	// Add some delay so we can see the LED change
        __delay_cycles(200000);
		counter++;

  }
}
