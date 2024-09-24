//  Execution.ino
//
//	Description : fonctions liées à l'exécution principale de la logique de l'ESP
//	Auteur : Perret Mélissa
//  Création : 	22/09/2024
//  Modifications :  --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "Execution.h"
#include "Wifi.h"     // pour wifiMulti
#include "Sommeil.h"  // pour PIN_REVEIL_PAR_STM


//// Fonction ExecutionReveilTimer: logique princiaple de l'ESP lorsqu'il est reveillé par le timer
//// Description: connexion Wifi, réveiller le STM, récupéreration et transmissions des valeurs du serveur, envoie notification Discord, retour en mode sommeil
//// Entrées: -
//// Sorties: -
void ExecutionReveilTimer() {

  // connexion WiFi
  if ((wifiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    if (MODE_DEBUG) {
      printf("[HTTP] begin...\n");
    }

    http.begin(SERVEUR_ADDRESSE_HTTP);  // pour indiquer le début de la communication HTTP

    ReveillerSTM();
    delay(DELAI_TRANSMISSION_STM_MS);  // pour s'assurer que le STM soit bien prêt à recevoir les trames UART

    ReceptionServeur(&http);  // réception et traitement des données du serveur

    EnvoyerSignalFinSTM();  // indiquer au STM que l'ESP à fini d'envoyer ses éventuelles trames UART

    // considérer les valeurs du STM comme non changées avant de recevoir les éventuels changements de la part du STM
    valeursSTM[ETAT_SEUILS].valeur = IDENTIQUE;
    valeursSTM[ETAT_BATTERIE].valeur = IDENTIQUE;
    ReceptionSTM(&http);

    EnvoyerNotificationsDiscord(&http);  // envoyer les éventuelles notificartions Discord en cas de changements de valeurs de la part du STM

    http.end();  // pour indiquer la fin de la communication HTTP
  }

  delay(DELAI_MODE_SOMMEIL_MS);  // délai éventuel avant de passer en mode sommeil (par précautions)

  EntreeModeSommeilProfond();  // entrer en mode sommeil
}

//// Fonction ExecutionReveilSTM: logique princiaple de l'ESP lorsqu'il est reveillé par le STM
//// Description: réception trames STM, envoie notification Discord, retour en mode sommeil
//// Entrées: -
//// Sorties: -
void ExecutionReveilSTM() {
  // connexion WiFi
  if ((wifiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    if (MODE_DEBUG) {
      printf("[HTTP] begin...\n");
    }

    http.begin(SERVEUR_ADDRESSE_HTTP);  // pour indiquer le début de la communication HTTP

    // marquer les valeurs du STM comme non changées avant de recevoir les éventuels changements de la part du STM
    valeursSTM[ETAT_SEUILS].valeur = IDENTIQUE;
    valeursSTM[ETAT_BATTERIE].valeur = IDENTIQUE;

    // procéder à la réception des données UART tant que le STM ne nous indique pas qu'il a terminé d'envoyer d'éventuelles trames UART
    do {
      ReceptionSTM(&http);
      delay(DELAI_RECEPTION_STM_MS);                    // Délai entre deux essais pour recevoir les trames UART attendues de la part du STM
    } while (digitalRead(PIN_REVEIL_PAR_STM) == HIGH);  // Attendre que le STM nous signale qu'il a terminé d'envoyer les trames UART

    ReceptionSTM(&http);  // Important de refaire une nouvelle réception de trames UART au cas où le STM aurait envoyé des trames juste avant d'indiquer le signal de fin de communication

    EnvoyerNotificationsDiscord(&http);  // envoyer les éventuelles notifications Discord en cas de changements de valeurs de la part du STM

    http.end();  // pour indiquer la fin de la communication HTTP
  }

  delay(DELAI_MODE_SOMMEIL_MS);  // délai avant de passer en mode sommeil (par précautions)

  EntreeModeSommeilProfond();
}