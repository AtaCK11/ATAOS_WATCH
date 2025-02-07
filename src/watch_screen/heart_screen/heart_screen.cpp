#include "ataos.h"

void heart_screen::draw_heart_screen(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        if (ataos->watch_screen.current_screen_page == SCREEN_HEARTRATE) {
            LOG_DEBUG(HEART_SENSOR_LOG_TAG, "BEAT AVG: %d", ataos->watch_heart_sensor.beat_avg);

            // fill the screen with black but keep the topbar
            int old_cursor_x = ataos->watch_tft.getCursorX();
            int old_cursor_y = ataos->watch_tft.getCursorY();
            ataos->watch_tft.fillRect(0, 30, 160, 140, ST7735_BLACK);

            ataos->watch_tft.setCursor(0, 30);
            ataos->watch_tft.setTextColor(ST7735_WHITE);
            ataos->watch_tft.setTextSize(2);
            ataos->watch_tft.println("Heart Rate");

            ataos->watch_tft.setCursor(0, 60);
            ataos->watch_tft.setTextColor(ST7735_WHITE);
            ataos->watch_tft.setTextSize(2);
            ataos->watch_tft.println("BPM:");
            

            ataos->watch_tft.setCursor(0, 90);
            ataos->watch_tft.setTextColor(ST7735_WHITE);
            ataos->watch_tft.setTextSize(2);
            ataos->watch_tft.println(ataos->watch_heart_sensor.beat_avg);

            ataos->watch_tft.setCursor(old_cursor_x, old_cursor_y);

            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }

}