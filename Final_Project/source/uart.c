/*
 * uart.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Arpit Savarkar
 *
 *      @brief: Instantiation and functionalities for communication over UART
 *
 *    Sources of Reference :
 * 		Textbooks : Embedded Systems Fundamentals with Arm Cortex-M based MicroControllers
 * 		Links: https://github.com/alexander-g-dean/ESF/tree/master/NXP/Code
 */

#include "uart.h"
#include "MKL25Z4.h"
#include "assert.h"
#include "global_defs.h"

#include "sysclock.h"
#include "queue.h"

Q_T TxQ, RxQ;

int __sys_write(int handle, char* buffer, int count) {
	if(buffer == NULL) {
		return -1;
	}
	while(Q_Full(&TxQ)) {
		; // Wait for the space to openup
	}

	if(Q_Enqueue(&TxQ, buffer, count) != count) {
		return -1;
	}

	if(!(UART0->C2 & UART0_C2_TIE_MASK)) {
		UART0->C2 |= UART0_C2_TIE(1);
	}

	return 0;
}

int __sys_readc(void) {
	char ch;
	if (Q_Dequeue(&RxQ, &ch, 1) != 1){
		return -1;
	}
	return ch;
}


/*
 * Initializing the UART for BAUD_RATE: 38400, Data Size: 8, Parity: None, Stop Bits: 2
 *
 * Parameters:
 *   baud_rate: uint32_t for the requested baud rate
 *
 * Returns:
 *   void
 */
void Init_UART0(uint32_t baud_rate) {
	uint16_t sbr;

	// Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;

	// Set UART clock to 48 MHz clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
//	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;

	// Set pins to UART0 Rx and Tx
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx

	// Set baud rate and oversampling ratio
	sbr = (uint16_t)((SYS_CLOCK)/(baud_rate * UART_OVERSAMPLE_RATE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);

	// Disable interrupts for RX active edge and LIN break detect, select one stop bit
	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(0) | UART0_BDH_LBKDIE(0);

	// Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(0) | UART0_C1_PE(0) | UART0_C1_PT(0);

	// Don't invert transmit data, don't enable interrupts for errors
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0)
			| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

	// Clear error flags
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

	// Send LSB first, do not invert received data
	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0);

	// Enable interrupts. Listing 8.11 on p. 234
	Q_Init(&TxQ);
	Q_Init(&RxQ);

	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable receive interrupts but not transmit interrupts yet
	UART0->C2 |= UART_C2_RIE(1);

	// Enable UART receiver and transmitter
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);

	LOG("\n\r Clock Gating and Instantiation for UART0 at 115200 Baud Rate Complete");

}


/*
 * Transmits String over to UART
 *
 * Parameters:
 *   str: String to Transmit over UART
 *	 count: The Length of the String to transmit
 * Returns:
 *   void
 */
void Send_String(const void* str, size_t count){
	Q_Enqueue(&TxQ, str, count);

	// start transmitting if it isint already
	if (!(UART0->C2 & UART0_C2_TIE_MASK)) {
		UART0->C2 |= UART0_C2_TIE(1);
	}
}


/*
 * Receive the Data from UART to Receive Buffer to store
 *
 * Parameters:
 *   str: String to Transmit over UART
 *	 count: The Length of the String to transmit
 * Returns:
 *   void
 */
size_t Receive_String(void* str, size_t count) {
	return Q_Dequeue(&RxQ, str, count);
}


/*
 * Helper function to Clear the Error flags
 *
 * Parameters:
 *   void
 * Returns:
 *   void
 */
static void clearUARTErrors(void) {
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);
}

/*
 * UART IRQ_Handler
 *
 * Parameters:
 *   void
 * Returns:
 *   void
 */
void UART0_IRQHandler(void) {

	uint8_t ch;

	if (UART0->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
		clearUARTErrors();
		ch = UART0->D;
	}

	if (UART0->S1 & UART0_S1_RDRF_MASK) {
			// received a character
			ch = UART0->D;
			Q_Enqueue(&RxQ, &ch, 1);
		}

	if ( (UART0->C2 & UART0_C2_TIE_MASK) && // transmitter interrupt enabled
				(UART0->S1 & UART0_S1_TDRE_MASK) ) {

		if(Q_Dequeue(&TxQ, &ch, 1)) {
			UART0->D = ch;
		} else {
			// queue is empty so disable transmitter interrupt
			UART0->C2 &= ~UART0_C2_TIE_MASK;
		}
	}
}
