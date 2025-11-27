/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>
#include <stdio.h>/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include <lcd4.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NORMAL 0
#define SET 1
#define TEMP_OFFSET_C   (-2.3f)   // LM35 reads ~2.3 °C high at room temp
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	extern void initialise_monitor_handles(void);
	int count = 50;
	char am_pm[3] = "AM";
	int hour=11;
	int min=59;
	char keypad_scan(void);
	int col_scan(void);
	char key = 'x';                                         // variables assigned for the following functions
	char input_code[6] = "";
	int i = 0;
	bool clkUpdateFlag = false;
	int mode = NORMAL;
	int temp;
	char tSym = 'C';
	int degreeMode = 0;
	// ---- LM35 / ADC variables ----
	uint32_t adcRaw = 0;
	float    tempC  = 0.0f;
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
  MX_TIM4_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  initialise_monitor_handles();
  HAL_TIM_Base_Start_IT(&htim4);
  initialise_monitor_handles();
  //LCD to uC connections: RS - PA0, E - PA1, DB4 - PA4, DB5 - PB0, DB6 - PC1, DB7 - PC0

  lcdSetup(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_4, GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_1, GPIOC, GPIO_PIN_0);

  lcdInit();

  char timeBuffer[30]; //used to stored formatted string for LCD display
  uint16_t readValue;

 // void lcdPrintChar(char c) {
    //  lcdData(c);  // or whatever function sends a character to your LCD

  void enterSetMode(void) {            // if '*' is pressed it calls to set mode function

    lcdCommand(lcdClear);     //clears LCD display
      lcdString("Set Mode"); //prompts user lcd is in set mode
      HAL_Delay(1000);
      char key;
          char input[3] = {0};
          uint8_t index;
          uint8_t newHour = 12, newMin = 0;
          char newAMPM[3] = "AM"; // variable for am/pm array

          // --- Set HOUR ---
          SET_HOUR:
              lcdCommand(lcdClear);
              lcdString("Set HOUR: " );     //prompts user to set hour
              index = 0;
              memset(input, 0, sizeof(input)); // clears input

              while (1) {
                  key = keypad_scan();   //call keypadscan to take input
                  HAL_Delay(100);
                  if (key >= '0' && key <= '9') {     //defines what can be pressed on the keypad for valid hour
                	  if(index < 2){
                      input[index++] = key;
                      lcdChar(key); // Display input
                  }
                  }
                  else if (key == '#' && index > 0) {
                      input[index] = '\0'; // Null-terminate string
                      newHour = atoi(input); // converts input to integer

                      if (newHour >= 1 && newHour <= 12) {
                          break; // Valid hour
                      }

                      // Error if not in valid range
                      lcdCommand(lcdClear);
                      lcdString("ERROR" );    // displays error if invalid user input
                      HAL_Delay(1000);
                      goto SET_HOUR;
                  }
              }

          // --- Set MINUTE ---
              SET_MIN:
                  lcdCommand(lcdClear);
                  lcdString("Set MIN: " );
                  index = 0;
                  memset(input, 0, sizeof(input));

                  while (1) {
                      key = keypad_scan();    //user input
                      HAL_Delay(10);
                      if (key >= '0' && key <= '9' && index < 2) {    //parameter for minutes
                          input[index++] = key;
                          lcdChar(key); // Show entered digit
                      } else if (key == '#' && index > 0) {
                          input[index] = '\0'; // End string
                          newMin = atoi(input);

                          if (newMin >= 0 && newMin <= 59) {
                              break; // Valid minute
                          }

                          // Show error if invalid
                          lcdCommand(lcdClear);
                          lcdString("ERROR " );    // error if invalid
                          HAL_Delay(1000);
                          goto SET_MIN;   // goes back to minute set if error
                      }
                  }

          // --- Set AM or PM ---
          SET_AMPM:
              lcdCommand(lcdClear);
              lcdString("AM OR PM:" );    // ask user to select am/pm
              char ampmChar = 0;

              while (1) {
                  key = keypad_scan();
                  HAL_Delay(10);
                  if ((key == 'A' || key == 'B')) {       // a or b pressed for am or pm
                      lcdChar(key);
                      ampmChar = key;
                  } else if (key == '#' && (ampmChar == 'A' || ampmChar == 'B')) {   // error if not pressed
                      if (ampmChar == 'A') {
                          newAMPM[0] = 'A';
                          newAMPM[1] = 'M';
                      } else if (ampmChar == 'B') {
                          newAMPM[0] = 'P';
                          newAMPM[1] = 'M';
                      }
                      newAMPM[2] = '\0';
                      break;
                  } else if (key == '#') {
                      lcdCommand(lcdClear);
                      lcdString("ERROR ");
                      HAL_Delay(1000);
                      goto SET_AMPM;
                  }
              }

          // --- Apply Time ---
          hour = newHour;
          min = newMin;
          count = 0;
          am_pm[0] = newAMPM[0];
          am_pm[1] = newAMPM[1];
          am_pm[2] = '\0';

          lcdCommand(lcdClear);
          lcdString("Time Updated ");
          HAL_Delay(1000);

          mode = NORMAL;  // Go back to normal clock mode
      }





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

