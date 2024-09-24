//  CommunicationServeur.ino
//
//	Description : fonctions liées à l'entrée et la sortie du mode sommeil
//	Auteur : Perret Mélissa
//  Création : 	22/09/2024
//  Modifications :  --

//	Version		:	V1.0
/*--------------------------------------------------------*/


#include "Sommeil.h"


//// Fonction EntreeModeSommeilProfond: logique pour entrer en mode sommeil profond
//// Description: active les conditions de réveil (pin pour permettre le réveil depuis le STM, et timer pour le prochain réveil) et rentre en mode sommeil profond
//// Entrées: -
//// Sorties: -
void EntreeModeSommeilProfond() {

  // Activation réveil possible via GPIO
  const gpio_config_t config = {
    .pin_bit_mask = BIT(PIN_REVEIL_PAR_STM),
    .mode = GPIO_MODE_INPUT,
  };
  ESP_ERROR_CHECK(gpio_config(&config)); // Pour configurer la pin de réveil
  ESP_ERROR_CHECK(esp_deep_sleep_enable_gpio_wakeup(BIT(PIN_REVEIL_PAR_STM), ESP_GPIO_WAKEUP_GPIO_HIGH)); // Pour indiquer la pin permettant de sortir du mode sommeil (Permet au STM de réveiller l'ESP si besoin)

  // http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  esp_sleep_enable_timer_wakeup(DUREE_SOMMEIL_MS * CONVERSION_MS_EN_uS); // Pour indiquer le délai avant de sortir du mode sommeil (si l'on est pas réveillé avant via la pin)

  if (MODE_DEBUG) {
    printf("Entree en mode sommeil pendant %d ms (ou via réveil STM pin GPIO%d)\n", DUREE_SOMMEIL_MS, PIN_REVEIL_PAR_STM);
  }

  // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/sleep_modes.html
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_deep_sleep_start(); // Pour entrer en mode sommeil profond
}


//// Fonction GestionReveil: logique exécutée au moment du réveil
//// Description: détermine la raison du réveil (lancement, STM ou timer), lance l'exécution de la logique correspondante au type de réveil
//// Entrées: -
//// Sorties: -
void GestionReveil() {
  esp_sleep_wakeup_cause_t raisonReveil; // Déclaration variable locale
  raisonReveil = esp_sleep_get_wakeup_cause(); // Pour obtenir la raison du réveil
  if (MODE_DEBUG) {
    printf("\nRaison du réveil: %d\n", raisonReveil);
  }

  // Si la raison du réveil est inconnue, c'est qu'il s'agit du premier démarrage de l'ESP
  if (raisonReveil == ESP_SLEEP_WAKEUP_UNDEFINED) {
    if (MODE_DEBUG) {
      printf("Initialisation au premier réveil\n");
    }

    // Initialisation au premier réveil
    // On met des valeurs incohérentes pour s'assurer que les premières valeurs qui seront reçues du serveur seront considérées comme différentes    
    valeursServeur[0] = -9999;
    valeursServeur[1] = -9999;
    valeursServeur[2] = -9999;
    valeursServeur[3] = -9999;
    valeursServeur[4] = -9999;
    valeursServeur[5] = -9999;

    ExecutionReveilTimer();
  } else {
    // Si la raison du réveil provient de la pin GPIO, c'est que l'ESP a été réveillé par le STM
    if (raisonReveil == ESP_SLEEP_WAKEUP_GPIO) {
      if (MODE_DEBUG) {
        printf("Réveil causé par le STM (trames UART à réceptionner)\n");
      }

      ExecutionReveilSTM();
    } else {
      // ESP réveillé par le timer
      if (MODE_DEBUG) {
        printf("Réveil causé par le timer\n");
      }

      ExecutionReveilTimer();
    }
  }
}