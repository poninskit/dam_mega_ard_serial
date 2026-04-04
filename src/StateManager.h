#pragma once

#include <Preferences.h>
#include <functional>
#include "globals.h"

//------------------------------------------------------------------------------
// Volume scale: 0–99
//   vol = 99  →  0 dB  (max)
//   vol = 50  → −49 dB (default)
//   vol =  0  → −99 dB (min, DAM clamps at −80 dB)
#define VOL_MIN      0
#define VOL_MAX      99
#define VOL_DEFAULT  50

//------------------------------------------------------------------------------
struct DACState {
    DAC_INPUT  input  = AUTO;
    int        volume = VOL_DEFAULT;
    bool       muted  = false;
    DAC_FILTER filter = LINEAR;
};

//------------------------------------------------------------------------------
class StateManager {
public:
    StateManager() = default;

    void loadState();
    void saveState();

    const DACState& getState() const { return _state; }

    void setInput (DAC_INPUT  input);
    void setVolume(int        vol);
    void setMuted (bool       muted);
    void setFilter(DAC_FILTER filter);

    using Callback = std::function<void(const DACState&)>;
    void onStateChange(Callback cb) { _cb = cb; }

private:
    DACState    _state;
    Preferences _prefs;
    Callback    _cb;

    void _notify() { if (_cb) _cb(_state); }
};
