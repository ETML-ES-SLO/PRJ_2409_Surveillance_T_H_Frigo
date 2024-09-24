/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
//#include "stm32f0xx.h"
#include "mesures.h"
#include "affichage.h"
#include "gestionBatterie.h"
#include "communication.h"
#include "sommeil.h"
#include "EPD_2in13_V4.h" // TEMP
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
ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c2;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

EtatSTM etatSTM = INITIALISATION;

// Variables mesures
Mesures mesures;

// https://www.arduino.cc/reference/en/language/variables/data-types/array/
DefinitionValeur valeursServeur[] = {
	{ "seuilTemperatureMin", 2 },
  { "seuilTemperatureMax", 8 },
  { "ecartTemperature", 1 },
  { "seuilHumiditeMin", 30 },
  { "seuilHumiditeMax", 60 },
  { "ecartHumidite", 5},
};

// Variables affichage
bool rafraichirEPaper = false;
bool receptionTramesEspTermine = false;

// Variables batterie
InfoBatterie infoBatterie;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			
		switch(etatSTM)
		{
			case INITIALISATION:
				Initialisation(); // Initialisation au lancement du STM
				etatSTM = RECEPTION; // Pret à recevoir les données de la part de l'ESP
				break;
			case RECEPTION:
				
				// Note: la fréquence d'exécution de l'état RECEPTION est lié au timeout de la réception UART (define UART_TIMEOUT)
				ReceptionnerTrameUART(&huart1, valeursServeur);
				
				// On vérifie que receptionTramesEspTermine pour être sûr qu'on a bien été réveillé par l'ESP
				// On vérifie que HAL_GPIO_ReadPin(REVEIL_GPIO_Port, REVEIL_Pin) est GPIO_PIN_RESET pour être sûr que l'ESP à fini d'envoyer ses trames UART
				if(receptionTramesEspTermine == true && HAL_GPIO_ReadPin(REVEIL_GPIO_Port, REVEIL_Pin) == GPIO_PIN_RESET)
				{
					receptionTramesEspTermine = false;
					etatSTM = VERIFIER_BATTERIE; // Passage à l'étape suivante
				}
				break;
			case VERIFIER_BATTERIE:
			{
				bool besoinMesurerEtatBatterie = GestionCheckBatterie(&hadc, &htim6, &infoBatterie);			
				if(besoinMesurerEtatBatterie == true)
				{
					etatSTM = MESURER_BATTERIE; // Une mesure de l'état de la batterie est nécessaire
				}
				else
				{
					etatSTM = MESURER_SONDE;
				}
				break;
			}
			case MESURER_BATTERIE:
			{
				// Attente que le compteur décrémenté par l'interruption timer atteigne 0 (délai entre activation transistor et mesure)
				if(infoBatterie.compteurCheckBatterie <= 0)
				{
					Etat nouvelEtatBatterie = MesurerEtatBatterie(&hadc, &htim6, &infoBatterie); // Procéder à la mesure de l'état de la batterie		
					if(nouvelEtatBatterie != infoBatterie.etatBatterie)
					{
						infoBatterie.etatBatterie = nouvelEtatBatterie;
						EnvoyerTrameUART(&huart1, ETAT_BATTERIE, infoBatterie.etatBatterie); // Indiquer à l'ESP que l'état de la batterie à changé
					}
					etatSTM = MESURER_SONDE;
				}
				break;		
			}
			case MESURER_SONDE:
			{
				Etat nouvelEtatSeuils = EffectuerMesuresSonde(&hi2c2, &mesures, valeursServeur); // Procéder aux mesures de la température et de l'humidité
				if(nouvelEtatSeuils != mesures.etatSeuils)
				{
					mesures.etatSeuils = nouvelEtatSeuils;
					EnvoyerTrameUART(&huart1, ETAT_SEUILS, mesures.etatSeuils); // Indiquer à l'ESP que l'état de l'alarme à changé
				}		
				
				// Indiquer à l'ESP qu'on a fini d'envoyer les trames UART pour qu'il puisse se rendormir (si ne l'est pas déjà)
				HAL_GPIO_WritePin(ALARME_GPIO_Port, ALARME_Pin, GPIO_PIN_RESET); 
						
				// Si un rafraichissement de l'écran est nécessaire
				if(rafraichirEPaper == true)
				{
					etatSTM = RAFRAICHIR_EPAPER;				
				}
				else
				{
					etatSTM = SOMMEIL;
				}
				
				break;
			}			
			case RAFRAICHIR_EPAPER:				
				rafraichirEPaper = false;
				AffichageEPaper(&mesures, valeursServeur); // Procéder au rafraichissement de l'écran
				etatSTM = SOMMEIL;
				break;
			case SOMMEIL:
				EntrerModeSommeil();
				etatSTM = REVEIL;
				break;
			case REVEIL:
				etatSTM = RECEPTION;
				break;
		}	
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x2000090E;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 39999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  huart1.Init.BaudRate = 115200;
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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WKUP_ESP_GPIO_Port, WKUP_ESP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RST_Pin|DC_Pin|EN_VBAT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_RED_Pin|ALARME_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ESP_EN_GPIO_Port, ESP_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : WKUP_ESP_Pin */
  GPIO_InitStruct.Pin = WKUP_ESP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WKUP_ESP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RST_Pin DC_Pin SPI_CS_Pin EN_VBAT_Pin */
  GPIO_InitStruct.Pin = RST_Pin|DC_Pin|SPI_CS_Pin|EN_VBAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BUSY_Pin */
  GPIO_InitStruct.Pin = BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUSY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_RED_Pin ALARME_Pin ESP_EN_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin|ALARME_Pin|ESP_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

