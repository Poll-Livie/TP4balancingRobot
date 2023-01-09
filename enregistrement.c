/*
 * enregistrement.c
 *
 *  Created on: 22 déc. 2022
 *      Author: paul_leveque
 */

#include "enregistrement.h"

circular_buf_t bufferIMU;

void enregistrement_init(){

	circular_buf_init(&bufferIMU);
}

enr_Error_Status enregistrement(){
	enr_Error_Status retVal = enr_Error;
	osEvent evt;
	int32_t angle;

	evt = osMessageGet(MsgBox_Angle_Enregistrement, 0);
	if(evt.status == osEventMessage){
		angle=evt.value.signals;
	}								// Fonctionnement queue vérifié

	// Ecriture de l'angle dans le buffer
	circular_buf_write_1(&bufferIMU,angle);

	// Envoi Stream
	envoiComStream(angle);

	verifAngle(angle);

	return retVal;
}

enr_Error_Status envoiComStream(int32_t IMU_Val){
	enr_Error_Status retVal = enr_Error;
	osMessagePut(MsgBox_Stream, IMU_Val,0);		// Fonctionnement queue vérifié
	return retVal;
}

enr_Error_Status verifAngle(int32_t IMU_Val){
	enr_Error_Status retVal = enr_Error;

	if (IMU_Val >= 25) {
		retVal = enr_valueAbove25Degrees;
	}else {
		retVal = enr_valueUnder25Degrees;
	}
	return retVal;
}
