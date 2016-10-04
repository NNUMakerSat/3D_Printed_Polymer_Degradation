/*
 * SPI_Pulling.c
 *
 *  Created on: Aug 16, 2016
 *      Author: aaronewing
 */

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "SPI_Polling.h"
uint8_t g_RXData;

////////////////////// SPI INIT /////////////////////////////////////
void init_SPI (uint8_t pin_Setting) {
	switch (pin_Setting) {
	case 0:								// Hub
	default:
		// Configure Primary Function Pins
		P1SEL0 |= BIT4 | BIT6 | BIT7;       		// P1.4 - CLK, P1.6 - SIMO, P1.7 - SOMI

		// configure as GPIO used to enable SPI write from Hub
		P4SEL0 &= ~BIT1;
		P4SEL1 &= ~BIT1;							// P4.1 - SYNC/Slave Select
		P4DIR |= BIT1;
		P4OUT |= BIT1;
		break;

	case 1:								// Hub comm
		// Configure Primary Function Pins
		P1SEL1 |= BIT6 | BIT7;              // P1.6 - SIMO, P1.7 - SOMI
		P2SEL1 |= BIT2;              		// P2.2 - CLK
		P1SEL0 &= ~BIT6 | BIT7;
		P2SEL0 &= ~BIT2;

		// configure as GPIO used to enable SPI write to Hub
		P1SEL0 &= ~HUB_CS;
		P1SEL1 &= ~HUB_CS;					// P1.2 - SYNC/Slave Select
		P1DIR |= HUB_CS;
		P1OUT &= ~HUB_CS;					// bring sync line low

		// Configure USCI_B0 for SPI operation
		UCB0CTLW0 |= UCSWRST;                      		// **Put state machine in reset**

		UCB0CTLW0 |= UCSYNC | UCMSB | UCCKPL;						// 3-pin, 8-bit slave

		UCB0CTLW0 &= ~UCMST;
		                                          	  	    // Clock polarity high, MSB
		UCB0CTLW0 |= UCSSEL__SMCLK;              	 		// SMCLK

		UCB0BRW = 0x0008;							// Divides SMCLK module by 8 (8MHz/8 = 1MHz)

		UCB0CTLW0 &= ~UCSWRST;
		// **Initialize USCI state machine**
		break;

	case 2:								// MSP430FR5969 DAC
		// Configure Primary Function Pins
		P1SEL1 |= BIT6 | BIT7;              // P1.6 - SIMO, P1.7 - SOMI
		P2SEL1 |= BIT2;              		// P2.2 - CLK

		// configure as GPIO used to enable SPI write to Hub
		P2SEL0 &= ~DAC_CS;
		P2SEL1 &= ~DAC_CS;					// P2.5 - SYNC/Slave Select, slect GPIO Mode
		P2DIR |= DAC_CS;					// Set P2.5 to output
		P2OUT |= DAC_CS;					// set P2.5 high

		// Configure USCI_B0 for SPI operation
		UCB0CTLW0 |= UCSWRST;                      		// **Put state machine in reset**

		UCB0CTLW0 |= UCSYNC | UCMSB | UCMST;						// 3-pin | 8-bit | Master mode
					                                          	  	    // Clock polarity high, MSB
		UCB0CTLW0 |= UCSSEL__SMCLK;              	 		// SMCLK

		UCB0BRW = 0x0008;							// Divides SMCLK module by 8 (8MHz/8 = 1MHz)

		UCB0CTLW0 &= ~UCSWRST;
		// **Initialize USCI state machine**
		break;

	case 3:								// Polymer degradation board - other
		// Configure Primary Function Pins
		P1SEL0 |= BIT6 | BIT7;              // P1.6 - SIMO, P1.7 - SOMI
		P2SEL1 |= BIT2;              		// P2.2 - CLK

		// configure as GPIO used to enable SPI write to Hub
		P1SEL0 &= ~BIT2;
		P1SEL1 &= ~BIT2;					// P1.2 - SYNC/Slave Select
		P1DIR |= BIT2;
		P1OUT |= BIT2;
		break;
	}
}

////////////////////// SPI WRITE 8 BIT /////////////////////////////////////
void write_uint8_SPI (uint8_t tx_Data_8, uint8_t device_CS) {
	while (!(UCB0IFG & UCTXIFG)){};							// If able to TX

	switch (device_CS) {					// Hub
		case 0:
		default:
			break;


		case HUB:
			__delay_cycles(16000000);
			P1OUT |= HUB_CS;								// Pulls SYNC high
			UCB0TXBUF = 0xCC;					// First byte
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			while (UCB0STATW & UCBUSY) {};					// while TXing
			__delay_cycles(5);							// delay to allow Hub to prepare for next byte
			P1OUT &= ~HUB_CS;								// put hub CS line low again
			break;

		case 2:
			P3OUT &= ~BIT6;									// Pulls SYNC low
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			UCB0TXBUF = tx_Data_8;							// 8 bits transmitted
			while (UCB0STATW & UCBUSY) {};
			P3OUT |= BIT6;
			break;

		case 3:
			P4OUT &= ~BIT6;									// Pulls SYNC low
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			UCB0TXBUF = tx_Data_8;							// 8 bits transmitted
			while (UCB0STATW & UCBUSY) {};
			P4OUT |= BIT6;
			break;
	}
}

