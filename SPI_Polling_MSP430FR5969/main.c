#include <msp430.h>
#include "SPI_Polling.h"
#include "Initialize.h"
#include "Motor_Control.h"


/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    PM5CTL0 &= ~LOCKLPM5;		// unlock I/O's

   // initialize_Ports();						// Init all non used ports
    initialize_Clocks();				// Sets up timers (takes care of FRAM issue)

    init_SPI (4);

    init_Hbridge();

    uint16_t motorAmp;
    uint16_t motorMax = 2100;
    uint16_t motorMin = 500;


    // ramp motor
    while(1)
    {
    	motorForward();
    for(motorAmp = motorMax; motorAmp > motorMin; motorAmp--)
    {
    	setMotorAmplitude(motorAmp);
    	__delay_cycles(10000);
    }
    }

   /*
	setMotorAmplitude(motorAmp);
 	uint16_t motorAmp = 1500;
	// put a square wave on motor
	while(1)
	{
		motorOpen();
		__delay_cycles(10);
		motorForward();
		__delay_cycles(200000);
		motorOpen();
		__delay_cycles(10);
		motorBrake();
		__delay_cycles(200000);
	}

*/


/*
	while(1)
	{
		write_uint16_SPI(DAC_DATA, dac);
		__delay_cycles(20000);
	}



	while(1)
	{
		for(DAC_DATA = DAC_MAX; DAC_DATA > DAC_MIN; DAC_DATA--)
		{
			write_uint16_SPI (DAC_DATA, dac);
			__delay_cycles(20000);
		}
	}

	*/
	return 0;
}
