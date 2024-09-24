/* ************************************************************************** */
/** Descriptive File Name

  @Company
    ETML-ES

  @File Name
    gestionBatterie.h

  @Auteurs
    - Perret Mélissa

  @Description
    Fonctions liées à la gestion de la batterie
 */
/* ************************************************************************** */



#ifndef _GESTION_BATTERIE_H
#define _GESTION_BATTERIE_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

#include "main.h" // pour Etat

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

// Structure pour stocker les informations liée à la batterie
typedef struct {
	uint32_t compteurCheckBatterie;
	Etat etatBatterie;
} InfoBatterie;

// Valeurs des resistances utilisées pour le pont diviseur de tension
#define R3_THEORIQUE 18000 // Valeur théorique de la résistance
#define R6_THEORIQUE 5100 // Valeur théorique de la résistance 

#define VALEUR_BATTERIE_VOLT 4.5 // Valeur batterie (4.5 ou 6.0)

#define VALEUR_3PILES_VOLT 	4.5 // Tension pour 3 piles
#define VALEUR_BATTERIE_4_5V_MIN_VOLT	4.0	// Valeur min acceptable pour que la batterie 4.5V soit considérée comme chargée
#define VALEUR_BATTERIE_4_5V_MAX_VOLT 4.8	// Valeur max acceptable pour que la batterie 4.5V soit considérée comme chargée

#define VALEUR_4PILES_VOLT 	6.0 // Tension pour 4 piles
#define VALEUR_BATTERIE_6V_MIN_VOLT 4.5 // Valeur min acceptable pour que la batterie 6V soit considérée comme chargée
#define VALEUR_BATTERIE_6V_MAX_VOLT 6.4 // Valeur max acceptable pour que la batterie 6V soit considérée comme chargée

// Attention: DELAI_ENTRE_DEUX_REVEILS_MS doit coincider avec la valeur DUREE_SOMMEIL_MS du côté de l'ESP (Sommeil.h)
//#define DELAI_ENTRE_DEUX_REVEILS_MS 1*3600*1000    	// Délai entre deux réveils du STM32 (1*3600*1000 = 1h).
#define DELAI_ENTRE_DEUX_REVEILS_MS 5000 							// Délai test entre deux réveils du STM32

//#define DELAI_VERIFICATION_BATTERIE_MS 4*3600*1000 	// Délai entre deux vérifications de batterie (4*3600*1000 = 4h)
#define DELAI_VERIFICATION_BATTERIE_MS 5000						// Délai test entre deux vérifications de batterie (faible délai pour tester le bon fonctionnement du code)

#define DELAI_ACTIVATION_TRANSISTOR_MS 10 // Délai entre l'enclenchement du transistor et la mesure de l'état de la batterie
#define DELAI_TIMER6_MS 10 // Délai entre 2 interruptions du timer 6

#define DEBUG_ALARME_BATTERIE false // Quand vrai, change l'état de la batterie à chaque réveil (permet de tester l'envoi des trames)


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

// *****************************************************************************
// *****************************************************************************
// Section: Prototypes
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------------	

bool GestionCheckBatterie(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, InfoBatterie *infoBatterie);
Etat MesurerEtatBatterie(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, InfoBatterie *infoBatterie);
void LancerCheckBatterie(TIM_HandleTypeDef* htim, InfoBatterie *infoBatterie);
Etat ControleEtatChargeBatterie(ADC_HandleTypeDef* hadc, InfoBatterie* infoBatterie);
void ArreterLectureTensionBatterie(TIM_HandleTypeDef* htim, InfoBatterie* infoBatterie);


#ifdef __cplusplus
}
#endif

#endif /* _GESTION_BATTERIE_H */
