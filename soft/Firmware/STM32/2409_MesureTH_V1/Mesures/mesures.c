//  mesures.c
//
//	Description : fonctions li�es aux mesures de la sonde (temp�rature et humidt�)
//	Auteur : Perret M�lissa 
//  Cr�ation : 16/09/2024
//  Modifications : --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "mesures.h"
#include "shtc3.h" // pour utiliser la sonde 


//// Fonction EffectuerMesuresSonde (mesure t�mparture et humidit�)
//// Description: mesure t�mparture et humidit�
////							v�rification d�passement des seuils
////							en cas de changement d'�tat, demande rafraichissement �cran et l'envoie de la trame UART 
//// Entr�es: Pointeur:I2C_HandleTypeDef hi2c2, Pointeur:Mesures mesures, Tableau:DefinitionValeur valeursServeur
//// Sorties: Etat (�tat de l'alarme li�e au d�passement des seuils)
Etat EffectuerMesuresSonde(I2C_HandleTypeDef* hi2c2, Mesures* mesures, DefinitionValeur valeursServeur[])
{
	if (shtc3_read_id(hi2c2)) // Si on d�tecte la sonde 
	{					
		// Effectuer les mesures
		if (shtc3_perform_measurements(hi2c2, &mesures->temperatureEntierActuelle, &mesures->humiditeEntierActuelle)) // Si la mesure est r�ussie
		{
			// Les r�sultats de la sonde sont transmis sous forme d'entier
			// Il s'agit en r�alit� de la valeur avec deux d�cimales, remis sous forme d'entier en �tant multipli� par 100
			// On effectue l'op�ration inverse (division par 100) pour r�cup�rer la valeur d�cimale
			mesures->temperatureActuelle = (float)mesures->temperatureEntierActuelle/100; 			
			mesures->humiditeActuelle = (float)mesures->humiditeEntierActuelle/100; 			
			
			// Pour �viter de rafraichisr l'�cran E-paper trop souvent, on mesure l'�cart entre la nouvelle valeur et la derni�re valeur affich�
			// Si l'�cart est sup�rieur � l'�cart minimal on demandera le rafraichissement de l'�cran
			// Les valeurs des �carts est transmises par le serveur
			
			// Ecart temp�rature
			double ecartTemperatureMinimal = valeursServeur[ECART_TEMPERATURE].valeur; // Ecart minimal de temperature pour effectuer un rafraichissement du E-paper
			float ecartTemperature = mesures->temperatureActuelle - mesures->temperatureAffichee; // Ecart entre la temp�rature mesur�e et la temp�rature affich�e
			if(ecartTemperature >= ecartTemperatureMinimal || ecartTemperature <= -ecartTemperatureMinimal) // Si l'�cart est plus important que l'�cart minimal
			{
				rafraichirEPaper = true; // Indiquer qu'un rafraichissement de l'�cran est n�cessaire
			}					
			
			// Ecart humidit�
			double ecartHumiditeMinimal= valeursServeur[ECART_HUMIDITE].valeur; // Ecart minimal d'humidit� pour effectuer un rafraichissement du E-paper
			float ecartHumidite = mesures->humiditeActuelle - mesures->humiditeAffichee; // Ecart entre l'humidit� mesur�e et l'humidit� affich�e
			if(ecartHumidite >= ecartHumiditeMinimal || ecartHumidite <= -ecartHumiditeMinimal) // Si l'�cart est plus important que l'�cart minimal
			{
				rafraichirEPaper = true; // Indiquer qu'un rafraichissement de l'�cran est n�cessaire
			}
			
			// V�rification d�passement des seuils (pour d�finir le nouvel �tat de l'alarme d�passement des seuils)
			bool depassementTemperature = mesures->temperatureActuelle < valeursServeur[SEUIL_TEMPERATURE_MIN].valeur || mesures->temperatureActuelle > valeursServeur[SEUIL_TEMPERATURE_MAX].valeur;
			bool depassementHumidite = mesures->humiditeActuelle < valeursServeur[SEUIL_HUMIDITE_MIN].valeur || mesures->humiditeActuelle > valeursServeur[SEUIL_HUMIDITE_MAX].valeur;
			Etat nouvelEtatSeuils = INDEFINI;
			if(depassementTemperature || depassementHumidite)
			{
				nouvelEtatSeuils = ALARME; // D�passement des seuils min/max (temp�rature et/ou humidit�)
			}
			else
			{
				nouvelEtatSeuils  = OK; // Pas de d�passement des seuils min/max (temp�rature et humidit�)
			}		
			
			// Code de test qui change l'�tat de l'alarme d�passement des seuils � chaque r�veil
			// Permet de tester l'envoi des trames vers l'ESP
			if(DEBUG_ALARME_SEUILS)
			{
				if(mesures->etatSeuils == ALARME) // Si l'�tat �tait ALARME on passe en OK
				{
					nouvelEtatSeuils = OK;
				}
				else
				{
					nouvelEtatSeuils = ALARME; // Si l'�tat n'�tait pas ALARME on passe en ALARME
				}
			}
			
			return nouvelEtatSeuils;
		}
		else
		{
			return mesures->etatSeuils; // Mesure impossible, pas de changement d'�tat
		}
	}
	else
	{
		return mesures->etatSeuils; // Mesure impossible, pas de changement d'�tat
	}
}
