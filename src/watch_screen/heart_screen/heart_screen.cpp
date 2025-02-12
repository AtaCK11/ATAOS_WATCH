#include "ataos.h"

void heart_screen::draw_heart_screen(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    int last_screen_page = -1; // Track the last screen mode

    while (1) {
        // Wait for the semaphore to be signaled (indicating a screen change)
        if (xSemaphoreTake(ataos->xHeartScreenSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
            // Check if the screen mode has changed
            LOG_DEBUG(HEART_SCREEN_LOG_TAG, "Heart Screen Active");
            if (ataos->watch_screen.current_screen_page == SCREEN_HEARTRATE) {
                // Initialize the heart rate screen (draw static text)
                ataos->watch_tft.setCursor(0, 30);
                ataos->watch_tft.setTextSize(2);
                ataos->smooth_print("Heart Rate");

                ataos->watch_tft.setCursor(0, 60);
                ataos->watch_tft.setTextSize(2);
                ataos->smooth_print("BPM:");

                last_screen_page = SCREEN_HEARTRATE; // Update the last screen mode
            }
        }

        // Ensure BPM updates happen regardless of the semaphore being given
        if (ataos->watch_screen.current_screen_page == SCREEN_HEARTRATE) {
            LOG_INFO(HEART_SCREEN_LOG_TAG, "BPM: %d", ataos->watch_heart_sensor.beat_avg);
            ataos->watch_tft.setCursor(0, 90);
            ataos->watch_tft.setTextSize(2);
            ataos->smooth_print(ataos->watch_heart_sensor.beat_avg);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        // Small delay to prevent hogging the CPU
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
