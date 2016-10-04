/*
 * Freq_Capture.c
 *
 *  Created on: Sep 19, 2016
 *      Author: CNogales
 */

#include <msp430.h>
#include <stdint.h>
#include "Freq_Capture.h"
#include "Circular_Buffer.h"
#include "Collect_Data.h"

#define Motor_ADC_1 0x08				// motor_ADC_1 P1.3
#define Motor_ADC_2 0x01				// motor_ADC_2 P3.0
void initCaptureTimer(void)
{
	TB0R = 0X0000;											   	// set timer value to 0
	TB0CTL |= TBSSEL_2 | ID_0 | MC_0 | TBIE;					// SMCLK | /1 | stop timer | enable inturrupt on timer
}

void startTimer(void)
{
	TB0CTL |= MC_2;											// start timer
}

void stopTimer(void)
{
	TB0CTL &= ~MC_2;											// stop timer
}
void initCaptureRegisters(void)
{
	// configure P1.4, CCI1A
	TB0CCTL1 |= CM_1 | CCIS_0 | CAP | CCIE | SCS; 			// capture on rising edge | CCI0A | Capture mode | enable capture/compare interrrupt | synchronus mode
	TB0CCTL1 &= ~CCIFG;										// clear inturrput bit

	// configure P1.5, CCI2A
	TB0CCTL2 |= CM_1 | CCIS_0 | CAP | CCIE | SCS; 			// capture on rising edge | CCI0A | Capture mode | enable capture/compare interrrupt | synchronus mode
	TB0CCTL2 &= ~CCIFG;										// clear inturrput bit

	// configure P3.4, CCI3A
	TB0CCTL3 |= CM_1 | CCIS_0 | CAP | CCIE | SCS; 			// capture on rising edge | CCI0A | Capture mode | enable capture/compare interrrupt | synchronus mode
	TB0CCTL3 &= ~CCIFG;										// clear inturrput bit

	// configure P3.5, CCI4A
	TB0CCTL4 |= CM_1 | CCIS_0 | CAP | CCIE | SCS; 			// capture on rising edge | CCI0A | Capture mode | enable capture/compare interrrupt | synchronus mode
	TB0CCTL4 &= ~CCIFG;										// clear inturrput bit

	// configure P3.6, CCI5A
	TB0CCTL5 |= CM_1 | CCIS_0 | CAP | CCIE | SCS; 			// capture on rising edge | CCI0A | Capture mode | enable capture/compare interrrupt | synchronus mode
	TB0CCTL5 &= ~CCIFG;										// clear inturrput bit
}

void initCapturePins(void)
{
	// select tertiary imput mode P1.4
	P1SEL0 |= CCI1A;
	P1SEL1 &= ~CCI1A;
	P1OUT &= ~CCI1A;

	// select tertiary imput mode P1.5
	P1SEL0 |= CCI2A;
	P1SEL1 &= ~CCI2A;
	P1OUT &= ~CCI2A;

	// select tertiary imput mode P3.4
	P3SEL0 |= CCI3A;
	P3SEL1 &= ~CCI3A;
	P3OUT &= ~CCI3A;

	// select tertiary imput mode P3.5
	P3SEL0 |= CCI4A;
	P3SEL1 &= ~CCI4A;
	P3OUT &= ~CCI4A;

	// select tertiary imput mode P3.6
	P3SEL0 |= CCI5A;
	P3SEL1 &= ~CCI5A;
	P3OUT &= ~CCI5A;
}

void initialize_ADC12(void)
{
	ADC12CTL0 &= ~ADC12ENC;                                    // disable the ADC enable bit

	P1DIR &= ~Motor_ADC_1;									// set motor_ADC_1 to input mode
	P3DIR &= ~Motor_ADC_2;									// set motor_ADC_2 to input mode

	// set Motor_ADC_1 to tertiary module function
	P1SEL0 |= Motor_ADC_1;
	P1SEL1 |= Motor_ADC_1;

	// set Motor_ADC_2 to tertiary module function
	P3SEL0 |= Motor_ADC_2;
	P3SEL1 |= Motor_ADC_2;

	ADC12CTL0 |= ADC12SHT0_10 | ADC12ON;                        // 512 clock cycles per sample | turn ADC on

    ADC12CTL1 |= ADC12SSEL_3 | ADC12PDIV_3 | ADC12DIV_7 | ADC12SHP | ADC12CONSEQ_1;  // select SMCLK as clock source | divide by zero | signal sourced from timer | single conversion multiple channels

    ADC12CTL2 |= ADC12RES_2;                                    // set ADC resolution to 12 bit
    ADC12CTL2 &= ~ADC12DF;                                      // set conversion results to be unsigned binary

    ADC12CTL3 = ADC12CSTARTADD_0;             // value is in memory register 0 | single conversion single channel

    // P1.3 ADC Setup
    ADC12MCTL0 |= ADC12INCH_3 | ADC12VRSEL_0;     			   // select input channel to be 9 | select VCC and VSS as V+ and V- | end of series
    ADC12MCTL0 &= ~ADC12EOS;

    // P3.0 ADC Setup
    ADC12MCTL1 |= ADC12INCH_12 | ADC12VRSEL_0;     			   // select input channel to be 9 | select VCC and VSS as V+ and V- | end of series
    ADC12MCTL1 |= ADC12EOS;

    ADC12IER0 |= ADC12IE0;                                      // enable interupt on ADC12IFG0 bit
    ADC12IER0 |= ADC12IE1;                                      // enable interupt on ADC12IFG1 bit

    ADC12CTL0 |= ADC12ENC;                                      // enable conversion
   // ADC12IFGR0 &= ~ADC12IFG0;                                   // clear inturrput flag
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void TB0_ISR(void) {
	switch (__even_in_range(TB0IV, TB0IV_TBIFG))
	{
	case TB0IV_NONE:
		break;               // No interrupt
	case TB0IV_TBCCR1:
		break;
	case TB0IV_TBCCR2:
		break;               // CCR2 not used
	case TB0IV_TBCCR3:
		break;               // CCR3 not used
	case TB0IV_TBCCR4:
		break;               // CCR4 not used
	case TB0IV_TBCCR5:
	/*
		if (periodCount == 2)
		{
			edge1 = (TB0CCR5 + offset);
		}

		if (periodCount == 3)
		{
			edge2 = (TB0CCR5 + offset);
		}

		periodCount += 1;
	*/
		timerData[i] = (TB0CCR5 + offset);
		i++;
		//offset = 0;									// set offset back to zero
		//TB0R = 0x00;								// reset timer

		break;               // CCR5 not used
	case TB0IV_TBCCR6:
		break;               // CCR6 not used
	case TB0IV_TBIFG:                       // overflow
		offset += 0x10000;
		break;
	default:
		break;
	}
}
