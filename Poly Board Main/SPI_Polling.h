/*
 * SPI_Pulling.h
 *
 *  Created on: Aug 16, 2016
 *      Author: aaronewing
 */
#include <msp430.h>
#include <stdint.h>

#ifndef SPI_POLLING_H_
#define SPI_POLLING_H_

#define DAC_CS 0x20;												// dac cs pin P2.5
#define DAC 2
#define HUB 1
#define HUB_CS 0x04													// hub cs P1.2
void init_SPI (uint8_t pin_Setting);								// initalizes SPI clk rate and which pins are being used
void write_uint8_SPI (uint8_t tx_Data_8, uint8_t device_CS);		// writes 8 bits with SPI
void write_uint16_SPI (uint16_t tx_Data_16, uint8_t device_CS);		// writes 16 bits with SPI
void write_uint32_SPI(uint32_t, uint8_t);
uint8_t read_SPI (void);											// reads 8 bits with SPI


#endif /* SPI_POLLING_H_ */
