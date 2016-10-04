/*
 * Circular_Buffer.c
 *
 *  Created on: Aug 10, 2016
 *      Author: Dr. Griffin
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Circular_Buffer.h"

#define MAXBUFFERSIZE 200

uint32_t g_buffer[MAXBUFFERSIZE];

uint8_t g_begin = 0;
uint8_t g_end = MAXBUFFERSIZE-1;
uint8_t g_top;
uint8_t g_bottom;
bool g_bufferEmpty;
bool g_bufferFull;

void init_Buffer(void){
    g_top = 0;
    g_bottom = 0;
    g_bufferFull = false;
    g_bufferEmpty = false;
}

void write_Buffer(uint32_t data) {
    g_buffer[g_bottom] = data;
    g_bottom++;
    if(g_bottom > g_end) {
        g_bottom = g_begin;
    }
    if(g_top == g_bottom){
        g_bufferFull = true;
    } else {
        g_bufferFull = false;
    }
}

uint32_t read_Buffer(void) {
    uint32_t readVal = g_buffer[g_top];
    g_top++;
    if(g_top > g_end) {
        g_top = g_begin;
    }
    if(g_top == g_bottom){
        g_bufferEmpty = true;
    } else {
        g_bufferEmpty = false;
    }
    return readVal;
}

bool is_Buffer_Full(void) {
    return g_bufferFull;
}

bool is_Buffer_Empty(void) {
    return g_bufferEmpty;
}


