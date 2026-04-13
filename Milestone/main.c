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
#include <stdbool.h>
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

typedef enum {
  IDLE,
  WAIT_FOR_LEN,
  RECEIVING,
  TRANSMITTING
} State;

typedef enum {
  UNASSIGNED,
  TAIL,
  HEAD
} Role;

uint8_t xbuf[1];
uint8_t ybuf[1];
volatile int msg_len = 0;
volatile Role role = UNASSIGNED;
volatile bool looped = false;
volatile State state = IDLE;
//volatile int message_received = 0; -> this was only used once?
volatile uint8_t *msg;
volatile uint8_t ID = 0x30;

static uint8_t msg_buffer[256];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static int current_len = 0;

  if (state == IDLE && huart == &huart1)
  {
    if (xbuf[0] == 0x0D)
    {
      state = WAIT_FOR_LEN;
      current_len = 0;
      if (!looped)
      {
        role = TAIL;
      }
    }

    HAL_UART_Receive_IT(&huart1, xbuf, 1);
  }
  else if (state == IDLE && huart == &huart2)
  {
    if (ybuf[0] == 0x0D)
    {
      state = WAIT_FOR_LEN;
      current_len = 0;
      role = HEAD;
      ID = 0x30;
    }

    HAL_UART_Receive_IT(&huart2, ybuf, 1);
  }

  else if (state == WAIT_FOR_LEN && huart == &huart1)
  {
    if (role == HEAD && looped)
    {
      msg_len = xbuf[0];

      if (msg_len > 256){
    	  msg_len = 256;
      }
      msg = msg_buffer;

      state = RECEIVING;

      HAL_UART_Receive_IT(&huart1, xbuf, 1);
    }
    else if (role == TAIL)
    {
      msg_len = xbuf[0];

      if (msg_len > 256){
    	  msg_len = 256;
      }
      msg = msg_buffer;

      state = RECEIVING;

      HAL_UART_Receive_IT(&huart1, xbuf, 1);
    }
  }
  else if (state == WAIT_FOR_LEN && huart == &huart2)
  {
    msg_len = ybuf[0];

    if (msg_len > 256){
    	msg_len = 256;
    }
    msg = msg_buffer;

    state = RECEIVING;

    HAL_UART_Receive_IT(&huart2, ybuf, 1);
  }

  else if (state == RECEIVING && huart == &huart1)
  {
    current_len++;
    msg[current_len - 1] = xbuf[0];

    if (current_len >= msg_len)
    {
      state = TRANSMITTING;
    }

    HAL_UART_Receive_IT(&huart1, xbuf, 1);
  }

  else if (state == RECEIVING && huart == &huart2)
  {
    current_len++;
    msg[current_len - 1] = ybuf[0];

    if (current_len >= msg_len)
    {
      state = TRANSMITTING;
    }

    HAL_UART_Receive_IT(&huart2, ybuf, 1);
  }
}

