#pragma once

//------------------------------------------------------------------------------
// VERSION
#define VERSION "1.0.0"

//------------------------------------------------------------------------------
// DEBUG
#define DEBUG 1

#if DEBUG
  #define LOG(s) Serial.print(s)
#else
  #define LOG(s)
#endif

//------------------------------------------------------------------------------
// HARDWARE PINS — Waveshare ESP32-S3-Touch-LCD-2.8B
//
// DAM 1021 UART: connect to the board's exposed UART header
//   Board header pin TXD (GPIO43) → DAM 1021 RX
//   Board header pin RXD (GPIO44) ← DAM 1021 TX
//
// IR receiver: connect to GPIO0 (BOOT button pad — safe to use as GPIO after boot)
//   Alternatively any free GPIO via a breakout wire.
#define IR_RECV_PIN      0     // GPIO0 – usable as regular GPIO after boot
#define DAC_UART_TX_PIN  43    // Board UART header TXD
#define DAC_UART_RX_PIN  44    // Board UART header RXD
#define DAC_UART_BAUD    115200

//------------------------------------------------------------------------------
// ENUMERATORS

enum DAC_INPUT {
    AUTO  = 0,
    USB   = 1,
    SPDIF = 2,
    OPT   = 3
};

enum DAC_FILTER {
    LINEAR  = 0,
    MIXED   = 1,
    MINIMUM = 2,
    SOFT    = 3
};

// All actions the system can perform
enum ACTION {
    NONE = 0,
    // Remote cycling actions
    CHANNEL_LEFT,      // cycle input backward
    CHANNEL_RIGHT,     // cycle input forward
    VOLUME_UP,
    VOLUME_DOWN,
    VOLUME_MUTE,
    FILTER,            // cycle filter forward
    ENTER,
    PLAY_PAUSE,
    MENU,
    // Touch direct-select actions
    INPUT_AUTO,
    INPUT_USB,
    INPUT_SPDIF,
    INPUT_OPT,
    FILTER_LINEAR,
    FILTER_MIXED,
    FILTER_MINIMUM,
    FILTER_SOFT,
};

// UI pages (only one for now)
enum PAGE {
    MAIN_PAGE = 0
};
