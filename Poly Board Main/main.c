#include <msp430.h> 
#include <stdint.h>
#include "Motor_Control.h"
#include "Initialize.h"
#include "SPI_Polling.h"
#include "Freq_Capture.h"
#include "Circular_Buffer.h"
#include <stdbool.h>
#include "FRAM_Write.h"
#include "Collect_Data.h"

uint16_t zero_16 = 0;
/*
 * main.c
 */

// function prototypes
void ExecuteStateMachine(void);

uint8_t FINISHED = 0;
uint16_t maxMotorVal = 2600;
uint32_t cantResonantPeriod5;					// averaged resonant period

enum PolySci_States {
	Start,
	Initialize,
	Collect_Data,
	Process_Data,
	Send_Data_To_Hub,
} PolySci_State;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;						// Stop watchdog timer
	
    PM5CTL0 &= ~LOCKLPM5;							// unlock I/O's-

    PolySci_State = Start;							// begin state machine in start mode

    cantCounter5 = 0;


    while ((!FINISHED == 1))
    {
    	ExecuteStateMachine();						// run the state machine
    }

   // while (1)
    //{
    //motorSquare(maxMotorVal);
   // }

	return 0;
}

void ExecuteStateMachine(void)
{
  	switch (PolySci_State)
  	{
  	case Start:
  		PolySci_State = Initialize;
  		break;

  	case Initialize:
  		initialize__Clocks_MPS430FR5969();				// initialize clock to 8MHz
  		init_SPI(HUB);									// initialize SPI settings to hub communiate with hub
  		//write_uint8_SPI(0xCC, HUB);						// write handshake byte to hub

  		init_SPI (DAC);									// initialize DAC SPI



  		initialize_ADC12();								// initialize ADC

  		init_Buffer();									// initialize Circular Buffer

  		// initialize capture registers
  		initCaptureRegisters();
  		initCapturePins();
  		initCaptureTimer();

  		//initialize motor
  		init_Hbridge();									// initialize H-Bridge pins
  		setMotorDirection(FORWARD);

  		PolySci_State = Collect_Data;
  		break;

  	case Collect_Data:
  		__enable_interrupt();							// enable interrupts
  		uint16_t count = 0;

  	// collect data from 100 pulses from the motor
  		for (count = 0; count < 30; count++)
  		{
  			i = 0;
  			ADC12IER0 |= ADC12IE0;                     // enable interupt on ADC12IFG0 bit
  			ADC12IER0 |= ADC12IE1;                     // enable interupt on ADC12IFG1 bit
  			ADC12CTL0 |= ADC12SC;               		 // begin sampling ADC on motor output lines
  			//periodCount = 0;
  			collectData(maxMotorVal, FORWARD);
  			//periodRes = (edge2 - edge1);
  			//cantData5[cantCounter5] = periodRes;
  			//cantCounter5++;
  			processPulseData();
  			zeroOutArrays();
  		}
  		//readFRAMCantileverData();
  		PolySci_State = Process_Data;
  		break;

  	case Process_Data:
  		medianFilter(findMedianCant5());
  		cantResonantPeriod5 = averageData();
  		PolySci_State = Send_Data_To_Hub;
  		break;

  	case Send_Data_To_Hub:
  		init_SPI(HUB);
  		//write_uint32_SPI(cantResonantPeriod5, HUB);
  		FINISHED = 1;									// finished collecting data
  		zeroOutFRAM();									// zero out the FRAM Array
  		break;
  	}
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
	ADC12CTL0 |= ADC12SC;                               // start conversion
	ADC12IFGR0 &= ~ADC12IFG0;                           // clear inturrput flag on ADC12MEM0
	ADC12IFGR0 &= ~ADC12IFG1;                           // clear inturrput flag on ADC12MEM1
}
