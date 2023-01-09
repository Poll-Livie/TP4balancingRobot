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

typedef enum enr_Error_Status {
	enr_Error,
	enr_ok,
	enr_wait,
	enr_send,
	enr_valueAbove25Degrees,
	enr_valueUnder25Degrees
}enr_Error_Status;

/*
 * @brief:  enable the waiting of the incomming data,
 * 			should be awake when Q_angle has value and this task time
 * @retVal: enr_Error_Status
 */
enr_Error_Status attenteData(void);


/*
 * @brief:
 *
 * @retVal: enr_Error_Status
 */
enr_Error_Status enregistrement(int32_t IMU_Val);


/*
 * @brief:
 *
 * @retVal: enr_Error_Status
 */
enr_Error_Status envoiComStream(int32_t IMU_Val);

/*
 * @brief:
 *
 * @retVal: enr_Error_Status
 */
void verifAngle(int32_t IMU_Val);






#endif /* MESN_BALANCINGROBOT_ENREGISTREMENT_H_ */
