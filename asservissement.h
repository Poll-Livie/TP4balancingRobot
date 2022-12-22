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

// Structures

typedef enum asser_Error_Status {
	asser_Error,
	asser_ok
}asser_Error_Status;

// Function prototype

asser_Error_Status asservissement_moteur();


#endif /* ASSERVISSEMENT_H_ */
