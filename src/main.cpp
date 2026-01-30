#include "HAL.h"
#include "secrets.h"
#include <time.h>
#include <PubSubClient.h>
#include <DHT.h>

// Validate security configuration
#if (defined(CHECK_PUB_KEY) && defined(CHECK_CA_ROOT)) || \
    (defined(CHECK_PUB_KEY) && defined(CHECK_FINGERPRINT)) || \
    (defined(CHECK_FINGERPRINT) && defined(CHECK_CA_ROOT))
  #error "Cannot have multiple security checks enabled"
#endif

// DHT sensor
#define DHTTYPE DHT11
DHT dht(HAL_DHT_PIN, DHTTYPE);

// MQTT client
WiFiClientSecure net;
PubSubClient client(net);

time_t now;

// MQTT callback for received messages
void receivedCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Received [");
    Serial.print(topic);
    Serial.print("]: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// Connect to MQTT broker
void mqtt_connect() {
    while (!client.connected()) {
        Serial.print("Time: ");
        Serial.print(ctime(&now));
        Serial.print("MQTT connecting ... ");

        if (client.connect(HOSTNAME, MQTT_USER, MQTT_PASS)) {
            Serial.println("connected.");
            client.subscribe(MQTT_SUB_TOPIC);
        } else {
            Serial.println("Connection failed, check MQTT credentials");
            Serial.print("Error code = ");
            Serial.println(client.state());

            if (client.state() == MQTT_CONNECT_UNAUTHORIZED) {
                HAL_deepSleep();
            }
            delay(5000);
        }
    }
}

// Connect to WiFi
void setupWiFi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);

    HAL_setHostname(HOSTNAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        if (WiFi.status() == WL_NO_SSID_AVAIL) {
            Serial.println("\nSSID not found");
            HAL_deepSleep();
        } else if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("\nConnection failed (wrong password or timeout)");
            HAL_deepSleep();
        }
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

// Sync time via NTP
void setupTime() {
    Serial.print("Syncing time via NTP");
    configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    now = time(nullptr);
    while (now < 1510592825) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }

    Serial.println(" done!");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    Serial.println(asctime(&timeinfo));
}

// Setup TLS security
void setupSecurity() {
    #if defined(CHECK_CA_ROOT)
        HAL_setCACert(net, digicert);
    #elif defined(CHECK_FINGERPRINT)
        net.setFingerprint(fp);
    #else
        net.setInsecure();
    #endif
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("=================================");
    Serial.print("Running on ");
    Serial.println(PLATFORM_NAME);
    Serial.println("=================================");

    // Initialize DHT sensor
    dht.begin();

    // Connect to WiFi
    setupWiFi();

    // Sync time
    setupTime();

    // Setup TLS
    setupSecurity();

    // Setup MQTT
    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(receivedCallback);
    mqtt_connect();
}

void loop() {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("Reconnecting WiFi");
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            Serial.print(".");
            delay(10);
        }
        Serial.println(" connected");
    } else {
        if (!client.connected()) {
            mqtt_connect();
        } else {
            client.loop();
        }
    }

    now = time(nullptr);

    // Read sensors
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    int lightRaw = analogRead(HAL_LDR_PIN);
    int lightPercent = map(lightRaw, 0, HAL_ADC_MAX, 0, 100);

    // Build JSON payloads
    String jsonHumidity = "{\"value\": " + String(humidity) + "}";
    String jsonTemp = "{\"value\": " + String(temperature) + "}";
    String jsonLight = "{\"value\": " + String(lightPercent) + "}";

    // Publish if readings are valid
    if (!isnan(humidity) && !isnan(temperature)) {
        client.publish(MQTT_PUB_TOPIC_HUMIDITY, jsonHumidity.c_str(), false);
        client.publish(MQTT_PUB_TOPIC_TEMP, jsonTemp.c_str(), false);
        client.publish(MQTT_PUB_TOPIC_LIGHT, jsonLight.c_str(), false);
    }

    // Print to serial
    Serial.print(MQTT_PUB_TOPIC_HUMIDITY);
    Serial.print(" -> ");
    Serial.println(jsonHumidity);

    Serial.print(MQTT_PUB_TOPIC_TEMP);
    Serial.print(" -> ");
    Serial.println(jsonTemp);

    Serial.print(MQTT_PUB_TOPIC_LIGHT);
    Serial.print(" -> ");
    Serial.println(jsonLight);

    delay(5000);
}
