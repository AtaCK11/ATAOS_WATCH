#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "ataos.h"
#include "watch_screen/rtc_time.h"
#include "logger/logger.h"
#include "settings.h"

TimeInformation server_received_time;

void on_data_recv_time(const uint8_t* messageInfo, const uint8_t *data, int data_len) {
    // Check if the received data is the time data
    LOG_DEBUG(TIME_REQUEST_LOG_TAG, "ESP-NOW Data received from server! Length: %d", data_len);
    if (data_len == sizeof(TimeInformation)) {
        memcpy(&server_received_time, data, sizeof(TimeInformation));
        LOG_DEBUG(TIME_REQUEST_LOG_TAG, "ESP-NOW Data Written: %d", data_len);
    }
}

void rtc_time::request_time(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {


        vTaskDelay(pdMS_TO_TICKS(SERVER_TIME_REQUEST_TIMER)); // Sleep for 10 seconds before the next cycle --> will be around 5 hours or if its requested by the user
    }
}

void rtc_time::calculate_time(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        LOG_DEBUG(TIME_REQUEST_LOG_TAG, "+1 Minute Time...");
        if (ataos->watch_time.received_time.minute == 59) {
            ataos->watch_time.received_time.minute = 0;
            if (ataos->watch_time.received_time.hour == 23) {
                ataos->watch_time.received_time.hour = 0;
            } else {
                ataos->watch_time.received_time.hour = ataos->watch_time.received_time.hour + 1;
            }
        } else {
            ataos->watch_time.received_time.minute = ataos->watch_time.received_time.minute + 1;
        }
        if (ataos->watch_screen.current_screen_page == SCREEN_HOME) {
            xSemaphoreGive(ataos->xHomeScreenSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(RTC_CALCULATE_TIME)); // Sleep for 1 minute
    }
}
