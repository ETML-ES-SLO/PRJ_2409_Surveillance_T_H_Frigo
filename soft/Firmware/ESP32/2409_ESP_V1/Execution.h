/* ************************************************************************** */
/** Descriptive File Name

  @Company
    ETML-ES

  @File Name
    Execution.h

  @Auteurs
    - Perret Mélissa

  @Description
    Fonctions liées à l'exécution principale de la logique de l'ESP
 */
/* ************************************************************************** */


#ifndef _EXECUTION_H
#define _EXECUTION_H

#define DELAI_TRANSMISSION_STM_MS 1000  // Délai entre le réveil du STM et l'envoi des trames UART pour s'assurer que le STM soit bien prêt à recevoir les trames UART
#define DELAI_RECEPTION_STM_MS 1000  // Délai entre deux essais pour recevoir les trames UART attendues de la part du STM
#define DELAI_MODE_SOMMEIL_MS 1000  // Délai avant de passer en mode sommeil (par précautions)


#endif /* _EXECUTION_H */