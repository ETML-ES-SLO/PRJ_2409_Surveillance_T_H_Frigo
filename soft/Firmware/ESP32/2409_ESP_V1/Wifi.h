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
#define WIFI_SSID "Wifi-Projet-Diplomes"
#define WIFI_MOT_DE_PASSE "DiplomeSLO2"
#define SERVEUR_ADDRESSE_HTTP "http://192.168.1.102:8080/MesureTH/api.php"


WiFiMulti wifiMulti;


#endif /* _WIFI_H */
