#pragma once

#include <Arduino.h>
#include "globals.h"

//==============================================================================
// RemoteInterface – Apple aluminium IR remote, NEC protocol, IRremote v4.x
//==============================================================================
class RemoteInterface {
public:
    explicit RemoteInterface(int recvPin);

    // Poll for the next action.  Call once per loop iteration.
    ACTION getAction(PAGE page = MAIN_PAGE);

    // True if the most-recent frame was an NEC auto-repeat (used for vol ramp)
    bool isRepeat() const { return _isRepeat; }

private:
    ACTION   action           = NONE;
    ACTION   prevAct          = NONE;
    bool     _isRepeat        = false;
    uint32_t lastRemoteMillis = 0;
};
