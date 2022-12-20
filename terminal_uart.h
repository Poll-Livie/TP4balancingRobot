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

#define TERM_BUFFER_SIZE 32
#define CIRC_BUFFER_ELMT_SIZE 100


/*
 *
 */
typedef enum term_cmd {
	home,
	read,
	dump,
	stream,
	help
}term_cmd;

typedef enum term_Error_Status {
	term_Error,
	term_cmd_ok
}term_Error_Status;

typedef struct term_mess_receivedTypeDef{
	uint8_t stringReceived[TERM_BUFFER_SIZE]; 		// character string received (written in terminal)
	term_cmd cmdReceived;
}term_mess_receivedTypeDef;


typedef struct circular_buf_t{
	uint32_t buffer_vals[CIRC_BUFFER_ELMT_SIZE];
	uint8_t writing_head;
	uint8_t reading_head;
	uint32_t buffer_size;
}circular_buf_t;

// initiates all values to 0,
// and buffer_size to CIRC_BUFFER_ELMT_SIZE
void circular_buf_init(circular_buf_t *circBuf);

// read one value to the buffer
// increments reading head
uint32_t circular_buf_read_1(circular_buf_t *circBuf);

/*
 * @brief : read all values from the buffer
 * 			increments reading head
 * @retval : returns a table of 100 vals
 */
uint32_t* circular_buf_read_100(circular_buf_t *circBuf);

// add one value to the buffer
// increments writing head
void circular_buf_write_1(circular_buf_t *circBuf, uint32_t valToAdd);


/*
 * @brief : Basicaly this is the os task
 */
term_Error_Status terminal(void);

/*
 * @brief : takes the incomming send values from uart, and translates it to the an term_cmd
 */
term_cmd commandAnalyser(term_mess_receivedTypeDef *messReceived);

void termCmdread(void);

void termCmddump(void);

void termCmdstream(void);

void termCmdHelp(void);

#endif /* MESN_BALANCINGROBOT_TERMINAL_UART_H_ */
