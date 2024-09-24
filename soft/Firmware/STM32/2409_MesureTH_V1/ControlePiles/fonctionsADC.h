/* ************************************************************************** */
/** Descriptive File Name

  @Company
    ETML-ES

  @File Name
    fonctionsADC.h

  @Auteurs
    - Perret Mélissa

  @Description
    Fonctions liée à l'ADC
 */
/* ************************************************************************** */



#ifndef _FONCTIONS_H
#define _FONCTIONS_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

#include "stm32f0xx_hal.h" // pour uint16_t et ADC_HandleTypeDef

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

#define VAL_ADC_MAX 4095 // Valeur max de l'ADC suivant sa resolution 12 bit
#define VREF_ADC_V 3.3	// Valeur de la tension max de l'adc

#define TIMEOUT_LECTURE_ADC_MS 10000 // Timeout lors de la mesure du registre du ADC


// *****************************************************************************
// *****************************************************************************
// Section: Prototypes
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------------	

uint16_t LectureValeurAdcBrute(ADC_HandleTypeDef* hadc, uint8_t channel);
float ConversionValeurAdcEnVolt(uint16_t valeurADC, uint32_t pontDiviseurResistanceHaute, uint32_t pontDiviseurResistanceBasse);
uint16_t ConversionVoltEnDac(float volt);


#ifdef __cplusplus
}
#endif

#endif /* _FONCTIONS_H */
