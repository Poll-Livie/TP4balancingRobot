/*
 * enregistrement.h
 *
 *  Created on: 22 déc. 2022
 *      Author: paul_leveque
 */

#ifndef MESN_BALANCINGROBOT_ENREGISTREMENT_H_
#define MESN_BALANCINGROBOT_ENREGISTREMENT_H_


#include "global_variable.h"
#include "circular_buffer.h"

typedef enum enr_Error_Status {
	enr_Error,
	enr_cmd_ok
}enr_Error_Status;

enr_Error_Status enregistrement(uint32_t IMU_Val);


#endif /* MESN_BALANCINGROBOT_ENREGISTREMENT_H_ */
