/*
 * Collect_Data.c
 *
 *  Created on: Sep 29, 2016
 *      Author: CNogales
 */

#include <msp430.h>
#include <stdint.h>
#include "Collect_Data.h"
#include "Motor_Control.h"
#include "Freq_Capture.h"
#include "Circular_Buffer.h"
#include "FRAM_Write.h"


void collectData(uint16_t motorVal, uint8_t polarity)
{
	uint16_t zero = 0;							// 16 bit zero
	setMotorDirection(polarity);				// set motor to forward or reverse
	setMotorAmplitude(motorVal);				// drive the motor at DC voltage
	__delay_cycles(2000000);					// 1 second delay
	setMotorAmplitude(zero);					// stop motor
	motorOpen();								// bring motor to open state as recomended from datasheet
	__delay_cycles(20);							// let voltage levels settle
	motorBrake();								// break motor
	while(ADC12MEM0 > 100){}					// while the motor voltage is not zero
	ADC12IER0 &= ~ADC12IE0;                     // disable interupt on ADC12IFG0 bit
	ADC12IER0 &= ~ADC12IE1;                     // disable interupt on ADC12IFG1 bit
	offset = 0;									// set offset back to zero
	TB0R = 0x00;								// reset timer
	startTimer();								// begin collecting data
	__delay_cycles(800000);						// delay 1s to collect data
	stopTimer();								// stop timer


	return;
}

void processPulseData(void)
{
	// pull capture register data from the circular buffer
	//for (i = 0; i < 50; i++)
	  //	{
	  	//	timerData[i] = read_Buffer();

	//  	}

	//init_Buffer();									// reinitialize Circular Buffer

	// calculate period of output from rising edge incraments
	for (i = 0; i < 50; i++)
	{
		periodData[i] = (timerData[i] - timerData[i - 1]);
	}

	// calculate the change in period
	for (i = 0; i < 50; i++)
	{
		deltaPeriod[i] = (periodData[i] - periodData[i - 1]);
	}

	// filter out bad data
	for (i = 0; i < 10; i++)
	{
		if (deltaPeriod[i] < 500 && deltaPeriod[i] > -500)
		{
			filteredData[i] = periodData[i - 1];
		}

	}

	// store data in master array in FRAM
	for (i = 0; i < 10; i++)
	{
		if (filteredData[i] > 0x00000000)
		{
			cantData5[cantCounter5] = filteredData[i];
			cantCounter5++;
		}
	}







}

uint32_t findMedianCant5(void)
{
	//////////////////////////////////// Bubble Sort //////////////////////////////////////////////////////

	uint32_t medianVal;					// median value

	uint8_t swap = 0;					// variable to tell if swap needs to take place
	uint32_t temp = 0;					// variable to swap data

	do
	{
		swap = 0;
		uint8_t bubbleCount = 0;

		for (bubbleCount = 0; bubbleCount < (cantCounter5 - 1); bubbleCount++)
		{
			if (cantData5[bubbleCount] > cantData5[bubbleCount + 1])
			{
				temp = cantData5[bubbleCount];
				cantData5[bubbleCount] = cantData5[bubbleCount + 1];
				cantData5[bubbleCount + 1] = temp;
				swap = 1;
			}
		}
	} while (swap);

	medianVal = cantData5[(cantCounter5 / 2)];

	return medianVal;

}

void medianFilter(uint32_t median)
{
	for (i = 0; i < cantCounter5; i++)
	{
		// if the data point is not within + or - 10000 of median then through out the data point
		if (cantData5[i] > (median + 5000) && cantData5[i] < (median - 5000))
		{
			cantData5[i] = 0;					// get rid of value
		}
	}
}
uint32_t averageData(void)
{
	uint32_t sum = 0;							// sum of period values
	uint32_t average = 0;						// average of period values

	for (i = 0; i < cantCounter5; i++)
	{
		sum = (sum + cantData5[i]);
	}

	average = (sum / cantCounter5);

	return average;
}

void zeroOutArrays(void)
{
	// zero out timer array
	for (i = 0; i < 50; i++)
	{
		timerData[i] = 0;

	}

	// zero out period array
	for (i = 0; i < 50; i++)
	{
		periodData[i] = 0;
	}

	// zero out deltaPeriod array
	for (i = 0; i < 50; i++)
	{
		deltaPeriod[i] = 0;
	}

	// zero out filteredData array
	for (i = 0; i < 10; i++)
	{
		filteredData[i] = 0;
	}

}

void readFRAMCantileverData(void)
{
	// read cantData5 array
	for (i = 0; i < 100; i++)
	{
		dataArr[i] = cantData5[i];
	}
}

void zeroOutFRAM(void)
{
	uint16_t z;
	for (z = cantCounter5; z < cantCounter5; z++)
	{
		cantData5[z] = 0;
	}
}

	//return 0;

