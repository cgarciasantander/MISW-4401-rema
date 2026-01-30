#ifdef ESP32

#include "HAL.h"

const char* PLATFORM_NAME = "ESP32";
const uint8_t HAL_LED_BUILTIN = 2;
const uint8_t HAL_DHT_PIN = 4;
const uint8_t HAL_LDR_PIN = 34;
const int HAL_ADC_MAX = 4095;

void HAL_setHostname(const char* hostname) {
    WiFi.setHostname(hostname);
}

void HAL_setCACert(WiFiClientSecure& client, const char* cert) {
    client.setCACert(cert);
}

void HAL_deepSleep() {
    ESP.deepSleep(0);
}

#endif // ESP32
