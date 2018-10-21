#include <msp430.h> 

/**
 * Kevin O'Hare and Luke Longo
 * Last updated: 10/21/18
 * main.c
 */
// Example Code used
/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 * 
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430F552x Demo - USCI_A0, 115200 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1048576hz = 1048576/115200 = ~9.1 (009h|01h)
//   ACLK = REFO = ~32768Hz, MCLK = SMCLK = default DCO = 32 x ACLK = 1048576Hz
//   See User Guide for baud rate divider table
//
//                 MSP430F552x
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |     P3.3/UCA0TXD|------------>
//            |                 | 115200 - 8N1
//            |     P3.4/UCA0RXD|<------------
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCSv4 and IAR Embedded Workbench Version: 4.21
//******************************************************************************


volatile int byte = 0;					// Track number of Bytes received
unsigned volatile int size = 0;				// Record Packet size

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Timer A0 Setup
	TA0CTL |= TASSEL_2 + MC_1;                  // Setup Timer A0 in UP mode w/ SMCLK
	TA0CCR0 = 256;                              // Set TA0CCR0 to 255

	TA0CCTL1 |= OUTMOD_3;                       // Enable Output
	TA0CCR1 = 0x00;                                // Set Duty Cycle to 0%

	TA0CCTL2 |= OUTMOD_3;                       // Enable Output
	TA0CCR2 = 0x00;                                // Set Duty Cycle to 0%

	TA0CCTL3 |= OUTMOD_3;                       // Enable Output
	TA0CCR3 = 0x00;                                // Set Duty Cycle to 0%

	// RGB Setup
	// RED
	P1OUT |= BIT2;                              // Set Pin 1.2 to High
	P1DIR |= BIT2;                              // Set Pin 1.2 to Output
	P1SEL |= BIT2;                              // Enable TA0.1 Output

	// GREEN
	P1OUT |= BIT3;                              // Set Pin 1.3 to High
	P1DIR |= BIT3;                              // Set Pin 1.3 to Output
    P1SEL |= BIT3;                              // Enable TA0.2 Output

    // BLUE
    P1OUT |= BIT4;                              // Set Pin 1.4 to High
    P1DIR |= BIT4;                              // Set Pin 1.4 to Output
    P1SEL |= BIT4;                              // Enable TA0.3 Output

    // UART Setup

    P4SEL |= BIT4+BIT5;
    // TX
    P3SEL |= BIT3;                              // Enable TX on pin 3.3

    // RX
    P3SEL |= BIT4;                              // Enable RX on pin 3.4

    // UART Initialization
    UCA1CTL1 |= UCSWRST;                        // CLear UART
    UCA1CTL1 |= UCSSEL_2;
    UCA1BR0 = 104;				// Used to set Baud Rate
    UCA1BR1 = 0;
    UCA1MCTL |= UCBRS_1 + UCBRF_0;
    UCA1CTL1 &= ~UCSWRST;                       // **Initialize USCI state machine**
    UCA1IE |= UCRXIE;                           // Enable Interrupt on RX
    UCA1IFG &= ~UCRXIFG;                        // Clear Interrupt Flag

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled

    while(1);

	return 0;
}


#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
  switch(byte)					// Performs action based on current amount of Bytes received
  {
  case 0:
      size = UCA1RXBUF;                    	// Save Packet Size
      break;                                    // Vector 0 - no interrupt
  case 1:
      TA0CCR1 = UCA1RXBUF;                      // Sets Red PWM
      break;
  case 2:
      TA0CCR2 = UCA1RXBUF;                      // Sets Green PWM
      break;
  case 3:
      TA0CCR3 = UCA1RXBUF;                      // Sets Blue PWM
      while(!(UCA1IFG & UCTXIFG));
          UCA1TXBUF = size - 3;			// Sends adjusted packet size to TX line	
      break;
  default:
      if(byte > size)				// Resets device when process is finished
      {
          byte = -1;	
          size = 0;
      }
      else
      {
          while(!(UCA1IFG & UCTXIFG));
              UCA1TXBUF = UCA1RXBUF;		// Sends received bytes along the TX line until packet size is reached
      }
      break;
  }
  byte++;					// Increases byte counter
}
