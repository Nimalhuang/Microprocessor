#include <msp430g2553.h>

#define uchar unsigned char
#define uint unsigned int

#define CPU_F ((double)16000000)//cpu frequency 16MHz
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define nop() _NOP();

void ClockConfig() {
	//This loop tries to trap the CPU in case the flash memory has been erased. We do not want this garbage value to be entered into calibration of DCO. Hence the trap.
	if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF) {
		while (1)
			;                             // If cal constants erased, trap CPU!!
	}
	//Check out the DCO range and clock setting
	BCSCTL1 = CALBC1_16MHZ;                        // Set range
	DCOCTL = CALDCO_16MHZ;                         // Set DCO step + modulation
	BCSCTL3 |= LFXT1S_2; //BCSCTL3 has a default condition of 5 (in decimal) which corresponds to certain settings. Now ORing with LFXT1S_2, will result in 0010 0101 (in binary) which leads to selection (refer user guide) of VLO, 6pF capacitor and other settings. This line configures the VLO as the source for LXTS1 and subsequently ACLK. For more details visit Chapter 5 of User Guide.
	IFG1 &= ~OFIFG; //we are clearing this default flag. (IFG1: interrupt flag register 1) This step clears bit 1 (bit naming starts from 0). (OFIFG: oscillator fault interrupt flag register)
	BCSCTL2 |= SELM_0 + DIVM_0; //This statement chooses (SELM_0: 0000 0000) DCO as the source for MCLK and SMCLK with internal resistor. The clock division is by 1.
}

void WDTStop() {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
}

void Port_Init() {

}

void main() {
	WDTStop();
	ClockConfig();					// Config System as 16MHz
}
