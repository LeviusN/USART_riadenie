/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void proccesDmaData(uint8_t element);
/* Space for your global variables. */

	// type your global variables here:
int mode = 0;
uint8_t D=0;
int PWM_intenz = 0;
int PWM_intenz_vanted = 0;
int direction = 1;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* Space for your local variables, callback registration ...*/

  	  //type your code here:
  USART2_RegisterCallback(proccesDmaData);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  LL_mDelay(1000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/* USER CODE BEGIN 4 */
/*
 * Implementation of function processing data received via USART.
 */
void proccesDmaData(uint8_t element) {
	static uint8_t dollar; static uint8_t index; static char dmaDATA[36];
//kontrola
	dmaDATA[index] = element;
	index++;
	if (index == 36) {
		dollar = 0;
		index = 0;
	}
	if (element == '$') {//pocitanie zaciatku a konca
		dollar++;
	}else if (dollar == 0) {
			return;
		}

//zaciatok spracovavacieho kodu
	if ((element == '$') && (dollar == 2)) {//ak dva dolare potom pokracovat
		uint8_t length=index;
		int state=0;
		char *data=dmaDATA;
		char fillis[length-1];
		char automatic[] = "auto";
		char manual[] = "manual";
		char PWd[] = "PWM";
		char stringPWM[]="NNN";
		int i = 1;
		uint8_t dimension;
//data
		while(i < length-1){//spracovanie stavu manual a auto
			i++;
			fillis[i-1] = data[i];
			fillis[length-2] = 0;
		}
		i = 0;
		while(i < length-3){//spracovanie stavu PWM bez cisla
			i++;
			stringPWM[i] = fillis[i];
			stringPWM[3] = 0;
		}
//urcenie modu
		state=(!strcmp(fillis, manual))*1;//ci sa jedna o manual
		state=state+(!strcmp(fillis, automatic))*2;//ci sa jedna o automat
		state=state+(!strcmp(stringPWM, PWd))*3;//ci sa jedna o PWM
		switch(state){
			case 1://manual
				mode = 1;
				PWM_intenz_vanted = 99;
				break;
			case 2://automat
				mode = 0;
				break;
			case 3://PWM
				PWM_intenz_vanted = (fillis[3]-48) * 10+(fillis[4]-48);//vypocet hodnoty
				break;
			default:
				break;
		}
//vypisanie do Putty
		switch(mode){
		uint8_t fillis[180];
			case 0:;//automat
				dimension = sprintf((char*) fillis,"State = auto with variable intenzity 0-%d\n\r",99);
				USART2_PutBuffer(fillis, dimension);
				break;
			case 1: ;//manual
				dimension = sprintf((char*) fillis,"State = manual and intenzity = %d\n\r",PWM_intenz_vanted);
				USART2_PutBuffer(fillis, dimension);
				break;
			default:
				break;
		}
		dollar = 0;
		index = 0;
	}
	}


void setDutyCycle(uint8_t D){
    TIM2->CCR1 = D;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
