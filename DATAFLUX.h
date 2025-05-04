/*  DATAFLUX.h
 *
 *  Panagiotis Prountzos 2025
 *
 */

#ifndef _DATAFLUX_H
#define _DATAFLUX_H
#include <esp_now.h>
#include <esp_wifi.h>

#define LEDPIN 15
#define ERRORSLEEPDURATIONMS 2500
#define NORMALSLEEPDURATIONMS 1000

typedef struct {
    float x;
    float y;
    float z;
    uint32_t id;
    uint32_t timestamp;
} message;

enum ErrorCodes {};

void handleError(uint8_t errorCode) {
    pinMode(LEDPIN, OUTPUT);
    for (uint8_t i = 0; i < errorCode; i++) {
        digitalWrite(LEDPIN, HIGH);
        delay(200);
        digitalWrite(LEDPIN, LOW);
        delay(200);
    }
}

void initEspNow() {
    esp_wifi_set_channel(10, WIFI_SECOND_CHAN_NONE);
    if (esp_now_init() != ESP_OK) handleError(5);
}


#endif