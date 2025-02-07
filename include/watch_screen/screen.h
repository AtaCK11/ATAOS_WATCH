#pragma once

#include "screen_navigation.h"
#include "tft_screen.h"

class screen {
public:
    
    ButtonState button_state = BUTTON_NONE;
    ButtonState last_button_state = BUTTON_NONE;
    ScreenPage current_screen_page = SCREEN_NONE; // Start with home screen
    ScreenPage wannabe_screen_page = SCREEN_NONE;
    
    bool button_in_cooldown = false;

};