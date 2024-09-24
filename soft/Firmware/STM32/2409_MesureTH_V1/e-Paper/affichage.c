// 	affichage.c
//
//	Description : fonctions liée à l'affichage E-paper
//	Auteur : Perret Mélissa 
//  Création : 16/09/2024
//  Modifications : --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "affichage.h"
#include "EPD_2in13_V4.h" // pour EPD_2in13_V4_Init etc.
#include "GUI_Paint.h" // pour les fonctions Paint (Paint_NewImage, Paint_DrawString_EN etc.)
#include <stdlib.h> // pour malloc et free


//// Fonction AffichageEPaper (affichage des données sur l'écran E-paper)
//// Description: affiche les données sur l'écran E-paper (valeurs provenant du serveur, valeurs mesurées avec la sonde) 
//// Entrées: Pointeur:Mesures mesures (données mesurées), Tableau:DefinitionValeur valeursServeur (valeurs provenant du serveur)
//// Sorties: uint16_t (-1 si problème, 0 si affiche réussi)
int AffichageEPaper(Mesures* mesures, DefinitionValeur valeursServeur[])
{
	// Initialisation du module de développement
	if(DEV_Module_Init() != 0)
	{
			return -1; // Initialisation échouée
	}
	
	// Créer une nouvelle image pour le cache
	UBYTE *Image;
	UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT; //instruction ternaire ?
	if((Image = (UBYTE *)malloc(Imagesize)) == NULL) 
	{
			return -1;  // Allocation impossible. Problème de HEAP_SIZE insuffisante ?
	}
	
	// Mettre à jour les données considérées comme affichées
	mesures->temperatureAffichee = mesures->temperatureActuelle;
	mesures->humiditeAffichee = mesures->humiditeActuelle;
	
	EPD_2in13_V4_Init(); // Initisaliation E-paper

	// Initialisation de l'image avec des pixels blancs
	Paint_NewImage(Image, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);  	
	Paint_SelectImage(Image);
	Paint_Clear(WHITE);
		
	// Première colonne 
	Paint_DrawString_EN(0, 45, "Actu", &Font16, WHITE, BLACK);
	Paint_DrawString_EN(0, 65, "Min", &Font16, WHITE, BLACK);
	Paint_DrawString_EN(0, 85, "Max", &Font16, WHITE, BLACK);	
	Paint_DrawString_EN(0, 105, "Ecart", &Font16, WHITE, BLACK);
			
	Paint_DrawLine(60, 23, 60, 122, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID); // Ligne verticale pour séparer les colonnes

	// Seconde colonne (température)
	Paint_DrawString_EN(23, 3, "Temperature", &Font16, WHITE, BLACK); // Titre colonne
	Paint_DrawString_EN(85, 23, "[", &Font16, WHITE, BLACK); // Unité colonne
	Paint_DrawCircle(100, 23, 2, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); // Unité colonne (symbole degré °)
	Paint_DrawString_EN(100, 23, "C]", &Font16, WHITE, BLACK); // Unité colonne

	Paint_DrawNumDecimals(75, 45, mesures->temperatureActuelle, &Font16, NOMBRE_DECIMALES_MESURES, BLACK, WHITE); // Température actuelle
	Paint_DrawNumDecimals(75, 65, valeursServeur[SEUIL_TEMPERATURE_MIN].valeur, &Font16, NOMBRE_DECIMALES_MESURES, BLACK, WHITE); // Température min			
	Paint_DrawNumDecimals(75, 85, valeursServeur[SEUIL_TEMPERATURE_MAX].valeur, &Font16, NOMBRE_DECIMALES_MESURES, BLACK, WHITE); // Température max
	Paint_DrawNumDecimals(75, 105, valeursServeur[ECART_TEMPERATURE].valeur, &Font16, NOMBRE_DECIMALES_MESURES, BLACK, WHITE); // Ecart température 
	
	Paint_DrawLine(150, 0, 150, 122, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID); // Ligne verticale pour séparer les colonnes
	
	// Troisième colonne (humidité)
	Paint_DrawString_EN(155, 3, "Humidite", &Font16, WHITE, BLACK);	// Titre colonne
	Paint_DrawString_EN(180, 23, "[%]", &Font16, WHITE, BLACK); // Unité colonne
	Paint_DrawNumDecimals(170, 43, mesures->humiditeActuelle, &Font16, NOMBRE_DECIMALES_MESURES, BLACK, WHITE); // Humidité actuelle
	Paint_DrawNumDecimals(170, 65, valeursServeur[SEUIL_HUMIDITE_MIN].valeur, &Font16, NOMBRE_DECIMALES_MESURES, BLACK, WHITE); // Humidité min
	Paint_DrawNumDecimals(170, 85, valeursServeur[SEUIL_HUMIDITE_MAX].valeur, &Font16, NOMBRE_DECIMALES_MESURES, BLACK, WHITE); // Humidité Max    
	Paint_DrawNumDecimals(170, 105, valeursServeur[ECART_HUMIDITE].valeur, &Font16, NOMBRE_DECIMALES_MESURES, BLACK, WHITE); // Ecart humidité

	Paint_DrawLine(0, 40, 250, 40, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID); // Ligne horizontale (séparation)

	EPD_2in13_V4_Display_Base(Image); // Affichage
	
	// Libérer la mémoire allouée pour l'image
	free(Image); 
	Image = NULL;
	
	DEV_Module_Exit(); // Fermeture
	
	return 0;
}
