#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "WiFi.h"

#include "bitmaps/cankaya_logo_bitmap.h"
#include "bitmaps/loading_bitmaps.h"
#include "bitmaps/weather_bitmaps.h"
#include "watch_screen/tft_screen.h"
#include "watch_screen/heart_screen/heart_screen.h"
#include "watch_screen/weather.h"
#include "watch_screen/topbar/topbar.h"
#include "settings.h"
#include "logger/logger.h"

#include "ataos.h"

#define PROD
#define DEV

ataos_firmware ataos;

// put function declarations here
void update_tft_screen(void * pvParameters);
void handle_button_press(void * pvParameters);

void setup() {
    Serial.begin(115200);
    LOG_DEBUG(SETUP_LOG_TAG, "- ATAOS v1-in-DEV");
    LOG_DEBUG(SETUP_LOG_TAG, "configTICK_RATE_HZ = %d", configTICK_RATE_HZ);
    
    delay(100);

    Wire.begin(26, 27); // SDA, SCL
    delay(100);

    Serial.print("ATAOS_WATCH MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Initialize SPI TFT
    ataos.watch_tft.initR(INITR_BLACKTAB);
    ataos.watch_tft.setRotation(1);

    ataos.watch_tft.fillScreen(ST7735_BLACK);
    ataos.watch_tft.drawBitmap(20, 4, epd_bitmap_cankaya_logo, 120, 120, TFT_YELLOW_COLOR);

    //ataos.xWeatherScreenSemaphore = xSemaphoreCreateBinary();
    ataos.xHeartScreenSemaphore = xSemaphoreCreateBinary();
    ataos.watch_screen.xScreenDrawSemaphore = xSemaphoreCreateBinary();

    delay(2500);
    pinMode(BUTTON_PIN_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_PIN_HOME, INPUT_PULLUP);
    pinMode(BUTTON_PIN_RIGHT, INPUT_PULLUP);
    delay(200);

    if (ataos.watch_heart_sensor.particleSensor.begin(Wire, I2C_SPEED_FAST) == false) {
        LOG_ERROR(HEART_SENSOR_LOG_TAG, "MAX30105 was not found. Please check wiring/power. ");
        return;
    }
    delay(200);

    ataos.watch_heart_sensor.particleSensor.setup();
    ataos.watch_heart_sensor.particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
    ataos.watch_heart_sensor.particleSensor.setPulseAmplitudeGreen(0);

    ataos.watch_tft.fillScreen(ST7735_BLACK);
    for (int i = 0; i < 9; i++) {
        //ataos.watch_tft.fillScreen(ST7735_BLACK);
        if (i == 0) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame1, 100, 100);
        } else if (i == 1) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame2, 100, 100);
        } else if (i == 2) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame3, 100, 100);
        } else if (i == 3) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame4, 100, 100);
        } else if (i == 4) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame1, 100, 100);
        } else if (i == 5) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame2, 100, 100);
        } else if (i == 6) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame3, 100, 100);
        } else if (i == 7) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame4, 100, 100);
        } else if (i == 8) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame1, 100, 100);
        }
        delay(300);
    }

    delay(1000);
    ataos.watch_tft.fillScreen(ST7735_BLACK);
    ataos.watch_tft.setTextColor(ST7735_WHITE);
    ataos.watch_tft.setCursor(10, 50);
    ataos.smooth_print("Old Text!!!!");
    delay(1000);
    // Print new text
    ataos.watch_tft.setTextColor(ST7735_WHITE);
    ataos.watch_tft.setCursor(10, 50);
    ataos.smooth_print("New Text");  // Old text is erased automatically

    delay(3000);

    LOG_DEBUG(SETUP_LOG_TAG, "Setup Complete... Starting Loop");

    ataos.watch_tft.fillScreen(ST7735_BLACK);
    ataos.watch_tft.setTextColor(ST7735_WHITE);
    ataos.watch_tft.setTextSize(2);
    ataos.watch_tft.setCursor(10, 30);
    ataos.watch_tft.println("PRESS HOME     TO START");

    xTaskCreate(update_tft_screen, "Update TFT Screen", 2048, NULL, 1, NULL);
    xTaskCreate(handle_button_press, "Handle Button Press", 4098, NULL, 1, NULL);

    xTaskCreate([](void * pvParameters) {
        ataos.watch_weather.request_weather(pvParameters);
    }, "Request Weather", 4096, & ataos, 1, NULL);
    // send &ataos_weather and &tft to the function
    xTaskCreate([](void * pvParameters) {
        ataos.watch_topbar.update_topbar_weather(pvParameters);
    }, "Request Weather", 4096, & ataos, 1, NULL);
    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_sensor.run_heart_sensor(pvParameters);
    }, "Heart Sensor Tasks", 4096, & ataos, 1, NULL);

    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_screen.draw_heart_screen(pvParameters);
    }, "Heart Screen", 4096, & ataos, 1, NULL);

    LOG_DEBUG(SETUP_LOG_TAG, "Starting Scheduler");
    //vTaskStartScheduler();
}

