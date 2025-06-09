/*  DATAFLUX.h
 *
 *  Panagiotis Prountzos 2025
 *
 *  Slave node branch
 */

#ifndef _DATAFLUX_H
#define _DATAFLUX_H

#include "modules/ALSMD/ALSMD.h"
#include "esp_wifi.h"

#define LED 15
#define CYCLE_DURATION_MS 1000
#define WIFI_CHANNEL 10 //fixed channel because master node needs both espnow and wifi :(

enum errorCodes {
    ESP_NOW_INIT_ERROR = 0x01,
    ESP_NOW_ADD_PEER_ERROR,
    ESP_NOW_REGISTER_CALLBACK_ERROR,
    ESP_NOW_TRANSMIT_ERROR,
    SENSOR_INIT_ERROR,
    SENSOR_I2C_ERROR
};

typedef struct message {
    float x;
    float y;
    float z;
    uint32_t id;
    uint32_t timestamp;
} message;

void indicateError(uint8_t errorCode) {
    pinMode(LED, OUTPUT);
    for (int i = 0; i < errorCode; i++) {
        digitalWrite(LED, HIGH);
        delay(200);
        digitalWrite(LED, LOW);
        delay(200);
    }
    delay(1000);
}

void deepSleep(uint32_t& elapsedTime) {
    uint32_t offset = millis();
    uint32_t sleepTime = CYCLE_DURATION_MS;

    if (offset < CYCLE_DURATION_MS) {
        sleepTime -= offset;
    }

    elapsedTime += offset + sleepTime;

    esp_sleep_enable_timer_wakeup(sleepTime * 1000);

    esp_deep_sleep_start();
}

void initSlave() {
    Wire.begin();
    WiFi.mode(WIFI_STA);
}

bool initESPNOW(esp_now_peer_info_t& peerInfo, const uint8_t* receiver,
                esp_now_send_cb_t onDataSent) {
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    if (esp_now_init() != ESP_OK) {
        indicateError(ESP_NOW_INIT_ERROR);
        return false;
    };

    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    // Register master node(receiver)
    memcpy(peerInfo.peer_addr, receiver, 6);

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        indicateError(ESP_NOW_ADD_PEER_ERROR);
        return false;
    };

    // Register the callback here
    if (esp_now_register_send_cb(onDataSent) != ESP_OK) {
        indicateError(ESP_NOW_REGISTER_CALLBACK_ERROR);
        return false;
    };

    return true;
}

bool initSensor(LSM303& sensor) {
    sensor.configure(ACCEL_RATE_0HZ, MAG_RATE_3HZ, ACCEL_MODE_POWERDOWN, MAG_MODE_SINGLE,
                     ACCEL_SCALE_2G, MAG_SCALE_8_1);
    if (!sensor.init()) {
        indicateError(SENSOR_INIT_ERROR);
        return false;
    };
    return true;
}

void publishMessage(const message* msg, const uint8_t* receiver, uint32_t& elapsedTime) {
    esp_err_t result = esp_now_send(receiver, (uint8_t*)msg, sizeof(message));
    if (result != ESP_OK) {
        indicateError(ESP_NOW_TRANSMIT_ERROR);
        deepSleep(elapsedTime);
    }
}

#endif
