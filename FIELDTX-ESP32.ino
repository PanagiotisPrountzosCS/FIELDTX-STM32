/*  FIELDTX-ESP32.ino
 *
 *  Panagiotis Prountzos 2025
 *
 */

#include "FIELDTX-ESP32.h"
#include "modules/ALSMD/ALSMD.h"

// anything else here

#include "esp_sleep.h"

RTC_DATA_ATTR LSM303 sensor(false, true);
RTC_DATA_ATTR bool shouldInitializeSensor = true;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    fvec3 accelReading;
    if (shouldInitializeSensor && sensor.checkI2CCommunication()) {
        sensor.setAccelPowerMode(ACCEL_MODE_LOW_POWER);
        sensor.setAccelDataRate(RATE_1_5HZ);
        sensor.setAccelScale(ACCEL_SCALE_8G);

        shouldInitializeSensor = false;
        Serial.println("Initialized sensor");
    }
    sensor.statusDump();
    sensor.readAccel(accelReading);
    Serial.println(accelReading.x);
    Serial.println(accelReading.y);
    Serial.println(accelReading.z);
    esp_sleep_enable_timer_wakeup(3'000'000);  // 1 second
}

void loop() {
    Serial.println("Deep sleeping");
    esp_deep_sleep_start();
}