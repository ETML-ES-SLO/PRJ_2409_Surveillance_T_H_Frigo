//  sommeil.c
//
//	Description : fonctions li�es � l'entr�e et sortie du mode sommeil
//	Auteur : Perret M�lissa 
//  Cr�ation : 21/09/2024
//  Modifications : --

//	Version		:	V1.0
/*--------------------------------------------------------*/

#include "sommeil.h"
#include "main.h" // pour GPIO, HAL, REVEIL_Pin et REVEIL_GPIO_Port


//// Fonction InitialiserGestionSommeil (configurations pour pouvoir �tre r�veill�)
//// Description: configuration de la pin pour pouvoir �tre r�veill� par l'ESP
//// Entr�es: -
//// Sorties: -
void InitialiserGestionSommeil()
{
	// Initialisation de la pin utilis�e pour pouvoir �tre reveill� par l'ESP
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = REVEIL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; 
	GPIO_InitStruct.Pull = GPIO_PULLUP | GPIO_PULLDOWN;
	HAL_GPIO_Init(REVEIL_GPIO_Port, &GPIO_InitStruct);
		
	// Activation de l'interruption pour pouvoir �tre reveill� par l'ESP
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	
	// Initialisation de la pin utilis�e pour r�veiller l'ESP (pour pouvoir le r�veiller plus tard)
	HAL_GPIO_WritePin(ALARME_GPIO_Port, ALARME_Pin, GPIO_PIN_RESET); 
}

//// Fonction EntrerModeSommeil (mettre le STM en mode stop)
//// Description: logique pour mettre le STM en mode stop et remise en route apr�s r�veil
//// Entr�es: -
//// Sorties: -
void EntrerModeSommeil()
{
	HAL_SuspendTick(); // D�sactiver le timer SysTick (pour �viter qu'il provoque un r�veil)
	
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); // Entrer en mode stop
	// Ex�cution du programme en pause tant que le STM n'est pas reveill� par l'ESP

	// Reprise de l'ex�cution du programme ici apr�s le r�veil
	HAL_ResumeTick(); // R�activer le timer SysTick
	HAL_GPIO_WritePin(ALARME_GPIO_Port, ALARME_Pin, GPIO_PIN_RESET); // Initialiser pin pour pouvoir r�veiller ESP plus tard
}
