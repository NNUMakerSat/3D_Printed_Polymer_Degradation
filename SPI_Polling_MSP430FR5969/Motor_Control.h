/*
 * Motor_Control.h
 *
 *  Created on: Sep 5, 2016
 *      Author: CNogales
 */

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#define FIN 0x04		// define FIN as 4.2
#define RIN 0x40		// define RIN as 2.6

void init_Hbridge(void);
void motorForward(void);
void motorReverse(void);
void motorBrake(void);
void motorOpen(void);
void setMotorAmplitude(uint16_t);

#endif /* MOTOR_CONTROL_H_ */
