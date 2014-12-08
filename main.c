
//-----------------------------------------------------------------
// Name:	Coulston
// File:	lab7.c
// Date:	Fall 2014
// Purp:	Collect analog samples from P1.3 and P1.4
//-----------------------------------------------------------------
#include "msp430g2553.h"
#include "start5.h"

int16 rightValue;
int16 leftValue;
int16 centerValue;

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void main(void) {

	initMSP430();

	while (1) {



		if (centerValue > 500) {
			P1OUT |= BIT6;
		}
		else{
			P1OUT &= ~BIT6;
		}
		if (rightValue > 500) {
				P1OUT |= BIT0;
			}
		if (leftValue > 500) {
						P1OUT &= ~BIT0;
					}



		// Configure P1.4 to be the ADC input

	} // end infinite loop

} // end main

void initMSP430() {

	IFG1 = 0; 					// clear interrupt flag1
	WDTCTL = WDTPW + WDTHOLD; 		// stop WD
	BCSCTL1 = CALBC1_8MHZ;										// 8MHz clock
	DCOCTL = CALDCO_8MHZ;

	P1DIR = BIT0 |BIT6;									// Set the red LED as output

	ADC10CTL0 = 0;
	ADC10CTL1 =  ADC10DIV_3;				// Channel 4, ADC10CLK/4
	ADC10AE0 = BIT3 |BIT4 | BIT5;		 			// Make P1.4 analog input
	ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;// Vcc & Vss as reference
//	ADC10CTL0 |= ADC10SC;

	TA0CCR0 = 0xFFFF;						// create a 16mS roll-over period
	TACTL &= ~TAIFG;	// clear flag before enabling interrupts = good practice
	TACTL = ID_3 | TASSEL_2 | MC_1 | TAIE;// Use 1:8 presclar off MCLK and enable interrupts
	_enable_interrupt();

} // end initMSP430

// -----------------------------------------------------------------------
// In order to produce a blink 1 second long, we need to accumulate some
// number of interrupts.  At 8Mhz, the slowest rollOver rate that we can
// generate is 15.258Hz with TA0CCR0=2^16-1.  If we just accumulated 15
// rollOvers, we would have a pretty bad error.  So, Lets back off on
// TA0CCR0 a little and accumulate 16 rollOvers to make a very precise
// 1 second pulse on the LED.
//
//	8 clks   1 cnt     10^6 uS      1 blink
//	------ * ------ * --------- * ------------- = 62500 cnts/rollOver
//	  1uS    8 clk     1 blink     16 rollOvers
//
// -----------------------------------------------------------------------
#pragma vector = TIMER0_A1_VECTOR				// This is from the MSP430G2553.h file
__interrupt void timerOverflow(void) {
	static int8 currentSample = LEFT;



	if (currentSample == LEFT) {
		selectInch(currentSample);

		rightValue = ADC10MEM;						// collect that 10-bit value

		ADC10CTL0 |= ADC10SC;				// Start a conversion
		currentSample = CENTER;
	} else if (currentSample == CENTER) {
		selectInch(currentSample);

		leftValue = ADC10MEM;

		ADC10CTL0 |= ADC10SC;							// Start a conversion
		currentSample = RIGHT;
	} else {
		selectInch(currentSample);

		centerValue = ADC10MEM;

		ADC10CTL0 |= ADC10SC;							// Start a conversion
		currentSample = LEFT;
	}

	TACTL &= ~TAIFG;
}



void selectInch(int8 num){
	if(num==LEFT){
		ADC10CTL0 &=  ~ENC;
		ADC10CTL1 =  INCH_3;
		ADC10CTL0 |=  ENC;
	}
	else if(num==CENTER){
		ADC10CTL0 &=  ~ENC;
			ADC10CTL1 =  INCH_4;
			ADC10CTL0 |=  ENC;
	}
	else{
		ADC10CTL0 &=  ~ENC;
			ADC10CTL1 =  INCH_5;
			ADC10CTL0 |=  ENC;

	}
}
