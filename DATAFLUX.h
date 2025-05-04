/*  DATAFLUX.h
 *
 *  Panagiotis Prountzos 2025
 *
 *  Master node branch
 */

#ifndef _DATAFLUX_H
#define _DATAFLUX_H

#define LED 15

#define TICK_DURATION 50
#define MESSAGE_QUEUE_MAX_SIZE 100
#define MESSAGE_QUEUE_FLUSH_SIZE 25
#define MESSAGE_QUEUE_FLUSH_INTERVAL 3000

typedef struct {
    float x;
    float y;
    float z;
    uint32_t id;
    uint32_t timestamp;
} message;

enum ErrorCodes { ESP_NOW_INIT_ERROR = 0x01 };

void indicateError(uint8_t errorCode) {
    pinMode(LED, OUTPUT);
    for (int i = 0; i < errorCode; i++) {
        digitalWrite(LED, HIGH);
        delay(200);
        digitalWrite(LED, LOW);
        delay(200);
    }
}

void initEspNow() {
    if (esp_now_init() != ESP_OK) {
        indicateError(1);
    }
}

void pollQueue(QueueHandle_t dataQueue, uint32_t& lastClear) {
    if (millis() - lastClear > MESSAGE_QUEUE_FLUSH_INTERVAL ||
        uxQueueMessagesWaiting(dataQueue) >= MESSAGE_QUEUE_FLUSH_SIZE) {
            message msg;

            Serial.println("Emptying queue");
            xQueueReset(dataQueue);
            lastClear = millis();
    }
}

#endif