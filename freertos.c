/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "MotorDriver.h"					// Pilote du moteur
#include "lsm6ds3.h"						// Pilote de la centrale inertielle
#include "mesn_uart.h"						// Pilote de la liaison UART
#include "libSBR_autom_obs-corr.h"			// Algo observateur et correcteur
#include "enregistrement.h"
#include "terminal_uart.h"
#include "asservissement.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SIZE_QUEUE_ANGLE_ENREGISTREMENT 30
#define SIZE_QUEUE_STREAM 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osMessageQDef(MsgBox_Angle_Enregistrement, SIZE_QUEUE_ANGLE_ENREGISTREMENT, int32_t);
osMessageQId MsgBox_Angle_Enregistrement;

osMessageQDef(MsgBox_Stream, SIZE_QUEUE_STREAM, int32_t);
osMessageQId MsgBox_Stream;

osMutexId MutexMoteur_ID;

/* USER CODE END Variables */
// osThreadId defaultTaskHandle;

osThreadId asservissementTaskHandle;
osThreadId enregistrementTaskHandle;
osThreadId terminalTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void asservissementTask(void const * argument);
void enregistrementTask(void const * argument);
void terminalTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	MESN_UART_PutString_Poll((uint8_t*)"\r\nERROR : stack overflow from task");
	MESN_UART_PutString_Poll((uint8_t*)pcTaskName);
	while(1);
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
	MESN_UART_PutString_Poll((uint8_t*)"\r\nERROR : Heap full!");
	while(1);
}
/* USER CODE END 5 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */

	osMutexDef(MutexMoteur);
	MutexMoteur_ID = osMutexCreate(osMutex(MutexMoteur));
	osMutexRelease(MutexMoteur_ID);

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	MsgBox_Angle_Enregistrement = osMessageCreate(osMessageQ(MsgBox_Angle_Enregistrement), NULL);
	MsgBox_Stream = osMessageCreate(osMessageQ(MsgBox_Stream), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */

  // osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  // defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	osThreadDef(ASSERVISSEMENT, asservissementTask, osPriorityHigh, 0, 256);
	asservissementTaskHandle = osThreadCreate(osThread(ASSERVISSEMENT), NULL);

	osThreadDef(ENREGISTREMENT, enregistrementTask, osPriorityNormal, 0, 256);
	enregistrementTaskHandle = osThreadCreate(osThread(ENREGISTREMENT), NULL);

	osThreadDef(TERMINAL, terminalTask, osPriorityLow, 0, 256);
	terminalTaskHandle = osThreadCreate(osThread(TERMINAL), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	uint8_t LSM6DS3_Res = 0;
	uint8_t tempString[100];
	int32_t i=0;	// Pour tester

	/* Init des periphs externes */
	MotorDriver_Init();
	MESN_UART_Init();
	if(LSM6DS3_begin(&LSM6DS3_Res) != IMU_SUCCESS){
		MESN_UART_PutString_Poll((uint8_t*)"\r\nIMU Error !");
		while(1);
	}

	/* Test des periphs externes */
	sprintf((char*)tempString, "\r\nInit done. LSM6DS3 reg = %02x", LSM6DS3_Res);
	MESN_UART_PutString_Poll(tempString);
	MotorDriver_Move(200);

	/* Test algo autom */
	sprintf((char*)tempString, "\r\nAngle = %ldmDeg", autoAlgo_angleObs(50,5));
	MESN_UART_PutString_Poll(tempString);

	enregistrement_init();		// Initialisation à pas OUBLIER  !!

	while(1){
		i++;
		osMessagePut(MsgBox_Angle_Enregistrement, i, 0);
		enregistrement(2);
	}

  /* Infinite loop */
  for(;;)
  {
	// MESN_UART_GetString(tempString,osWaitForever);
	MESN_UART_PutString_Poll((uint8_t*)"On est dans le main start default task");
	// HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


void asservissementTask(void const * argument){
	uint32_t tick =0;
	uint8_t LSM6DS3_Res = 0;
	uint8_t tempString[100];

	// initialisation du buffer circulaire des 100 vals à 0;
	MotorDriver_Init();
	MESN_UART_Init();
	enregistrement_init();

	if(LSM6DS3_begin(&LSM6DS3_Res) != IMU_SUCCESS){
		MESN_UART_PutString_Poll((uint8_t*)"\r\nIMU Error !");
		while(1);
	}

	/* Test des periphs externes */
	sprintf((char*)tempString, "\r\nInit done. LSM6DS3 reg = %02x", LSM6DS3_Res);
	MESN_UART_PutString_Poll(tempString);
	MotorDriver_Move(200);

	/* Test algo autom */
	sprintf((char*)tempString, "\r\nAngle = %ldmDeg", autoAlgo_angleObs(50,5));
	MESN_UART_PutString_Poll(tempString);
	tick = osKernelSysTick();
	while(1){

		if (asservissement_moteur()==asser_Error) {
			MESN_UART_PutString_Poll((uint8_t*)"erreur asservissement");

		}
		osDelayUntil(&tick, 10);
	}
}


void enregistrementTask(void const * argument){
	uint32_t tick =0;


	tick = osKernelSysTick();
	while(1){

		if (enregistrement()==enr_Error) {
			MESN_UART_PutString_Poll((uint8_t*)"erreur enregistrement");
		}
		//osDelayUntil(&tick, 10);
	}
}


void terminalTask(void const * argument){
	uint32_t tick =0;
	tick = osKernelSysTick();
	while(1){

		if (terminal()==term_Error) {
			MESN_UART_PutString_Poll((uint8_t*)"erreur terminal");
		}
		//osDelayUntil(&tick, 1);
	}
}





/* USER CODE END Application */

