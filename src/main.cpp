/*******************************************************************************
 * DAM 1021 Controller – ESP32-S3-Touch-LCD-2.8B
 *
 * Controls the Soekris DAM 1021 DAC over hardware UART (Serial1).
 * UI: LVGL v8 on 240×320 touch display.
 * Remote: Apple aluminium IR remote, NEC protocol (IRremote v4).
 *
 * DAM 1021 Serial protocol (@115200 baud):
 *   Commands sent by us:
 *     Vxxx   Volume  (e.g. "V-49"  = −49 dB,  "V0" = 0 dB)
 *     Ix     Input   (I3=AUTO  I0=USB  I1=SPDIF  I2=OPT)
 *     Fx     Filter  (F4=Linear  F5=Mixed  F6=Minimum  F7=Soft)
 *
 *   Status messages sent by DAM (optional / informational):
 *     Rx.xx  Firmware version
 *     Vxxx   Volume acknowledgement
 *     Ix     Input acknowledgement
 *     Fx     Filter acknowledgement
 *     Lxxx   Lock speed  (000=unlocked, 044-384=PCM kHz)
 *******************************************************************************/

#include <Arduino.h>
#include "globals.h"
#include "StateManager.h"
#include "Remote.h"
#include "UI.h"

//──────────────────────────────────────────────────────────────────────────────
// Module instances
//──────────────────────────────────────────────────────────────────────────────
static StateManager* state      = nullptr;
static RemoteInterface* remote  = nullptr;
static UI*           ui         = nullptr;

//──────────────────────────────────────────────────────────────────────────────
// DAM 1021 serial helpers
//──────────────────────────────────────────────────────────────────────────────

// Volume scale: vol 0-99  →  dB = vol - 99  (vol=99 → 0dB, vol=50 → −49dB)
static void sendVolume(int vol)
{
    int dB = vol - 99;
    if (dB >= 0) {
        Serial1.print("V");
        Serial1.println(dB);
    } else {
        Serial1.print("V");
        Serial1.println(dB);   // println(negative int) prints the '-' sign automatically
    }
    LOG("DAC cmd: V" + String(dB) + "\n");
}

static void sendMute(bool muted, int vol)
{
    // Mute by sending minimum volume, unmute by restoring
    if (muted) {
        Serial1.println("V-99");
    } else {
        sendVolume(vol);
    }
}

static void sendInput(DAC_INPUT input)
{
    // DAM 1021: I3=AUTO  I0=USB  I1=SPDIF  I2=OPT
    const char* cmds[4] = { "I3", "I0", "I1", "I2" };
    Serial1.println(cmds[(int)input]);
    LOG("DAC cmd: " + String(cmds[(int)input]) + "\n");
}

static void sendFilter(DAC_FILTER filter)
{
    // DAM 1021: F4=Linear  F5=Mixed  F6=Minimum  F7=Soft
    const char* cmds[4] = { "F4", "F5", "F6", "F7" };
    Serial1.println(cmds[(int)filter]);
    LOG("DAC cmd: " + String(cmds[(int)filter]) + "\n");
}

//──────────────────────────────────────────────────────────────────────────────
// Action handler – central logic for both touch and remote inputs
//──────────────────────────────────────────────────────────────────────────────
static void handleAction(ACTION action)
{
    if (action == NONE) return;

    const DACState& s = state->getState();

    switch (action) {
    // ── Volume ────────────────────────────────────────────────────────────────
    case VOLUME_UP:
        if (!s.muted && s.volume < VOL_MAX) {
            state->setVolume(s.volume + 1);
            sendVolume(state->getState().volume);
        }
        break;

    case VOLUME_DOWN:
        if (!s.muted && s.volume > VOL_MIN) {
            state->setVolume(s.volume - 1);
            sendVolume(state->getState().volume);
        }
        break;

    case VOLUME_MUTE:
        state->setMuted(!s.muted);
        sendMute(state->getState().muted, state->getState().volume);
        break;

    // ── Input (direct select) ─────────────────────────────────────────────────
    case INPUT_AUTO:  state->setInput(AUTO);  sendInput(AUTO);  break;
    case INPUT_USB:   state->setInput(USB);   sendInput(USB);   break;
    case INPUT_SPDIF: state->setInput(SPDIF); sendInput(SPDIF); break;
    case INPUT_OPT:   state->setInput(OPT);   sendInput(OPT);   break;

    // ── Input (cycle via remote) ──────────────────────────────────────────────
    case CHANNEL_LEFT:
    {
        int next = ((int)s.input - 1 + 4) % 4;
        state->setInput((DAC_INPUT)next);
        sendInput(state->getState().input);
    }
        break;

    case CHANNEL_RIGHT:
    {
        int next = ((int)s.input + 1) % 4;
        state->setInput((DAC_INPUT)next);
        sendInput(state->getState().input);
    }
        break;

    // ── Filter (direct select) ────────────────────────────────────────────────
    case FILTER_LINEAR:  state->setFilter(LINEAR);  sendFilter(LINEAR);  break;
    case FILTER_MIXED:   state->setFilter(MIXED);   sendFilter(MIXED);   break;
    case FILTER_MINIMUM: state->setFilter(MINIMUM); sendFilter(MINIMUM); break;
    case FILTER_SOFT:    state->setFilter(SOFT);    sendFilter(SOFT);    break;

    // ── Filter (cycle via remote) ─────────────────────────────────────────────
    case FILTER:
    {
        int next = ((int)s.filter + 1) % 4;
        state->setFilter((DAC_FILTER)next);
        sendFilter(state->getState().filter);
    }
        break;

    default:
        break;
    }
}

