/*
 * circular_buffer.h
 *
 *  Created on: 20 déc. 2022
 *      Author: paul_leveque
 */

#ifndef MESN_BALANCINGROBOT_CIRCULAR_BUFFER_H_
#define MESN_BALANCINGROBOT_CIRCULAR_BUFFER_H_

#include "stdint.h"
#include "cmsis_os.h"

#define CIRC_BUFFER_ELMT_SIZE 100

typedef struct circular_buf_t{
	int32_t buffer_vals[CIRC_BUFFER_ELMT_SIZE];
	uint8_t writing_head;
	uint8_t reading_head;
	uint32_t buffer_size;
}circular_buf_t;

// initiates all values to 0,
// and buffer_size to CIRC_BUFFER_ELMT_SIZE
void circular_buf_init(circular_buf_t *circBuf);

// read one value to the buffer
// increments reading head
int32_t circular_buf_read_1(circular_buf_t *circBuf);

/*
 * @brief : read all values from the buffer
 * 			increments reading head
 * @retval : returns a table of 100 vals
 */
int32_t* circular_buf_read_100(circular_buf_t *circBuf);

// add one value to the buffer
// increments writing head
void circular_buf_write_1(circular_buf_t *circBuf, int32_t valToAdd);



#endif /* MESN_BALANCINGROBOT_CIRCULAR_BUFFER_H_ */
