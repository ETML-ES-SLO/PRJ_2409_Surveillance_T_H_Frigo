//  gestionBatterie.c
//
//	Description : fonctions li�es � la gestion de la batterie
//	Auteur : Perret M�lissa 
//  Cr�ation : 09/06/2024
//  Modifications : --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "main.h"
#include "stdbool.h"
#include "fonctionsADC.h"
#include "gestionBatterie.h"


//// Fonction GestionCheckBatterie (regarder si une nouvelle v�rification de la batterie est n�cessaire)
//// Description: d�cr�mente le compteur li� � la v�rification de la batterie, d�clenche le lancement de la v�rification batterie si besoin
//// Entr�es: Pointeur:ADC_HandleTypeDef hadc, Pointeur:TIM_HandleTypeDef htim, Pointeur:InfoBatterie infoBatterie
//// Sorties: bool (true si besoin de mesurer l'�tat de la batterie)
bool GestionCheckBatterie(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, InfoBatterie *infoBatterie)
{
	if(infoBatterie->compteurCheckBatterie > 0)
	{
		infoBatterie->compteurCheckBatterie--; // D�crementer le compteur � chaque r�veil
	}
		
	if(infoBatterie->compteurCheckBatterie == 0) 
	{
		// Si le compteur vaut 0, il est temps de proc�der � une nouvelle v�rification de l'�tat de la batterie
		LancerCheckBatterie(htim, infoBatterie);
		return true;
	}
	else
	{
		return false; // Pas de v�rification batterie n�cessaire pour le moment
	}
}

//// Fonction MesurerEtatBatterie (d�termine l'�tat de la batterie)
//// Description: utilise ControleEtatChargeBatterie pour r�cup�rer l'�tat de la batterie et ArreterLectureTensionBatterie pour terminer l'op�ration
//// Entr�es: Pointeur:ADC_HandleTypeDef hadc, Pointeur:TIM_HandleTypeDef htim, Pointeur:InfoBatterie infoBatterie
//// Sorties: Etat (�tat de la batterie)
Etat MesurerEtatBatterie(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, InfoBatterie *infoBatterie)
{
	Etat nouvelEtatBatterie = ControleEtatChargeBatterie(hadc, infoBatterie);
	ArreterLectureTensionBatterie(htim, infoBatterie);				
	return nouvelEtatBatterie;	
}


//// Fonction LancerCheckBatterie (�tape pr�liminaire avant de pouvoir mesurer l'�tat de la batterie)
//// Description: enclenche le transistor n�cessaire pour pouvoir lire l'�tat de la batterie, met en place le timer pour le d�lai avant mesure
//// Entr�es: Pointeur:TIM_HandleTypeDef htim, Pointeur:InfoBatterie infoBatterie
//// Sorties: -
void LancerCheckBatterie(TIM_HandleTypeDef* htim, InfoBatterie* infoBatterie)
{
	// Enclenche le transistor n�cessaire pour pouvoir lire l'�tat de la batterie
	HAL_GPIO_WritePin(EN_VBAT_GPIO_Port,EN_VBAT_Pin,GPIO_PIN_SET);

	// Activation de l'interruption timer pour pouvoir attendre un certain d�lai (DELAI_ACTIVATION_TRANSISTOR_MS) avant de lire l'�tat de la batterie
	HAL_TIM_Base_Start_IT(htim);		
	
	// Compteur cadenc� sur l'interruption timer
	// Nombre de fois que l'on doit d�cr�menter le compteur dans l'interruption timer avant de proc�der � la mesure
	infoBatterie->compteurCheckBatterie = DELAI_ACTIVATION_TRANSISTOR_MS / DELAI_TIMER6_MS;
}

//// Fonction ControleEtatChargeBatterie
//// Description: Cont�le l'�tat de charge de la batterie (allume une LED en cas de probl�me d�tect�)
//// Entr�es: Pointeur:ADC_HandleTypeDef hadc, Pointeur:InfoBatterie infoBatterie
//// Sorties: Etat (�tat de la batterie)
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
			// Probl�me d�tect� (mauvaise tension de batterie ou batterie d�charg�e)		
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET); // Allume la LED rouge
			nouvelEtatBatterie = ALARME;
		}
		else
		{
			// Aucun probl�me (batterie avec la bonne tension et charg�e)
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET); // Eteint la LED rouge		
			nouvelEtatBatterie = OK;			
		}
	}
	else if (VALEUR_BATTERIE_VOLT == VALEUR_4PILES_VOLT)
	{
		if((valeurVBat  > VALEUR_BATTERIE_6V_MAX_VOLT )||(valeurVBat  < VALEUR_BATTERIE_6V_MIN_VOLT ))
		{
			// Probl�me d�tect� (mauvaise tension de batterie ou batterie d�charg�e)		
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET); // Allume la LED rouge
			nouvelEtatBatterie = ALARME;
		}
		else
		{
			// Aucun probl�me (batterie avec la bonne tension et charg�e)
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET); // Eteint la LED rouge
			nouvelEtatBatterie = OK;
		}
	}
	
	// Code de test qui change l'�tat de la batterie � chaque r�veil
	// Permet de tester l'activation de la LED et l'envoi des trames UART vers l'ESP
	if(DEBUG_ALARME_BATTERIE)
	{
		if(infoBatterie->etatBatterie == ALARME) // Si l'�tat �tait ALARME on passe en OK
		{
			nouvelEtatBatterie = OK;
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET); // Eteint la LED rouge
		}
		else // Si l'�tat n'�tait pas ALARME on passe en ALARME
		{
			nouvelEtatBatterie = ALARME;
			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET); // Allume la LED rouge
		}
	}
	
	return nouvelEtatBatterie;
}


//// Fonction ArreterLectureTensionBatterie
//// Description: R�initialise le transistor utilis� pour lire l'�tat de la batterie, d�fini le compteur pour le d�lai avant la prochaine v�rification
//// Entr�es: Pointeur:TIM_HandleTypeDef htim, Pointeur:InfoBatterie infoBatterie
//// Sorties: -
void ArreterLectureTensionBatterie(TIM_HandleTypeDef* htim, InfoBatterie* infoBatterie)
{	
	// R�initialiser le transistor utilis� pour lire l'�tat de la batterie
	HAL_GPIO_WritePin(EN_VBAT_GPIO_Port,EN_VBAT_Pin,GPIO_PIN_RESET);
	
	// Arr�t de l'interruption timer utilis�e pour attendre entre l'enclenchement du transistor et la mesure de l'�tat de la batterie
	HAL_TIM_Base_Stop_IT(htim);
	
	// Compteur cadenc� sur la fr�quence de r�veil du STM
	// Nombre de fois que le STM doit se r�veiller avant de proc�der � une nouvelle v�rification de l'�tat de la batterie
	infoBatterie->compteurCheckBatterie = DELAI_VERIFICATION_BATTERIE_MS / DELAI_ENTRE_DEUX_REVEILS_MS;  
}
