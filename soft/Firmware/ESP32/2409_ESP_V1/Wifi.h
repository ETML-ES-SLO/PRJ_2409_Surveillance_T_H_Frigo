/* ************************************************************************** */
/** Descriptive File Name

  @Company
    ETML-ES

  @File Name
    Wifi.h

  @Auteurs
    - Perret Mélissa

  @Description
    Fonctions liées au Wifi
 */
/* ************************************************************************** */


#ifndef _WIFI_H
#define _WIFI_H


#include <WiFiMulti.h> // pour WiFiMulti


// Wifi école
//#define WIFI_SSID "Wifi-Projet-Diplomes"
//#define WIFI_MOT_DE_PASSE "DiplomeSLO2"
//#define SERVEUR_ADDRESSE_HTTP "http://192.168.1.102:8080/MesureTH/api.php"

// Wifi perso
#define WIFI_SSID "SFR_7CD8"
#define WIFI_MOT_DE_PASSE "26dkjf2mjs26vyyk6pwg"
#define SERVEUR_ADDRESSE_HTTP "http://192.168.1.41:9090/MesureTH/api.php"

// Wifi test
//#define ADDRESSE_HTTP "https://www.howsmyssl.com/a/check"


WiFiMulti wifiMulti;


#endif /* _WIFI_H */