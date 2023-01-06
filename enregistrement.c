/*
 * enregistrement.c
 *
 *  Created on: 22 déc. 2022
 *      Author: paul_leveque
 */

#include "enregistrement.h"


enr_Error_Status attenteData(void){
	enr_Error_Status retVal = enr_Error;

	return retVal;
}

enr_Error_Status enregistrement(uint32_t IMU_Val){
	enr_Error_Status retVal = enr_Error;

	return retVal;
}

enr_Error_Status envoiComStream(uint32_t IMU_Val){
	enr_Error_Status retVal = enr_Error;

	return retVal;
}

enr_Error_Status verifAngle(uint32_t IMU_Val){
	enr_Error_Status retVal = enr_Error;

	if (IMU_Val >= 25) {
		retVal = enr_valueAbove25Degrees;
	}else {
		retVal = enr_valueUnder25Degrees;
	}
	return retVal;
}
