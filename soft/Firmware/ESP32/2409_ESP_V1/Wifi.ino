//  CommunicationServeur.ino
//
//	Description : fonctions liées au Wifi
//	Auteur : Perret Mélissa
//  Création : 	22/09/2024
//  Modifications :  --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "Wifi.h"


//// Fonction ConfigurerWifi: configuration de la connexion Wifi
//// Description: initie la connexion au réseau Wifi en utilisant le SSID et le mot de passe
//// Entrées: -
//// Sorties: -
void ConfigurerWifi() {
  wifiMulti.addAP(WIFI_SSID, WIFI_MOT_DE_PASSE); // Connexion au réseau Wifi
}