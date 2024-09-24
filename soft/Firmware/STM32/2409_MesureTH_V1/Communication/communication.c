// 	communication.c
//
//	Description : fonctions li�es � la communication ESP/STM
//	Auteur : Perret M�lissa 
//  Cr�ation : 16/09/2024
//  Modifications : --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "communication.h"
#include <string.h> // pour memset et memcpy


uint8_t receptionTramesUART[TAILLE_BUFFER]; // Tableau pour stocker les trames UART re�ues

// Important: sizeof ne renvoie pas les bonnes valeurs quand utilis� pour d�finir la valeurs de variables d�clar�es en const
uint8_t OCTET_DEBUT = 0x02; // STX (Start of Text)
uint8_t OCTET_FIN = 0x03; // ETX (End of Text)
uint8_t TAILLE_TRAME = sizeof(OCTET_DEBUT) + sizeof(int8_t) + sizeof(char) + sizeof(double) + sizeof(OCTET_FIN);


//// Fonction ReceptionnerTrameUART (r�ception analyse et traitement des trames UART provenant de l'ESP)
//// Description: r�ception des donn�es UART
////              analyse des donn�es pour localiser les trames (caract�res de d�but et de fin)
////              analyse des trames re�ues pour extraire et stocker les donn�es re�ues
//// Entr�es: Pointeur:UART_HandleTypeDef huart, Tableau:DefinitionValeur valeursServeur (tableau pour identifier et stocker les valeurs re�ues)
//// Sorties: -
void ReceptionnerTrameUART(UART_HandleTypeDef* huart, DefinitionValeur valeursServeur[])
{
	int uartStatut; // D�claration variable locale (valeur de retour de la lecture UART)
	
	do
	{			
		// R�ception donn�es UART
		uartStatut = HAL_UART_Receive(huart, (uint8_t*)receptionTramesUART, TAILLE_TRAME, UART_RECEPTION_TIMEOUT_MS);
		if(uartStatut == HAL_OK)
		{
			// On parcourt le buffer pour trouver toutes les trames (identifi�es par OCTET_DEBUT)
			for (int indexDebutTrame = 0; indexDebutTrame < TAILLE_BUFFER; indexDebutTrame++) 
			{
				if (receptionTramesUART[indexDebutTrame] == OCTET_DEBUT) // D�but de trame trouv�
				{
					int indexFinTrame = indexDebutTrame + TAILLE_TRAME - 1;
					if(indexFinTrame < TAILLE_BUFFER && receptionTramesUART[indexFinTrame] == OCTET_FIN) // Si on a bien une fin de trame comme attendu
					{													
						uint8_t index = receptionTramesUART[indexDebutTrame + 1]; // Deuxi�me octet de la trame correspond � l'identifiant de la trame															
						// Trois�me octet correspond � un s�parateur ':' pour faciltier la lecture des trames lors du debuggage																
						
						// Exemple de trame
						// 0:12
						// Effet de la trame: mettre la valeur de SEUIL_TEMPERATURE_MIN � 12
						// valeursServeur[0].valeur = 12
						
						// Explications pour memcpy : https://en.cppreference.com/w/cpp/string/byte/memcpy
						// Les 8 octets suivants (octets 4 � 11) correspondent � la valeur de la trame
						// On copie la valeur de la trame dans la variable qu'on utilise pour stocker les valeurs du serveur
						memcpy(&valeursServeur[index].valeur, &receptionTramesUART[indexDebutTrame + 3], sizeof(double));
						
						rafraichirEPaper = true; // Une nouvelle valeur re�ue implique qu'un rafraichissement de l'�cran sera n�cessaire
					}
				}						 
			}
		}
	} while (uartStatut == HAL_OK); // Continuer tant qu'on re�oit quelque chose
}

//// Fonction EnvoyerTrameUART (construire et transmettre une trame UART � l'ESP)
//// Description: construire et transmettre la trame UART � l'ESP
//// Entr�es: Pointeur:UART_HandleTypeDef huart, uint8_t index, double valeur
//// Sorties: -
void EnvoyerTrameUART(UART_HandleTypeDef* huart, uint8_t index, double valeur)
{
		HAL_GPIO_WritePin(ALARME_GPIO_Port, ALARME_Pin, GPIO_PIN_SET); // R�veiller l'ESP
		HAL_Delay(DELAI_ENVOIE_TRAME_MS); // D�lai pour �tre s�r que l'ESP soit r�veill� et pr�t � recevoir les trames
	
    uint8_t trame[TAILLE_TRAME];  // Buffer pour stocket la trame � envoyer
    char separateur = ':'; // S�parateur entre l'index et la valeur dans les trames, pour faciliter le debuggage

    // Explications memset : https://cplusplus.com/reference/cstring/memset/
    memset(trame, 0, sizeof(trame)); // R�initialisation de la trame en mettant tous les bits � 0 (par pr�cautions)

    // Explications memcpy : https://en.cppreference.com/w/cpp/string/byte/memcpy
    memcpy(&trame[0], &OCTET_DEBUT, sizeof(OCTET_DEBUT));                                                                 			// OCTET_DEBUT
    memcpy(&trame[sizeof(OCTET_DEBUT)], &index, sizeof(index));                                                           			// index
    memcpy(&trame[sizeof(OCTET_DEBUT) + sizeof(index)], &separateur, sizeof(separateur));                                      	// ':'
    memcpy(&trame[sizeof(OCTET_DEBUT) + sizeof(index) + sizeof(separateur)], &valeur, sizeof(double));                         	// valeur
    memcpy(&trame[sizeof(OCTET_DEBUT) + sizeof(index) + sizeof(separateur) + sizeof(valeur)], &OCTET_FIN, sizeof(OCTET_FIN));  	// OCTET_FIN

		HAL_UART_Transmit(huart, trame, sizeof(trame), 1000); // Transmission de la trame � l'ESP
}