////---------------------------------------------------------------------------------	

//// Fonction Initialisation
//// Description: Logique utilisée pour initialiser le système (initialisation des variables, calibration ADC, initialisation de la gestion du mode sommeil)
//// Entrées: -
//// Sorties: -
void Initialisation()
{
	mesures.etatSeuils = INDEFINI; // Pour s'assurer qu'un changement d'état de seuils aura bien lieu au lancement du programme
	
	infoBatterie.etatBatterie = INDEFINI; // Pour s'assurer qu'un changement d'état de batterie aura bien lieu au lancement du programme
	infoBatterie.compteurCheckBatterie = 0; // Pour s'assurer qu'une vérification de l'état de la  batterie ait lieu au lancement du programme
		
	HAL_ADCEx_Calibration_Start(&hadc); // Calibration ADC (utilisé pour vérifier l'état de la batterie)
	
	InitialiserGestionSommeil();	
}

//// Fonction HAL_TIM_PeriodElapsedCallback: interruption Timer
//// Description: fonction d'interruption du timer (délai entre activation transistor et mesure de l'état de la batterie)
//// Entrées: Pointeur:TIM_HandleTypeDef htim
//// Sorties: -
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// Decompte du compteur utilisé pour le délai entre l'activation du transistor et la mesure de l'état de la batterie
	if(etatSTM == MESURER_BATTERIE)
	{
		if(infoBatterie.compteurCheckBatterie > 0) 
		{
			infoBatterie.compteurCheckBatterie--;		
		}
		
	// Quand le compteur vaudra 0
	// Ce sera le moment de mesurer la tension de la batterie (à la prochaine exécution de la boucle main)
	}
}

//// Fonction EXTI4_15_IRQHandler: interruption pins GPIO
//// Description: fonction d'interruptioin de la pin utilisée par l'ESP pour réveiller le STM
//// Entrées: -
//// Sorties: -
void EXTI4_15_IRQHandler(void) 
{
	if(HAL_GPIO_ReadPin(REVEIL_GPIO_Port, REVEIL_Pin) == GPIO_PIN_SET)
	{		
		receptionTramesEspTermine = true; // Indique que l'ESP à fini d'envoyer ses éventuelles trames UART
	}
	
	// Pour s'assurer que l'état de l'interruption soit bien réinitialisé
	HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_5);
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
