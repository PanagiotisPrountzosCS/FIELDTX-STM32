/*  DATAFLUX.ino
 *
 *  Panagiotis Prountzos 2025
 *
 */

#include "DATAFLUX.h"
#include "modules/ALSMD/ALSMD.h"

// anything else here

#include "esp_sleep.h"

RTC_DATA_ATTR LSM303 sensor(true, false);
RTC_DATA_ATTR bool firstBoot = true;

void setup() {
    auto start = millis();
    Serial.begin(115200);
    Wire.begin();

    fvec3 magReading;
    if (firstBoot) {
        sensor.configure(ACCEL_RATE_0HZ, MAG_RATE_3HZ, ACCEL_MODE_POWERDOWN, MAG_MODE_CONTINUOUS,
                         ACCEL_SCALE_2G, MAG_SCALE_8_1);
        sensor.init();
        Serial.println("Initialized sensor");
        firstBoot = false;
    }

    sensor.readMag(magReading);
    // this should be the ble advertisement part of all this!
    Serial.println(magReading.x);
    Serial.println(magReading.y);
    Serial.println(magReading.z);

    auto end = millis();
    Serial.print("Deep sleeping for 1 second, after running for ");
    Serial.print(end - start);
    Serial.println(" milliseconds!");

    esp_sleep_enable_timer_wakeup(1'000'000);  // 1 second
    esp_deep_sleep_start();
}

void loop() {}
