/*
!!!! MEGA 2560:

comment out #define USE_SPI_LIB from both Sd2Card.h and Sd2Card.cpp..
//#define USE_SPI_LIB

Modify Sd2Card.h to enable the softSPI for Mega 2560
#define MEGA_SOFT_SPI 1  //change it to non-zero from 0

Reverse for UNO

Make sure to use uploader protokoll in platformio.ini:
upload_protocol = stk500v2
*/


/*
 Serial Port runtime Control
 When in the normal runmode the controller will send short status messages and receive short
 commands over the serial port using the COM_RXD and COM TXD signals.

 Status Messages
 Rx.xx uManager firmware version
 Mx Configuration mode, see table above
 Ix Input select mode, 3 = Auto, 0 = USB, 1 = SPDIF
 Fx Filter Type, 4 = linear, 5 = mixed, 6 = minimum, 7 = soft
 Lxxx Link Speed, 000 = unlocked, 044-384 PCM speed, 02M, 05M & 11M DSD speed
 Vxxx Volume, -80 to +10, default set to 0
 Px Phase, N = Normal, I = Inverted, default is Normal

 Command Messages
 Ix Input select mode, 3 = auto, 0 = USB, 1 = SPDIF
 Fx Filter Type, 4 = linear, 5 = mixed, 6 = minimum, 7 = soft
 Vxxx Volume, -80 to +10
 Px Phase, N = Normal, I = Inverted

 The dam1021 will acknowledge all command messages as status messages.
*/


/*
Make sure to set DAC volume to start volume in uManager over rs323
Volume should be equal to software volume

"set volume = -49" //scale 0-99 -> start 50 = 99 - 50 = (-)49dB
 */

 //-------------------------------------------------------------------
 //-------------------------------------------------------------------

#include <globals.h>
#include <auxiliary.h>
#include <functions.h>
#include <graphics.h>


//-------------------------------------------------------------------
void setup() {

    // if(DEBUG)Serial.begin(9600);
    // LOG("Serial started:");

    //Start serial communication with DAC
    Serial.begin(115200);


    tft.reset();
    uint16_t identifier = tft.readID();
   // identifier = 0x9325; //0x9341; //0x8357 //0x9325 //Adafruit
    identifier = 0x9320; //MCUFRIEND_kbv
    tft.begin(identifier);

    tft.fillScreen(Black);
    tft.setRotation(3);

    //-----
    cGreen = getCustomColor(50,200,50);
    cWhite = getCustomColor(220,220,220);
    cDarkGray = getCustomColor(70,70,70);
    //-----

    irrecv.enableIRIn(); // Start the IR receiver

    action = NONE;
    lastAction = NONE;
    pressTime = lastInterruptTime = millis();


    //-----
    //Default channel, volume and filter
    printInput  ( inputToString( dac_input ) );
    printVolume ( dac_volume );
    printFilter ( filterToString( dac_filter ) );
    //-----

    //display hold
    displayMessage ( "hold", posCheck.x, posCheck.y , cDarkGray );
    //getActionFromTouch
    displayMessage("info", posInfo.x, posInfo.y, cWhite  );


    //-----
    //Progmem bitmaps
    myDrawBitmap(xLeft, yUp, left_60, 60, 60 );
    myDrawBitmap(xRight, yUp, right_60, 60, 60 );
    myDrawBitmap(xLeft, yDown, vdown_60, 60, 60 );
    myDrawBitmap(xRight, yDown, vup_60, 60, 60 );

}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
void loop() {

    // First get ACTION, then EXECUTE

    //----
    //GET ACTION
    //Check REMOTE for action
    action = getActionFromRemote();

    //check TOUCH for action if no action from remote
    if( action == NONE ){
      //Touch doesnt work consistently, it returns every some cycles NONE action even on touch
      action = getActionFromTouch();
    }


      //EXECUTE ACTION
      //this ist where all the instructions are going to be made
      if( action != NONE ){
        lastAction = proceedAction ( action, lastAction );
      }
      //reset action
      action = NONE;


}
//-------------------------------------------------------------------
