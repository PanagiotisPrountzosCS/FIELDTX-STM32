/*  DATAFLUX.ino
 *
 *  Panagiotis Prountzos 2025
 *
 */

#include <WiFi.h>
#include <esp_now.h>

#include "DATAFLUX.h"

// Receiver MAC address
uint8_t masterMAC[] = {0x54, 0x32, 0x04, 0x3D, 0x5C, 0xF4};

RTC_DATA_ATTR LSM303 sensor(true, false);
RTC_DATA_ATTR bool shouldInitLSM = true;
RTC_DATA_ATTR esp_now_peer_info_t masterInfo{};
RTC_DATA_ATTR fvec3 magData;
RTC_DATA_ATTR message slaveMessage;

void setup() {
    // initializations
    setCpuFrequencyMhz(80);
    Wire.begin();
    WiFi.mode(WIFI_STA);

    // setup esp_now
    initEspNow(&masterInfo, masterMAC);

    // setup LSM303
    if (shouldInitLSM) {
        initLSM(&sensor);
        shouldInitLSM = false;
    }

    // validate I2C communication
    if(!sensor.checkI2CCommunication()){
        shouldInitLSM = true;
        handleError(LSM_COMMUNICATION_FAIL);
        deepSleep(ERRORSLEEPDURATIONMS);
    }

    // make measurement

    sensor.readMag(magData);

    // package it

    slaveMessage = package(magData);

    // transmit it

    transmitMessage(&slaveMessage, masterMAC);

    // deepsleep, until next iteration
    deepSleep(NORMALSLEEPDURATIONMS);
}

void loop() {
    // Should not reach here
}
