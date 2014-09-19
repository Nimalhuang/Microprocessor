//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = 16MHz
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  J. Stevenson
//  Texas Instruments, Inc
//  July 2011
//  Built with Code Composer Studio v5
//***************************************************************************************

#include <msp430g2553.h>

void ClockConfig();
void P1Init();
void WDTStop();
void KeyInit();
void LedInit();

void KeyInit(){
	P1DIR &= ~BIT3;
	P1REN |= BIT3;
	P1OUT |= BIT3;
}

void ClockConfig(){
	//This loop tries to trap the CPU in case the flash memory has been erased. We do not want this garbage value to be entered into calibration of DCO. Hence the trap.
	if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)
	{
	  while(1);                                    // If cal constants erased, trap CPU!!
	  }

	//Check out the DCO range and clock setting
	BCSCTL1 = CALBC1_16MHZ;                        // Set range
	DCOCTL = CALDCO_16MHZ;                         // Set DCO step + modulation
	BCSCTL3 |= LFXT1S_2; //BCSCTL3 has a default condition of 5 (in decimal) which corresponds to certain settings. Now ORing with LFXT1S_2, will result in 0010 0101 (in binary) which leads to selection (refer user guide) of VLO, 6pF capacitor and other settings. This line configures the VLO as the source for LXTS1 and subsequently ACLK. For more details visit Chapter 5 of User Guide.
	IFG1 &= ~OFIFG;  //we are clearing this default flag. (IFG1: interrupt flag register 1) This step clears bit 1 (bit naming starts from 0). (OFIFG: oscillator fault interrupt flag register)
	BCSCTL2 |= SELM_0 + DIVM_0;     //This statement chooses (SELM_0: 0000 0000) DCO as the source for MCLK and SMCLK with internal resistor. The clock division is by 1.
}

void P1Init(){
	P1DIR = 0x00;
	P1OUT = 0x00;					// P1 All Reset
}

void LedInit(){
	P1DIR |= BIT0 + BIT6;			// Set P1.0 & P1.6 to output direction
	P1OUT |= BIT0 + BIT6;		// Toggle P1.0 P1.6 using exclusive-OR
}

void WDTStop(){
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
}

void main() {
	WDTStop();
	ClockConfig();					// Config System as 16MHz
	P1Init();
	LedInit();
	KeyInit();

	for(;;) {

		if(!(P1IN & BIT3)){
			_delay_cycles(3200000);		// Delay 400ms
			P1OUT ^= BIT0 + BIT6;		// Toggle P1.0 P1.6 using exclusive-OR
		}
		else{
			P1OUT = P1OUT;
		}
	}
	
}
