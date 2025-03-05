#include <Arduino.h>
//#include <spo2_algorithm.h>
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
            ir_particle = irValue;
            sample_hr_plot[sample_hr_plot_spot++] = irValue;
            sample_hr_plot_spot %= 250; // dürüm
        }

        
        if (checkForBeat(irValue)) {
            TickType_t current_tick = xTaskGetTickCount();
            TickType_t delta_ticks = current_tick - last_beat_tick;
            last_beat_tick = current_tick;

            
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
        vTaskDelay(pdMS_TO_TICKS(HEART_SENSOR_TIMER));
        
    }
}

void heart_sensor::read_temperature(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        particleSensor.enableDIETEMPRDY();
        vTaskDelay(pdMS_TO_TICKS(500));
        temperature = particleSensor.readTemperature();
        LOG_INFO(HEART_SENSOR_TEMPERATURE_LOG_TAG, "%.2f", temperature);
        particleSensor.disableDIETEMPRDY();
        vTaskDelay(pdMS_TO_TICKS(READ_TEMPERATURE_TIMER));
        
    }
}

void heart_sensor::read_spo2(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        
        ataos->watch_heart_sensor.particleSensor.setPulseAmplitudeRed(0x33); // 0x33 = 10mA
        vTaskDelay(pdMS_TO_TICKS(100));

        for (int i = 0; i < 100; i++) {
            ir_buffer[i] = particleSensor.getIR();
            red_buffer[i] = particleSensor.getRed();
        }

        maxim_heart_rate_and_oxygen_saturation(ir_buffer, buffer_size, red_buffer, &spo2, &spo2_valid, &beat, &beat_valid);

        LOG_INFO(HEART_SENSOR_SPO2_LOG_TAG, "%d", spo2);

        ataos->watch_heart_sensor.particleSensor.setPulseAmplitudeRed(0);
        vTaskDelay(pdMS_TO_TICKS(READ_SPO2_TIMER));
    }
}

void heart_sensor::log_sensor_data(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        
        LOG_INFO(HEART_SENSOR_SPO2_LOG_TAG, "%d", 99);
        LOG_INFO(HEART_SENSOR_HR_LOG_TAG, "%d", beat_avg);
        LOG_INFO(HEART_SENSOR_SPO2_LOG_TAG, "%d", 99);
        LOG_INFO(HEART_SENSOR_TEMPERATURE_LOG_TAG, "%.2f", temperature);

        vTaskDelay(pdMS_TO_TICKS(HEART_SENSOR_LOG_GENERIC_TIMER));
    }
}

void heart_sensor::log_ir_data(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        LOG_INFO(HEART_SENSOR_IR_LOG_TAG, "%d", ataos->watch_heart_sensor.ir_particle);
        vTaskDelay(pdMS_TO_TICKS(50000));
    }
}