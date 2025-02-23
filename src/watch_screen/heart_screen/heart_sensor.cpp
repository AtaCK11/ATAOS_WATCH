#include <Arduino.h>
#include "ataos.h"

void heart_sensor::run_heart_sensor(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    TickType_t last_beat_tick = xTaskGetTickCount();
    while (1) {
        //LOG_DEBUG(HEART_SENSOR_LOG_TAG, "Found MAX30105");
        //LOG_DEBUG(HEART_SENSOR_LOG_TAG, "%d", configTICK_RATE_HZ);
        long irValue = particleSensor.getIR();

        if (irValue < 50000) {
            //LOG_INFO(HEART_SENSOR_LOG_TAG, "Finger not detected");
        }

        if (checkForBeat(irValue)) {
            TickType_t current_tick = xTaskGetTickCount();
            TickType_t delta_ticks = current_tick - last_beat_tick;
            last_beat_tick = current_tick;

            float deltaSeconds = (delta_ticks * portTICK_PERIOD_MS) / 1000.0;

            // Calculate beats per minute (BPM)
            float bpm = 60.0 / deltaSeconds;

            // Only process valid BPM values.
            if (bpm > 20 && bpm < 255) {
                // Store the BPM (cast to a byte) in the rates array
                rates[rate_spot++] = (byte)bpm;
                rate_spot %= RATE_SIZE; // Wrap the index if needed

                // Compute the average BPM from the stored rates
                beat_avg = 0;
                for (byte x = 0; x < RATE_SIZE; x++) {
                    beat_avg += rates[x];
                }
                beat_avg /= RATE_SIZE;
            }
        } 
        
        //LOG_INFO(HEART_SENSOR_LOG_TAG, "IR=%d, BPM=%d, Avg BPM=%d", irValue, mbpm, beat_avg);
        vTaskDelay(pdMS_TO_TICKS(5));
        
    }
}

void heart_sensor::read_temperature(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        particleSensor.enableDIETEMPRDY();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 sec
        temperature = particleSensor.readTemperature();
        LOG_INFO(HEART_SENSOR_LOG_TAG, "Body Temperature: %.2f", temperature);
        particleSensor.disableDIETEMPRDY();
        vTaskDelay(pdMS_TO_TICKS(60000)); // 1 min
        
    }
}