while (1)
  {
	  // Check if user wants to enter Set Mode
	    char key = keypad_scan();
	    if (key == '*') {
	        mode = SET;
	    }

if (mode == NORMAL){
// PRINTS TO THE LCD
	    if(clkUpdateFlag == true && mode == NORMAL){
	    	lcdCommand(lcdClear);
	    	sprintf(timeBuffer,"%02d:%02d:%02d %s %d%c ", hour, min, count, am_pm, temp, tSym); //prints to LCD
	    	lcdString(timeBuffer);
	    	clkUpdateFlag = false;
	    }
}
	else if (mode == SET) {
		enterSetMode();  // Will handle its own display and set values
	}
}

}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */

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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 15999;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|ROW4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GREEN_LED_Pin
                          |ROW1_Pin|ROW2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|ROW3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BLUE_PUSH_BUTTON_Pin */
  GPIO_InitStruct.Pin = BLUE_PUSH_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BLUE_PUSH_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 ROW4_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|ROW4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA4 GREEN_LED_Pin
                           ROW1_Pin ROW2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GREEN_LED_Pin
                          |ROW1_Pin|ROW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 ROW3_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|ROW3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : COL3_Pin COL4_Pin */
  GPIO_InitStruct.Pin = COL3_Pin|COL4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : COL2_Pin COL1_Pin */
  GPIO_InitStruct.Pin = COL2_Pin|COL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){      //calls unto temp adc to swithc between F and C
	if (GPIO_Pin == BLUE_PUSH_BUTTON_Pin){
	degreeMode++;
    }
}





void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim4)
    {
        // ----- existing clock code -----
        count++;
        if (count >= 60) {
            count = 0;
            min++;
            if (min >= 60) {
                min = 0;
                hour++;
                if (hour == 12) {
                    am_pm[0] = (am_pm[0] == 'A') ? 'P' : 'A';
                }
                if (hour > 12) {
                    hour = 1;
                }
            }
        }
        // --------------------------------

        // ----- Read ADC and convert to °C with calibration -----
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        uint32_t adcRaw = HAL_ADC_GetValue(&hadc1);

        // ADC count -> volts (assuming 3.3 V ADC reference)
        float vSense = ((float)adcRaw * 3.3f) / 4095.0f;

        // LM35: 10 mV/°C => V * 100 = °C (raw)
        float tempC = vSense * 100.0f;

        // Apply calibration offset (sensor reads ~2.3 °C high)
        tempC += TEMP_OFFSET_C;

        if (degreeMode % 2 == 0) {
            // Show °C
            temp = (int)(tempC + 0.5f);
            tSym = 'C';
        } else {
            // Show °F
            float tempF = tempC * 9.0f / 5.0f + 32.0f;
            temp = (int)(tempF + 0.5f);
            tSym = 'F';
        }
        // --------------------------------

        clkUpdateFlag = true;
    }
}




int col_scan(void) {                  //col scan for keypad
    if (HAL_GPIO_ReadPin(COL1_GPIO_Port, COL1_Pin) == GPIO_PIN_RESET) {
        return 0;  // Column 1 pressed
    }
    if (HAL_GPIO_ReadPin(COL2_GPIO_Port, COL2_Pin) == GPIO_PIN_RESET) {
        return 1;  // Column 2 pressed
    }
    if (HAL_GPIO_ReadPin(COL3_GPIO_Port, COL3_Pin) == GPIO_PIN_RESET) {
        return 2;  // Column 3 pressed
    }
    if (HAL_GPIO_ReadPin(COL4_GPIO_Port, COL4_Pin) == GPIO_PIN_RESET) {
        return 3;  // Column 4 pressed
    }
    return 5;  // No key pressed
}
char keypad_scan(void)
{
    // Keymap for 4x4 keypad
    static const char key_map[4][4] = {
        {'1', '2', '3', 'A'}, // 1st row
        {'4', '5', '6', 'B'}, // 2nd row
        {'7', '8', '9', 'C'}, // 3rd row
        {'*', '0', '#', 'D'}, // 4th row
    };

    // Scan each row one by one
    for (int row = 0; row < 4; row++) {
        // 1) Set all rows HIGH
        HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_SET);

        // 2) Drive the current row LOW
        if (row == 0) {
            HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
        } else if (row == 1) {
            HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
        } else if (row == 2) {
            HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
        } else { // row == 3
            HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_RESET);
        }

        // 3) Short delay for signals to settle
        HAL_Delay(5);

        int col = col_scan();

        // 4) If a column is active, we found a key
        if (col < 4) {
            char key = key_map[row][col];

            // 5) Wait until key is released to avoid double reads
            while (col_scan() < 4) {
                HAL_Delay(5);   // simple debounce
            }

            return key;  // One character per physical press
        }
    }

    // No key pressed
    return 'x';
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
