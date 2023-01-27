/*
 * enregistrement.c
 *
 *  Created on: 22 déc. 2022
 *      Author: paul_leveque
 */

#include "enregistrement.h"

circular_buf_t bufferIMU;

/**
  * @brief  Enregistrement initialization
  * @param  None
  * @retval None
  */
void enregistrement_init(){

	// Initialisation et création du buffer_circulaire
	circular_buf_init(&bufferIMU);
}

/**
  * @brief  Enregistrement des angles observés dans un buffer circulaire
  * @param  None
  * @retval MeSN_StatusTypedef
  */
MeSN_StatusTypedef enregistrement(){
	MeSN_StatusTypedef retVal = USER_ERROR;
	osEvent evt;
	int32_t angle;

	// On récupère l'angle venant de la tâche asservissement
	evt = osMessageGet(MsgBox_Angle_Enregistrement, osWaitForever);
	if(evt.status == osEventMessage){
		angle=evt.value.signals;
		// Ecriture de l'angle dans le buffer
		circular_buf_write_1(&bufferIMU,angle);

		// Envoi Stream
		envoiComStream(angle);

		verifAngle(angle);
		// at this point everything worked
		retVal = USER_OK;
	}
	return retVal;
}

/**
  * @brief  Commande Stream : envoie dans une queue de message que le terminal va lire
  * @param  IMU_Val : angle observé
  * @retval MeSN_StatusTypedef
  */
MeSN_StatusTypedef envoiComStream(int32_t IMU_Val){
	MeSN_StatusTypedef retVal = USER_OK;
	osMessagePut(MsgBox_Stream, IMU_Val,0);
	return retVal;
}

/**
  * @brief  Vérifie l'angle d'inclinaison du dispositif, et allume une LED si l'angle observé dépasse 25°
  * @param  IMU_Val : angle observé
  * @retval None
  */
void verifAngle(int32_t IMU_Val){

	static uint32_t counter = 0;


	// if IMU val > 25° LED ON
	if (IMU_Val > 25000 || IMU_Val < -25000) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		counter = 0;
	}
	// Else LED OFF
	else {
		if (counter >= 0 && counter < 90) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		}
		else if (counter < 100) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			counter = 0;
		}
	}
	counter++;

}




