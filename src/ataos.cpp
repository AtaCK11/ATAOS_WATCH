#include "ataos.h"

/*
    1 is default 6x8, 2 is 12x16, 3 is 18x24, etc
    26 characters per line for 6x8 font (1)
    13 characters per line for 12x16 font (2)
    8 characters per line for 18x24 font (3)
*/

void ataos_firmware::smooth_print(String text) {
    if (watch_screen.old_screen_text == text) {
        return;
    }

    if (watch_screen.old_screen_text.length() > text.length()) {
        // Clear the old text
        int text_length = text.length();
        for (int i = 0; i < watch_screen.old_screen_text.length() - text_length; i++) {
            text += " ";
        }
    }

    // Set text color and print
    watch_tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    watch_tft.print(text);

    // Store the new text for comparison next time
    watch_screen.old_screen_text = text;
}

void ataos_firmware::smooth_print(int value) {
    smooth_print(String(value));
}

void ataos_firmware::smooth_print(float value) {
    smooth_print(String(value, 2));
}

void ataos_firmware::smooth_print_middle(String text, int text_size) {
    if (watch_screen.old_screen_text == text) {
        return;
    }

    if (watch_screen.old_screen_text.length() > text.length()) {
        // Clear the old text
        int text_length = text.length();
        for (int i = 0; i < watch_screen.old_screen_text.length() - text_length; i++) {
            text += " ";
        }
    }

    watch_tft.setTextSize(text_size);
    watch_tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

    // for text size 1 and 2
    // 1 is 6x8, 2 is 12x16, 3 is 18x24, 4 is 24x32, 5 is 30x40
    if (text_size == 1) {
        watch_tft.setCursor(80 - (text.length() * 3), watch_tft.getCursorY());
    } else if (text_size == 2) {
        watch_tft.setCursor(80 - (text.length() * 6), watch_tft.getCursorY());
    } else if (text_size == 3) {
        watch_tft.setCursor(80 - (text.length() * 9), watch_tft.getCursorY());
    } else if (text_size == 4) {
        watch_tft.setCursor(80 - (text.length() * 12), watch_tft.getCursorY());
    } else if (text_size == 5) {
        watch_tft.setCursor(80 - (text.length() * 15), watch_tft.getCursorY());
    } else {
        watch_tft.setCursor(80 - (text.length() * 3), watch_tft.getCursorY());
    }

    // Set text color and print
    watch_tft.print(text);

    // Store the new text for comparison next time
    watch_screen.old_screen_text = text;
}

void ataos_firmware::smooth_print_middle(int value, int text_size) {
    smooth_print_middle(String(value), text_size);
}

void ataos_firmware::smooth_print_middle(float value, int text_size) {
    smooth_print_middle(String(value, 2), text_size);
}

void ataos_firmware::agressive_print(String text) {

}

void ataos_firmware::clear_smooth_print() {
    watch_screen.old_screen_text = "";
}

void ataos_firmware::clear_screen() {
    watch_tft.fillRect(0, 30, 160, 140, ST7735_BLACK); // Clear the screen area
}

void ataos_firmware::clear_entire_screen() {
    watch_tft.fillScreen(ST7735_BLACK); // Clear the screen area
}