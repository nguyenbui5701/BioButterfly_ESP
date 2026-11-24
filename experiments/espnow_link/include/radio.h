#pragma once
#include <Arduino.h>
#include <esp_now.h>
#include "link_protocol.h"

namespace radio {
constexpr uint8_t channel = 6;
struct Event {
    bool send, success;
    uint32_t ms;
    uint8_t peer[6];
    int length;
    link::Wire bytes;
};
bool init();
bool pop(Event& event);
uint32_t drops();
const uint8_t* peer();
bool send(const link::Wire& bytes);
bool channel_ok();
// Bounded line parser; returns true only for a complete non-overflowed line.
bool command(char (&line)[48]);
bool parse_start(const char* line, uint32_t& run, char& mode);
}
