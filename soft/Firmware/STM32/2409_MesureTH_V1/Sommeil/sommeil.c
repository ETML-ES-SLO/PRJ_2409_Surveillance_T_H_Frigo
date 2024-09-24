//  sommeil.c
//
//	Description : fonctions liées à l'entrée et sortie du mode sommeil
//	Auteur : Perret Mélissa 
//  Création : 21/09/2024
//  Modifications : --

//	Version		:	V1.0
/*--------------------------------------------------------*/

#include "sommeil.h"
#include "main.h" // pour GPIO, HAL, REVEIL_Pin et REVEIL_GPIO_Port


//// Fonction InitialiserGestionSommeil (configurations pour pouvoir être réveillé)
//// Description: configuration de la pin pour pouvoir être réveillé par l'ESP
//// Entrées: -
//// Sorties: -
void InitialiserGestionSommeil()
{
	// Initialisation de la pin utilisée pour pouvoir être reveillé par l'ESP
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = REVEIL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; 
	GPIO_InitStruct.Pull = GPIO_PULLUP | GPIO_PULLDOWN;
	HAL_GPIO_Init(REVEIL_GPIO_Port, &GPIO_InitStruct);
		
	// Activation de l'interruption pour pouvoir être reveillé par l'ESP
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	
	// Initialisation de la pin utilisée pour réveiller l'ESP (pour pouvoir le réveiller plus tard)
	HAL_GPIO_WritePin(ALARME_GPIO_Port, ALARME_Pin, GPIO_PIN_RESET); 
}

//// Fonction EntrerModeSommeil (mettre le STM en mode stop)
//// Description: logique pour mettre le STM en mode stop et remise en route après réveil
//// Entrées: -
//// Sorties: -
void EntrerModeSommeil()
{
	HAL_SuspendTick(); // Désactiver le timer SysTick (pour éviter qu'il provoque un réveil)
	
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); // Entrer en mode stop
	// Exécution du programme en pause tant que le STM n'est pas reveillé par l'ESP

	// Reprise de l'exécution du programme ici après le réveil
	HAL_ResumeTick(); // Réactiver le timer SysTick
	HAL_GPIO_WritePin(ALARME_GPIO_Port, ALARME_Pin, GPIO_PIN_RESET); // Initialiser pin pour pouvoir réveiller ESP plus tard
}
