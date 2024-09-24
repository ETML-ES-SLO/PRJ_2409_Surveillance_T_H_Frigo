//  gestionBatterie.c
//
//	Description : fonctions liées à la gestion de la batterie
//	Auteur : Perret Mélissa 
//  Création : 09/06/2024
//  Modifications : --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "main.h"
#include "stdbool.h"
#include "fonctionsADC.h"
#include "gestionBatterie.h"


//// Fonction GestionCheckBatterie (regarder si une nouvelle vérification de la batterie est nécessaire)
//// Description: décrémente le compteur lié à la vérification de la batterie, déclenche le lancement de la vérification batterie si besoin
//// Entrées: Pointeur:ADC_HandleTypeDef hadc, Pointeur:TIM_HandleTypeDef htim, Pointeur:InfoBatterie infoBatterie
//// Sorties: bool (true si besoin de mesurer l'état de la batterie)
bool GestionCheckBatterie(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, InfoBatterie *infoBatterie)
{
	if(infoBatterie->compteurCheckBatterie > 0)
	{
		infoBatterie->compteurCheckBatterie--; // Décrementer le compteur à chaque réveil
	}
		
	if(infoBatterie->compteurCheckBatterie == 0) 
	{
		// Si le compteur vaut 0, il est temps de procéder à une nouvelle vérification de l'état de la batterie
		LancerCheckBatterie(htim, infoBatterie);
		return true;
	}
	else
	{
		return false; // Pas de vérification batterie nécessaire pour le moment
	}
}

//// Fonction MesurerEtatBatterie (détermine l'état de la batterie)
//// Description: utilise ControleEtatChargeBatterie pour récupérer l'état de la batterie et ArreterLectureTensionBatterie pour terminer l'opération
//// Entrées: Pointeur:ADC_HandleTypeDef hadc, Pointeur:TIM_HandleTypeDef htim, Pointeur:InfoBatterie infoBatterie
//// Sorties: Etat (état de la batterie)
Etat MesurerEtatBatterie(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, InfoBatterie *infoBatterie)
{
	Etat nouvelEtatBatterie = ControleEtatChargeBatterie(hadc, infoBatterie);
	ArreterLectureTensionBatterie(htim, infoBatterie);				
	return nouvelEtatBatterie;	
}


//// Fonction LancerCheckBatterie (étape préliminaire avant de pouvoir mesurer l'état de la batterie)
//// Description: enclenche le transistor nécessaire pour pouvoir lire l'état de la batterie, met en place le timer pour le délai avant mesure
//// Entrées: Pointeur:TIM_HandleTypeDef htim, Pointeur:InfoBatterie infoBatterie
//// Sorties: -
void LancerCheckBatterie(TIM_HandleTypeDef* htim, InfoBatterie* infoBatterie)
{
	// Enclenche le transistor nécessaire pour pouvoir lire l'état de la batterie
	HAL_GPIO_WritePin(EN_VBAT_GPIO_Port,EN_VBAT_Pin,GPIO_PIN_SET);

	// Activation de l'interruption timer pour pouvoir attendre un certain délai (DELAI_ACTIVATION_TRANSISTOR_MS) avant de lire l'état de la batterie
	HAL_TIM_Base_Start_IT(htim);		
	
	// Compteur cadencé sur l'interruption timer
	// Nombre de fois que l'on doit décrémenter le compteur dans l'interruption timer avant de procéder à la mesure
	infoBatterie->compteurCheckBatterie = DELAI_ACTIVATION_TRANSISTOR_MS / DELAI_TIMER6_MS;
}

