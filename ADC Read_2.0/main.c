/*
 * code for Goertzle Algrithm was obtained at http://www.embedded.com/design/configurable-systems/4024443/The-Goertzel-Algorithm
*/

#include <msp430.h> 
#include <stdio.h>
#include <stdint.h>
#include <cmath>
/*
 * main.c
 */

// function prototypes
void Clock_Setup(void);
void ADC12_Setup(void);
void Timer_A_Setup(void);
void goetzle_algorithm(float, unsigned int*);

#define Data_Start_Address 0xD000


// struct definition to hold the frequency and magnitude of the waveform
    struct Freq_Mag
    {
        float freq;
        float mag;
    };

// intialize constants for goertzles algrithm


const float sample_rate = 300;                       // constant sample rate
const float num_double = 3000;
const int num_int = 3000;                             // number of samples
const float pi = 3.1415;

unsigned int* data;
//unsigned int data_arr[num];

struct Freq_Mag resonant_frequency[3];

int i;

// initialize data_arr in FRAM
//#pragma PERSISTENT(data_arr)
//unsigned long data_arr[6000];

//struct Freq_Mag results[];

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	                // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;                       // disable high impedence mode

   float target_frequency;                      // variable to hold target frequency

    // set resonant frequency back to zero
    resonant_frequency[0].mag = 0;
    resonant_frequency[0].freq = 0;
    resonant_frequency[1].mag = 0;
    resonant_frequency[1].freq = 0;
    resonant_frequency[2].mag = 0;
    resonant_frequency[2].freq = 0;




   // Clock_Setup();                              // set up clock
    Timer_A_Setup();                            // Set up ADC Timer
    ADC12_Setup();                              // Initialize ADC
    __enable_interrupt();                       // enable interrupts

    // start motor
    // put a nested loop here to collect and process individual blocks of data

    int j;
    for (j = 10; j > 0; j--)
    {
        // set resonant frequency back to zero
        resonant_frequency[0].mag = 0;
        resonant_frequency[0].freq = 0;
        resonant_frequency[1].mag = 0;
        resonant_frequency[1].freq = 0;
        resonant_frequency[2].mag = 0;
        resonant_frequency[2].freq = 0;


        TA0CTL |= MC_1;                                                  // resume timmer
        i = 0;

        data = (unsigned int *) Data_Start_Address;                        // point pointer at first element in data array



        while (i < num_int)
        {

        }

        TA0CTL &= ~MC_1;                                                   // stop timmer


        for (target_frequency = 42; target_frequency < 52; target_frequency += 0.1)
        {
            goetzle_algorithm(target_frequency, data);              // run goetzle algorithm
        }


        i = 0;                                                           // reset counter

    }



    // Run FFT on ADC Value (Goertzle algrithm)


	return 0;
}
void Clock_Setup(void)
{
    CSCTL0 = CSKEY;                  // unlock clock registers
    CSCTL1 |= DCORSEL | DCOFSEL_0;   // select 1 MHz clock frequency
    CSCTL2 |= SELS_3;                // select DCOCLK as clock source for SMCLK
    CSCTL3 |= DIVS_0;                // divide SMCLK by 1
    CSCTL0_H = 0;                    // lock clock registers
}
void Timer_A_Setup(void)
{
    TA0CCR0 = 3321;                              // set the timer to count up to an arbitrary number, set to sampleing rate of 300 Hz
    TA0CTL |= MC_0 | ID_0 | TASSEL_2 | TACLR;    // continuous mode | divide clock by 1 | select SMCLK as clock source
    TA0CCTL0 |= CCIE;                            // enable timer interupt
    return;
}

void ADC12_Setup(void)
{

    ADC12CTL0 &= ~ADC12ENC;                                    // disable the ADC enable bit


    P9DIR &= ~0x02;                                            // set pin P9.1 to input mode

    // set pin 9.1 to tertiary module function
    P9SEL0 |= 0x02;
    P9SEL1 |= 0x02;

    ADC12CTL0 |= ADC12SHT0_2 | ADC12ON;                        // 16 clock cycles per sample | turn ADC on

    ADC12CTL1 |= ADC12SSEL_3 | ADC12DIV_0 | ADC12SHP;          // select SMCLK as clock source


    ADC12CTL2 |= ADC12RES_2;                                    // set ADC resolution to 12 bit
    ADC12CTL2 &= ~ADC12DF;                                      // set conversion results to be unsigned binary

    ADC12CTL3 = ADC12CSTARTADD_0 | ADC12CONSEQ_0;               // value is in memory register 0 | single conversion single channel

    ADC12MCTL0 |= ADC12INCH_9 | ADC12VRSEL_0;        // select input channel to be 9 | select VCC and VSS as V+ and V- | end of series
    ADC12MCTL0 &= ~ADC12EOS;

    ADC12IER0 |= ADC12IE0;                                      // enable interupt on ADC12IFG0 bit
    ADC12CTL0 |= ADC12ENC;                                      // enable conversion
    ADC12IFGR0 &= ~ADC12IFG0;                                   // clear inturrput flag

    return;
}

void goetzle_algorithm(float target_frequency, unsigned int* data_ptr)
{
    float Q0, Q1, Q2;                                    // variables to hold ADC values
    float k;
    float cosine, coeff, magnitude, w;
   // double sine;

    // initialize ADC values to 0
    Q0 = 0;
    Q1 = 0;
    Q2 = 0;

    // calculate goetzle algorithm constants
    k = ((num_double * target_frequency)/sample_rate);
    w = (2.0 * pi * k) / num_double;
    cosine = cos(w);
    //sine = sin(w);
    coeff = 2.0 * cosine;

    data = (unsigned int *) Data_Start_Address;                        // point pointer at first element in data array

    unsigned int c;

    for (c = 0; c < num_int; c++)
    {

    // calculate the magnitude at the target frequency
    //Q0 = coeff * Q1 - Q2 + *data_block_ptr;
    Q0 = coeff * Q1 - Q2 + *data_ptr;
    Q2 = Q1;
    Q1 = Q0;



    data_ptr++;                                       // incrament pointer to next data point
    }

    //real = (Q1 - Q2 * cosine);
    //imag = (Q2 * sine);
    //magnitude = real * real + imag * imag;

    magnitude = ((Q1 * Q1) + (Q2 * Q2) - (Q1 * Q2 * coeff));  // calculate magnitude, *magnitude value is the square of the actual magnitude




    // if statement to find the larges magnitude
    if (magnitude > resonant_frequency[0].mag)
    {
        resonant_frequency[0].mag = magnitude;
        resonant_frequency[0].freq = target_frequency;
    }
    else if (magnitude > resonant_frequency[1].mag)
    {
        resonant_frequency[1].mag = magnitude;
        resonant_frequency[1].freq = target_frequency;
    }
    else if (magnitude > resonant_frequency[2].mag)
    {
        resonant_frequency[2].mag = magnitude;
        resonant_frequency[2].freq = target_frequency;
    }

   // return magnitude;
}




#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
    *data = ADC12MEM0;
    ADC12IFGR0 &= ~ADC12IFG0;                                 // clear inturrput flag
    i++;                                                      // incrament counter
    data++;                                                   // incrament pointer to move to next data point in FRAM
}



#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void)
{
    ADC12CTL0 |= ADC12SC;                                      // start conversion
}


