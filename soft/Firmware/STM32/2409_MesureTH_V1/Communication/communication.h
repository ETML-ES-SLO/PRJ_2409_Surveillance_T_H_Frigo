/* ************************************************************************** */
/** Descriptive File Name

  @Company
    ETML-ES

  @File Name
    communication.h

  @Auteurs
    - Perret M�lissa

  @Description
    Fonctions li�es � la communication ESP/STM
 */
/* ************************************************************************** */



#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
 
 #include "main.h" // pour DefinitionValeur
 

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

#define TAILLE_BUFFER 50 // Taille du tableau utilis� pour stocker les donn�es UART re�ues
#define UART_RECEPTION_TIMEOUT_MS 3000 // Timeout lors de la r�ception UART
#define DELAI_ENVOIE_TRAME_MS 1000 // D�lai entre le r�veil de l'ESP et l'envoie de trame pour s'assurer que l'ESP est pr�t � recevoir les trames


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

void ReceptionnerTrameUART(UART_HandleTypeDef* huart, DefinitionValeur valeursServeur[]);
void EnvoyerTrameUART(UART_HandleTypeDef* huart, uint8_t index, double valeur);


#ifdef __cplusplus
}
#endif

#endif /* _COMMUNICATION_H */
