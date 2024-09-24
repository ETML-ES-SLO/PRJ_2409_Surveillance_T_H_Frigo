/* ************************************************************************** */
/** Descriptive File Name

  @Company
    ETML-ES

  @File Name
    Sommeil.h

  @Auteurs
    - Perret Mélissa

  @Description
    Fonctions liées à l'entrée et la sortie du mode sommeil
 */
/* ************************************************************************** */

#ifndef _SOMMEIL_H
#define _SOMMEIL_H


// Attention: DUREE_SOMMEIL_MS doit coincider avec la valeur de DELAI_ENTRE_DEUX_REVEILS_MS du côté du STM (gestionBatterie.h)
#define DUREE_SOMMEIL_MS 5000    // Durée du mode sommeil de l'ESP en ms (quand il n'est pas réveillé par le STM). 
#define CONVERSION_MS_EN_uS 1000 // Facteur pour convertir des millisecondes en microsecondes

#define PIN_REVEIL_PAR_STM GPIO_NUM_4


#endif /* _SOMMEIL_H */