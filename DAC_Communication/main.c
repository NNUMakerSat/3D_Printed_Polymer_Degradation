#include <msp430.h> 

#define DAC_CLK 0x10
#define DAC_DATA 0x20
#define SYNC 0x08
#define FIN 0x02
#define RIN 0x01


void DAC_Write(void);
void Dec_To_Bin(int);
void Timer_A_Setup(void);

unsigned int bin[12];
int DAC_Value = 2000;

/*
 * main.c
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	                // Stop watchdog timer
	
    PM5CTL0 &= ~LOCKLPM5;                       // disable high impedence mode

    Timer_A_Setup();                            // set up timer

    // configure motor forward / reverse pins; pins P4.0 and P4.1
    P4SEL0 &= ~FIN & ~RIN;                      // configure as GPIO's
    P4SEL1 &= ~FIN & ~RIN;
    P4DIR |= FIN | RIN;                         // output direction

    // select forward direction on motor
    P4OUT |= FIN;
    P4OUT &= ~RIN;

    // configure P2.4 and P2.5 as GPIO's
    P2SEL0 &= ~DAC_CLK & ~DAC_DATA & ~SYNC;
    P2SEL1 &= ~DAC_CLK & ~DAC_DATA & ~SYNC;
    P2DIR |= DAC_CLK | DAC_DATA | SYNC;         // configure P2.4 and P2.5 as output pins
    P2OUT &= ~DAC_CLK & ~DAC_DATA;              // initialize the pins low
    P2OUT |= SYNC;                              // bring the SYNC line high

    __enable_interrupt();                       // enable interrupts

    int DAC_Min = 500;                         // set to go up to

while (1)
{
    while (DAC_Value > DAC_Min)
    {

    }
    DAC_Value = 2000;

}
	//return 0;
}


void DAC_Write(void)
{
    P2OUT&= ~SYNC;                      // pull the SYNC line low to begin write sequence

    // write first two bits to be zero, since the data line is already low begining the clock cycle with the data lines low
    // will write zero to the first two bits
    P2OUT |= DAC_CLK;
    __delay_cycles(12);
    P2OUT &= ~DAC_CLK;
    __delay_cycles(12);
    // second clock cycle
    P2OUT |= DAC_CLK;
    __delay_cycles(12);
    P2OUT &= ~DAC_CLK;
    __delay_cycles(12);

    int i;
    for (i = 11; i >= 0; i--)
    {
        P2OUT |= DAC_CLK;               // begin clock sequency

        if (bin[i] == 1)
        {
            P2OUT |= DAC_DATA;          // if set pin high if that spot is a 1 in binary
        }
        else
        {
            P2OUT &= ~DAC_DATA;         // set data line low if spot is a 0 in binary
        }
        __delay_cycles(2);              // wait 2 clock cycles

        P2OUT &= ~DAC_CLK;              // bring clock low

        __delay_cycles(12);              // wait again
    }

    // finish writing to the 16 bit register
    P2OUT |= DAC_CLK;
    __delay_cycles(12);
    P2OUT &= ~DAC_CLK;
    __delay_cycles(12);
    // second clock cycle
    P2OUT |= DAC_CLK;
    __delay_cycles(12);
    P2OUT &= ~DAC_CLK;
    __delay_cycles(12);

    P2OUT &= ~DAC_DATA;         // set data line low
    __delay_cycles(100);

    P2OUT |= SYNC;                  // pull SYNC line high to indicate write is complete
}

void Timer_A_Setup(void)
{
    TA0CCR0 = 20000;                              // set the timer to count up to an arbitrary number, set to sampleing rate of 300 Hz
    TA0CTL |= MC_1 | ID_0 | TASSEL_2 | TACLR;    // continuous mode | divide clock by 1 | select SMCLK as clock source
    TA0CCTL0 |= CCIE;                            // enable timer interupt
    return;
}

void Dec_To_Bin(int dec)
{
   unsigned int j;
    // initialize the binary array elements to zero
    for (j = 0; j < 12; j++)
    {
        bin[j] = 0;
    }

    if (dec >= 2048)
    {
        bin[11] = 1;
        dec -= 2048;
    }

    if (dec >= 1024)
    {
        bin[10] = 1;
        dec -= 1024;
    }

    if (dec >= 512)
        {
            bin[9] = 1;
            dec -= 512;
        }

    if (dec >= 256)
        {
            bin[8] = 1;
            dec -= 256;
        }

    if (dec >= 128)
        {
            bin[7] = 1;
            dec -= 128;
        }

    if (dec >= 64)
        {
            bin[6] = 1;
            dec -= 64;
        }

    if (dec >= 32)
        {
            bin[5] = 1;
            dec -= 32;
        }

    if (dec >= 16)
        {
            bin[4] = 1;
            dec -= 16;
        }

    if (dec >= 8)
            {
                bin[3] = 1;
                dec -= 8;
            }

    if (dec >= 4)
            {
                bin[2] = 1;
                dec -= 4;
            }

    if (dec >= 2)
            {
                bin[1] = 1;
                dec -= 2;
            }

    if (dec >= 1)
            {
                bin[0] = 1;
                dec -= 1;
            }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void)
{
    Dec_To_Bin(DAC_Value);                      // convert DAC_Value to binary
    DAC_Write();                                // Write DAC_Value to DAC
    DAC_Value--;                                // incrament DAC Value
    //__delay_cycles(2000);
}
