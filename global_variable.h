/*
 * global_variable.h
 *
 *  Created on: 20 déc. 2022
 *      Author: paul_leveque
 */

#ifndef MESN_BALANCINGROBOT_GLOBAL_VARIABLE_H_
#define MESN_BALANCINGROBOT_GLOBAL_VARIABLE_H_

#include "circular_buffer.h"

extern circular_buf_t bufferIMU;

/* Queue de message --------------------------------------------------*/
extern osMessageQId MsgBox_Angle_Enregistrement;	// Queue de message entre la tâche asservissement et la tâche enregistrement
extern osMessageQId MsgBox_Stream;

/* Mutex ------------------------------------------------------------*/
extern osMutexId MutexMoteur_ID;

/* variables --------------------------------------------------------*/

#endif /* MESN_BALANCINGROBOT_GLOBAL_VARIABLE_H_ */
