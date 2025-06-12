/*  DATAFLUX.ino
 *
 *  Panagiotis Prountzos 2025
 *
 *  Master node branch
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <esp_now.h>

#include "DATAFLUX.h"

WiFiClientSecure secureClient;  // change to secure client
PubSubClient client(secureClient);
QueueHandle_t dataQueue;
uint32_t lastQueueClear;

const char* ssid = "sample";
const char* password = "sample";

const char* mqtt_server = "sample";
const int mqtt_port = 8883;
const char* mqtt_topic = "test";
const char* mqtt_username = "sample";
const char* mqtt_password = "sample";

const char* root_ca = "f";

void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (len != sizeof(message)) {
        return;
    }

    message msg;
    memcpy(&msg, data, len);

    xQueueSend(dataQueue, &msg, portMAX_DELAY);
}

void setup() {
    // Serial.begin(115200);
    WiFi.mode(WIFI_AP_STA);

    initEspNow();

    // int ch = WiFi.channel();
    // Serial.println(ch);

    if (esp_now_register_recv_cb(onReceive) != ESP_OK)
        indicateError(ESP_NOW_REGISTER_CALLBACK_ERROR, true);

    dataQueue = xQueueCreate(MESSAGE_QUEUE_MAX_SIZE, sizeof(message));

    if (dataQueue == NULL) indicateError(X_QUEUE_INIT_ERROR, true);
    lastQueueClear = millis();

    setup_wifi(ssid, password);

    setup_mqtt(&secureClient, &client, root_ca, mqtt_server, mqtt_port);
}

void loop() {
    if (!client.connected()) {
        // Serial.println("Disconnected from mqtt");
        reconnect_mqtt(&client, mqtt_username, mqtt_password);
    }
    client.loop();
    pollQueue(&client, mqtt_topic, dataQueue, lastQueueClear);
    delay(TICK_DURATION);
}