//// Fonction ControleEtatChargeBatterie
//// Description: Contôle l'état de charge de la batterie (allume une LED en cas de problème détecté)
//// Entrées: Pointeur:ADC_HandleTypeDef hadc, Pointeur:InfoBatterie infoBatterie
//// Sorties: Etat (état de la batterie)
Etat ControleEtatChargeBatterie(ADC_HandleTypeDef* hadc, InfoBatterie* infoBatterie)
{
	float valeurBruteADC_VBAT = 0;
	float valeurVBat = 0;
	
	// Lecture de valeur du ADC et sauvegarde
	valeurBruteADC_VBAT = LectureValeurAdcBrute(hadc, ADC_CHANNEL_6);
	
	// Convertie la valeur brute de l'ADC en volt
	valeurVBat = ConversionValeurAdcEnVolt(valeurBruteADC_VBAT, R3_THEORIQUE, R6_THEORIQUE);

	Etat nouvelEtatBatterie = INDEFINI;
	
	// Enbranchement en fonction de la tension des piles
	// Pour changer la tension de la charge, modifier la valeur de VALEUR_BATTERIE_VOLT dans gestionBatterie.h
	if (VALEUR_BATTERIE_VOLT == VALEUR_3PILES_VOLT)
	{
		if((valeurVBat > VALEUR_BATTERIE_4_5V_MAX_VOLT ) || (valeurVBat < VALEUR_BATTERIE_4_5V_MIN_VOLT ))
		{
			// Problème détecté (mauvaise tension de batterie ou batterie déchargée)		
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET); // Allume la LED rouge
			nouvelEtatBatterie = ALARME;
		}
		else
		{
			// Aucun problème (batterie avec la bonne tension et chargée)
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET); // Eteint la LED rouge		
			nouvelEtatBatterie = OK;			
		}
	}
	else if (VALEUR_BATTERIE_VOLT == VALEUR_4PILES_VOLT)
	{
		if((valeurVBat  > VALEUR_BATTERIE_6V_MAX_VOLT )||(valeurVBat  < VALEUR_BATTERIE_6V_MIN_VOLT ))
		{
			// Problème détecté (mauvaise tension de batterie ou batterie déchargée)		
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET); // Allume la LED rouge
			nouvelEtatBatterie = ALARME;
		}
		else
		{
			// Aucun problème (batterie avec la bonne tension et chargée)
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET); // Eteint la LED rouge
			nouvelEtatBatterie = OK;
		}
	}
	
	// Code de test qui change l'état de la batterie à chaque réveil
	// Permet de tester l'activation de la LED et l'envoi des trames UART vers l'ESP
	if(DEBUG_ALARME_BATTERIE)
	{
		if(infoBatterie->etatBatterie == ALARME) // Si l'état était ALARME on passe en OK
		{
			nouvelEtatBatterie = OK;
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET); // Eteint la LED rouge
		}
		else // Si l'état n'était pas ALARME on passe en ALARME
		{
			nouvelEtatBatterie = ALARME;
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET); // Allume la LED rouge
		}
	}
	
	return nouvelEtatBatterie;
}


//// Fonction ArreterLectureTensionBatterie
//// Description: Réinitialise le transistor utilisé pour lire l'état de la batterie, défini le compteur pour le délai avant la prochaine vérification
//// Entrées: Pointeur:TIM_HandleTypeDef htim, Pointeur:InfoBatterie infoBatterie
//// Sorties: -
void ArreterLectureTensionBatterie(TIM_HandleTypeDef* htim, InfoBatterie* infoBatterie)
{	
	// Réinitialiser le transistor utilisé pour lire l'état de la batterie
	HAL_GPIO_WritePin(EN_VBAT_GPIO_Port,EN_VBAT_Pin,GPIO_PIN_RESET);
	
	// Arrêt de l'interruption timer utilisée pour attendre entre l'enclenchement du transistor et la mesure de l'état de la batterie
	HAL_TIM_Base_Stop_IT(htim);
	
	// Compteur cadencé sur la fréquence de réveil du STM
	// Nombre de fois que le STM doit se réveiller avant de procéder à une nouvelle vérification de l'état de la batterie
	infoBatterie->compteurCheckBatterie = DELAI_VERIFICATION_BATTERIE_MS / DELAI_ENTRE_DEUX_REVEILS_MS;  
}
