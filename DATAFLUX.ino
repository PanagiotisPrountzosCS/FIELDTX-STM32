/*  DATAFLUX.ino
 *
 *  Panagiotis Prountzos 2025
 *
 *  Slave node branch
 */

#include <WiFi.h>
#include <esp_now.h>

#include "DATAFLUX.h"

// firmware variables
LSM303 sensor(true, false);
uint8_t masterMAC[] = {0x30, 0xC9, 0x22, 0xB0, 0xE4, 0x6C};
uint32_t selfID = 1;

// status variables
fvec3 magReading;
message msg;
bool skipCycle = false;
RTC_DATA_ATTR uint32_t elapsedTime;

esp_now_peer_info_t masterInfo;

void dataTransmissionCallback(const uint8_t *mac_addr, esp_now_send_status_t status) {
    deepSleep(elapsedTime);
}

void setup() {
    initSlave();

    msg.id = selfID;

    if (!initESPNOW(masterInfo, masterMAC, dataTransmissionCallback)) skipCycle = true;

    if (!initSensor(sensor)) skipCycle = true;

    if (!sensor.checkI2CCommunication()) {
        indicateError(SENSOR_I2C_ERROR);
        skipCycle = true;
    }
    if (skipCycle) deepSleep(elapsedTime);

    sensor.readMag(magReading);
    msg.x = magReading.x;
    msg.y = magReading.y;
    msg.z = magReading.z;
    msg.timestamp = elapsedTime + millis();

    publishMessage(&msg, masterMAC, elapsedTime);
}

void loop() {
    // should NEVER reach here
    delay(1000);
}
