#include <msp430g2553.h>
#include "LCD1602.h"

#define CPU_F ((double)16000000)//cpu frequency 16MHz
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define nop() _NOP();

#define RS_1 P2OUT |= RS
#define RS_0 P2OUT &= ~RS
#define EN_1 P2OUT |= EN
#define EN_0 P2OUT &= ~EN

void ClockConfig();
void WDTStop();
void PortInit();
void LCD_init();
void write_cmd(unsigned char cmd);
void write_data(unsigned char Data);
void LCD1602_Char(unsigned char row,unsigned char column,char ch);
void LCD1602_String(unsigned char row,unsigned char column,unsigned char *p);


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


void PortInit(){
	P2DIR = 0xff;
	P1DIR = 0xff;
	P2OUT = 0x00;
	P1OUT = 0x00;
}

void write_cmd(unsigned char cmd)  //write command
{
	RS_0;

//	P2OUT=0x00;
//	P2OUT |= ((cmd>>2)&data_bus);
//	EN_1;
//	delay_us(500);
//	EN_0;
//
//	P2OUT=0x00;
//	P2OUT |= ((cmd<<2)&data_bus);
//	EN_1;
//	delay_us(500);
//	EN_0;
	P1OUT=0x00;
	P1OUT |=cmd;
	EN_1;
	delay_us(500);
	EN_0;

}

void write_data(unsigned char Data)	 //write data
{
	RS_1;

//	P2OUT=0x00;
//	P2OUT |= ((Data>>2)&data_bus);
//	EN_1;
//	delay_us(500);
//	EN_0;
//
//	P2OUT=0x00;
//	P2OUT |= ((Data<<2)&data_bus);
//	EN_1;
//	delay_us(500);
//	EN_0;
	P1OUT=0x00;
	P1OUT |=Data;
	EN_1;
	delay_us(500);
	EN_0;
}

void LCD1602_Char(unsigned char row,unsigned char column,char ch) //write char
{
	unsigned char pos;
	if(row == 1) pos = 0x80;
	if(row == 2) pos = 0xc0;
	pos = pos + column - 1;
	write_cmd(pos);
	write_data(ch);
}

void LCD1602_String(unsigned char row,unsigned char column,unsigned char *p) //write string
{
	unsigned char pos;
	if(row == 1) pos = 0x80;
	if(row == 2) pos = 0xc0;
	pos = pos + column - 1;
	write_cmd(pos);
	while(1)
	{
		if(*p == '\0') break;
		write_data(*p);
		p++;
	}
}

void LCD_init()	   //LCD1602 init
{
	write_cmd(0x38);//set display mod: 8-line/5x8 font
	delay_us(500);
	write_cmd(0x0f);//enable display, enable cursor, cursor blink
	delay_us(500);
	write_cmd(0x06);//cursor right shift, disable whole screen shift
	delay_us(500);
	write_cmd(0x01);//clear screen
	delay_us(500);
	write_cmd(0x80);//init ram address: row 1
	delay_us(500);
}

void WDTStop() {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
}

void main() {
	WDTStop();
	ClockConfig();					// Config System as 16MHz
	PortInit();
	LCD_init();
	LCD1602_String(1, 1, "key pull test:");
	LCD1602_String(2, 1, "key num:");
	LCD1602_String(2, 9, "01");

}
