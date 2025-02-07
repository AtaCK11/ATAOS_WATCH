#pragma once

#include <cstdint>

class settings {
public:
    uint8_t server_mac_adress[6];

    settings() : server_mac_adress{0x00, 0x11, 0x22, 0x33, 0x44, 0x55} {}
};