/* ************************************************************************** */
/** Descriptive File Name

  @Company
    ETML-ES

  @File Name
    mesures.h

  @Auteurs
    - Perret Mélissa

  @Description
    Fonctions liées aux mesures de la sonde (température et humidté)
 */
/* ************************************************************************** */



#ifndef _MESURES_H
#define _MESURES_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
 
#include "main.h" // pour Etat et DefinitionValeur


#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

// Structure pour stocker les informations liées aux mesures de la température et de l'humidité
typedef struct {
	int32_t temperatureEntierActuelle;
	float temperatureActuelle; 
	float temperatureAffichee; 
	
	int32_t humiditeEntierActuelle;
	float humiditeActuelle;	
	float humiditeAffichee;
	
	Etat etatSeuils;
} Mesures;


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

#define DEBUG_ALARME_SEUILS false // Quand vrai, change l'état des seuils (alarme) à chaque réveil (permet de tester l'envoi des trames)


// *****************************************************************************
// *****************************************************************************
// Section: Prototypes
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------------	

Etat EffectuerMesuresSonde(I2C_HandleTypeDef* hi2c2, Mesures* mesures, DefinitionValeur valeursServeur[]);


#ifdef __cplusplus
}
#endif

#endif /* _MESURES_H */
