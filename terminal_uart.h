/*
 * terminal_uart.h
 *
 *  Created on: 19 déc. 2022
 *      Author: paul_leveque
 */

#ifndef MESN_BALANCINGROBOT_TERMINAL_UART_H_
#define MESN_BALANCINGROBOT_TERMINAL_UART_H_

#include "cmsis_os.h"
#include "stdio.h"
#include "mesn_uart.h"
#include "global_variable.h"
#include "circular_buffer.h"
#include "errorStatus.h"

#define TERM_BUFFER_SIZE 32

/*
 *
 */
typedef enum term_cmd {
	home,
	error,
	read,
	dump,
	stream,
	help
}term_cmd;


typedef struct term_mess_receivedTypeDef{
	uint8_t stringReceived[TERM_BUFFER_SIZE]; 		// character string received (written in terminal)
	term_cmd cmdReceived;
}term_mess_receivedTypeDef;

// circular_buf_t bufferIMU;


/*
 * @brief : Basicaly this is the os task
 */
MeSN_StatusTypedef terminal(void);

/*
 * @brief : takes the incomming send values from uart, and translates it to the an term_cmd
 */
term_cmd commandAnalyser(term_mess_receivedTypeDef *messReceived);

void termCmdread(void);

void termCmddump(void);

void termCmdstream(void);

void termCmdHelp(void);

#endif /* MESN_BALANCINGROBOT_TERMINAL_UART_H_ */
