/*
 * terminal_uart.c
 *
 *  Created on: 19 déc. 2022
 *      Author: paul_leveque
 */

#include "terminal_uart.h"
#include "string.h"

/* Private Variable --------------------------------------------------*/
//static osMessageQDef(streamQ, 1, uint32_t); // Define stream queue
//static osMessageQId streamQ_ID;

uint8_t streamToggleFlag=0;


/* Public function definitions Variable ------------------------------*/

/**
  * @brief  Terminal : permet de communiquer avec la machine, et de lui passer les commandes read, dump, stream et help
  * @param  None
  * @retval MeSN_StatusTypedef
  */
MeSN_StatusTypedef terminal(void){
	term_mess_receivedTypeDef messReceived;
	uint8_t i_termIndex=0;
	MeSN_StatusTypedef retVal;


	// Basicaly this is the os task
	MESN_UART_PutString_Poll((uint8_t*)"\r\nbalancing robot terminal :~$ ");
	for (i_termIndex = 0; i_termIndex < TERM_BUFFER_SIZE; i_termIndex++) {
		messReceived.stringReceived[i_termIndex]= '\0';
	}

	// take input from terminal
	MESN_UART_GetString(messReceived.stringReceived,osWaitForever);
	if(messReceived.stringReceived[0]=='\n'){
		for(int i= 0;i<TERM_BUFFER_SIZE-1;i++){
			messReceived.stringReceived[i]=messReceived.stringReceived[i+1];
		}
	}

	switch (commandAnalyser(&messReceived)) {
	case read:
		termCmdread();
		retVal = USER_OK;
		break;
	case dump:
		termCmddump();
		retVal = USER_OK;
		break;
	case stream:
		streamToggleFlag = !streamToggleFlag;
		termCmdstream();
		retVal = USER_OK;
		break;
	case help:
		termCmdHelp();
		retVal = USER_OK;
		break;
	case home:
		retVal = USER_OK;
		break;
	default:
		retVal = USER_ERROR;
		break;
	}
	return retVal;
}

/**
 * @brief  Permet de communiquer avec la machine, de filtrer la commande reçu et de l'interpréter,
 * 		   pour lui donner les commandes read, dump, stream et help.
 * @param  term_mess_receivedTypeDef* : commande envoyée par l'utilisateur
 * @retval term_cmd : renvoit la commande interprétée
 */
term_cmd commandAnalyser(term_mess_receivedTypeDef *messReceived){
	// uint8_t messToSend[32];	// messaging buffer
	term_cmd retVal = error;
	// TODO make sure to check the whole word, not just the fisrt letter

	if(!strcmp(messReceived->stringReceived,"read")){
		retVal =  read;
	}else if(!strcmp(messReceived->stringReceived,"dump")){
		retVal =  dump;
	}else if(!strcmp(messReceived->stringReceived,"stream")){
		retVal =  stream;		// Ajouter variables pour arrêter ou lancer le stream
	}else if(!strcmp(messReceived->stringReceived,"help")){
		retVal =  help;
	}else if(!strcmp(messReceived->stringReceived,"")){
			retVal =  home;
	}else{
		retVal =  error;
	}


	if (retVal == error) {
		MESN_UART_PutString_Poll(messReceived->stringReceived);
		MESN_UART_PutString_Poll((uint8_t*)" is not a command");
	}
	return retVal;
}

/**
 * @brief  Fonction pour lire la dernière valeur du buffer circulaire, et l'afficher sur le terminal
 * @param  None
 * @retval None
 */
void termCmdread(void){
	// MESN_UART_PutString_Poll((uint8_t*)"\r\nfunction not implemented yet");
	uint8_t messToSend[20];
	sprintf(messToSend,(uint8_t*)"%ld",circular_buf_read_1(&bufferIMU));
	MESN_UART_PutString_Poll(messToSend);
}

/**
 * @brief : Fonction pour lire les 100 dernière valeurs du buffer circulaire, et les afficher sur le terminal sous forme de tableau
 * Nous affichons les valeurs les plus anciennes jusqu'au plus récentes dans le tableau.
 * @param : None
 * @retval : None
 */
void termCmddump(void){
	int32_t *tab_read;
	uint8_t messToSend[20];
	int32_t i=0,j=0;
	tab_read=circular_buf_read_100(&bufferIMU);

	for (i = 0; i < CIRC_BUFFER_ELMT_SIZE/10; i++) {
		for (j = 0; j < CIRC_BUFFER_ELMT_SIZE/10; j++) {
			sprintf(messToSend,(uint8_t*)"%10ld ",tab_read[i*10+j]);
			MESN_UART_PutString_Poll(messToSend);
		}
		MESN_UART_PutString_Poll((uint8_t*)"\n\r");
	}
}

/**
 * @brief  Fonction pour lire toute les 10 ms la dernière valeur du buffer circulaire, et l'afficher sur le terminal
 * @param  None
 * @retval None
 */
void termCmdstream(void){
	osEvent evt;
	uint8_t messToSend[100];
	uint8_t stopChar[2];

	// A mettre dans une boucle infinie

	while (streamToggleFlag != 0 ){
		evt = osMessageGet(MsgBox_Stream, 1);
		MESN_UART_GetString(stopChar, 10);
		if (stopChar[0] == 's' || stopChar[0] == 'q') {
			streamToggleFlag = !streamToggleFlag;

			break;
		}
		sprintf(messToSend,(uint8_t*)"%6ld\r",evt.value.signals);
		MESN_UART_PutString_Poll(messToSend);
	}
}

/**
 * @brief  Fonction pour afficher les commandes disponibles sur le terminal
 * @param  None
 * @retval None
 */
void termCmdHelp(void){  // works
	MESN_UART_PutString_Poll((uint8_t*) "\r\nUsable Commands for the time being : \r\n");
	MESN_UART_PutString_Poll((uint8_t*) "\r\n\t -> read :");
	MESN_UART_PutString_Poll((uint8_t*) "\r\n\t\t\t Retourne la dernière valeur mesurée de l’angle d’inclinaison du robot (en milli-Degrés) \n");

	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t -> dump :");
	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t\t\t Retourne les 100 dernières valeurs mesurées (en milli-Degrés) \n");

	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t -> stream :");
	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t\t\t Retourne en continu, toutes les 10 ms, la dernière valeur mesurée (en milli-Degrés) \n\r \
		en effaçant côté terminal la précédente valeur affichée. La sortie du mode stream \n\r	\
		s’affectue en appuyant sur la touche <enter> côté terminal ou q ou s \n\r ");

	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t -> help :");
	MESN_UART_PutString_Poll((uint8_t*)"\r\n\t\t\t Retourne la liste des commandes supportées par l’application et leur descriptions \n");
}