void transmit(uint8_t *msg, uint8_t msg_len, bool toComp)
{
  uint8_t return_char = 0x0D;

  if (toComp)
  {
    HAL_UART_Transmit(&huart2, &return_char, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    HAL_UART_Transmit(&huart2, &msg_len, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
  }
  else
  {
    HAL_UART_Transmit(&huart1, &return_char, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    HAL_UART_Transmit(&huart1, &msg_len, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
  }
  for (int i = 0; i < msg_len; i++)
  {
    if (toComp)
    {
      HAL_UART_Transmit(&huart2, &msg[i], 1, HAL_MAX_DELAY);
    }
    else
    {
      HAL_UART_Transmit(&huart1, &msg[i], 1, HAL_MAX_DELAY);
    }
    HAL_Delay(1);
  }
  state = IDLE;
}

uint8_t checksum (uint8_t str[], int msg_len) {
    uint8_t temp = str[0];
    for (int i = 1; i < msg_len; i++) {
        temp = temp^str[i];
    }
    return temp;
}
/*
uint8_t extract (uint8_t input[]) {

    uint8_t check;
    char stringcheck[5];


    for (int i = strlen(input); i >= 0 ; i--) {
            // if statement clause idea, takes elements from this video: https://www.youtube.com/watch?v=p6uqGop26es&t=327s
            if (strstr((char *)&input[i], "_") == (char *)&input[i]) {
                strcpy(stringcheck, &input[i+2]);
                check = atoi(&input[i+2]);
                for (int j = i+2; j < i+2+strlen(stringcheck); j++)
                input[j] = input[j+strlen(stringcheck)];
                break;
            }
        }
        return check;
}
*/

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
  HAL_Delay(250); // delay to stop transient response/floating pins triggering ISR
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

  uint8_t *ptr;
  uint8_t byte_len;

  while (1)
  {
    if (state == TRANSMITTING)
    {
      // holding data for 250ms
      HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

      if (role == TAIL || looped)
      {
        //extract checksum from message
        uint8_t old_check = msg[msg_len - 1];

        //perform checksum and compare with old_checksum
        uint8_t check = checksum(msg, msg_len - 1);
        // if the checksums don't match turn on LD3 and then halt processes
        if (old_check != check)
        {
          HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
          while(1){
        	  HAL_Delay(1000);
          }
        }
      }

      if (role == TAIL)
      {
        ID = msg[msg_len - 2] + 1;
      }


      int new_len;
      uint8_t* new_msg;
      if (looped == false){
    	  if (role == HEAD){
              new_len = msg_len + 6;
              new_msg = calloc(new_len, 1);

              // if new_msg cannot allocate mem cancel the program
              if (new_msg == NULL)
              {
                return 1;
              }

              for (int i = 0; i < msg_len; i++)
              {
                new_msg[i] = msg[i];
              }

              new_msg[msg_len] = 'B';
              new_msg[msg_len + 1] = '0';
              new_msg[msg_len + 2] = '6';
              new_msg[msg_len + 3] = '_';
              new_msg[msg_len + 4] = ID;

              //do checksum on new message and append
              uint8_t new_check = checksum(new_msg, new_len - 1);
              new_msg[msg_len + 5] = new_check;
    	  } else {
              new_len = msg_len + 5;
              new_msg = calloc(new_len, 1);

              // if new_msg cannot allocate mem cancel the program
              if (new_msg == NULL)
              {
                return 1;
              }

              for (int i = 0; i < msg_len - 1; i++)
              {
                new_msg[i] = msg[i];
              }

              new_msg[msg_len - 1] = 'B';
              new_msg[msg_len + 0] = '0';
              new_msg[msg_len + 1] = '6';
              new_msg[msg_len + 2] = '_';
              new_msg[msg_len + 3] = ID;

              //do checksum on new message and append
              uint8_t new_check = checksum(new_msg, new_len - 1);
              new_msg[msg_len + 4] = new_check;
    	  }

      } else {
    	  new_len = msg_len;
    	  new_msg = calloc(new_len, 1);

			// if new_msg cannot allocate mem cancel the program
			if (new_msg == NULL)
			{
			  return 1;
			}

			for (int i = 0; i < msg_len - 1; i++)
			{
			  new_msg[i] = msg[i];
			}

			//do checksum on new message and append
			uint8_t new_check = checksum(new_msg, new_len - 1);
			new_msg[msg_len - 1] = new_check;
      }

      ptr = new_msg;
      byte_len = new_len;

      // logic to define whether the stm should transmit through tx or uart2
      if (role == HEAD && !looped)
      {
        transmit(ptr, byte_len, false);
        looped = true;
      }
      else if (role == HEAD && looped)
      {
        transmit(ptr, byte_len, true);
        looped = false;
      }
      else if (role == TAIL)
      {
        transmit(ptr, byte_len, false);
      }

      free(new_msg);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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

#ifdef USE_FULL_ASSERT
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
