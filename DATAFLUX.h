/*  DATAFLUX.h
 *
 *  Panagiotis Prountzos 2025
 *
 */

#ifndef _DATAFLUX_H
#define _DATAFLUX_H

#include "modules/ALSMD/ALSMD.h"

#define LEDPIN 15
#define ERRORSLEEPDURATIONMS 2500
#define NORMALSLEEPDURATIONMS 1000

uint32_t SLAVE_ID = 0;

typedef struct {
    uint8_t payload[16];
} message;

enum ErrorCodes {
    ESP_NOW_INIT_FAIL = 0x01,
    ESP_NOW_ADD_PEER_FAIL = 0x02,
    ESP_NOW_TRANSMIT_FAIL = 0x03,
    LSM_INIT_FAIL = 0x04,
    LSM_COMMUNICATION_FAIL = 0x05
};

message package(fvec3 sensorData) {
    message result;

    memcpy(result.payload, &sensorData.x, 4);
    memcpy(result.payload + 4, &sensorData.y, 4);
    memcpy(result.payload + 8, &sensorData.z, 4);
    memcpy(result.payload + 12, &SLAVE_ID, 4);

    return result;
}

void handleError(uint8_t errorCode) {
    pinMode(LEDPIN, OUTPUT);
    for (uint8_t i = 0; i < errorCode; i++) {
        digitalWrite(LEDPIN, HIGH);
        delay(200);
        digitalWrite(LEDPIN, LOW);
        delay(200);
    }
}

void deepSleep(uint32_t durationms) {
    esp_sleep_enable_timer_wakeup(durationms * 1000LL);
    esp_deep_sleep_start();
}

void initEspNow(esp_now_peer_info_t* masterInfo, const uint8_t* masterMac) {
    if (esp_now_init() != ESP_OK) {
        handleError(ESP_NOW_INIT_FAIL);
        deepSleep(ERRORSLEEPDURATIONMS);
    }

    memcpy(masterInfo->peer_addr, masterMac, 6);
    masterInfo->channel = 0;
    masterInfo->encrypt = false;

    if (esp_now_add_peer(masterInfo) != ESP_OK) {
        handleError(ESP_NOW_ADD_PEER_FAIL);
        deepSleep(ERRORSLEEPDURATIONMS);
    }
}

void initLSM(LSM303* sensor) {
    sensor->configure(ACCEL_RATE_10HZ, MAG_RATE_0_75HZ, ACCEL_MODE_POWERDOWN, MAG_MODE_SINGLE,
                      ACCEL_SCALE_2G, MAG_SCALE_1_3);

    if (!sensor->init()) {
        handleError(LSM_INIT_FAIL);
        deepSleep(ERRORSLEEPDURATIONMS);
    }
}

void transmitMessage(const message* slaveMessage, const uint8_t* masterMac) {
    esp_err_t result =
        esp_now_send(masterMac, slaveMessage->payload, sizeof(slaveMessage->payload));

    // consider removing deep sleep on this one, what happens if data isnt transmitted correct?

    if (result != ESP_OK) {
        handleError(ESP_NOW_TRANSMIT_FAIL);
        deepSleep(ERRORSLEEPDURATIONMS);
    }
}

#endif