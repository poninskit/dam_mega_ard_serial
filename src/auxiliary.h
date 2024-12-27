 #include <globals.h>


//----------------------------
void displayMessage ( const char* message,  int x, int y, uint16_t color = cWhite){


    //Print on screen to let user know
    tft.setTextSize(2);
    tft.fillRect ( x, y, 90, 20, Black );
    tft.setCursor( x, y);
    tft.setTextColor(color);
    tft.println( message );

}



//----------------------------
void displayInfo(){


  if(!Serial.available()){
    displayMessage ( "none", posInfo.x, posInfo.y, cWhite );
    delay(delayInfo);
    displayMessage ( "info", posInfo.x, posInfo.y, cWhite );
    return;
  }


  //get Serial buffer
  String str = Serial.readStringUntil('\n');

  if(str.length() <= 10){
    displayMessage( str.c_str() , posInfo.x, posInfo.y, cWhite );
  }else{
    displayMessage( "unkn." , posInfo.x, posInfo.y, cWhite );
  }

  delay(delayInfo);
  displayMessage ( "info", posInfo.x, posInfo.y, cWhite  );


}

//----------------------------
void printVolume ( int vol ){

    tft.setTextSize( posVolume.z );
    tft.fillRect( posVolume.x, posVolume.y, 90,60, Black );
    tft.setCursor( posVolume.x, posVolume.y );
    tft.setTextColor( cWhite );
    tft.println( vol );
}

//----------------------------
int volToDB( int vol ){
  int dB;
  dB = dac_max_volume - vol;
  return dB;
}

//----------------------------
void muteRamp( int vol ){
  // Rump up or down to mute
  if ( dimmed ){ // dimmed, rump up volume
     Serial.print( "V-" );
     Serial.println( volToDB(vol) );
  }else{ //dimm volume down
     Serial.print( "V-" );
     Serial.println( volToDB(dac_mute_vol) );
  }
}


//----------------------------
int setVolume ( ACTION act, int vol ){

    switch (act){
     case VOLUME_UP:
     if ( dimmed ) muteRamp( vol );
         if (vol < dac_max_volume){
           vol += 1;
           // Increment as per the UD value
           Serial.print( "V-" );
           Serial.println( volToDB(vol) );
           delay(delayVol);
         }
       break;
     case VOLUME_DOWN:
     if ( dimmed ) muteRamp( vol );
        if(vol > dac_min_volume){
           vol -= 1;
          // Increment or Decrement as per the UD value
          Serial.print( "V-" );
          Serial.println( volToDB(vol) );
          delay(delayVol);
        }
       break;
     case VOLUME_MUTE:
          // Ramp up or down to mute
          muteRamp ( vol );
       break;

     default:
       break;
    }

  return vol;
}
//----------------------------
void printInput (String input_name){

      tft.setTextSize(posChannel.z);
      tft.fillRect( posChannel.x, posChannel.y, 100,30, Black );
      tft.setCursor(posChannel.x, posChannel.y);
      tft.setTextColor(cGreen);
      tft.println( input_name );
}
//----------------------------
String setInput (DAC_INPUT dac_in){


    switch (dac_in){
    case AUTO:
      Serial.println("I3");
      break;
    case USB:
      Serial.println("I0");
      break;
    case SPDIF:
      Serial.println("I1");
      break;
    case OPT:
      Serial.println("I2");
      break;
   default:
    return "error";
    break;
    }

return inputToString( dac_in );
}


//----------------------------
void printFilter (String filter_name){

  //center text, 6 digits is centered
  int off_x = ( 6 - filter_name.length() ) * 6;

  tft.setTextSize( posFilter.z );
  tft.fillRect ( posFilter.x + off_x - 20, posFilter.y, 90,20, Black );
  tft.setCursor( posFilter.x + off_x, posFilter.y );
  tft.setTextColor( cGreen );
  tft.println( filter_name );

}

//----------------------------
String setFilter (DAC_FILTER dac_ftr){

    switch (dac_ftr){
    case LINEAR:
      Serial.println("F4");
      break;
    case MIXED:
      Serial.println("F5");
      break;
    case MINIMUM:
      Serial.println("F6");
      break;
    case SOFT:
      Serial.println("F7");
      break;
   default:
    return "error";
    }

  return filterToString (dac_ftr);
}

//Custom colors
//----------------------------
uint16_t getCustomColor(uint8_t red, uint8_t green, uint8_t blue)
{
  red   >>= 3;
  green >>= 2;
  blue  >>= 3;
  return (red << 11) | (green << 5) | blue;
}


//-----------------------------------------------
//Adafruit tft.drawRGBBitmap(xLeft, yUp, left_60, 60, 60 ) modified
void myDrawBitmap(int16_t x, int16_t y, const unsigned short *bitmap, int16_t w, int16_t h ) {

  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      tft.writePixel(x + i, y, pgm_read_word(&bitmap[j * w + i]));
    }
  }
}
