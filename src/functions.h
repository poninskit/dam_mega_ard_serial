#include <globals.h>


extern void printVolume ( int vol );
extern int setVolume ( ACTION act, int vol );
extern void printInput (String input_name);
extern String setInput (DAC_INPUT dac_in);
extern void printFilter (String filter_name);
extern String setFilter (DAC_FILTER dac_ftr);
extern void displayInfo ( );

unsigned long lastRemoteMillis = 0; //Prevente Debounce in Remote,  it keeps track of time between clicks

//------------------------------------------------------------------------------
/*
Bits 1-16: Apple Custom ID (Always the same)
Bits 17-23: The Pairing ID (This changes per remote!)
Bits 24-31: The Button Command
Bit 32: Odd parity bit

mask the results with 0x0000FF00 to skip pairing
or update switch to short masked codes
UP: 0x0D, DOWN: 0x0B, LEFT: 0x08, RIGHT: 0x07, MENU: 0x02, CENTER: 0x5D/0x04
*/
ACTION getActionFromRemote(){

  if ( !irrecv.decode( &results ) )
    return NONE;

  uint32_t val = results.value;
  LOG( "\nRemote RAW:" + String( val, HEX ) + "  " );

  // 1. Handle Repeat
  if (val == 0xFFFFFFFF) {
    irrecv.resume();
    if (prevAct == VOLUME_UP || prevAct == VOLUME_DOWN) return prevAct;
    return NONE;
  }

  // 2. DEBOUNCE CHECK
  // If the last button press was less than 250ms ago, ignore this one.
  // This prevents the "double action" on Play/Enter/Menu.
  if (millis() - lastRemoteMillis < 200) {
    irrecv.resume();
    return NONE; 
  }

  // 2. Extract the Command
  // Based on your log "77e1502a", we need to grab the "50" part.
  // We shift right by 8 bits to remove "2a", then mask the next 8 bits.
  uint32_t command = (val >> 8) & 0xFF;

  LOG( "Extracted Command: " + String(command, HEX) );

  switch (command) {
    case 0xD0: case 0xE1:
    case 0x50:            
      action = VOLUME_UP;
      break;
    case 0xB0: case 0x30: 
      action = VOLUME_DOWN;
      break;
    case 0x10: case 0x90: 
      action = CHANNEL_LEFT;
      break;
    case 0xE0: case 0x60: 
      action = CHANNEL_RIGHT;
      break;
    case 0xBA: case 0x3A: // MIDDLE (Select)
    case 0x20: case 0xA0: // SPAM Bits, repeats the code so it can cause bounce
      action = VOLUME_MUTE;
      break;
    case 0x40: case 0xC0: 
      action = FILTER;
      break;
    case 0x5E: case 0xDE: // PLAY/PAUSE
    case 0x7A: case 0xFA: // SPAM Bits, repeats the code so it can cause bounce
      action = VOLUME_MUTE;
      break;
    default:
      action = NONE;
      break;


  }

  LOG(" -> Action Assigned: " + String(action) );
  
  // Update the timer so we know when this last successful press happened
  lastRemoteMillis = millis();

  irrecv.resume();
  prevAct = action;
return action;
}

