#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "ataos.h"
#include "watch_screen/weather.h"
#include "logger/logger.h"
#include "settings.h"

WeatherData server_received_weather;


void on_data_recv_weather(const uint8_t* messageInfo, const uint8_t *data, int data_len) {
    // Check if the received data is the weather data
    LOG_DEBUG(WEATHER_REQUEST_LOG_TAG, "ESP-NOW Data received from server! Length: %d", data_len);
    if (data_len == sizeof(WeatherData)) {
        memcpy(&server_received_weather, data, sizeof(WeatherData));
        LOG_DEBUG(WEATHER_REQUEST_LOG_TAG, "ESP-NOW Data Written: %d", data_len);
    }
}

void weather::request_weather(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        

        vTaskDelay(pdMS_TO_TICKS(SERVER_WEATHER_REQUEST_TIMER)); // Sleep for 10 seconds before the next cycle --> will be around 5 hours or if its requested by the user
    }
}
