#include "ataos.h"
#include "bitmaps/weather_bitmaps.h"

void weather_screen::draw_weather_screen(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        // Wait for the semaphore to be signaled (indicating a screen change)
        if (xSemaphoreTake(ataos->xWeatherScreenSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {

            // log the data that will be displayed on the screen
            LOG_INFO(WEATHER_SCREEN_LOG_TAG, "City: %s", ataos->watch_weather.received_weather.city);
            LOG_INFO(WEATHER_SCREEN_LOG_TAG, "Weather: %s", ataos->watch_weather.received_weather.weather_description);
            LOG_INFO(WEATHER_SCREEN_LOG_TAG, "Temperature: %.2f", ataos->watch_weather.received_weather.temperature);
            LOG_INFO(WEATHER_SCREEN_LOG_TAG, "Humidity: %d", ataos->watch_weather.received_weather.humidity);
            LOG_INFO(WEATHER_SCREEN_LOG_TAG, "Wind Speed: %.2f", ataos->watch_weather.received_weather.wind_speed);

            if (strcmp(ataos->watch_weather.received_weather.weather_description, "clear sky") == 0) {
                // clear sky
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Clear Sky");
                ataos->watch_tft.drawRGBBitmap(35, 35, epd_bitmap_clear_sky_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "few clouds") == 0) {
                // few clouds
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Few Clouds");
                ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_few_clouds_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "scattered clouds") == 0) {
                // scattered clouds
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Scattered Clouds");
                ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_few_clouds_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "broken clouds") == 0) {
                // broken clouds
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Broken Clouds");
                ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_few_clouds_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "shower rain") == 0) {
                // shower rain
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Shower Rain");
                ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_rain_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "rain") == 0) {
                // rain
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Rain");
                ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_rain_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "thunderstorm") == 0) {
                // thunderstorm
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Thunderstorm");
                ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_thunderstorm_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "snow") == 0) {
                // snow
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Snow");
                ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_snow_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "mist") == 0) {
                // mist
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Mist");
                ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_mist_static, 31, 31);
        
            } else {
                // unknown weather
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Unknown Weather");
                ataos->watch_tft.drawRGBBitmap(35, 35, epd_bitmap_thunderstorm_static, 31, 31);
            }
            
        }

        // Ensure the screen updates happen regardless of the semaphore being given
        if (ataos->watch_screen.current_screen_page == SCREEN_WEATHER) {
            
        }

        // Small delay to prevent hogging the CPU
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