//----------------------------
ACTION getActionFromTouch (){


  ACTION act = NONE;

  //-----
  //get Touch and translate to action enum
  p = ts.getPoint(); // It changes pin cofiguration restore it after getting pos

  // Restore PINS for TFT!! as get Point changes IT!
  pinMode(LCD_CS, OUTPUT);   // Enable outputs
  pinMode(LCD_CD, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  pinMode( LCD_RESET, OUTPUT);


  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!


  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    //LOG ("Touch pressure: " + String(p.z));

    //ROW 1, volume down, filter, channel left
    if(p.y > 180 && p.y < 380){
      if(p.x > 620 && p.x < 860){
        act = CHANNEL_LEFT;
      }
      if(p.x > 440 && p.x < 610){
        act = FILTER;
      }
      if(p.x > 190 && p.x < 430){
        act = VOLUME_DOWN;
      }
    }

    //ROW 2, channel name, volume
    if(p.y > 410 && p.y < 680){
      if(p.x > 620 && p.x < 860){
        act = CHANNEL_RIGHT;
      }
      if(p.x > 440 && p.x < 610){
        act = HOLD;
      }
      if(p.x > 190 && p.x < 430){ //tap on volume number
        act = VOLUME_MUTE;
      }
    }


    //ROW 3, vol up, channel right, info
    if(p.y > 710 && p.y < 910){
      if(p.x > 620 && p.x < 860){
        act = CHANNEL_RIGHT;
      }
      if(p.x > 440 && p.x < 610){
        act = NONE;
      }
      if(p.x > 190 && p.x < 430){
        act = VOLUME_UP;
      }
    }

    //LOG( "Touch Action: " + actionToString(act) );
  }



return act;
}


//----------------------------
//This function executes ACTION, change and returns as lastACTION
ACTION proceedAction ( ACTION act, ACTION lastAct ){

  LOG("Action: " + actionToString(action) + " / Last action: " + actionToString(lastAction));

 // proceed actions
  switch (act){
    case NONE:
      break;
    case RESET:
      break;
    case CHANNEL_LEFT:
        if(dac_input == AUTO){
          dac_input = OPT;
        }else {
          dac_input = (DAC_INPUT)((int)(dac_input) - 1);
        }
        printInput( setInput (dac_input) );
        LOG(dac_input);
        delay(delayInput);
      break;
    case CHANNEL_RIGHT:
        if(dac_input == OPT){
          dac_input = AUTO;
        }else {
          dac_input = (DAC_INPUT)((int)(dac_input) + 1);
        }
        printInput(setInput (dac_input));
        LOG(dac_input);
        delay(delayInput);
      break;
    case VOLUME_UP:
        dac_volume = setVolume ( VOLUME_UP, dac_volume);
        dimmed = false;
        printVolume( dac_volume );
        LOG(dac_volume);
      break;
    case VOLUME_DOWN:
        dac_volume = setVolume ( VOLUME_DOWN, dac_volume);
        dimmed = false;
        printVolume( dac_volume);
        LOG(dac_volume);
      break;
    case VOLUME_MUTE:
        if( dac_volume > dac_mute_vol){
            if ( dimmed ){
              printVolume( dac_volume );
              setVolume ( VOLUME_MUTE, dac_volume );
              dimmed = false;
            }else{
              printVolume( dac_mute_vol );
              setVolume ( VOLUME_MUTE, dac_mute_vol );
              dimmed = true;
            }
          delay(delayMute);
        }
      break;
    case FILTER:

          if(dac_filter == SOFT){
            dac_filter = LINEAR;
          }else {
            dac_filter = (DAC_FILTER)((int)(dac_filter) + 1);
          }

          printFilter( setFilter (dac_filter) );
          delay(delayFilter);

      break;
    case HOLD:

          // detect holding time
          int act_after = 500;
          int wait_after_act = act_after + 500;
          hold_tapped_only = false;

          int interval = millis() - pressTime;
          if( lastAct != HOLD || interval > wait_after_act ){ //interval wait to start new saving procedure

            pressTime = lastInterruptTime = millis();
            LOG("----------first touch----------");

          }else{ //  hold

            //First holding
            int no_touch = millis() - lastInterruptTime;
            if( no_touch > 250 ){ //reset as holding interrupt detected
              act = RESET; //this will set all new
              pressTime = lastInterruptTime = millis();
              LOG("Interruption, start new counter (reset)");
              break;
            }else{
              lastInterruptTime = millis(); //update interrupt time
            }

            interval = millis() - pressTime;
            LOG( interval );
            if( interval > act_after ){ // holding 1.5s

            /*
            This is where the touch screen hold action should be placed
            */

            //perform serial read
            displayInfo();

            pressTime = millis(); //update press time
          }else if ( interval < 500 ){
            hold_tapped_only = true;
          }
        }
        break;
  }


return act;
}
