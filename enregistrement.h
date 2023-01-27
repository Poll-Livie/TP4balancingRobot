/*
 * enregistrement.h
 *
 *  Created on: 22 déc. 2022
 *      Author: paul_leveque
 */

#ifndef MESN_BALANCINGROBOT_ENREGISTREMENT_H_
#define MESN_BALANCINGROBOT_ENREGISTREMENT_H_

#include "gpio.h"
#include "global_variable.h"
#include "circular_buffer.h"
#include "asservissement.h" // test avec l'angle
#include "errorStatus.h"


void enregistrement_init();

/*
 * @brief:
 *
 * @retVal: enr_Error_Status
 */
MeSN_StatusTypedef enregistrement();


/*
 * @brief:
 *
 * @retVal: enr_Error_Status
 */
MeSN_StatusTypedef envoiComStream(int32_t IMU_Val);

/*
 * @brief:
 *
 * @retVal: enr_Error_Status
 */

void verifAngle(int32_t IMU_Val);







#endif /* MESN_BALANCINGROBOT_ENREGISTREMENT_H_ */
