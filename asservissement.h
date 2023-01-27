/*
 * asservissement.h
 *
 *  Created on: 22 déc. 2022
 *      Author: arthu
 */

#ifndef ASSERVISSEMENT_H_
#define ASSERVISSEMENT_H_

#include "cmsis_os.h"
#include "stdio.h"
#include "lsm6ds3.h"
#include "libSBR_autom_obs-corr.h"
#include "MotorDriver.h"
#include "global_variable.h"
#include "errorStatus.h"

#define SIZE_QUEUE_ANGLE_ENREGISTREMENT 30 		// Dépend de la fréquence à laquelle nous appelons la tâche enregistrement


// Function prototype

MeSN_StatusTypedef asservissement_moteur();


#endif /* ASSERVISSEMENT_H_ */
