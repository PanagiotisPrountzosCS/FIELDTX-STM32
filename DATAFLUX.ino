/*  DATAFLUX.ino
 *
 *  Panagiotis Prountzos 2025
 *
 *  MASTER NODE - THIS BRANCH HOLDS THE FIRMWARE FOR THE MASTER ESP32
 *
 */

#include <WiFi.h>
#include <esp_now.h>

#include "DATAFLUX.h"

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (len != sizeof(message)) {
        Serial.println("Received invalid message size");
        return;
    }

    message msg;
    memcpy(&msg, data, sizeof(msg));

    Serial.print("ID: ");
    Serial.print(msg.id);
    Serial.print(" | x: ");
    Serial.print(msg.x);
    Serial.print(" | y: ");
    Serial.print(msg.y);
    Serial.print(" | z: ");
    Serial.println(msg.z);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    initEspNow();

    esp_now_register_recv_cb(onReceive);
}

void loop() {
    delay(1000);  // Passive wait
}
