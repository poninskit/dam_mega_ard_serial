#include "StateManager.h"
#include <Arduino.h>

void StateManager::loadState() {
    _prefs.begin("dam1021", true);
    _state.input  = (DAC_INPUT) _prefs.getUChar("input",  (uint8_t)AUTO);
    _state.volume =              _prefs.getInt  ("volume", VOL_DEFAULT);
    _state.muted  =              _prefs.getBool ("muted",  false);
    _state.filter = (DAC_FILTER)_prefs.getUChar("filter", (uint8_t)LINEAR);
    _prefs.end();

    // Clamp in case stored value is out of range
    _state.volume = constrain(_state.volume, VOL_MIN, VOL_MAX);
}

void StateManager::saveState() {
    _prefs.begin("dam1021", false);
    _prefs.putUChar("input",  (uint8_t)_state.input);
    _prefs.putInt  ("volume", _state.volume);
    _prefs.putBool ("muted",  _state.muted);
    _prefs.putUChar("filter", (uint8_t)_state.filter);
    _prefs.end();
}

void StateManager::setInput(DAC_INPUT input) {
    if (_state.input != input) {
        _state.input = input;
        saveState();
        _notify();
    }
}

void StateManager::setVolume(int vol) {
    vol = constrain(vol, VOL_MIN, VOL_MAX);
    if (_state.volume != vol) {
        _state.volume = vol;
        saveState();
        _notify();
    }
}

void StateManager::setMuted(bool muted) {
    if (_state.muted != muted) {
        _state.muted = muted;
        saveState();
        _notify();
    }
}

void StateManager::setFilter(DAC_FILTER filter) {
    if (_state.filter != filter) {
        _state.filter = filter;
        saveState();
        _notify();
    }
}
