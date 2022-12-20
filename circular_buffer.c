/*
 * circular_buffer.c
 *
 *  Created on: 20 déc. 2022
 *      Author: paul_leveque
 */

#include "circular_buffer.h"

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
