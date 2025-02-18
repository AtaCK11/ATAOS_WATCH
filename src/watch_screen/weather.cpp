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
        // print the mac address of  ataos->watch_settings->server_mac_adress
        LOG_DEBUG(WEATHER_REQUEST_LOG_TAG, "Server MAC Address: %02X:%02X:%02X:%02X:%02X:%02X", ataos->watch_settings.server_mac_adress[0],  ataos->watch_settings.server_mac_adress[1],  ataos->watch_settings.server_mac_adress[2],  ataos->watch_settings.server_mac_adress[3],  ataos->watch_settings.server_mac_adress[4],  ataos->watch_settings.server_mac_adress[5]);

        // Wake up, do ESP-NOW call with the server (call API, get weather data)
        LOG_DEBUG(WEATHER_REQUEST_LOG_TAG, "Enabling ESP-NOW...");

        // Set device as Station mode (required for ESP-NOW)
        WiFi.mode(WIFI_STA);
        vTaskDelay(pdMS_TO_TICKS(200)); //delay(100);
        if (esp_now_init() != ESP_OK) {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "ESP-NOW initialization failed!");
        }

        
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr,  ataos->watch_settings.server_mac_adress, 6); // Assign server MAC address
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "Failed to add ESP-NOW peer!");
        }

        esp_err_t result_recv = esp_now_register_recv_cb(on_data_recv_weather);
        if (result_recv != ESP_OK) {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "Failed to register receive callback");
        }

        // Prepare weather request
        strcpy(requested_weather.requestType, "Current"); // Example request type

        // Send weather request via ESP-NOW
        esp_err_t result = esp_now_send(ataos->watch_settings.server_mac_adress, (uint8_t *)&requested_weather, sizeof(requested_weather));
        if (result == ESP_OK) {
            LOG_INFO(WEATHER_REQUEST_LOG_TAG, "Weather request sent!");
        } else {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "Failed to send request! Error: 0x%X", result);
        }

        // Wait for response (timeout of 5 seconds)
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 seconds
        if (server_received_weather.temperature != received_weather.temperature ||
            server_received_weather.humidity != received_weather.humidity) {
            // If response is received, store weather data
            received_weather = server_received_weather;
            LOG_INFO(WEATHER_REQUEST_LOG_TAG, "Weather data received!");
            responseReceived = false; // Reset the flag
        } else {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "No response received from server.");
        }

        // Go to sleep
        LOG_DEBUG(WEATHER_REQUEST_LOG_TAG, "Disabling ESP-NOW...");
        esp_now_unregister_recv_cb();
        esp_now_deinit();
        //esp_now
        WiFi.mode(WIFI_OFF);
        LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "%d", responseReceived);
        responseReceived = false;


        vTaskDelay(pdMS_TO_TICKS(10000)); // Sleep for 10 seconds before the next cycle --> will be around 5 hours or if its requested by the user
    }
}
