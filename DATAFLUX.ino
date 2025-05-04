/*  DATAFLUX.ino
 *
 *  Panagiotis Prountzos 2025
 *
 *  Master node branch
 */

#include <WiFi.h>
#include <esp_now.h>

#include "DATAFLUX.h"

QueueHandle_t dataQueue;
uint32_t lastClear;

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (len != sizeof(message)) return;

    message msg;
    memcpy(&msg, data, len);

    xQueueSend(dataQueue, &msg, portMAX_DELAY);
    Serial.println("Message received");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    initEspNow();

    // Init queue
    dataQueue = xQueueCreate(MESSAGE_QUEUE_MAX_SIZE, sizeof(message));

    // Register callback
    lastClear = millis();
    esp_now_register_recv_cb(onReceive);
}

void loop() {
    pollQueue(dataQueue, lastClear);
    delay(TICK_DURATION);  // Passive wait
}
