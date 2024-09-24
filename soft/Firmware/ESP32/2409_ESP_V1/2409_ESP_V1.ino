// Notes
// Comment afficher des variables de type String dans des printf: https://stackoverflow.com/questions/66555803/serial-printf-with-string-object-of-which-length-is-11-or-more
// Autres fichiers: les autres fichiers .ino sont intégrés par ordre alphabétique. Utilisation de .h pour avoir plus de contrôles pour les variables et les defines
// Les prototypes ne doivent pas être déclarés dans les .h, cela créé des conflits avec les prototypes générés automatiquement par Arduino IDE


// Les fichiers .h créés à part  doivent absolument être ajoutés dans le fichier principal !
// https://forum.arduino.cc/t/variable-or-field-declared-void/687410/11
#include "CommunicationServeur.h"
#include "CommunicationSTM.h"
#include "Discord.h"
#include "Execution.h"
#include "Sommeil.h"
#include "Wifi.h"


// Quand le mode debug est activé, affiche de nombreux messages dans le moniteur série pour pouvoir débugger plus facilement
// Attention: les printf sont égalements envoyés avec les données UART au STM, ce qui empêche le STM de bien recevoir les vraies trames UART envoyées
#define MODE_DEBUG false


//// Fonction setup: point d'entrée du code (fonction créée automatiquement et appelé au lancement ou au réveil de l'ESP)
//// Description: configuration de la communication UAR, configuration du Wifi, appel de la fonction GestionReveil
//// Entrées: -
//// Sorties: -
void setup() {

  Serial.begin(115200);  // Démarre le sérial

  ConfigurerUART();

  ConfigurerWifi();

  GestionReveil();
}


//// Fonction loop: fonction d'execution (fonction créée automatiquement et appelée en continu lors de l'exécution)
//// Description: fonction vide, car la logique d'exécution (fonctions ExecutionReveilTimer ou ExecutionReveilSTM) ne s'exécute qu'une seule fois avant de repasser en mode sommeil profond
//// Entrées: -
//// Sorties: -
void loop() {
}