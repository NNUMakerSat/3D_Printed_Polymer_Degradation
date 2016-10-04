/*
 * Motor_Control.c
 *
 *  Created on: Sep 5, 2016
 *      Author: CNogales
 */

#include "Motor_Control.h"
#include "SPI_Polling.h"
#include <stdint.h>
#include "Freq_Capture.h"

void init_Hbridge()
{
	// configure FIN and RIN on H-Bridge
	    P4SEL0 &= ~FIN;
	    P4SEL1 &= ~FIN;
	    P4DIR |= FIN;
	    P4OUT &= ~FIN;						// set FIN low to select open state of H-Bridg

	    P2SEL0 &= ~RIN;
	    P2SEL1 &= ~RIN;
	    P2DIR |= RIN;
	    P2OUT &= ~RIN;						// set RIN low to select open state on direction on H-Bridge

	    // put dac in known state and set vref to zero
	    uint16_t zero = 0;
	    write_uint16_SPI(zero, DAC);
	    motorOpen();							// set motor in open direction
	    __delay_cycles(100);
}

void setMotorDirection(uint8_t direction)
{
	 if (direction == FORWARD)	  				 // if direction is foward
	 {
		 motorForward();
	 }
	 else if (direction == REVERSE)			// if direction is reverse
	 {
		 motorReverse();
	 }
	 else								// else set motor foward
		 motorForward();
}

void setMotorAmplitude(uint16_t MotorVoltage)
{
	write_uint16_SPI(MotorVoltage, DAC);		// drive motor at dc volage
}
void motorForward(void)
{
	P4OUT |= FIN;			// put motor in forward direction
	P2OUT &= ~RIN;
}

void motorReverse(void)
{
	P4OUT &= ~FIN;			// put motor in reverse direction
	P2OUT |= RIN;
}

void motorBrake(void)
{
	P4OUT |= FIN;			// put motor in brake mode
	P2OUT |= RIN;
}

void motorOpen(void)
{
	P4OUT &= ~FIN;			// put motor in open mode
	P2OUT &= ~RIN;
	__delay_cycles(20);		// allow voltage to settle
}

void motorRamp(uint16_t maxValue, uint16_t minValue)
{
    uint16_t dacValue;
    for (dacValue = maxValue; dacValue > minValue; dacValue--)
    {
    	setMotorAmplitude(dacValue);			// set motor to its voltage
    	__delay_cycles(10000);
    }
}
void motorSquare(uint16_t peakVoltage)
{
	uint16_t zero = 0;								// 16 bit zero
	motorForward();
	setMotorAmplitude(peakVoltage);				// drive the motor at DC voltage
	__delay_cycles(2000000);						// 1 second delay
	setMotorAmplitude(zero);					// stop motor
	motorOpen();									// bring motor to open state as recomended from datasheet
	__delay_cycles(20);							// let voltage levels settle
	motorBrake();
	__delay_cycles(2000000);
	// break motor
	//while(ADC12MEM0 > 100){}
	//ADC12IER0 &= ~ADC12IE0;                     // disable interupt on ADC12IFG0 bit
	//ADC12IER0 &= ~ADC12IE1;                     // disable interupt on ADC12IFG1 bit
	//startTimer();
}





