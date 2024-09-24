//  CommunicationServeur.ino
//
//	Description : fonctions liées à la communication entre l'ESP et le serveur
//	Auteur : Perret Mélissa
//  Création : 	22/09/2024
//  Modifications :  --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "CommunicationServeur.h"
#include <Arduino_JSON.h>  // Librairie pour manipuler et traiter les données JSON reçues par le serveur
#include <HTTPClient.h>    // pour HTTPClient


//// Fonction ReceptionServeur: logique pour recevoir les données de la part du STM
//// Description: envoi d'une requête HTTP Get au serveur
////              réception de la réponse, analyse de la réponse pour extraire les données reçues (seuils min/max et écarts pour température et humidité)
////              appel de la fonction TraiterValeurServeur pour traiter les données reçues
//// Entrées: Pointeur:HTTPClient http (client http utilisé pour effectuer les requêtes auprès du serveur)
//// Sorties: -
void ReceptionServeur(HTTPClient* http) {

  if (MODE_DEBUG) {
    printf("[HTTP] Requete GET...\n");
  }

  http->setTimeout(HTTP_TIMEOUT_MS);

  int httpCode;
  int nombreTentativesHTTP = 0;

  do {

    if(nombreTentativesHTTP > 0)
    {
      delay(DELAI_NOUVELLE_TENTATIVE_HTTP_MS); // S'il s'agit d'une nouvelle tentative, attendre un peu avant de réessayer
    }

    nombreTentativesHTTP++;

    // Démarrer la connexion et envoyer la requête GET
    int httpCode = http->GET();

    // HttpCode est négatif en cas d'erreur
    if (httpCode > 0) {
      // Le header HTTP à été envoyé et une réponse du serveur à été reçue
      if (MODE_DEBUG) {
        printf("[HTTP] Requete GET... code: %d\n", httpCode);
      }

      if (httpCode == HTTP_CODE_OK) {
        String reponseServeur = http->getString(); // Obtenir la réponse du serveur

        if (MODE_DEBUG) {
          printf("[HTTP] Requete GET reponse serveur: %s\n", reponseServeur.c_str());
        }

        // Parsing JSON
        JSONVar myObject = JSON.parse(reponseServeur);
        JSONVar keys = myObject.keys();

        // Traitement des valeurs
        for (int i = 0; i < keys.length(); i++) {
          String nom = keys[i];
          double valeur = std::atof(myObject[keys[i]]);  // utilisation de la fonction atof pour convertir de string en double
          TraiterValeurServeur(nom, valeur);
        }
      }
    } else {
      printf("[HTTP] GET... echec, erreur: %s\n", http->errorToString(httpCode).c_str());
    }
  } while (httpCode <= 0 && nombreTentativesHTTP < NOMBRE_TENTATIVES_REQUETES);
}

//// Fonction TraiterValeurServeur: logique pour traiter une donnée reçue par le serveur
//// Description: vérifier si la valeur à changé
////              mettre à jour la valeur stockée dans la RTC
////              construire et transmettre la trame UART au STM
//// Entrées: String nom (nom du paramètre modifié)
////          double valeur (valeur du paramètre modifié)
//// Sorties: -
void TraiterValeurServeur(String nom, double valeur) {
  int8_t index = IndexValeurServeur(nom);  // Récupérer index du paramètre modifié (-1 si non trouvé)
  if (index == -1) {
    printf("Nouvelle valeur provenant du serveur. Impossible de trouver le nom %s dans nomValeursServeur\n", nom.c_str());
  } else if (valeur != valeursServeur[index]) {  // Si la valeur n'a pas changé, inutile d'en informer le STM

    if (MODE_DEBUG) {
      printf("Nouvelle valeur provenant du serveur. Nom:%s Avant:%lf Maintenant:%lf\n", nomValeursServeur[index].c_str(), valeursServeur[index], valeur);
    }

    valeursServeur[index] = valeur;  // Mise à jour de la valeur stockée dans la RTC

    // Rappel format des trames :
    // double trame[] = {0,0,0,0,0} // 40 octets
    // trame[0] = OCTET_DEBUT;
    // trame[1] = index
    // trame[2] = ':';
    // trame[3] à trame[10] = valeur;
    // trame[11] = OCTET_FIN;

    // Explications memset : https://cplusplus.com/reference/cstring/memset/
    memset(trame, 0, sizeof(trame));  // Réinitialisation de la trame en mettant tous les bits à 0 (par précautions)

    // Explications memcpy : https://en.cppreference.com/w/cpp/string/byte/memcpy
    memcpy(&trame[0], &OCTET_DEBUT, sizeof(OCTET_DEBUT));                                                                      // OCTET_DEBUT
    memcpy(&trame[sizeof(OCTET_DEBUT)], &index, sizeof(index));                                                                // index
    memcpy(&trame[sizeof(OCTET_DEBUT) + sizeof(index)], &separateur, sizeof(separateur));                                      // ':'
    memcpy(&trame[sizeof(OCTET_DEBUT) + sizeof(index) + sizeof(separateur)], &valeur, sizeof(double));                         // valeur
    memcpy(&trame[sizeof(OCTET_DEBUT) + sizeof(index) + sizeof(separateur) + sizeof(valeur)], &OCTET_FIN, sizeof(OCTET_FIN));  // OCTET_FIN

    int writeResult = uart_write_bytes(uart_num, trame, sizeof(trame));  // Transmission de la trame au STM

    if (MODE_DEBUG) {
      printf("\nEnvoie de %d octets en UART\n", writeResult);
    }
  }
}

//// Fonction IndexValeurServeur: logique pour retrouver depuis le nom d'un paramètre son index dans le tableau des valeurs
//// Description: parcourir le tableau nomValeursServeur pour trouver l'index où la valeur correspond au nom indiqué en paramètre d'entrée
//// Entrées: String nom (nom du paramètre recherché)
//// Sorties: iont8_t (index du paramètre dans le tableau des valeurs stockées, -1 si non trouvé)
int8_t IndexValeurServeur(String nom) {
  int tailleTableau = sizeof(valeursServeur) / sizeof(valeursServeur[0]);  // On recalcule la taille du tableau (pour plus d'informations sur la fonction sizeof : https://www.arduino.cc/reference/en/language/variables/utilities/sizeof/)
  for (int i = 0; i < tailleTableau; i++) {
    if (nomValeursServeur[i] == nom) {
      return i;  // Nom trouvé
    }
  }

  return -1;  // Nom introuvable
}