/*
 * Motor_Control.c
 *
 *  Created on: Sep 5, 2016
 *      Author: CNogales
 */

#include "Motor_Control.h"
#include "SPI_Polling.h"

void init_Hbridge(void) {
	// configure FIN and RIN on H-Bridge
	    P4SEL0 &= ~FIN;
	    P4SEL1 &= ~FIN;
	    P4DIR |= FIN;
	    P4OUT &= ~FIN;			// set FIN low to select open state of H-Bridg

	    P2SEL0 &= ~RIN;
	    P2SEL1 &= ~RIN;
	    P2DIR |= RIN;
	    P2OUT &= ~RIN;			// set RIN low to select open state on direction on H-Bridge
}

void setMotorAmplitude(uint16_t MotorVoltage){
	uint8_t dac = 5;					// selects the SYNC/SS pin DAC
	write_uint16_SPI(MotorVoltage, dac);		// drive motor at dc volage
}
void motorForward(void) {
	P4OUT |= FIN;			// put motor in forward direction
	P2OUT &= ~RIN;
}

void motorReverse(void) {
	P4OUT &= ~FIN;			// put motor in reverse direction
	P2OUT |= RIN;
}

void motorBrake(void) {
	P4OUT |= FIN;			// put motor in brake mode
	P2OUT |= RIN;
}

void motorOpen(void) {
	P4OUT &= ~FIN;			// put motor in open mode
	P2OUT &= ~RIN;
}
