#include "UI.h"

using namespace esp_panel::board;
using namespace esp_panel::drivers;

//──────────────────────────────────────────────────────────────────────────────
// Colour palette  (RGB hex)
//──────────────────────────────────────────────────────────────────────────────
#define C_BG            lv_color_hex(0x0D1117)   // near-black background
#define C_HEADER        lv_color_hex(0x161B22)   // slightly lighter header
#define C_SURFACE       lv_color_hex(0x21262D)   // raised surface / inactive btn
#define C_BORDER        lv_color_hex(0x30363D)   // subtle divider
#define C_ACCENT        lv_color_hex(0x58A6FF)   // blue accent (active / vol btns)
#define C_ACCENT_DARK   lv_color_hex(0x1F6FEB)   // pressed state
#define C_MUTE_ACTIVE   lv_color_hex(0xF85149)   // red when muted
#define C_TEXT          lv_color_hex(0xE6EDF3)   // primary text
#define C_TEXT_DIM      lv_color_hex(0x7D8590)   // section labels, inactive text

//──────────────────────────────────────────────────────────────────────────────
// Layout constants — Waveshare ESP32-S3-Touch-LCD-2.8B: 240 × 320
//──────────────────────────────────────────────────────────────────────────────
static constexpr int SCR_W = 240;
static constexpr int SCR_H = 320;

//  Section          Y    H
//  Header           0    40
//  Vol number      40   100   (large dB label)
//  Vol buttons    140    48
//  Divider        188     1
//  Input label    189    20
//  Input buttons  209    48
//  Divider        257     1
//  Filter label   258    20
//  Filter buttons 278    42
//  Mute bar       287    33   (to SCR_H=320)

static constexpr int HEADER_Y  = 0,   HEADER_H  = 40;
static constexpr int VOL_NUM_Y = 40,  VOL_NUM_H = 100;
static constexpr int VOL_BTN_Y = 140, VOL_BTN_H = 48;
static constexpr int INP_LBL_Y = 190, INP_LBL_H = 20;
static constexpr int INP_BTN_Y = 210, INP_BTN_H = 48;
static constexpr int FLT_LBL_Y = 260, FLT_LBL_H = 20;
static constexpr int FLT_BTN_Y = 280, FLT_BTN_H = 36;  // shares space with mute
static constexpr int MUTE_Y    = 287, MUTE_H    = 33;

static constexpr int PAD      = 6;
static constexpr int BTN_GAP  = 4;
static constexpr int BTN4_W   = (SCR_W - 2*PAD - 3*BTN_GAP) / 4;  // = 51 px
static constexpr int RADIUS_SM = 8;
static constexpr int RADIUS_LG = 12;

//==============================================================================
// Construction & board init
//==============================================================================
UI::UI()
{
    _board = new Board();
    _board->init();

    if (!_board->begin()) {
        LOG("Warning: board->begin() failed\n");
    }

    // ── Backlight ─────────────────────────────────────────────────────────────
    auto* bl = _board->getBacklight();
    if (bl) {
        bl->on();
        LOG("Backlight on\n");
    } else {
        LOG("Warning: no backlight driver\n");
    }
    // ──────────────────────────────────────────────────────────────────────────

    auto lcd   = _board->getLCD();
    auto touch = _board->getTouch();

    delay(100);

    if (!lcd)   { LOG("LCD is NULL\n");   }
    if (!touch) { LOG("Touch is NULL\n"); }

    if (lcd && touch) {
        lvgl_port_init(lcd, touch);
        LOG("LVGL init OK\n");
        delay(100);
    } else {
        LOG("LVGL init skipped\n");
    }
}

