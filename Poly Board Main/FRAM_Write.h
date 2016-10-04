/*
 * FRAM_Write.h
 *
 *  Created on: Sep 29, 2016
 *      Author: CNogales
 */

#include <stdint.h>

#ifndef FRAM_WRITE_H_
#define FRAM_WRITE_H_

// allocate cantilever 1 data array
#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(cantData1)
static const uint32_t cantData1[5000] = {0};
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent unsigned long cantData1[5000] = {0};
#else
#error Compiler not supported!
#endif

// allocate cantilever 1 data array
#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(cantData2)
static const uint32_t cantData2[5000] = {0};
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent unsigned long cantData2[5000] = {0};
#else
#error Compiler not supported!
#endif

// allocate cantilever 1 data array
#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(cantData3)
static const uint32_t cantData3[5000] = {0};
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent unsigned long cantData3[5000] = {0};
#else
#error Compiler not supported!
#endif

// allocate cantilever 1 data array
#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(cantData4)
static const uint32_t cantData4[5000] = {0};
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent unsigned long cantData4[5000] = {0};
#else
#error Compiler not supported!
#endif

// allocate cantilever 1 data array
#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(cantData5)
static uint32_t cantData5[5000] = {0};
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent unsigned long cantData5[5000] = {0};
#else
#error Compiler not supported!
#endif

static uint8_t cantCounter1 = 0;
static uint8_t cantCounter2 = 0;
static uint8_t cantCounter3 = 0;
static uint8_t cantCounter4 = 0;
uint8_t cantCounter5;


#endif /* FRAM_WRITE_H_ */
