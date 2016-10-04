/*
 * Collect_Data.h
 *
 *  Created on: Sep 29, 2016
 *      Author: CNogales
 */

#ifndef COLLECT_DATA_H_
#define COLLECT_DATA_H_

void collectData(uint16_t, uint8_t);
void processPulseData(void);
uint32_t findMedianCant5(void);
void zeroOutArrays(void);
uint32_t averageData(void);
void medianFilter(uint32_t median);
void readFRAMCantileverData(void);
void zeroOutFRAM(void);
uint8_t i;
uint32_t timerData[50];
uint32_t periodData[50];
int32_t deltaPeriod[50];
int32_t filteredData[10];
uint32_t dataArr[100];



#endif /* COLLECT_DATA_H_ */