//==============================================================================
// Styles
//==============================================================================
void UI::_initStyles()
{
    // Screen background
    lv_style_init(&_style_scr);
    lv_style_set_bg_color (&_style_scr, C_BG);
    lv_style_set_bg_opa   (&_style_scr, LV_OPA_COVER);
    lv_style_set_border_width(&_style_scr, 0);
    lv_style_set_pad_all  (&_style_scr, 0);

    // Section label (dim small text)
    lv_style_init(&_style_section_lbl);
    lv_style_set_text_color(&_style_section_lbl, C_TEXT_DIM);
    lv_style_set_text_font (&_style_section_lbl, &lv_font_montserrat_16);

    // Row button – inactive
    lv_style_init(&_style_btn_inactive);
    lv_style_set_bg_color    (&_style_btn_inactive, C_SURFACE);
    lv_style_set_bg_opa      (&_style_btn_inactive, LV_OPA_COVER);
    lv_style_set_border_color(&_style_btn_inactive, C_BORDER);
    lv_style_set_border_width(&_style_btn_inactive, 1);
    lv_style_set_radius      (&_style_btn_inactive, RADIUS_SM);
    lv_style_set_text_color  (&_style_btn_inactive, C_TEXT_DIM);
    lv_style_set_text_font   (&_style_btn_inactive, &lv_font_montserrat_18);
    lv_style_set_pad_all     (&_style_btn_inactive, 4);

    // Row button – active / selected
    lv_style_init(&_style_btn_active);
    lv_style_set_bg_color    (&_style_btn_active, C_ACCENT);
    lv_style_set_bg_opa      (&_style_btn_active, LV_OPA_COVER);
    lv_style_set_border_width(&_style_btn_active, 0);
    lv_style_set_radius      (&_style_btn_active, RADIUS_SM);
    lv_style_set_text_color  (&_style_btn_active, lv_color_hex(0x0D1117));
    lv_style_set_text_font   (&_style_btn_active, &lv_font_montserrat_18);
    lv_style_set_pad_all     (&_style_btn_active, 4);

    // Volume ±  buttons
    lv_style_init(&_style_vol_btn);
    lv_style_set_bg_color    (&_style_vol_btn, C_ACCENT);
    lv_style_set_bg_opa      (&_style_vol_btn, LV_OPA_COVER);
    lv_style_set_border_width(&_style_vol_btn, 0);
    lv_style_set_radius      (&_style_vol_btn, RADIUS_LG);
    lv_style_set_text_color  (&_style_vol_btn, lv_color_hex(0x0D1117));
    lv_style_set_text_font   (&_style_vol_btn, &lv_font_montserrat_40);
    lv_style_set_pad_all     (&_style_vol_btn, 0);

    // Mute button – not muted
    lv_style_init(&_style_mute_inactive);
    lv_style_set_bg_color    (&_style_mute_inactive, C_SURFACE);
    lv_style_set_bg_opa      (&_style_mute_inactive, LV_OPA_COVER);
    lv_style_set_border_color(&_style_mute_inactive, C_BORDER);
    lv_style_set_border_width(&_style_mute_inactive, 1);
    lv_style_set_radius      (&_style_mute_inactive, RADIUS_SM);
    lv_style_set_text_color  (&_style_mute_inactive, C_TEXT_DIM);
    lv_style_set_text_font   (&_style_mute_inactive, &lv_font_montserrat_16);

    // Mute button – muted
    lv_style_init(&_style_mute_active);
    lv_style_set_bg_color    (&_style_mute_active, C_MUTE_ACTIVE);
    lv_style_set_bg_opa      (&_style_mute_active, LV_OPA_COVER);
    lv_style_set_border_width(&_style_mute_active, 0);
    lv_style_set_radius      (&_style_mute_active, RADIUS_SM);
    lv_style_set_text_color  (&_style_mute_active, C_TEXT);
    lv_style_set_text_font   (&_style_mute_active, &lv_font_montserrat_16);
}

