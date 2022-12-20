/*
 * terminal_uart.c
 *
 *  Created on: 19 déc. 2022
 *      Author: paul_leveque
 */

#include "terminal_uart.h"


void circular_buf_init(circular_buf_t *circBuf){
	circBuf->buffer_size = CIRC_BUFFER_ELMT_SIZE;
	circBuf->writing_head = 0;
	circBuf->reading_head = 0;
}

uint32_t circular_buf_read_1(circular_buf_t *circBuf){
	uint32_t retVal;
	retVal = circBuf->buffer_vals[circBuf->reading_head++];
	if (circBuf->reading_head > CIRC_BUFFER_ELMT_SIZE)
			circBuf->reading_head = 0;
	return retVal;
}

uint32_t* circular_buf_read_100(circular_buf_t *circBuf){
	uint32_t retVal[CIRC_BUFFER_ELMT_SIZE];
	uint32_t i ;
	for (i = 0; i < CIRC_BUFFER_ELMT_SIZE; ++i) {
		retVal[i]= circBuf->buffer_vals[circBuf->reading_head++];
		if (circBuf->reading_head > CIRC_BUFFER_ELMT_SIZE)
			circBuf->reading_head = 0;
	}
	return retVal;
}

void circular_buf_write_1(circular_buf_t *circBuf, uint32_t valToAdd){
	circBuf->buffer_vals[circBuf->writing_head++] = valToAdd;
	if (circBuf->writing_head > CIRC_BUFFER_ELMT_SIZE)
		circBuf->writing_head = 0;
}

term_Error_Status terminal(void){
	term_mess_receivedTypeDef messReceived;
	uint8_t i_termIndex=0;
	uint8_t inchar = 0;
	term_Error_Status retVal;

	// Basicaly this is the os task
	MESN_UART_PutString_Poll((uint8_t*)"\r\nbalancing robot terminal :~$ ");
	for (i_termIndex = 0; i_termIndex < TERM_BUFFER_SIZE; ++i_termIndex) {
		messReceived.stringReceived[i_termIndex]= '\0';
	}

	// take input from terminal
	// TODO FIXME make sure the first character is written again
	for (i_termIndex = 1; i_termIndex < TERM_BUFFER_SIZE; ++i_termIndex) {
		do {
			MESN_UART_GetString(&inchar,10);
		} while (inchar == '\0');
		if (inchar == '\n') break;
		else if (inchar == '\177') {
			if (i_termIndex > 1) {
				i_termIndex--;
			}
		}
		else {
			messReceived.stringReceived[i_termIndex] = inchar;
		}
	}

	messReceived.stringReceived[0] = ' ';

	switch (commandAnalyser(&messReceived)) {
		case read:
			termCmdread();
			retVal = term_cmd_ok;
			break;
		case dump:
			termCmddump();
			retVal = term_cmd_ok;
			break;
		case stream:
			termCmdstream();
			retVal = term_cmd_ok;
			break;
		case help:
			termCmdHelp();
			retVal = term_cmd_ok;
			break;
		default:
			break;
	}
	return retVal;
}

term_cmd commandAnalyser(term_mess_receivedTypeDef *messReceived){
	// uint8_t messToSend[32];	// messaging buffer

	// TODO make sure to check the whole word, not just the fisrt letter
	switch (messReceived->stringReceived[1]) {
		case 'r'
		:
			return read;
			break;
		case 'd':
			return dump;
			break;
		case 's':
			return stream;
			break;
		case 'h':
			return help;
			break;
		default:
			return home;
			break;
	}
	MESN_UART_PutString_Poll(messReceived->stringReceived);
	MESN_UART_PutString_Poll((uint8_t*)" is not a command");
	return home;
}

void termCmdread(void){
	// MESN_UART_PutString_Poll((uint8_t*)"\r\nfunction not implemented yet");
	uint8_t messToSend[8];
	sprintf(messToSend,"%ld",circular_buf_read_1(&buffer_IMU));
	// MESN_UART_PutString_Poll((uint8_t*)circular_buf_read_1(&buffer_IMU));
}

void termCmddump(void){
	MESN_UART_PutString_Poll((uint8_t*)"\r\nfunction not implemented yet");
}

void termCmdstream(void){
	MESN_UART_PutString_Poll((uint8_t*)"\r\nfunction not implemented yet");
}

void termCmdHelp(void){  // works
	MESN_UART_PutString_Poll((uint8_t*) "\r\nUsable Commands for the time being : \r\n");
	MESN_UART_PutString_Poll((uint8_t*) "\r\n\t -> read :");
	MESN_UART_PutString_Poll((uint8_t*) "\r\n\t\t\t Retourne la dernière valeur mesurée de l’angle d’inclinaison du robot (en milli-Degrés) \n");

	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t -> dump :");
	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t\t\t Retourne les 100 dernières valeurs mesurées (en milli-Degrés) \n");

	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t -> stream :");
	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t\t\t Retourne en continu, toutes les 10 ms, la dernière valeur mesurée (en milli-Degrés) \n \
		en effaçant côté terminal la précédente valeur affichée. La sortie du mode stream \n	\
		s’affectue en appuyant sur la touche <enter> côté terminal \n");

	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t -> help :");
	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t\t\t Retourne la liste des commandes supportées par l’application et leur descriptions \n");
}




