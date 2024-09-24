//  Discord.ino
//
//	Description : fonctions liées à l'envoi de notifications Discoprd
//	Auteur : Perret Mélissa
//  Création : 	22/09/2024
//  Modifications :  --

//	Version		:	V1.0
/*--------------------------------------------------------*/

#include "Discord.h"
#include <WiFiClientSecure.h>  // pour envoyer notifications
#include <HTTPClient.h>        // pour HTTPClient


// Code provenant initialement de la libraiie Discord_WebHook : // https://github.com/usini/usini_discord_webhook/tree/main
// La librairie ne fonctionnait pas (erreur de compilation lié à un mauvais #include)
// Pour régler le problème, le code principal de la librairie à été directmeent intégré dans le projet
// Les fichiers originaux de la libraire (ainsi que la licence) sont inclus dans le dossier du projet.


//// Fonction EnvoyerNotificationsDiscord: gère l'envoie des notifications Discord (état batterie et état seuils alarme)
//// Description: gère l'envoie des notifications Discord (état batterie et état seuils alarme)
//// Entrées: Pointeur:HTTPClient http
//// Sorties: -
void EnvoyerNotificationsDiscord(HTTPClient* http) {

  // Traitement état batterie
  switch ((Etat)valeursSTM[ETAT_BATTERIE].valeur) {
    case FONCTIONEL:
      EnvoyerNotificationDiscord(http, false, "Charge OK");
      break;
    case ALARME:
      EnvoyerNotificationDiscord(http, false, "Charge faible !");
      break;
    case IDENTIQUE:
      break;
  }

  // Traitement alarme seuils
  switch ((Etat)valeursSTM[ETAT_SEUILS].valeur) {
    case FONCTIONEL:
      EnvoyerNotificationDiscord(http, false, "Seuils OK");
      break;
    case ALARME:
      EnvoyerNotificationDiscord(http, false, "Alarme seuils !");
      break;
    case IDENTIQUE:
      break;
  }
}

//// Fonction EnvoyerNotificationDiscord: gère l'envoie d'une notifications Discord (texte passé en paramètre d'entreé)
//// Description: gère l'envoie d'une notifications Discord (texte passé en paramètre d'entreé)
//// Entrées: Pointeur:HTTPClient http (client http utilisé pour effectuer la requête)
////          bool syntheseVocale (indique si la synthèse vocale pour le message est activé, pour permettre aux utilisés d'utilité la synthèse vocale d'entendre le message).
///           String message (text à envoyer dans Discord)
//// Sorties: bool (indique si la notification à bien été envoyée)
bool EnvoyerNotificationDiscord(HTTPClient* http, bool syntheseVocale, String message) {

  String discord_tts = "false";
  if (syntheseVocale) {
    discord_tts = "true";
  }

  // Utilisation de WiFiClientSecure pour pouvoir créer une requête HTTPS
  WiFiClientSecure* client = new WiFiClientSecure;
  bool ok = false;
  if (client) {
    client->setInsecure();  // Désactiver la vérification certificat SSL

    if (MODE_DEBUG) {
      printf("[HTTP] Connexion à Discord...\n");
      printf("[HTTP] Message: %s\n", message.c_str());
      printf("[HTTP] Synthèse Vocale: %s\n", discord_tts.c_str());
    }

    // Débuter requête HTTPS
    if (http->begin(*client, DISCORD_WEBHOOK_URL)) { // pour indiquer le début de la communication HTTP
      http->addHeader("Content-Type", "application/json");  // Définir la requête sous forme de JSON

      // Envoyer la requête POST
      int httpCode = http->POST("{\"content\":\"" + message + "\",\"tts\":" + discord_tts + "}");
      if (httpCode > 0) {  // Si un code HTTP code est renvoyé
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_NO_CONTENT) {
          // https://support.discord.com/hc/en-us/articles/228383668-Intro-to-Webhooks
          if (MODE_DEBUG) {
            printf("[HTTP] OK\n");
          }
          ok = true;
        } else {
          if (MODE_DEBUG) {
            // Erreur
            printf("[HTTP] ERREUR: %s\n", http->getString().c_str());
            ok = false;
          }
        }        
      } else {
        if (MODE_DEBUG) {
          // Serveur non accessible
          printf("[HTTP] ERREUR: %s\n", http->errorToString(httpCode).c_str());
          ok = false;
        }
      }
    } else {
      if (MODE_DEBUG) {
        // Echec de la requête
        printf("[HTTP] Connexion impossible\n");
        ok = false;
      }
    }
    http->end(); // pour indiquer la fin de la communication HTTP
  } else {
    if (MODE_DEBUG) {
      printf("[HTTP] Impossible de creer client\n");
      ok = false;
    }
  }
  delete client;
  return ok;
}