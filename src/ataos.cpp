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


void ataos_firmware::agressive_print(String text) {

}