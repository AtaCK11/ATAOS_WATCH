#include "ataos.h"
#include <esp_timer.h>

void home_screen::draw_home_screen(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        // Wait for the semaphore to be signaled (indicating a screen change)
        if (xSemaphoreTake(ataos->xHomeScreenSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
            LOG_DEBUG(HOME_SCREEN_LOG_TAG, "Home Screen Active");
        }



        // Small delay to prevent hogging the CPU
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