void loop() {
    /* boooo! */ }

void update_tft_screen(void * pvParameters) {
    while (1) {
        String screen_text = "ERR";
        bool screen_needs_update = false;
        //tft.fillScreen(ST7735_BLACK);  // clear the screen

        switch (ataos.watch_screen.button_state) {
        case BUTTON_LEFT_PRESSED:
            screen_text = "Left Button Pressed - Weather Menu";
            ataos.watch_screen.wannabe_screen_page = SCREEN_WEATHER;
            //xSemaphoreGive(ataos.xWeatherScreenSemaphore);
            break;

        case BUTTON_HOME_PRESSED:
            screen_text = "Home Button Pressed - Home Screen";
            ataos.watch_screen.wannabe_screen_page = SCREEN_HOME;
            break;

        case BUTTON_RIGHT_PRESSED:
            screen_text = "Right Button Pressed - Heart Rate Menu";
            ataos.watch_screen.wannabe_screen_page = SCREEN_HEARTRATE;
            //ataos.watch_screen.current_screen_page = SCREEN_HEARTRATE;
            xSemaphoreGive(ataos.xHeartScreenSemaphore);
            break;

        case BUTTON_LEFT_DOUBLE_CLICK:
            screen_text = "Left Button Double Clicked - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            ataos.watch_tft.drawRGBBitmap(1, 1, epd_bitmap_rain_static, 31, 31);
            break;

        case BUTTON_HOME_DOUBLE_CLICK:
            screen_text = "Home Button Double Clicked - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            ataos.watch_tft.drawRGBBitmap(1, 1, epd_bitmap_snow_static, 31, 31);

            break;

        case BUTTON_RIGHT_DOUBLE_CLICK:
            screen_text = "Right Button Double Clicked - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            ataos.watch_tft.drawRGBBitmap(1, 1, epd_bitmap_clear_sky_static, 31, 31);
            break;

        case BUTTON_LEFT_HOLDING:
            screen_text = "Left Button Holding - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            analogWrite(TFT_BLK, 255);
            break;

        case BUTTON_HOME_HOLDING:
            screen_text = "Home Button Holding - Settings";
            ataos.watch_screen.wannabe_screen_page = SCREEN_SETTINGS;
            break;

        case BUTTON_RIGHT_HOLDING:
            screen_text = "Right Button Holding - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            analogWrite(TFT_BLK, 0);
            break;

        case BUTTON_NONE:
        default:
            screen_text = "No Button Pressed - Dont change the screen";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            break;
        }

        if (ataos.watch_screen.current_screen_page != ataos.watch_screen.wannabe_screen_page) {
            screen_needs_update = true;
        }

        if (screen_needs_update) {
            //ataos.watch_tft.fillRect(0, 30, 160, 140, ST7735_BLACK); // clear the screen
            //ataos.watch_tft.setTextColor(TFT_WHITE_COLOR);
            //ataos.watch_tft.setTextSize(2);
            //ataos.watch_tft.setCursor(0, 30);
            //ataos.watch_tft.println(screen_text);
            ataos.watch_screen.current_screen_page = ataos.watch_screen.wannabe_screen_page;
            LOG_INFO(SCREEN_LOG_TAG, "Screen Updated: %s", screen_text.c_str());
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void handle_button_press(void *pvParameters) {
    static TickType_t last_debounce_time = 0;
    static TickType_t last_press_time = 0;
    static ButtonState last_button_pressed = BUTTON_NONE;
    static bool waiting_for_double_click = false;

    while (1) {
        ButtonState current_button_state = BUTTON_NONE;

        // Cooldown: prevent single-click detection after a double-click or hold.
        if (ataos.watch_screen.button_in_cooldown) {
            if ((xTaskGetTickCount() - last_press_time) >= pdMS_TO_TICKS(BUTTON_COOLDOWN_TIME)) {
                // Reset cooldown flag once enough time has passed.
                ataos.watch_screen.button_in_cooldown = false;
            }
        }

        if (!ataos.watch_screen.button_in_cooldown) {
            // Read the button states. (Assumes HIGH means pressed.)
            if (digitalRead(BUTTON_PIN_LEFT) == HIGH) {
                current_button_state = BUTTON_LEFT_PRESSED;
            } else if (digitalRead(BUTTON_PIN_HOME) == HIGH) {
                current_button_state = BUTTON_HOME_PRESSED;
            } else if (digitalRead(BUTTON_PIN_RIGHT) == HIGH) {
                current_button_state = BUTTON_RIGHT_PRESSED;
            }

            // Debounce: if the reading has changed, update the debounce timer.
            if (current_button_state != ataos.watch_screen.last_button_state) {
                last_debounce_time = xTaskGetTickCount();
            }

            // After the debounce delay, check if the state has really changed.
            if ((xTaskGetTickCount() - last_debounce_time) > pdMS_TO_TICKS(BUTTON_DEBOUNCE_DELAY)) {
                if (current_button_state != ataos.watch_screen.button_state) {
                    ataos.watch_screen.button_state = current_button_state;

                    if (ataos.watch_screen.button_state != BUTTON_NONE) {
                        // Check for a double click: same button pressed within the double click interval.
                        if (ataos.watch_screen.button_state == last_button_pressed &&
                            ((xTaskGetTickCount() - last_press_time) < pdMS_TO_TICKS(BUTTON_DOUBLE_CLICK_TIME))) {
                            // Double click detected: update the state accordingly.
                            switch (ataos.watch_screen.button_state) {
                                case BUTTON_LEFT_PRESSED:
                                    ataos.watch_screen.button_state = BUTTON_LEFT_DOUBLE_CLICK;
                                    LOG_DEBUG(BUTTON_LOG_TAG, "Left Button Double Clicked");
                                    break;
                                case BUTTON_HOME_PRESSED:
                                    ataos.watch_screen.button_state = BUTTON_HOME_DOUBLE_CLICK;
                                    LOG_DEBUG(BUTTON_LOG_TAG, "Home Button Double Clicked");
                                    break;
                                case BUTTON_RIGHT_PRESSED:
                                    ataos.watch_screen.button_state = BUTTON_RIGHT_DOUBLE_CLICK;
                                    LOG_DEBUG(BUTTON_LOG_TAG, "Right Button Double Clicked");
                                    break;
                                default:
                                    break;
                            }
                            waiting_for_double_click = false;
                            // Set cooldown to prevent immediate single-click detection.
                            ataos.watch_screen.button_in_cooldown = true;
                            last_press_time = xTaskGetTickCount();
                        } else {
                            // Single click detected, but wait to see if a double click follows.
                            waiting_for_double_click = true;
                            last_press_time = xTaskGetTickCount();
                            LOG_DEBUG(BUTTON_LOG_TAG, "Single Click Detected");
                        }
                        last_button_pressed = ataos.watch_screen.button_state;
                    }
                }
            }
        }

        // Check for a button hold.
        // (Note: digitalRead(last_button_pressed) assumes that the button state values correspond to a valid pin number.)
        if (ataos.watch_screen.button_state != BUTTON_NONE && digitalRead(last_button_pressed) == HIGH) {
            if ((xTaskGetTickCount() - last_press_time) > pdMS_TO_TICKS(BUTTON_HOLD_TIME)) {
                // Button hold detected; update the state accordingly.
                switch (last_button_pressed) {
                    case BUTTON_LEFT_PRESSED:
                        ataos.watch_screen.button_state = BUTTON_LEFT_HOLDING;
                        LOG_DEBUG(BUTTON_LOG_TAG, "Left Button Holding");
                        break;
                    case BUTTON_HOME_PRESSED:
                        ataos.watch_screen.button_state = BUTTON_HOME_HOLDING;
                        LOG_DEBUG(BUTTON_LOG_TAG, "Home Button Holding");
                        break;
                    case BUTTON_RIGHT_PRESSED:
                        ataos.watch_screen.button_state = BUTTON_RIGHT_HOLDING;
                        LOG_DEBUG(BUTTON_LOG_TAG, "Right Button Holding");
                        break;
                    default:
                        break;
                }
                // Set cooldown to prevent immediate single-click detection.
                ataos.watch_screen.button_in_cooldown = true;
                last_press_time = xTaskGetTickCount();
            }
        }

        // If waiting for a potential double click but the time expires, clear the flag.
        if (waiting_for_double_click &&
            ((xTaskGetTickCount() - last_press_time) > pdMS_TO_TICKS(BUTTON_DOUBLE_CLICK_TIME))) {
            waiting_for_double_click = false;
            LOG_DEBUG("BUTTON", "Double Click Timeout");
        }

        // Store the current raw reading for the next debounce comparison.
        ataos.watch_screen.last_button_state = current_button_state;

        // Delay the task briefly before the next loop iteration.
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}