////////////////////// SPI WRITE 16 BIT /////////////////////////////////////
void write_uint16_SPI (uint16_t tx_Data_16, uint8_t device_CS) {
	while (!(UCB0IFG & UCTXIFG)){};							// If able to TX

	switch (device_CS) {
		case 0:									// Hub
		default:
			P4OUT &= ~BIT1;									// Pulls SYNC low
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			UCB0TXBUF = (tx_Data_16 >> 8);					// First 8 bits transmitted (Control bits and data)
			while (!(UCB0IFG & UCTXIFG)) {};
			UCB0TXBUF = tx_Data_16;							// Last 8 bits transmitted (overflow expected and is fine)
			while (UCB0STATW & UCBUSY) {};
			P4OUT |= BIT1;
			break;

		case 1:
			P2OUT &= ~BIT8;									// Pulls SYNC low
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			UCB0TXBUF = (tx_Data_16 >> 8);					// First 8 bits transmitted (Control bits and data)
			while (!(UCB0IFG & UCTXIFG)) {};
			UCB0TXBUF = tx_Data_16;							// Last 8 bits transmitted (overflow expected and is fine)
			while (UCB0STATW & UCBUSY) {};
			P2OUT |= BIT6;
			break;

		case 2:
			P2OUT &= ~DAC_CS;									// Pulls SYNC low
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			UCB0TXBUF = (tx_Data_16 >> 6);					// First 8 bits transmitted (Control bits and data)
			while (!(UCB0IFG & UCTXIFG)) {};
			UCB0TXBUF = tx_Data_16;							// Last 8 bits transmitted (overflow expected and is fine)
			while (UCB0STATW & UCBUSY) {};
			P2OUT |= DAC_CS;
			break;

		case 3:
			P4OUT &= ~BIT6;									// Pulls SYNC low
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			UCB0TXBUF = (tx_Data_16 >> 8);					// First 8 bits transmitted (Control bits and data)
			while (!(UCB0IFG & UCTXIFG)) {};
			UCB0TXBUF = tx_Data_16;							// Last 8 bits transmitted (overflow expected and is fine)
			while (UCB0STATW & UCBUSY) {};
			P4OUT |= BIT6;
			break;

		case 4:
			P2OUT &= ~DAC_CS;									// Pulls SYNC low
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			UCB0TXBUF = (tx_Data_16 >> 8);					// First 8 bits transmitted (Control bits and data)
			while (!(UCB0IFG & UCTXIFG)) {};
			UCB0TXBUF = tx_Data_16;							// Last 8 bits transmitted (overflow expected and is fine)
			while (UCB0STATW & UCBUSY) {};
			P2OUT |= DAC_CS;
			break;
	}
}

////////////////////// SPI WRITE 32 BIT /////////////////////////////////////
void write_uint32_SPI (uint32_t tx_Data_32, uint8_t device_CS) {
	while (!(UCB0IFG & UCTXIFG)){};							// If able to TX

	switch (device_CS) {
		case 0:
		default:
			P4OUT &= ~BIT1;									// Pulls SYNC low
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			UCB0TXBUF = (tx_Data_32 >> 8);					// First 8 bits transmitted (Control bits and data)
			while (!(UCB0IFG & UCTXIFG)) {};
			UCB0TXBUF = tx_Data_32;							// Last 8 bits transmitted (overflow expected and is fine)
			while (UCB0STATW & UCBUSY) {};
			P4OUT |= BIT1;
			break;

		case 1:												// hub
			P1OUT |= HUB_CS;								// Pulls SYNC low
			UCB0TXBUF = (tx_Data_32 >> 24);					// First byte
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			while (UCB0STATW & UCBUSY) {};					// while TXing
			__delay_cycles(5);							// delay to allow Hub to prepare for next byte

			UCB0TXBUF = (tx_Data_32 >> 16);					// Second Byte
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			while (UCB0STATW & UCBUSY) {};
			__delay_cycles(5);							// delay for hub to prepare for another byte

			UCB0TXBUF = (tx_Data_32 >> 8);					// Third Byte
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			while (UCB0STATW & UCBUSY) {};
			__delay_cycles(5);							// delay to allow Hub to prepare for next byte

			UCB0TXBUF = tx_Data_32;							// Fourth and last byte
			while (!(UCB0IFG & UCTXIFG)) {};    			// While TXing
			while (UCB0STATW & UCBUSY) {};
			P1OUT &= ~HUB_CS;								// Pulls SYNC low
			break;

	}
}
////////////////////// SPI READ POLLING //////////////////////////////////
uint8_t read_SPI (void) {
	while (!(UCB0IFG & UCRXIFG)) {};    			// While RX flag is high
	g_RXData = UCB0RXBUF;							// First 8 bits transmitted (Control bits and data)
	return g_RXData;
}




////////////////////// LEGAL /////////////////////////////////////
/* XT1 config section:
 *
 *
 * --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************

