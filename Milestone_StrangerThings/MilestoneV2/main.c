#include <msp430.h> 

//We back bitches
/**
 * main.c
 */

int byte = 0;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Timer A0 Setup
	TA0CTL |= TASSEL_2 + MC_1;                  // Setup Timer A0 in UP mode w/ SMCLK
	TA0CCR0 = 256;                              // Set TA0CCR0 to 255

	TA0CCTL1 |= OUTMOD_7;                       // Enable Output
	TA0CCR1 = 0;                                // Set Duty Cycle to 0%

	TA0CCTL2 |= OUTMOD_7;                       // Enable Output
	TA0CCR2 = 0;                                // Set Duty Cycle to 0%

	TA0CCTL3 |= OUTMOD_7;                       // Enable Output
	TA0CCR3 = 0;                                // Set Duty Cycle to 0%

	// RGB Setup
	// RED
	P1OUT |= BIT2;                              // Set Pin 1.2 to Output
	P1SEL |= BIT2;                              // Enable TA0.1 Output

	// RED
    P1OUT |= BIT3;                              // Set Pin 1.3 to Output
    P1SEL |= BIT3;                              // Enable TA0.2 Output

    // RED
    P1OUT |= BIT4;                              // Set Pin 1.4 to Output
    P1SEL |= BIT4;                              // Enable TA0.3 Output

    // UART Setup
    // TX
    P3SEL |= BIT3;                              // Enable TX on pin 3.3

    // RX
    P3SEL |= BIT4;                              // Enable RX on pin 3.4

    //UART Initialization
    UCA0IE |= UCRXIE;                           // Enable Interrupt on RX
    UCA0IFG &= ~UCRXIFG;                        // Clear Interrupt Flag

    // Baud Rate
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL |= UCBRS0;
    UCA0MCTL &= ~UCBRF0;



	return 0;
}
