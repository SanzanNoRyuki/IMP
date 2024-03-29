/* Header file with all the essential definitions for a given type of MCU */
#include "MK60DZ10.h"

#include "alphabet.h"	// Alphabet
#include "messages.h"	// Predefined messages & matrices
#include <string.h>		// Strlen
#include <stdbool.h>    // Bool

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))


/* Constants specifying delay loop duration */
#define	tdelay1			100
#define tdelay2 		25


/* Configuration of the necessary MCU peripherals */
void SystemConfig() {
	/* Turn on all port clocks */
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;

	/* Set corresponding PTA pins (column activators of 74HC154) for GPIO functionality */
	PORTA->PCR[8] = ( 0|PORT_PCR_MUX(0x01) );  // A0
	PORTA->PCR[10] = ( 0|PORT_PCR_MUX(0x01) ); // A1
	PORTA->PCR[6] = ( 0|PORT_PCR_MUX(0x01) );  // A2
	PORTA->PCR[11] = ( 0|PORT_PCR_MUX(0x01) ); // A3

	/* Set corresponding PTA pins (rows selectors of 74HC154) for GPIO functionality */
	PORTA->PCR[26] = ( 0|PORT_PCR_MUX(0x01) );  // R0
	PORTA->PCR[24] = ( 0|PORT_PCR_MUX(0x01) );  // R1
	PORTA->PCR[9] = ( 0|PORT_PCR_MUX(0x01) );   // R2
	PORTA->PCR[25] = ( 0|PORT_PCR_MUX(0x01) );  // R3
	PORTA->PCR[28] = ( 0|PORT_PCR_MUX(0x01) );  // R4
	PORTA->PCR[7] = ( 0|PORT_PCR_MUX(0x01) );   // R5
	PORTA->PCR[27] = ( 0|PORT_PCR_MUX(0x01) );  // R6
	PORTA->PCR[29] = ( 0|PORT_PCR_MUX(0x01) );  // R7

	/* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
	PORTE->PCR[28] = ( 0|PORT_PCR_MUX(0x01) ); // #EN

	/* Change corresponding PTA port pins as outputs */
	PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

	/* Change corresponding PTE port pins as outputs */
	PTE->PDDR = GPIO_PDDR_PDD( GPIO_PIN(28) );
}

/* Variable delay loop */
void delay(int t1, int t2) {
	int i, j;
	for(i=0; i<t1; i++) {
		for(j=0; j<t2; j++);
	}
}

/* Conversion of requested column number into the 4-to-16 decoder control.  */
void column_select(unsigned int col_num)
{
	unsigned i, result, col_sel[4];
	for (i = 0; i < 4; i++) {
		result     = col_num / 2;	  // Whole-number division of the input number
		col_sel[i] = col_num % 2;
		col_num    = result;

		switch(i) {
			// Selection signal A0
		   case 0:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(8))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(8)));
				break;
			// Selection signal A1
			case 1:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(10))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(10)));
				break;
			// Selection signal A2
			case 2:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(6))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(6)));
				break;
			// Selection signal A3
			case 3:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(11))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(11)));
				break;
			// Otherwise nothing to do...
			default:
				break;
		}
	}
}

void lightup(unsigned int row_num) {
	switch (row_num) {
		case 0:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
			break;
		case 1:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
			break;
		case 2:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
			break;
		case 3:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
			break;
		case 4:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
			break;
		case 5:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
			break;
		case 6:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
			break;
		case 7:
			PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
			break;
		default:
			break;
	}
}

void blackout(unsigned int row_num) {
	switch (row_num) {
		case 0:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
			break;
		case 1:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
			break;
		case 2:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
			break;
		case 3:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
			break;
		case 4:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
			break;
		case 5:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
			break;
		case 6:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
			break;
		case 7:
			PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
			break;
		default:
			break;
	}
}

void show_message(int message, int column, int offset) {
	switch (message) {
		case 0:
			for (int row = 0; row < 8; row++) {
				if (matrix1[row][column + offset]) lightup(row);
				else blackout(row);
			}
			break;
		case 1:
			for (int row = 0; row < 8; row++) {
				if (matrix2[row][column + offset]) lightup(row);
				else blackout(row);
			}
			break;
		case 2:
			for (int row = 0; row < 8; row++) {
				if (matrix3[row][column + offset]) lightup(row);
				else blackout(row);
			}
			break;
		case 3:
			for (int row = 0; row < 8; row++) {
				if (matrix4[row][column + offset]) lightup(row);
				else blackout(row);
			}
			break;
		default:
			break;
	}
}

int main(void) {
    SystemConfig();
    PTA->PDOR |= GPIO_PDOR_PDO(0x3F000280);
    delay(tdelay1, tdelay2);

    int message = 0;											         /* MESSAGE INDEX 0-3 */
    int delayer = 0;
    int offset  = 0;
    bool button = false;
    for (;;) {
    	for (int column = 0; column < 16; column++) {
    		column_select(column);
    		show_message(message, column, offset);

    		PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28) );
    		delay(tdelay1, tdelay2);
    		PTE->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
    	}
		if (++delayer % 7 == 0) {
			if (++offset == strlen(messages[message]) * 7 - 15) offset = 0;
			delayer = 1;
		}
		if (button) if (++message >= 4) message = 0;
    }
    return 0;
}
