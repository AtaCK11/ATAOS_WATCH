#pragma once

#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "heartRate.h"

class heart_sensor {
public:
    void run_heart_sensor(void *pvParameters);
    MAX30105 particleSensor;

    const byte RATE_SIZE = 4;
    byte rates[4];
    byte rate_spot = 0;
    long last_beat = 0;

    uint32_t ir_buffer[100]; //infrared LED sensor data
    uint32_t red_buffer[100];

    float mbpm;
    int beat_avg;
};