/*
 * terminal_uart.c
 *
 *  Created on: 19 déc. 2022
 *      Author: paul_leveque
 */

#include "terminal_uart.h"

/* Private Variable --------------------------------------------------*/
//static osMessageQDef(streamQ, 1, uint32_t); // Define stream queue
//static osMessageQId streamQ_ID;

void term_init(void){								// potentiellement pas utile
	/* Create RX queue between RX ISR and task */
	//streamQ_ID = osMessageCreate(osMessageQ(streamQ), NULL);
}

/* Public function definitions Variable ------------------------------*/
term_Error_Status terminal(void){
	term_mess_receivedTypeDef messReceived;
	uint8_t i_termIndex=0;
	uint8_t inchar = 0;
	term_Error_Status retVal;
	uint8_t streamToggleFlag=0;

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
		else if (inchar == '\177') {		// si on efface, le curseur du buffer circulaire ne s'incrémente pas
			if (i_termIndex > 1) {		// et si on efface quand il n'y a rien on ne fait rien
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
			streamToggleFlag = !streamToggleFlag;
			termCmdstream(streamToggleFlag);
			retVal = term_cmd_ok;
			break;
		case help:
			termCmdHelp();
			retVal = term_cmd_ok;
			break;
		default:
			retVal = term_Error;
			break;
	}
	return retVal;
}

term_cmd commandAnalyser(term_mess_receivedTypeDef *messReceived){
	// uint8_t messToSend[32];	// messaging buffer
	term_cmd retVal = error;
	// TODO make sure to check the whole word, not just the fisrt letter
	switch (messReceived->stringReceived[1]) {
		case 'r':
			retVal =  read;
			break;
		case 'd':
			retVal =  dump;
			break;
		case 's':
			retVal =  stream;		// Ajouter variables pour arrêter ou lancer le stream
			break;
		case 'h':
			retVal =  help;
			break;
		default:
			retVal =  error;
			break;
	}
	if (retVal == error) {
		MESN_UART_PutString_Poll(messReceived->stringReceived);
		MESN_UART_PutString_Poll((uint8_t*)" is not a command");
	}
	return retVal;
}

void termCmdread(void){
	// MESN_UART_PutString_Poll((uint8_t*)"\r\nfunction not implemented yet");
	uint8_t messToSend[20];
	sprintf(messToSend,(uint8_t*)"%ld",circular_buf_read_1(&bufferIMU));
	MESN_UART_PutString_Poll(messToSend);
}

void termCmddump(void){
	int32_t *tab_read;
	uint8_t messToSend[20];
	int32_t i=0,j=0;
	tab_read=circular_buf_read_100(&bufferIMU);

	for (i = 0; i < CIRC_BUFFER_ELMT_SIZE/10; ++i) {
		for (j = 0; j < CIRC_BUFFER_ELMT_SIZE/10; ++j) {
			sprintf(messToSend,(uint8_t*)"%10ld ",tab_read[i*10+j]);
			MESN_UART_PutString_Poll(messToSend);
		}
		MESN_UART_PutString_Poll((uint8_t*)"\n\r");
	}
}

void termCmdstream(uint8_t streamToggle){
	osEvent evt; // TODO maybe to put inside task ?
	uint8_t messToSend[100];

	// A mettre dans une boucle infinie

	while (streamToggle != 0 ){
		evt = osMessageGet(MsgBox_Stream, 100);

		sprintf(messToSend,(uint8_t*)"%ld\n\r",evt.value.signals);
		MESN_UART_PutString_Poll(messToSend);
	}
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




