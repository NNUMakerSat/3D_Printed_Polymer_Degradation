/*
 * code for Goertzle Algrithm was obtained at http://www.embedded.com/design/configurable-systems/4024443/The-Goertzel-Algorithm
 */

#include <msp430.h> 
#include <stdio.h>
#include <stdint.h>
/*
 * main.c
 */

// function prototypes
void ADC12_Setup(void);
void Timer_A_Setup(void);

int results[400];
int i;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	    // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;           // disable high impedence mode

    Timer_A_Setup();                // Set up ADC Timer
    ADC12_Setup();                  // Initialize ADC
    __enable_interrupt();           // enable interrupts


    while (i < 399)
    {

    }


    // Run FFT on ADC Value (Goertzle algrithm)


	//return 0;
}

void Timer_A_Setup(void)
{
    TA0CCR0 = 4984;     // set the timer to count up to an arbitrary number, set to sampleing rate of 200 Hz
    TA0CTL |= MC_1 | ID_0 | TASSEL_2 | TACLR;    // continuous mode | divide clock by 1 | select SMCLK as clock source
    TA0CCTL0 |= CCIE;   // enable timer interupt
    return;
}





void ADC12_Setup(void)
{

    ADC12CTL0 &= ~ADC12ENC;                                    // disable the ADC enable bit


    P9DIR &= ~0x02;                                           // set pin P9.1 to input mode

    // set pin 9.1 to tertiary module function
    P9SEL0 |= 0x02;
    P9SEL1 |= 0x02;

    ADC12CTL0 |= ADC12SHT0_2 | ADC12ON;                        // 16 clock cycles per sample | turn ADC on

    ADC12CTL1 |= ADC12SSEL_3 | ADC12DIV_0 | ADC12SHP;          // select SMCLK as clock source


    ADC12CTL2 |= ADC12RES_2;                                    // set ADC resolution to 12 bit
    ADC12CTL2 &= ~ADC12DF;                                      // set conversion results to be unsigned binary

    ADC12CTL3 = ADC12CSTARTADD_0 | ADC12CONSEQ_0;               // value is in memory register 0 | single conversion single channel

    ADC12MCTL0 |= ADC12INCH_9 | ADC12VRSEL_0 | ADC12EOS;        // select input channel to be 9 | select VCC and VSS as V+ and V- | end of series

    ADC12IER0 |= ADC12IE0;                                      // enable interupt on ADC12IFG0 bit
    ADC12CTL0 |= ADC12ENC;                                      // enable conversion
    ADC12IFGR0 &= ~ADC12IFG0;                                   // clear inturrput flag

    return;
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
    results[i] = (ADC12MEM0);
    ADC12IFGR0 &= ~ADC12IFG0;                                  // clear inturrput flag
    i++;
}



#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void)
{
    ADC12CTL0 |= ADC12SC;                                      // start conversion
}


