#include "ataos.h"

/*
    1 is default 6x8, 2 is 12x16, 3 is 18x24, etc
    26 characters per line for 6x8 font (1)
    13 characters per line for 12x16 font (2)
    8 characters per line for 18x24 font (3)
*/


/**
 * @brief Compares and prints the given text on the TFT screen, it will adjust text length and store the new text for future use.
 * 
 * @param text The text to be printed on the screen
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

/**
 * @brief Compares and prints the given text on the TFT screen, it will adjust text length and store the new text for future use.
 * Also, it will print the text in the middle of the screen.
 * 
 * @param text The text to be printed on the screen
 * @param text_size The size of the text to be printed
 */
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
    switch (text_size) {
        case 1:
            watch_tft.setCursor(80 - (text.length() * 3), watch_tft.getCursorY());
            break;
        case 2:
            watch_tft.setCursor(80 - (text.length() * 6), watch_tft.getCursorY());
            break;
        case 3:
            watch_tft.setCursor(80 - (text.length() * 9), watch_tft.getCursorY());
            break;
        case 4:
            watch_tft.setCursor(80 - (text.length() * 12), watch_tft.getCursorY());
            break;
        default:
            watch_tft.setCursor(80 - (text.length() * 3), watch_tft.getCursorY());
            break;
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

/**
 * @brief Adds unnecessary spaces to the end of the text to remove the previous text from the screen.
 * 
 * @param text The text to be printed on the screen
 * @param text_size The size of the text to be printed
 */
void ataos_firmware::line_removal_print(String text, int text_size) {
    // remove the entire screen line that will contain the text
    switch (text_size) {
        case 1:
            for (int i = 0; i < 26 - text.length(); i++) { text += " "; }
            break;
        case 2:
            for (int i = 0; i < 13 - text.length(); i++) { text += " "; }
            break;
        case 3:
            for (int i = 0; i < 8 - text.length(); i++) { text += " "; }
            break;
        case 4:
            for (int i = 0; i < 6 - text.length(); i++) { text += " "; }
            break;
        default:
            break;
    }

    watch_tft.setTextSize(text_size);
    watch_tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    watch_tft.print(text);
}

void ataos_firmware::line_removal_print_middle(int value, int text_size) {
    line_removal_print_middle(String(value), text_size);
}

void ataos_firmware::line_removal_print_middle(float value, int text_size) {
    line_removal_print_middle(String(value, 2), text_size);
}

/**
 * @brief Adds unnecesary spaces to write the `text` in the middle of the screen
 * 
 * @param text The text to be printed on the screen
 * @param text_size The size of the text to be printed
 */
void ataos_firmware::line_removal_print_middle(String text, int text_size) {
    // fill the left and the right side of the text with spaces
    String new_text = "";
    switch (text_size) {
        case 1:
            for (int i = 0; i < (26 - text.length()) / 2; i++) { new_text += " "; }
            new_text += text;
            for (int i = 0; i < (26 - text.length()) / 2; i++) { new_text += " "; }
            break;
        case 2:
            for (int i = 0; i < (14 - text.length()) / 2; i++) { new_text += " "; }
            new_text += text;
            for (int i = 0; i < (14 - text.length()) / 2; i++) { new_text += " "; }
            break;
        case 3:
            for (int i = 0; i < (8 - text.length()) / 2; i++) { new_text += " "; }
            new_text += text;
            for (int i = 0; i < (8 - text.length()) / 2; i++) { new_text += " "; }
            break;
        case 4:
            for (int i = 0; i < (6 - text.length()) / 2; i++) { new_text += " "; }
            new_text += text;
            for (int i = 0; i < (6 - text.length()) / 2; i++) { new_text += " "; }
            break;
        default:
            break;
    }

    watch_tft.setTextSize(text_size);
    watch_tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    watch_tft.print(new_text);
}

void ataos_firmware::line_removal_print(int value, int text_size) {
    line_removal_print(String(value), text_size);
}

void ataos_firmware::line_removal_print(float value, int text_size) {
    line_removal_print(String(value, 2), text_size);
}

void ataos_firmware::agressive_print(String text) {

}

void ataos_firmware::clear_smooth_print() {
    watch_screen.old_screen_text = "";
}

void ataos_firmware::clear_screen() {
    watch_tft.fillRect(0, 30, 160, 128, ST7735_BLACK); // Clear the screen area
}

void ataos_firmware::clear_entire_screen() {
    watch_tft.fillScreen(ST7735_BLACK); // Clear the screen area
}