//──────────────────────────────────────────────────────────────────────────────
// Setup
//──────────────────────────────────────────────────────────────────────────────
void setup()
{

    Serial.begin(115200);
    delay(3000);
    Serial.println("=== BOOT START ===");
    Serial.printf("Free heap: %lu\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM: %lu\n", ESP.getFreePsram());
    Serial.flush();

    // Test 1: Board object
    Serial.println("Creating Board...");
    Serial.flush();
    auto* board = new esp_panel::board::Board();
    Serial.println("Board created OK");
    Serial.flush();

    // Test 2: init
    Serial.println("Calling board->init()...");
    Serial.flush();
    board->init();
    Serial.println("board->init() OK");
    Serial.flush();

    // Test 3: begin
    Serial.println("Calling board->begin()...");
    Serial.flush();
    bool ok = board->begin();
    Serial.printf("board->begin() = %s\n", ok ? "OK" : "FAILED");
    Serial.flush();

    // Test 4: backlight
    auto* bl = board->getBacklight();
    Serial.printf("Backlight ptr = %p\n", bl);
    if (bl) { bl->on(); Serial.println("Backlight ON"); }
    Serial.flush();

    // Test 5: LCD + touch
    auto* lcd   = board->getLCD();
    auto* touch = board->getTouch();
    Serial.printf("LCD=%p  Touch=%p\n", lcd, touch);
    Serial.flush();

    Serial.println("=== BOOT PROBE COMPLETE ===");





    // // USB serial for debug (if DEBUG=1)
    // if (DEBUG) Serial.begin(115200);
    
    // LOG("DAM 1021 controller starting\n");
    // Serial.println("\n\n--- Restarting DAM 1021 Controller ---\n");


    // // Hardware UART to DAM 1021
    // Serial1.begin(DAC_UART_BAUD, SERIAL_8N1, DAC_UART_RX_PIN, DAC_UART_TX_PIN);
    // delay(200);  // let DAM 1021 boot

    // // State (load persisted values before UI is built)
    // state = new StateManager();
    // state->loadState();

    // // UI (initialises display + touch + LVGL, then builds widgets)
    // ui = new UI();
    // ui->setActionCallback(handleAction);
    // ui->createUI(state->getState());

    // // Wire state changes → UI updates (LVGL must be running by now)
    // state->onStateChange([](const DACState& s) {
    //     if (!lvgl_port_lock(50)) return;
    //     ui->updateVolume(s.volume, s.muted);
    //     ui->updateInput (s.input);
    //     ui->updateFilter(s.filter);
    //     lvgl_port_unlock();
    // });

    // // IR remote (initialised after LVGL so its Serial.println doesn't interfere)
    // remote = new RemoteInterface(IR_RECV_PIN);

    // // Push initial state to DAM 1021
    // const DACState& s = state->getState();
    // sendVolume(s.muted ? 0 : s.volume);
    // delay(50);
    // sendInput (s.input);
    // delay(50);
    // sendFilter(s.filter);

    // LOG("Setup complete\n");
}

//──────────────────────────────────────────────────────────────────────────────
// Loop
//──────────────────────────────────────────────────────────────────────────────
void loop()
{

    // Poll IR remote (LVGL runs in its own FreeRTOS task via lvgl_port)
    ACTION act = remote->getAction();
    if (act != NONE) {
        handleAction(act);
        // Small delay after remote to prevent double-fire on non-repeat actions
        if (!remote->isRepeat()) delay(30);
    }

    // Optional: drain any status messages from DAM 1021
    while (Serial1.available()) {
        String msg = Serial1.readStringUntil('\n');
        msg.trim();
        if (msg.length() > 0) {
            LOG("DAM status: " + msg + "\n");
        }
    }
}
