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

void verifAngle(int32_t IMU_Val){

	enr_Error_Status retVal = enr_Error;
	static uint32_t counter = 0;
	int32_t angleObs = 0;

	// angleObs = autoAlgo_angleObs(0, IMU_Val);
	// if IMU val > 25° LED ON
	if (IMU_Val > 25000 || IMU_Val < -25000) {
		retVal = enr_valueAbove25Degrees;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		counter = 0;
	}
	// Else LED OFF
	else {
		retVal = enr_valueUnder25Degrees;
		if (counter >= 0 && counter < 9) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		}
		else if (counter < 10) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			counter = 0;
		}
	}
	counter++;
	// return retVal;

}




