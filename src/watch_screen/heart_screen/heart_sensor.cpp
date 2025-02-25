#include <Arduino.h>
#include <spo2_algorithm.h>
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
        } else {
            //LOG_INFO(HEART_SENSOR_LOG_TAG, "Finger detected");
            // add particleSensor.getIR() and particleSensor.getRed(); into the buffer
            ir_buffer[buffer_spot] = particleSensor.getIR();
            red_buffer[buffer_spot] = particleSensor.getRed();
            buffer_spot++;
            buffer_spot %= buffer_size; // Wrap the index if needed
        }

        
        if (checkForBeat(irValue)) {
            TickType_t current_tick = xTaskGetTickCount();
            TickType_t delta_ticks = current_tick - last_beat_tick;
            last_beat_tick = current_tick;

            ir_particle = particleSensor.getIR();
            red_particle = particleSensor.getRed();

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

void heart_sensor::read_spo2(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {

        maxim_heart_rate_and_oxygen_saturation(ir_buffer, buffer_size, red_buffer, &spo2, &spo2_valid, &beat, &beat_valid);

        LOG_INFO(HEART_SENSOR_LOG_TAG, "SpO2: %d, IsValid: %d", spo2, spo2_valid);
        //LOG_DEBUG(HEART_SENSOR_LOG_TAG, "IR: %d, RED: %d", ir_particle, red_particle);
        vTaskDelay(pdMS_TO_TICKS(10000)); // 1 min
    }
}
