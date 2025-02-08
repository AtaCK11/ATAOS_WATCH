#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "WiFi.h"

#include "logger/logger.h"

#include "watch_screen/screen.h"

#include "watch_screen/heart_screen/heart_screen.h"
#include "watch_screen/heart_screen/heart_sensor.h"
#include "watch_screen/weather_screen/weather_screen.h"
#include "watch_screen/weather.h"
#include "watch_screen/topbar/topbar.h"
#include "settings.h"

class ataos_firmware {
public:

    Adafruit_ST7735 watch_tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

    settings watch_settings;
    screen watch_screen;
    weather watch_weather;
    topbar watch_topbar;

    heart_sensor watch_heart_sensor;

    heart_screen watch_heart_screen;
    weather_screen watch_weather_screen;

    // semaphores
    SemaphoreHandle_t xWeatherScreenSemaphore = NULL;
    SemaphoreHandle_t xHeartScreenSemaphore = NULL;

    // Screen drawing functions
    void smooth_print(String text);
    void agressive_print(String text);
};