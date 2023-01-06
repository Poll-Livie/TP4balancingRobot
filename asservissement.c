/*
 * asservissement.c
 *
 *  Created on: 22 déc. 2022
 *      Author: arthur
 */

#include "asservissement.h"


// A placer dans freeRTOS (Normal si il y a des erreurs)
/* Mutex ------------------------------------------------------------
	osMutexCreate(MutexMoteur);
	*/
/* Queues de messages --------------------------------------------------
	osMessageQDef(MsgBox_Angle_Enregistrement, SIZE_QUEUE_ANGLE_ENREGISTREMENT, int32_t);
	MsgBox_Angle_Enregistrement = osMessageCreate(osMessageQ(MsgBox_Angle_Enregistrement), NULL);

	*/

// Fin A placer dans freeRTOS

// Avant utilisation, bien initialiser le moteur et l'IMU

asser_Error_Status asservissement_moteur(){

	// ATTENTION les variables sont des entiers SIGNES

	asser_Error_Status retVal = asser_ok;

	int32_t angleObs;					//milli-deg -> angle entre l'axe du robot et l'axe vetical
	int32_t acc_mg;						//milli-g
	int32_t rotAng_mDegSec;				//milli-deg/s
	int32_t commandeAEnvoyer;			// pour mille de la vitesse max


	// Récupération des valeurs de acc_mg, rotAng_mDegSec et angleObs
	// et Mutex I2C déjà implémenté dans la librairie
	if(LSM6DS3_readMgAccelX(&acc_mg) != IMU_SUCCESS){	// Sur l'axe X
		retVal = asser_Error;
	}
	if(LSM6DS3_readMdpsGyroY(&rotAng_mDegSec) != IMU_SUCCESS){	// Sur l'axe Y
		retVal = asser_Error;
	}

	// Calcul l'angle de différence en milli-degrés
	angleObs = autoAlgo_angleObs(acc_mg, rotAng_mDegSec);
	// Calcul la commande à renvoyé au moteur
	commandeAEnvoyer = autoAlgo_commandLaw(angleObs, rotAng_mDegSec);

	// Envoi de commandeAEnvoyer au moteur et prends MutexMoteur
	osMutexWait(MutexMoteur,0);
	MotorDriver_Move(commandeAEnvoyer);
	osMutexRelease(MutexMoteur);

	// Envoi de angleObs dans la queue pour enregistrement
	osMessagePut(MsgBox_Angle_Enregistrement,angleObs,10);		// Timeout de 10 millisec

	return retVal;

}
