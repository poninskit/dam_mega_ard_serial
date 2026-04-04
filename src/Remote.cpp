#include "Remote.h"

#define DECODE_NEC   // must come before IRremote.hpp to disable all other decoders
#undef  DEBUG        // globals.h may define DEBUG=0; IRremote checks #ifdef DEBUG (not #if)
                     // which would trigger verbose internal logging even when DEBUG=0
#include <IRremote.hpp>

//==============================================================================
// RemoteInterface  –  Apple aluminium IR remote, NEC protocol, IRremote v4.x
//==============================================================================
/*
  Apple Remote NEC 32-bit frame layout (MSB→LSB in the raw 32-bit word):
    bits 31..16  Custom ID  (always 0x77E1 for Apple remotes)
    bits 15.. 8  Command byte
    bits  7.. 0  Pairing/generation byte (ignored – we only match the command)

  IRremote v4 stores the address bits in bit-reversed order vs. the logical NEC
  order, so the custom ID seen in the raw value is 0x87EE (bit-reverse of 0x77E1).

  Known command bytes (v4 bit layout):
    Up button    0x0B
    Down button  0x0D
    Right button 0x07
    Left button  0x08
    Center       0x5D
    Menu         0x02
    Play/Pause   0x5E
*/

RemoteInterface::RemoteInterface(int recvPin)
{
    pinMode(recvPin, INPUT_PULLUP);
    IrReceiver.begin((uint_fast8_t)recvPin, DISABLE_LED_FEEDBACK);

    LOG(IrReceiver.isIdle() ? "IrReceiver: OK\n" : "IrReceiver: init may have failed\n");
}

//------------------------------------------------------------------------------
ACTION RemoteInterface::getAction(PAGE /*page*/)
{
    if (!IrReceiver.decode()) {
        return NONE;
    }

    // ── Auto-repeat handling ──────────────────────────────────────────────────
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
        IrReceiver.resume();
        if (prevAct == VOLUME_UP || prevAct == VOLUME_DOWN) {
            _isRepeat = true;
            return prevAct;
        }
        return NONE;
    }
    _isRepeat = false;

    // ── Reject zero frames ────────────────────────────────────────────────────
    if (IrReceiver.decodedIRData.decodedRawData == 0) {
        IrReceiver.resume();
        return NONE;
    }

    uint32_t val = IrReceiver.decodedIRData.decodedRawData;

    // v4 bit layout (MSB→LSB):  pair_id[31:24] | cmd[23:16] | custom_id[15:0]
    uint8_t  command   = (val >> 16) & 0xFF;
    uint16_t custom_id = (val >>  0) & 0xFFFF;

    LOG("\nRemote RAW: 0x" + String(val, HEX) +
        "  id: 0x"  + String(custom_id, HEX) +
        "  cmd: 0x" + String(command,   HEX));

    // ── Only accept Apple aluminium remote ────────────────────────────────────
    const uint16_t APPLE_ID = 0x87EE;   // bit-reversed 0x77E1
    if (custom_id != APPLE_ID) {
        LOG(" → unknown remote, ignoring\n");
        IrReceiver.resume();
        return NONE;
    }

    // ── Debounce (200 ms) ─────────────────────────────────────────────────────
    if (millis() - lastRemoteMillis < 200) {
        IrReceiver.resume();
        return NONE;
    }

    // ── Map command → action ──────────────────────────────────────────────────
    //  Physical button    cmd    logical action
    //  Up                 0x0B   cycle input backward
    //  Down               0x0D   cycle input forward
    //  Right              0x07   volume up
    //  Left               0x08   volume down
    //  Center             0x5D   mute toggle
    //  Menu               0x02   cycle filter
    //  Play/Pause         0x5E   (future use)
    switch (command) {
        case 0x0B: action = CHANNEL_LEFT;  break;
        case 0x0D: action = CHANNEL_RIGHT; break;
        case 0x07: action = VOLUME_UP;     break;
        case 0x08: action = VOLUME_DOWN;   break;
        case 0x5D: action = VOLUME_MUTE;   break;
        case 0x02: action = FILTER;        break;
        case 0x5E: action = PLAY_PAUSE;    break;
        default:   action = NONE;          break;
    }

    LOG(" → action: " + String(action) + "\n");

    lastRemoteMillis = millis();
    IrReceiver.resume();
    prevAct = action;
    return action;
}