//==============================================================================
// Layout builder
//==============================================================================
void UI::_buildLayout()
{
    lv_obj_t* scr = lv_scr_act();
    lv_obj_add_style(scr, &_style_scr, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    // ── HEADER ────────────────────────────────────────────────────────────────
    lv_obj_t* header = lv_obj_create(scr);
    lv_obj_set_size(header, SCR_W, HEADER_H);
    lv_obj_set_pos (header, 0, HEADER_Y);
    lv_obj_set_style_bg_color    (header, C_HEADER, 0);
    lv_obj_set_style_bg_opa      (header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_radius      (header, 0, 0);
    lv_obj_set_style_pad_all     (header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    // Bottom border line on header
    lv_obj_set_style_border_side (header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(header, C_BORDER, 0);
    lv_obj_set_style_border_width(header, 1, 0);

    lv_obj_t* title = lv_label_create(header);
    lv_label_set_text(title, "DAM 1021");
    lv_obj_set_style_text_color(title, C_TEXT_DIM, 0);
    lv_obj_set_style_text_font (title, &lv_font_montserrat_22, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, PAD, 0);

    lv_obj_t* ver = lv_label_create(header);
    lv_label_set_text(ver, VERSION);
    lv_obj_set_style_text_color(ver, C_BORDER, 0);
    lv_obj_set_style_text_font (ver, &lv_font_montserrat_12, 0);
    lv_obj_align(ver, LV_ALIGN_RIGHT_MID, -PAD, 0);

    // ── VOLUME NUMBER ─────────────────────────────────────────────────────────
    _vol_label = lv_label_create(scr);
    lv_label_set_text(_vol_label, "-49 dB");
    lv_obj_set_style_text_color(_vol_label, C_TEXT, 0);
    lv_obj_set_style_text_font (_vol_label, &lv_font_montserrat_36, 0);
    lv_obj_set_size(_vol_label, SCR_W, VOL_NUM_H);
    lv_obj_set_pos (_vol_label, 0, VOL_NUM_Y);
    lv_obj_set_style_text_align(_vol_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_top   (_vol_label, 28, 0);

    // ── VOLUME BUTTONS ────────────────────────────────────────────────────────
    static constexpr int VOL_BTN_W = 100;
    lv_obj_t* vol_down = lv_btn_create(scr);
    lv_obj_add_style(vol_down, &_style_vol_btn, 0);
    lv_obj_set_size(vol_down, VOL_BTN_W, VOL_BTN_H - 6);
    lv_obj_set_pos (vol_down, PAD, VOL_BTN_Y + 3);
    lv_obj_add_event_cb(vol_down, _onVolDown, LV_EVENT_CLICKED, this);
    lv_obj_t* vd_lbl = lv_label_create(vol_down);
    lv_label_set_text(vd_lbl, LV_SYMBOL_MINUS);
    lv_obj_set_style_text_font(vd_lbl, &lv_font_montserrat_32, 0);
    lv_obj_center(vd_lbl);

    lv_obj_t* vol_up = lv_btn_create(scr);
    lv_obj_add_style(vol_up, &_style_vol_btn, 0);
    lv_obj_set_size(vol_up, VOL_BTN_W, VOL_BTN_H - 6);
    lv_obj_set_pos (vol_up, SCR_W - PAD - VOL_BTN_W, VOL_BTN_Y + 3);
    lv_obj_add_event_cb(vol_up, _onVolUp, LV_EVENT_CLICKED, this);
    lv_obj_t* vu_lbl = lv_label_create(vol_up);
    lv_label_set_text(vu_lbl, LV_SYMBOL_PLUS);
    lv_obj_set_style_text_font(vu_lbl, &lv_font_montserrat_40, 0);
    lv_obj_center(vu_lbl);

    // Divider line after volume
    lv_obj_t* div1 = lv_obj_create(scr);
    lv_obj_set_size(div1, SCR_W - 2*PAD, 1);
    lv_obj_set_pos (div1, PAD, INP_LBL_Y - 1);
    lv_obj_set_style_bg_color    (div1, C_BORDER, 0);
    lv_obj_set_style_bg_opa      (div1, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(div1, 0, 0);
    lv_obj_set_style_radius      (div1, 0, 0);

    // ── INPUT SECTION ─────────────────────────────────────────────────────────
    lv_obj_t* inp_lbl = lv_label_create(scr);
    lv_label_set_text(inp_lbl, "INPUT");
    lv_obj_add_style(inp_lbl, &_style_section_lbl, 0);
    lv_obj_set_pos(inp_lbl, PAD, INP_LBL_Y + 3);

    const char* inp_names[4] = { "AUTO", "USB", "SPDF", "OPT" };
    for (int i = 0; i < 4; i++) {
        _input_btns[i] = lv_btn_create(scr);
        lv_obj_add_style(_input_btns[i], &_style_btn_inactive, 0);
        lv_obj_set_size(_input_btns[i], BTN4_W, INP_BTN_H - 8);
        lv_obj_set_pos (_input_btns[i], PAD + i*(BTN4_W + BTN_GAP), INP_BTN_Y + 4);
        lv_obj_add_event_cb(_input_btns[i], _onInput, LV_EVENT_CLICKED, this);
        lv_obj_set_user_data(_input_btns[i], (void*)(intptr_t)i);
        lv_obj_t* lbl = lv_label_create(_input_btns[i]);
        lv_label_set_text(lbl, inp_names[i]);
        lv_obj_center(lbl);
    }

    // Divider
    lv_obj_t* div2 = lv_obj_create(scr);
    lv_obj_set_size(div2, SCR_W - 2*PAD, 1);
    lv_obj_set_pos (div2, PAD, FLT_LBL_Y - 1);
    lv_obj_set_style_bg_color    (div2, C_BORDER, 0);
    lv_obj_set_style_bg_opa      (div2, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(div2, 0, 0);
    lv_obj_set_style_radius      (div2, 0, 0);

    // ── FILTER SECTION ────────────────────────────────────────────────────────
    lv_obj_t* flt_lbl = lv_label_create(scr);
    lv_label_set_text(flt_lbl, "FILTER");
    lv_obj_add_style(flt_lbl, &_style_section_lbl, 0);
    lv_obj_set_pos(flt_lbl, PAD, FLT_LBL_Y + 3);

    const char* flt_names[4] = { "LIN", "MIX", "MIN", "SOFT" };
    for (int i = 0; i < 4; i++) {
        _filter_btns[i] = lv_btn_create(scr);
        lv_obj_add_style(_filter_btns[i], &_style_btn_inactive, 0);
        lv_obj_set_size(_filter_btns[i], BTN4_W, FLT_BTN_H - 8);
        lv_obj_set_pos (_filter_btns[i], PAD + i*(BTN4_W + BTN_GAP), FLT_BTN_Y + 4);
        lv_obj_add_event_cb(_filter_btns[i], _onFilter, LV_EVENT_CLICKED, this);
        lv_obj_set_user_data(_filter_btns[i], (void*)(intptr_t)i);
        lv_obj_t* lbl = lv_label_create(_filter_btns[i]);
        lv_label_set_text(lbl, flt_names[i]);
        lv_obj_center(lbl);
    }

    // Divider
    lv_obj_t* div3 = lv_obj_create(scr);
    lv_obj_set_size(div3, SCR_W - 2*PAD, 1);
    lv_obj_set_pos (div3, PAD, MUTE_Y - 1);
    lv_obj_set_style_bg_color    (div3, C_BORDER, 0);
    lv_obj_set_style_bg_opa      (div3, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(div3, 0, 0);
    lv_obj_set_style_radius      (div3, 0, 0);

    // ── MUTE BUTTON ───────────────────────────────────────────────────────────
    _mute_btn = lv_btn_create(scr);
    lv_obj_add_style(_mute_btn, &_style_mute_inactive, 0);
    lv_obj_set_size(_mute_btn, SCR_W - 2*PAD, MUTE_H - 4);
    lv_obj_set_pos (_mute_btn, PAD, MUTE_Y + 2);
    lv_obj_add_event_cb(_mute_btn, _onMute, LV_EVENT_CLICKED, this);
    _mute_lbl = lv_label_create(_mute_btn);
    lv_label_set_text(_mute_lbl, LV_SYMBOL_MUTE "  MUTE");
    lv_obj_center(_mute_lbl);
}

//==============================================================================
// Public interface
//==============================================================================
void UI::createUI(const DACState& initialState)
{
    if (!_board) return;

    if (!lvgl_port_lock(200)) {
        LOG("LVGL lock timeout in createUI\n");
        return;
    }

    _initStyles();
    _buildLayout();

    // Apply initial state without sending serial commands
    updateVolume(initialState.volume, initialState.muted);
    updateInput (initialState.input);
    updateFilter(initialState.filter);

    lvgl_port_unlock();
}

void UI::updateVolume(int vol, bool muted)
{
    if (!_vol_label) return;

    char buf[16];
    int dB = vol - 99;   // vol=99 → 0dB, vol=50 → -49dB
    if (dB >= 0) {
        snprintf(buf, sizeof(buf), "+%d dB", dB);
    } else {
        snprintf(buf, sizeof(buf), "%d dB", dB);
    }
    lv_label_set_text(_vol_label, buf);

    // Mute button style
    if (_mute_btn) {
        lv_obj_remove_style_all(_mute_btn);
        if (muted) {
            lv_obj_add_style(_mute_btn, &_style_mute_active, 0);
            lv_label_set_text(_mute_lbl, LV_SYMBOL_MUTE "  MUTED");
        } else {
            lv_obj_add_style(_mute_btn, &_style_mute_inactive, 0);
            lv_label_set_text(_mute_lbl, LV_SYMBOL_MUTE "  MUTE");
        }
    }
}

void UI::updateInput(DAC_INPUT input)
{
    for (int i = 0; i < 4; i++) {
        if (!_input_btns[i]) continue;
        lv_obj_remove_style_all(_input_btns[i]);
        if (i == (int)input) {
            lv_obj_add_style(_input_btns[i], &_style_btn_active, 0);
        } else {
            lv_obj_add_style(_input_btns[i], &_style_btn_inactive, 0);
        }
        lv_obj_invalidate(_input_btns[i]);
    }
}

void UI::updateFilter(DAC_FILTER filter)
{
    for (int i = 0; i < 4; i++) {
        if (!_filter_btns[i]) continue;
        lv_obj_remove_style_all(_filter_btns[i]);
        if (i == (int)filter) {
            lv_obj_add_style(_filter_btns[i], &_style_btn_active, 0);
        } else {
            lv_obj_add_style(_filter_btns[i], &_style_btn_inactive, 0);
        }
        lv_obj_invalidate(_filter_btns[i]);
    }
}

//==============================================================================
// Static event callbacks
//==============================================================================
void UI::_onVolDown(lv_event_t* e)
{
    UI* ui = (UI*)lv_event_get_user_data(e);
    if (ui->_actionCb) ui->_actionCb(VOLUME_DOWN);
}

void UI::_onVolUp(lv_event_t* e)
{
    UI* ui = (UI*)lv_event_get_user_data(e);
    if (ui->_actionCb) ui->_actionCb(VOLUME_UP);
}

void UI::_onMute(lv_event_t* e)
{
    UI* ui = (UI*)lv_event_get_user_data(e);
    if (ui->_actionCb) ui->_actionCb(VOLUME_MUTE);
}

void UI::_onInput(lv_event_t* e)
{
    UI*  ui  = (UI*)lv_event_get_user_data(e);
    int  idx = (int)(intptr_t)lv_obj_get_user_data(lv_event_get_target(e));
    static const ACTION acts[4] = { INPUT_AUTO, INPUT_USB, INPUT_SPDIF, INPUT_OPT };
    if (ui->_actionCb) ui->_actionCb(acts[idx]);
}

void UI::_onFilter(lv_event_t* e)
{
    UI*  ui  = (UI*)lv_event_get_user_data(e);
    int  idx = (int)(intptr_t)lv_obj_get_user_data(lv_event_get_target(e));
    static const ACTION acts[4] = { FILTER_LINEAR, FILTER_MIXED, FILTER_MINIMUM, FILTER_SOFT };
    if (ui->_actionCb) ui->_actionCb(acts[idx]);
}
