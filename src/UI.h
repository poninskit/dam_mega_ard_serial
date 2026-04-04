#pragma once

#include <Arduino.h>
#include <functional>
#include "globals.h"
#include "StateManager.h"
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"

using ActionCallback = std::function<void(ACTION)>;

//==============================================================================
// UI – LVGL v8 interface for Waveshare ESP32-S3-Touch-LCD-2.8B (240 × 320)
//==============================================================================
class UI {
public:
    UI();

    // Call once after construction to build the LVGL widget tree.
    void createUI(const DACState& initialState);

    // Register callback that fires when the user touches a control.
    void setActionCallback(ActionCallback cb) { _actionCb = cb; }

    // Called from main whenever DAC state changes.
    void updateVolume(int vol, bool muted);
    void updateInput (DAC_INPUT  input);
    void updateFilter(DAC_FILTER filter);

private:
    esp_panel::board::Board* _board = nullptr;

    // ── Widgets ───────────────────────────────────────────────────────────────
    lv_obj_t* _vol_label         = nullptr;
    lv_obj_t* _mute_btn          = nullptr;
    lv_obj_t* _mute_lbl          = nullptr;
    lv_obj_t* _input_btns[4]     = {};
    lv_obj_t* _filter_btns[4]    = {};

    // ── Styles ────────────────────────────────────────────────────────────────
    lv_style_t _style_scr;
    lv_style_t _style_section_lbl;
    lv_style_t _style_btn_inactive;
    lv_style_t _style_btn_active;
    lv_style_t _style_vol_btn;
    lv_style_t _style_mute_inactive;
    lv_style_t _style_mute_active;

    // ── Helpers ───────────────────────────────────────────────────────────────
    ActionCallback _actionCb;

    void _initStyles();
    void _buildLayout();

    // Static LVGL event callbacks (user_data = UI*)
    static void _onVolDown (lv_event_t* e);
    static void _onVolUp   (lv_event_t* e);
    static void _onMute    (lv_event_t* e);
    static void _onInput   (lv_event_t* e);
    static void _onFilter  (lv_event_t* e);
};
