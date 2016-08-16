/*
 * Original Code taken from Connor Nogales and
 * changed for SPI vs. bit banging by Aaron Ewing
 * on 8/12/2016 with Connor's permission
 */
#include <msp430.h> 

#define UCA0SOMI 0x02		// P2.1
#define UCA0CLK 0x04		// P2.2
#define UCA0SIMO 0x01		// P2.0

/*
#define UCA0SOMI 0x10		// P2.4
#define UCA0CLK 0x20		// P2.5
#define UCA0SIMO 0x08			// P2.3
*/
void DAC_Write(void);
void Dec_To_Bin(int);
void Timer_A_Setup(void);

unsigned int bin[12];
int DAC_Value = 2000;

// main.c
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	                // Stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;                       // disable high impedence mode

    Timer_A_Setup();                            // set up timer

    UCA0CTLW0 |= UCSWRST;                                // enable eUSCI initialization of registers

    // configure P2.4 and P2.5 as GPIO's
    P2SEL0 &= ~UCA0SOMI & ~UCA0CLK & ~UCA0SIMO;
    P2SEL1 &= ~UCA0SOMI & ~UCA0CLK & ~UCA0SIMO;
    P2DIR |= UCA0SOMI | UCA0CLK | UCA0SIMO;         // configure P2.1 and P2.2 as output pins

    P2OUT &= ~UCA0SOMI & ~UCA0CLK;              // initialize the pins low
    P2OUT |= UCA0SIMO;                              // bring the UCA0SIMO line high

    UCA0MCTLW = 0x00;                                    // clear modulation register, required for SPI

    UCA0CTLW0 |= UCMST | UCSYNC | UCSSEL_2 | UCMSB;      // select master mode | select SPI mode | SMCLK | MSB first

    __delay_cycles(20);                                  // wait for UCA0SIMO and UCA0SOMI to settle to final volatage

    UCA0CTLW0 &= ~UCSWRST;                               // lock eUSCI registers

    UCA0IFG &= ~UCTXIFG & ~UCRXIFG;                      // clear RX and TX interrupt flags

    __enable_interrupt();                       // enable interrupts

    UCA0IE |= UCTXIE;                                    // enable TX interrupt


    while (1) {

	}
}


void DAC_Write(void)
{
    P2OUT&= ~UCA0SIMO;                      // pull the UCA0SIMO line low to begin write sequence

    // write first two bits to be zero, since the data line is already low begining the clock cycle with the data lines low
    // will write zero to the first two bits
    P2OUT |= UCA0SOMI;
    __delay_cycles(12);
    P2OUT &= ~UCA0SOMI;
    __delay_cycles(12);
    // second clock cycle
    P2OUT |= UCA0SOMI;
    __delay_cycles(12);
    P2OUT &= ~UCA0SOMI;
    __delay_cycles(12);

    int i;
    for (i = 11; i >= 0; i--)
    {
        P2OUT |= UCA0SOMI;               // begin clock sequency

        if (bin[i] == 1)
        {
            P2OUT |= UCA0CLK;          // if set pin high if that spot is a 1 in binary
        }
        else
        {
            P2OUT &= ~UCA0CLK;         // set data line low if spot is a 0 in binary
        }
        __delay_cycles(2);              // wait 2 clock cycles

        P2OUT &= ~UCA0SOMI;              // bring clock low

        __delay_cycles(12);              // wait again
    }

    // finish writing to the 16 bit register
    P2OUT |= UCA0SOMI;
    __delay_cycles(12);
    P2OUT &= ~UCA0SOMI;
    __delay_cycles(12);
    // second clock cycle
    P2OUT |= UCA0SOMI;
    __delay_cycles(12);
    P2OUT &= ~UCA0SOMI;
    __delay_cycles(12);

    P2OUT &= ~UCA0CLK;         // set data line low
    __delay_cycles(100);

    P2OUT |= UCA0SIMO;                  // pull UCA0SIMO line high to indicate write is complete

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
    UCA0TXBUF = DAC_Value;
}
