//  CommunicationSTM.ino
//
//	Description : fonctions liées à la communication entre l'ESP et le STM
//	Auteur : Perret Mélissa
//  Création : 	22/09/2024
//  Modifications :  --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "CommunicationSTM.h"
#include "CommunicationServeur.h"  // pour OCTET_DEBUT et OCTET_FIN
#include <HTTPClient.h>            // pour HTTPClient


//// Fonction ConfigurerUART: configuration de la transmission UART
//// Description: défini les pins utilisées pour l'écriture et la lecture UART
//// Entrées: -
//// Sorties: -
void ConfigurerUART() {
  uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };

  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));         // Configuration UART
  ESP_ERROR_CHECK(uart_set_pin(uart_num, TXD_PIN, RXD_PIN, -1, -1));  // Indiquer les pins utilisées pour l'écriture et la lecture UART
}

//// Fonction ReveillerSTM: logique pour réveiller le STM
//// Description: change l'état de la pin utilisée pour réveiller le STM
//// Entrées: -
//// Sorties: -
void ReveillerSTM() {
  pinMode(PIN_POUR_REVEILLER_STM, OUTPUT);  // Important de mettre la pin en mode OUTPUT pour pouvoir modifier sa valeur
  digitalWrite(PIN_POUR_REVEILLER_STM, HIGH);

  if (MODE_DEBUG) {
    printf("Reveil du STM en mettant la pin %d à l'état %d\n", PIN_POUR_REVEILLER_STM, digitalRead(PIN_POUR_REVEILLER_STM));
  }
}

//// Fonction EnvoyerSignalFinSTM: logique pour indiquer au STM que l'ESP a fini de lui transmettre des trames UART
//// Description: met la pin utilisée pour réveiller le STM à l'état bas (la pin à l'état haut réveille le STM, et l'état bas lui indique qu'il a reçu toutes les trames nécessaires de la part de l'ESP)
//// Entrées: -
//// Sorties: -
void EnvoyerSignalFinSTM() {
  pinMode(PIN_POUR_REVEILLER_STM, OUTPUT);  // Important de mettre la pin en mode OUTPUT pour pouvoir modifier sa valeur
  digitalWrite(PIN_POUR_REVEILLER_STM, LOW);

  if (MODE_DEBUG) {
    printf("Envoie signal fin transmission au STM en mettant la pin %d à l'état %d\n", PIN_POUR_REVEILLER_STM, digitalRead(PIN_POUR_REVEILLER_STM));
  }
}

//// Fonction ReceptionSTM: logique pour recevoir les trames UART de la part du STM
//// Description: réception des données UART
////              analyse des données pour localiser les trames (caractères de début et de fin)
////              analyse des trames reçues pour extraire et traiter les données reçues
////              envoi de requête HTTP PUT pour transmettre les nouvelles données au serveur (états alarme seuils et batterie)
//// Entrées: Pointeur:HTTPClient http (client http utilisé pour effectuer les requêtes auprès du serveur)
//// Sorties: -
void ReceptionSTM(HTTPClient* http) {
  int length = 0;

  do {
    // Réception UART
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    length = uart_read_bytes(uart_num, receptionTramesUART, length, UART_RECEPTION_TIMEOUT_MS);

    if (length > 0) {  // Si on a reçu quelque chose

      // On parcourt le buffer pour trouver toutes les trames (identifiées par OCTET_DEBUT)
      for (int indexDebutTrame = 0; indexDebutTrame < length; indexDebutTrame++) {
        if (receptionTramesUART[indexDebutTrame] == OCTET_DEBUT) {  // Début de trame trouvé
          int indexFinTrame = indexDebutTrame + TAILLE_TRAME - 1;
          if (indexFinTrame < length && receptionTramesUART[indexFinTrame] == OCTET_FIN)  // Si on a bien une fin de trame comme attendu
          {
            uint8_t index = receptionTramesUART[indexDebutTrame + 1];  // Deuxième octet de la trame correspond à l'identifiant de la trame
            // Troisème octet correspond à un séparateur ':' pour faciltier la lecture des trames lors du debuggage

            // Explications pour memcpy : https://en.cppreference.com/w/cpp/string/byte/memcpy
            // Les 8 octets suivants (octets 4 à 11) correspondent à la valeur de la trame
            double valeur;
            memcpy(&valeur, &receptionTramesUART[indexDebutTrame + 3], sizeof(double));  // Copier les 8 octets de la trame représentant la valeur dans la variable valeur

            if (MODE_DEBUG) {
              printf("Nouvelle valeur provenant du STM. Nom:%s Avant:%lf Maintenant:%lf\n", valeursSTM[index].nom.c_str(), valeursSTM[index].valeur, valeur);
            }

            valeursSTM[index].valeur = valeur;  // Mise à jour de la valeur

            // Envoie de la requête PUT pour transmettre la nouvelle valeur au serveur
            String putString = valeursSTM[index].nom + "=" + valeur;

            http->setTimeout(HTTP_TIMEOUT_MS);
            
            int httpCode;
            int nombreTentativesHTTP = 0;
            do {

              if(nombreTentativesHTTP > 0)
              {
                delay(DELAI_NOUVELLE_TENTATIVE_HTTP_MS); // S'il s'agit d'une nouvelle tentative, attendre un peu avant de réessayer
              }

              nombreTentativesHTTP++;

              httpCode = http->PUT(putString);

              // httpCode est négatif en cas d'erreur
              if (httpCode > 0) {
                // Le header HTTP à été envoyé et une réponse du serveur à été reçue

                if (MODE_DEBUG) {
                  printf("[HTTP] Requete PUT... code: %d\n", httpCode);
                }

                if (httpCode == HTTP_CODE_OK) {
                  String reponseServeur = http->getString();
                  if (MODE_DEBUG) {
                    printf("[HTTP] Requete PUT reponse serveur: %s\n", reponseServeur.c_str());
                  }
                }
              } else {
                printf("[HTTP] Requete PUT... echec, erreur: %s\n", http->errorToString(httpCode).c_str());
              }
            } while (httpCode <= 0 && nombreTentativesHTTP < NOMBRE_TENTATIVES_REQUETES);
          }
        }
      }
    }
  } while (length > 0);  // Continuer tant qu'on reçoit quelque chose
}