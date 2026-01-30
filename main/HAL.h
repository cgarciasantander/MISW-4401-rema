#ifndef HAL_H
#define HAL_H

#include <Arduino.h>

// Platform-specific includes
#if defined(ESP32)
    #include <WiFi.h>
    #include <WiFiClientSecure.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <WiFiClientSecure.h>
#endif

// Platform constants (defined in platform-specific .cpp files)
extern const char* PLATFORM_NAME;
extern const uint8_t HAL_LED_BUILTIN;
extern const uint8_t HAL_DHT_PIN;
extern const uint8_t HAL_LDR_PIN;
extern const int HAL_ADC_MAX;

// Platform-specific functions
void HAL_setHostname(const char* hostname);
void HAL_setCACert(WiFiClientSecure& client, const char* cert);
void HAL_deepSleep();

#endif // HAL_H
