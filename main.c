/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t xbuf[1];
uint8_t ybuf[1];
volatile uint8_t head_buf;
volatile uint8_t tail_buf;
volatile int msg_len = 0;
volatile int current_len = 0;
volatile uint8_t roleNum = 0;
volatile int looped = 0;
volatile int state = 0; // 0 = Waiting, 1 = Waiting for Length, 2 = Receiving Message, 3 = Sending Message
volatile int message_received = 0;
volatile uint8_t* msg;
volatile uint8_t ID = 0x30;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (state == 0 && huart == &huart1){
		HAL_UART_Receive_IT(&huart1, xbuf, 1);
		tail_buf = xbuf[0];
		if (tail_buf == 0x0D){
			state = 1;
			current_len = 0;
			if (looped == 0){
				roleNum = 1;
			}
		}
	} else if (state == 0 && huart == &huart2){
		HAL_UART_Receive_IT(&huart2, ybuf, 1);
		head_buf = ybuf[0];
		if (head_buf == 0x0D){
			state = 1;
			current_len = 0;
			roleNum = 2;
			ID = 0x30;
		}
	} else if (state == 1 && huart == &huart1){
		if (roleNum == 2 && looped == 1){
			HAL_UART_Receive_IT(&huart1, xbuf, 1);
			msg_len = xbuf[0];
			msg = (uint8_t*)malloc(msg_len);
			state = 2;
		} else if (roleNum == 1){
			HAL_UART_Receive_IT(&huart1, xbuf, 1);
			msg_len = xbuf[0];
			msg = (uint8_t*)malloc(msg_len);
			state = 2;
		}
	} else if (state == 1 && huart == &huart2 && roleNum == 2){
		HAL_UART_Receive_IT(&huart2, ybuf, 1);
		msg_len = ybuf[0];
		msg = (uint8_t*)malloc(msg_len);
		state = 2;
	} else if (state == 2 && huart == &huart1){
		if (roleNum == 2 && looped == 1){
			HAL_UART_Receive_IT(&huart1, xbuf, 1);
			tail_buf = xbuf[0];
			current_len++;
			msg[current_len - 1] = tail_buf;
			if (current_len >= msg_len){
			    state = 3;
			}
		} else if (roleNum == 1){
			HAL_UART_Receive_IT(&huart1, xbuf, 1);
			tail_buf = xbuf[0];
			current_len++;
			msg[current_len - 1] = tail_buf;
			if (current_len >= msg_len){
			    state = 3;
			}
		}
	} else if (state == 2 && huart == &huart2 && roleNum == 2 && looped == 0){
		HAL_UART_Receive_IT(&huart2, ybuf, 1);
		head_buf = ybuf[0];
		current_len++;
		msg[current_len - 1] = head_buf;
		if (current_len >= msg_len){
		    state = 3;
		}
	}
}

void transmit(uint8_t* msg, uint8_t msg_len, int toComp){
	uint8_t return_char = 0x0D;
	if (toComp == 1){
		HAL_UART_Transmit(&huart2, &return_char, 1, HAL_MAX_DELAY);
		HAL_Delay(10);
		HAL_UART_Transmit(&huart2, &msg_len, 1, HAL_MAX_DELAY);
		HAL_Delay(10);
	} else {
		HAL_UART_Transmit(&huart1, &return_char, 1, HAL_MAX_DELAY);
		HAL_Delay(10);
		HAL_UART_Transmit(&huart1, &msg_len, 1, HAL_MAX_DELAY);
		HAL_Delay(10);
	}
	for (int i = 0; i < msg_len; i++){
		if (toComp == 1){
			HAL_UART_Transmit(&huart2, &msg[i], 1, HAL_MAX_DELAY);
		} else {
			HAL_UART_Transmit(&huart1, &msg[i], 1, HAL_MAX_DELAY);
		}
		HAL_Delay(10);
	}
	state = 0;
}

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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(250);
  HAL_UART_Receive_IT(&huart1, xbuf, 1);
  HAL_UART_Receive_IT(&huart2, ybuf, 1);

  /*
  uint8_t* msg = NULL;
  int bytes;
  int looped = 0;
  */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (state == 3) {
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		  HAL_Delay(250);
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

		  if (roleNum == 1){
			  ID = msg[msg_len - 1] + 1;
		  }
		  int new_len = msg_len + 5;
		  uint8_t* new_msg = (uint8_t*)malloc(new_len);
		  for (int i = 0; i < msg_len; i++){
		      new_msg[i] = msg[i];
		  }
		  new_msg[msg_len] = 'B';
		  new_msg[msg_len + 1] = '0';
		  new_msg[msg_len + 2] = '6';
		  new_msg[msg_len + 3] = '_';
		  new_msg[msg_len + 4] = ID;

		  uint8_t** ptr = &new_msg;
		  uint8_t byte_len = new_len;
		  if (roleNum == 2 && looped == 0){
			  transmit(*ptr, byte_len, 0);
			  looped = 1;
		  } else if (roleNum == 2 && looped == 1){
			  transmit(*ptr, byte_len, 1);
			  looped = 0;
		  } else if (roleNum == 1){
			  transmit(*ptr, byte_len, 0);
		  }
	  }

    /* USER CODE END WHILE */

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
