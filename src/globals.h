#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Adafruit_GFX.h>   // Core graphics library
#include <MCUFRIEND_kbv.h>  //TFT Screen
#include <TouchScreen.h>    //Touch screen
#include <IRremote2.h>      //Remote control

//------------------------------------------------------------------------------
//DEBUGGIG
#define DEBUG 0

#if DEBUG == 1
  #define LOG(s) Serial.print(String(s) + "\n")
#else
  #define LOG(s)
#endif



//----------------------------
//Action
enum ACTION{
  NONE = 0,
  CHANNEL_LEFT,
  CHANNEL_RIGHT,
  VOLUME_UP,
  VOLUME_DOWN,
  VOLUME_MUTE,
  FILTER,
  HOLD,
  RESET
};
ACTION action;
ACTION lastAction; //main last action
ACTION prevAct; //remote previous action


static String actionToString(ACTION action)
{
    switch (action)
    {
    case NONE:              return "none";
    case CHANNEL_LEFT:      return "channel left";
    case CHANNEL_RIGHT:     return "channel right";
    case VOLUME_UP:         return "volume up";
    case VOLUME_DOWN:       return "volume down";
    case VOLUME_MUTE:       return "volume mute";
    case FILTER:            return "filter";
    case HOLD:              return "hold";
    case RESET:             return "reset";
    default:                return "unknown";
    }
}


//----------------------------
//Channel
enum DAC_INPUT{
  AUTO = 0,
  USB = 1,
  SPDIF = 2,
  OPT = 3
};
DAC_INPUT dac_input = AUTO;



static String inputToString(DAC_INPUT dac_in)
{
    switch (dac_in)
    {
      case AUTO:    return "AUTO";
      case USB:     return "USBI";
      case SPDIF:   return "SPDF";
      case OPT:     return "OPTC";
      default:      return "unknown";
    }
}


//----------------------------
//Filter
enum DAC_FILTER{
  LINEAR = 0,
  MIXED = 1,
  MINIMUM = 2,
  SOFT = 3
};
DAC_FILTER dac_filter = LINEAR;


static String filterToString(DAC_FILTER dac_fr)
{
    switch (dac_fr)
    {
      case LINEAR:    return "linear";
      case MIXED:     return "mixed";
      case MINIMUM:   return "minimum";
      case SOFT:      return "soft";
      default:        return "unknown";
    }
}


// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin


//Vxxx Volume, -80 to +10 dB , used -79 - 0dB (20 to 99 steps)
int dac_volume = 50;
int dac_max_volume = 99;
int dac_min_volume = 20;
int dac_mute_vol = 30;
bool dimmed = false;
bool hold_tapped_only = false;


// Touch Function
uint8_t YP = A1;  // must be an analog pin, use "An" notation!
uint8_t XM = A2;  // must be an analog pin, use "An" notation!
uint8_t YM = 7;   // can be a digital pin
uint8_t XP = 6;   // can be a digital pin
uint8_t SwapXY = 0;

#define MINPRESSURE 20
#define MAXPRESSURE 1000



/* some RGB color definitions                                                 */
#define Black           0x0000      /*   0,   0,   0 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Red             0xF800      /* 255,   0,   0 */
#define White           0xFFFF      /* 255, 255, 255 */



unsigned char __Gnbmp_image_offset  = 0;        // offset "Color"

// Declarations for BMP positions
//60px
const int __Gnbmp_height = 60;                 // bmp hight
const int __Gnbmp_width  = 60;                 // bmp width

#define BUFFPIXEL       30                     // must be a divisor of width
#define BUFFPIXEL_X3    90                     // BUFFPIXELx3

int xLeft = 26;
int yUp = 33;
int xRight = 229;
int yDown= 153 ;


//define custom colors
uint16_t cGreen;
uint16_t cWhite;
uint16_t cDarkGray;


// delays on interaction
int delayVol = 25;
int delayMute = 250;
int delayInput = 500;
int delayFilter = 650;
int delayInfo = 700;

unsigned long pressTime = 0;
unsigned long lastInterruptTime = 0;

int RECV_PIN = 23;   //Remote pin


MCUFRIEND_kbv tft;
//MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


TSPoint p(0,0,0); //touchpoint
TSPoint posChannel (114, 50, 4);
TSPoint posVolume (120, 158, 7);
TSPoint posFilter (20, 114, 2);
TSPoint posCheck  (136, 114, 2);
TSPoint posInfo   (233, 114, 2);

IRrecv irrecv(RECV_PIN); // Remote
decode_results results; // Remote


//------------------------------------------------------------------------------
#endif // GLOBALS_H
