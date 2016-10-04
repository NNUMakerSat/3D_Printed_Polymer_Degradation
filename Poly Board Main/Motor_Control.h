/*
 * Motor_Control.h
 *
 *  Created on: Sep 5, 2016
 *      Author: CNogales
 */
#include <stdbool.h>
#include <stdint.h>

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#define FIN 0x04		// define FIN as 4.2
#define RIN 0x40		// define RIN as 2.6

void init_Hbridge();
void setMotorDirection(uint8_t);
void motorForward(void);
void motorReverse(void);
void motorBrake(void);
void motorOpen(void);
void setMotorAmplitude(uint16_t);
void motorRamp (uint16_t, uint16_t);
void motorSquare(uint16_t);

static const uint8_t FORWARD = 1;
static const uint8_t REVERSE = 0;
#endif /* MOTOR_CONTROL_H_ */
