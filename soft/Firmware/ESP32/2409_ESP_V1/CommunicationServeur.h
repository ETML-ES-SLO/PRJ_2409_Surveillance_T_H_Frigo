/* ************************************************************************** */
/** Descriptive File Name

  @Company
    ETML-ES

  @File Name
    CommunicationServeur.h

  @Auteurs
    - Perret Mélissa

  @Description
    Fonctions liées à la communication entre l'ESP et le serveur
 */
/* ************************************************************************** */


#ifndef _COMMUNICATION_SERVEUR_H
#define _COMMUNICATION_SERVEUR_H

#define NOMBRE_TENTATIVES_REQUETES 10 // Combien de fois on essaye d'effectuer les requêtes HTTP (car des fois on obtient un read timeout sur le réseau de l'école)
#define HTTP_TIMEOUT_MS 10000 // Combien de fois on essaye d'effectuer les requêtes HTTP (car des fois on obtient un read timeout sur le réseau de l'école)
#define DELAI_NOUVELLE_TENTATIVE_HTTP_MS 2000 // Délai avant de réessayer une requête HTTP

// Tableau ASCII: https://ss64.com/ascii.html
const uint8_t OCTET_DEBUT = 0x02;  // Caractère STX (Start of Text) utilisé pour identifier les débuts de trames
const uint8_t OCTET_FIN = 0x03;    // Caractère ETX (End of Text) utilisé pour identifier les fins de trames

// Trame: OCTET_DEBUT + index + ':' + valeur + OCTET_FIN
const uint8_t TAILLE_TRAME = sizeof(OCTET_DEBUT) + sizeof(uint8_t) + sizeof(char) + sizeof(double) + sizeof(OCTET_FIN);

const char separateur = ':';  // Séparateur entre l'index et la valeur dans les trames, pour faciliter le debuggage

// Rappel format des trames :
// trame[0] = OCTET_DEBUT;
// trame[1] = index;
// trame[2] = ':';
// trame[3] à trame[10] = valeur;
// trame[11] = OCTET_FIN;
uint8_t trame[TAILLE_TRAME];  // Tableau pour envoyer les trames


// https://www.arduino.cc/reference/en/language/variables/data-types/array/
// Utilisation d'un tableau de String et d'un tableau de double séparés (au lieu d'utiliser un tableau de DefinitionValeur) car les valeurs String ne peuvent pas être sauvegardées dans la RTC
String nomValeursServeur[6]{
  "seuilTemperatureMin",
  "seuilTemperatureMax",
  "ecartTemperature",
  "seuilHumiditeMin",
  "seuilHumiditeMax",
  "ecartHumidite",
};

// Note : ne pas définir les valeurs initiales directement ici pour éviter de réinitialiser les valeurs à chaque réveil
//        les valeurs sont initialisées dans la fonction GestionReveil uniquement lorsqu'on vient de démarrer l'ESP
RTC_DATA_ATTR double valeursServeur[6];


#endif /* _COMMUNICATION_SERVEUR_H */