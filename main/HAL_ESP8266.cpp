#ifdef ESP8266

#include "HAL.h"

const char* PLATFORM_NAME = "ESP8266";
const uint8_t HAL_LED_BUILTIN = 2;
const uint8_t HAL_DHT_PIN = 4;  // D2 on NodeMCU
const uint8_t HAL_LDR_PIN = A0;
const int HAL_ADC_MAX = 1023;

// BearSSL certificate for ESP8266
static BearSSL::X509List* certList = nullptr;

void HAL_setHostname(const char* hostname) {
    WiFi.hostname(hostname);
}

void HAL_setCACert(WiFiClientSecure& client, const char* cert) {
    if (certList != nullptr) {
        delete certList;
    }
    certList = new BearSSL::X509List(cert);
    client.setTrustAnchors(certList);
}

void HAL_deepSleep() {
    ESP.deepSleep(0);
}

#endif // ESP8266
