#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "bitmaps/weather_bitmaps.h"
#include "ataos.h"
#include "watch_screen/weather.h"
#include "logger/logger.h"
#include "settings.h"

WeatherData server_received_weather;
bool responseReceived = false;

void on_data_recv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    // Check if the received data is the weather data
    if (data_len == sizeof(WeatherData)) {
        memcpy(&server_received_weather, data, sizeof(WeatherData));
        responseReceived = true;
    }
}

void weather::request_weather(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {

        // print the mac address of  ataos->watch_settings->server_mac_adress
        LOG_DEBUG(WEATHER_REQUEST_LOG_TAG, "Server MAC Address: %02X:%02X:%02X:%02X:%02X:%02X", ataos->watch_settings.server_mac_adress[0],  ataos->watch_settings.server_mac_adress[1],  ataos->watch_settings.server_mac_adress[2],  ataos->watch_settings.server_mac_adress[3],  ataos->watch_settings.server_mac_adress[4],  ataos->watch_settings.server_mac_adress[5]);

        // Initialize Wi-Fi
        const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_err_t wifi_err = esp_wifi_init(&cfg);
        if (wifi_err != ESP_OK) {
            LOG_ERROR(TOPBAR_WEATHER_LOG_TAG, "Failed to initialize WiFi! Error: %d", wifi_err);
        }

        // Wake up, do ESP-NOW call with the server (call API, get weather data)
        LOG_DEBUG(WEATHER_REQUEST_LOG_TAG, "Enabling ESP-NOW...");

        // Set device as Station mode (required for ESP-NOW)
        WiFi.mode(WIFI_STA);
        vTaskDelay(pdMS_TO_TICKS(200)); //delay(100);
        if (esp_now_init() != ESP_OK) {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "ESP-NOW initialization failed!");
        }

        // esp_now_register_recv_cb(onDataRecv);

        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr,  ataos->watch_settings.server_mac_adress, 6); // Assign server MAC address
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "Failed to add ESP-NOW peer!");
        }

        // Prepare weather request
        strcpy(requested_weather.requestType, "Current"); // Example request type

        // Send weather request via ESP-NOW
        esp_err_t result = esp_now_send(ataos->watch_settings.server_mac_adress, (uint8_t *)&requested_weather, sizeof(requested_weather));
        if (result == ESP_OK) {
            LOG_INFO(WEATHER_REQUEST_LOG_TAG, "Weather request sent!");
        } else {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "Failed to send request!");
        }

        // Wait for response (timeout of 5 seconds)
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 seconds
        if (responseReceived) {
            // If response is received, store weather data
            received_weather = server_received_weather;
            LOG_INFO(WEATHER_REQUEST_LOG_TAG, "Weather data received!");
            responseReceived = false; // Reset the flag
        } else {
            LOG_ERROR(WEATHER_REQUEST_LOG_TAG, "No response received from server.");
        }

        // Go to sleep
        LOG_DEBUG(WEATHER_REQUEST_LOG_TAG, "Disabling ESP-NOW...");
        esp_now_deinit();
        esp_wifi_disconnect();
        esp_wifi_stop();
        esp_wifi_deinit();
        responseReceived = false;

        vTaskDelay(pdMS_TO_TICKS(10000)); // Sleep for 10 seconds before the next cycle --> will be around 5 hours or if its requested by the user
    }
}

/*
void topbar::update_topbar_weather(void *pvParameters) {
    while (1) {

        WiFiMode_t currentMode = WiFi.getMode();

        if (currentMode == WIFI_OFF) {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Wi-Fi is OFF");
        } else if (currentMode == WIFI_STA) {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Wi-Fi is in STA mode");
        } else if (currentMode == WIFI_AP) {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Wi-Fi is in AP mode");
        }

        // Initialize Wi-Fi
        const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_err_t wifi_err = esp_wifi_init(&cfg);
        if (wifi_err != ESP_OK) {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Failed to initialize WiFi!");
            return;
        }

        // Wake up, do ESP-NOW call with the server (call API, get weather data)
        LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Enabling ESP-NOW...");

        // Set device as Station mode (required for ESP-NOW)
        WiFi.mode(WIFI_STA);
        vTaskDelay(200 / portTICK_PERIOD_MS); //delay(100);
        if (esp_now_init() != ESP_OK) {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "ESP-NOW initialization failed!");
            return;
        }

        // Register callback for data reception
        esp_now_register_recv_cb(onDataRecv);

        // Set up peer information for ESP-NOW
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, serverMacAddress, 6); // Assign server MAC address
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Failed to add ESP-NOW peer!");
            return;  // We can return here because the peer setup failed.
        }

        // Prepare weather request
        strcpy(requested_weather.requestType, "Current"); // Example request type

        // Send weather request via ESP-NOW
        esp_err_t result = esp_now_send(serverMacAddress, (uint8_t *)&requested_weather, sizeof(requested_weather));
        if (result == ESP_OK) {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Weather request sent!");
        } else {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Failed to send request!");
        }

        // Wait for response (timeout of 5 seconds)
        vTaskDelay(5000 / portTICK_PERIOD_MS); // 5 seconds

        if (responseReceived) {
            // If response is received, store weather data
            received_weather = server_received_weather;
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Weather data received!");
            responseReceived = false; // Reset the flag
        } else {
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "No response received from server.");
        }

        // Go to sleep
        LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Disabling ESP-NOW...");
        esp_now_deinit();
        esp_wifi_disconnect();
        esp_wifi_stop();
        esp_wifi_deinit();

        vTaskDelay(10000 / portTICK_PERIOD_MS); // Sleep for 10 seconds before the next cycle :: will be around 5 hours or if its requested by the user
    }
}
*/