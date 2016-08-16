#include <msp430.h>

// define the SPI pins
#define UCA0SIMO 0x01       // P2.0
#define UCA0SOMI 0x02       // P2.1
#define UCA0CLK 0x04        // P2.2
#define POT_1_CS 0x10       // P1.4


char Pot_Value;
unsigned int WRITE_IN_PROGRESS;

void Configure_SPI_Master_Registers(void);
void Pot_Write(unsigned int);

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;                            // Stop watchdog timer

    Configure_SPI_Master_Registers();

    //__enable_interrupt();                                // enable interrupts

    unsigned int TX_DATA;

    //UCA0IE |= UCTXIE;                                    // enable TX interrupt


   // UCA0TXBUF = TX_DATA;                                 // put data in TX buffer
    while(1)
    {

        for (TX_DATA = 255; TX_DATA > 0; TX_DATA--)
        {

        P1OUT |= POT_1_CS;
        __delay_cycles(100000);

        //TX_DATA = 0;
        P1OUT &= ~POT_1_CS;
        while(!(UCA0IFG & UCTXIFG)){};
        UCA0TXBUF = TX_DATA;
        while((UCA0STATW & UCBUSY)){};
        P1OUT |= POT_1_CS;
        }

    }
}


void Configure_SPI_Master_Registers(void)
{
    // Configure GPIO
    P1SEL1 |= BIT5;                           // USCI_A0 operation
    P2SEL0 |= BIT0 | BIT1;                    // USCI_A0 operation
    PJSEL0 |= BIT4 | BIT5;                    // For XT1

    // configure POT_1_CS as GPIO used to enable SPI write to POT
        P1SEL0 &= ~POT_1_CS;
        P1SEL1 &= ~POT_1_CS;
        P1DIR |= POT_1_CS;

    // Disable the GPIO power-on default high-impedance mode to activate
      // previously configured port settings
      PM5CTL0 &= ~LOCKLPM5;

      // XT1 Setup
      CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
      CSCTL1 = DCOFSEL_0;                       // Set DCO to 1MHz
      CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
      CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // set all dividers
      CSCTL4 &= ~LFXTOFF;
      do
      {
        CSCTL5 &= ~LFXTOFFG;                    // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
      }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
      CSCTL0_H = 0;                             // Lock CS registers

      // Configure USCI_A0 for SPI operation
      UCA0CTLW0 = UCSWRST;                      // **Put state machine in reset**
      //UCA0CTLW0 |= UCMST | UCSYNC | UCCKPL | UCMSB; // 3-pin, 8-bit SPI master
                                                // Clock polarity high, MSB

      UCA0CTLW0 |= UCMST | UCSYNC | UCMSB | UCCKPL; // 3-pin, 8-bit SPI master
                                                      // Clock polarity high, MSB

      UCA0CTLW0 |= UCSSEL__ACLK;                // ACLK
      UCA0BR0 = 0x02;                           // /2
      UCA0BR1 = 0;                              //
      UCA0MCTLW = 0;                            // No modulation
      UCA0CTLW0 &= ~UCSWRST;                    // **Initialize USCI state machine**
      UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt



    /*

    UCA0CTLW0 |= UCSWRST;                                // enable eUSCI initialization of registers

    // configure digital I/O Pins; select primary module function
    // P2.0 is UCA0SIMO, P2.1 is UCA0SOMI, P2.2 is UCA0CLK, P1.4 is POT_1_CS
    P2SEL0 |= UCA0SIMO | UCA0SOMI | UCA0CLK;
    P2SEL1 &= ~UCA0SIMO & ~UCA0SOMI & ~UCA0CLK;


    // configure POT_1_CS as GPIO used to enable SPI write to POT
    P1SEL0 &= ~POT_1_CS;
    P1SEL1 &= ~POT_1_CS;
    P1DIR |= POT_1_CS;

    UCA0MCTLW = 0x00;                                    // clear modulation register, required for SPI

    UCA0CTLW0 |= UCMST + UCSYNC + UCSSEL_2 + UCMSB;      // select master mode | select SPI mode | SMCLK | MSB first

    UCA0BRW = 16;                                      // set the Bit Clock equal to the BRCLK

    __delay_cycles(20);                                  // wait for UCA0SIMO and UCA0SOMI to settle to final volatage

    UCA0CTLW0 &= ~UCSWRST;                               // lock eUSCI registers

    //UCA0IFG &= ~UCTXIFG & ~UCRXIFG;                      // clear RX and TX interrupt flags

    */
}

/*void Pot_Write(unsigned int data)
{
    WRITE_IN_PROGRESS = 1;
    P1OUT &= ~POT_1_CS;                             // set cs pin low to indicate write is beginning
    UCA0TXBUF = data;                             // put data in TX buffer

    while (WRITE_IN_PROGRESS == 1)
    {

    }

    __delay_cycles(10);                                 // wait for data to be transmitted


}

#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    WRITE_IN_PROGRESS = 0;
    P1OUT |= POT_1_CS;                               // set cs pin high to indicate write is completed
    UCA0IFG &= ~UCTXIFG & ~UCRXIFG;                      // clear RX and TX interrupt flags
}

*/
