/*
 * terminal_uart.c
 *
 *  Created on: 19 déc. 2022
 *      Author: paul_leveque
 */

#include "terminal_uart.h"

/* Private Variable --------------------------------------------------*/
static osMessageQDef(streamQ, 1, uint32_t); // Define stream queue
static osMessageQId streamQ_ID;

void term_init(void){
	/* Create RX queue between RX ISR and task */
	streamQ_ID = osMessageCreate(osMessageQ(streamQ), NULL);
}

/* Public function definitions Variable ------------------------------*/
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
	uint8_t messToSend[10];
	sprintf(messToSend,"%d",circular_buf_read_1(&bufferIMU));
	MESN_UART_PutString_Poll(messToSend);
}

void termCmddump(void){
	MESN_UART_PutString_Poll((uint8_t*)"\r\nfunction not implemented yet");
}

void termCmdstream(void){
	osEvent evt; // TODO maybe to put inside task ?
	uint8_t messToSend[11];

	MESN_UART_PutString_Poll((uint8_t*)"\r\nfunction not implemented yet");
	evt = osMessageGet(streamQ_ID, osWaitForever);

	sprintf(messToSend,"%d\n",evt.value);
	MESN_UART_PutString_Poll(messToSend);

	// TODO ajouter osMessagePut() dans le système avec l'ISR sur l'IMU
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




