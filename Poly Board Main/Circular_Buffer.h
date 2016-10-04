/*
 * Circular_Buffer.h
 *
 *  Created on: Aug 10, 2016
 *      Author: Dr. Griffin
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

void init_Buffer(void);
void write_Buffer(uint32_t data);
uint32_t read_Buffer(void);
bool is_Buffer_Full(void);
bool is_Buffer_Empty(void);

#endif /* CIRCULAR_BUFFER_H_ */
