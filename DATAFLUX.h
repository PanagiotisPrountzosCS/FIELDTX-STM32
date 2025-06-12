/*  DATAFLUX.h
 *
 *  Panagiotis Prountzos 2025
 *
 *  Master node branch
 */

#ifndef _DATAFLUX_H
#define _DATAFLUX_H

#include <PubSubClient.h>

#include "esp_wifi.h"

#define LED 2

#define TICK_DURATION 250
#define MESSAGE_QUEUE_MAX_SIZE 100
#define MESSAGE_QUEUE_FLUSH_SIZE 25
#define MESSAGE_QUEUE_FLUSH_INTERVAL 3000
#define CONNECT_WIFI_TIMEOUT 30000  // attempt to connect to wifi for 1 minute
#define MQTT_CONNECT_TIMEOUT 30000  // same, but mqtt
#define WIFI_CHANNEL 11
#define MAX_MQTT_MESSAGE_SIZE 256

typedef struct {
    float x;
    float y;
    float z;
    uint32_t id;
    uint32_t timestamp;
} message;

constexpr uint32_t msgBufferSize = MAX_MQTT_MESSAGE_SIZE / sizeof(message);

enum ErrorCodes {
    ESP_NOW_INIT_ERROR = 0x01,
    ESP_NOW_REGISTER_CALLBACK_ERROR,
    X_QUEUE_INIT_ERROR,
    WIFI_CONNECT_ERROR,
    MQTT_CONNECT_ERROR
};

void indicateError(uint8_t errorCode, bool triggerReset) {
    pinMode(LED, OUTPUT);
    for (int i = 0; i < errorCode; i++) {
        digitalWrite(LED, HIGH);
        delay(200);
        digitalWrite(LED, LOW);
        delay(200);
    }
    delay(1000);
    if (triggerReset) abort();  // reset system
}

void initEspNow() {
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    if (esp_now_init() != ESP_OK) indicateError(ESP_NOW_INIT_ERROR, true);
}

void pollQueue(PubSubClient* client, const char* mqtt_topic, QueueHandle_t dataQueue,
               uint32_t& lastClear) {
    if (millis() - lastClear > MESSAGE_QUEUE_FLUSH_INTERVAL ||
        uxQueueMessagesWaiting(dataQueue) >= MESSAGE_QUEUE_FLUSH_SIZE) {
        // empty queue in here
        uint32_t counter = 0;
        message msgArray[msgBufferSize];
        while (xQueueReceive(dataQueue, msgArray + counter, 0) == pdTRUE) {
            counter++;
            if (counter == msgBufferSize) {
                client->publish(mqtt_topic, (uint8_t*)msgArray, counter * sizeof(message));
                // Serial.println("Transmitted data : ");
                // Serial.println(counter * sizeof(message));
                counter = 0;
            }
        }

        if (counter) {
            client->publish(mqtt_topic, (uint8_t*)msgArray, counter * sizeof(message));
            // Serial.println("Transmitted data : ");
            // Serial.println(counter * sizeof(message));
        }
        lastClear = millis();
        // Serial.println("==========");
    }
}

void setup_wifi(const char* ssid, const char* password) {
    uint32_t startTime = millis();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < CONNECT_WIFI_TIMEOUT) {
        delay(500);
    }
    if (WiFi.status() != WL_CONNECTED) indicateError(WIFI_CONNECT_ERROR, true);
    //Serial.println("Wifi Connected");
}

void setup_mqtt(WiFiClientSecure* secureClient, PubSubClient* client,
                const char* root_ca,  // change WiFiClientSecure*
                const char* mqtt_server, const uint32_t mqtt_port) {
    secureClient->setCACert(root_ca);  // change uncomment this

    client->setServer(mqtt_server, mqtt_port);
}

void reconnect_mqtt(PubSubClient* client, const char* mqtt_username, const char* mqtt_password) {
    uint32_t startTime = millis();
    while (!client->connected() && millis() - startTime < MQTT_CONNECT_TIMEOUT) {
        if (client->connect("DATAFLUX MASTER NODE", mqtt_username, mqtt_password)) {
            client->publish("diagnostic", "Master node reconnected");
        } else {
            delay(500);
        }
    }
    if (!client->connected()) indicateError(MQTT_CONNECT_ERROR, true);
    //Serial.println("MQTT Connected");
}

#endif