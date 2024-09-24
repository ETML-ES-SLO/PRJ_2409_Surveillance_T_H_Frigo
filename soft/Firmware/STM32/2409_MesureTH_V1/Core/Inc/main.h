/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef struct {
  char* nom;
  double valeur;
} DefinitionValeur;

typedef enum {
    SEUIL_TEMPERATURE_MIN,
    SEUIL_TEMPERATURE_MAX,
    ECART_TEMPERATURE,
    SEUIL_HUMIDITE_MIN,
		SEUIL_HUMIDITE_MAX,
		ECART_HUMIDITE,
} ValeursServeur;

typedef enum {
    ETAT_SEUILS,
		ETAT_BATTERIE,
} ValeursSTM;

typedef enum {    
    OK,				// 0
		ALARME,		// 1
		INDEFINI, // 2
} Etat;

typedef enum {    
		INITIALISATION,
		VERIFIER_BATTERIE,
		MESURER_BATTERIE,
		RECEPTION,
		MESURER_SONDE,
		RAFRAICHIR_EPAPER,
		SOMMEIL,
		REVEIL,
} EtatSTM;

extern bool rafraichirEPaper;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Initialisation(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define WKUP_ESP_Pin GPIO_PIN_13
#define WKUP_ESP_GPIO_Port GPIOC
#define RST_Pin GPIO_PIN_1
#define RST_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_2
#define DC_GPIO_Port GPIOA
#define BUSY_Pin GPIO_PIN_3
#define BUSY_GPIO_Port GPIOA
#define SPI_CS_Pin GPIO_PIN_4
#define SPI_CS_GPIO_Port GPIOA
#define VAL_VBAT_Pin GPIO_PIN_6
#define VAL_VBAT_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_14
#define LED_RED_GPIO_Port GPIOB
#define EN_VBAT_Pin GPIO_PIN_10
#define EN_VBAT_GPIO_Port GPIOA
#define ALARME_Pin GPIO_PIN_3
#define ALARME_GPIO_Port GPIOB
#define ESP_EN_Pin GPIO_PIN_8
#define ESP_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define DIN_Pin GPIO_PIN_7
#define DIN_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_5
#define SCK_GPIO_Port GPIOA

#define REVEIL_Pin GPIO_PIN_5
#define REVEIL_GPIO_Port GPIOB

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
