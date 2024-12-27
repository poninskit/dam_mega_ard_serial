#include <globals.h>


extern void printVolume ( int vol );
extern int setVolume ( ACTION act, int vol );
extern void printInput (String input_name);
extern String setInput (DAC_INPUT dac_in);
extern void printFilter (String filter_name);
extern String setFilter (DAC_FILTER dac_ftr);
extern void displayInfo ( );

//------------------------------------------------------------------------------
ACTION getActionFromRemote(){
/*
The Apple remote returns the following codes:
The value for the third byte when we discard the least significant bit is:
    HEX:
    Repaet -   FFFFFFFF / 4294967295
    Up -       77E1D06C / 2011287660
    Down -     77E1B06C / 2011279468
    Right -    77E1E06C / 2011291756
    left -     77E1106C / 2011238508
    middle -   77E1BA6C or 77E1206C / 2011282028 or 2011242604
    menu -    77E1406C / 2011250796
    play -     77E17A6C or 77E1206C / 2011265644 or 2011242604
*/

  if ( !irrecv.decode( &results ) )
    return NONE;


    LOG( "Remote:" + String( results.value ) );

    switch (results.value){
      case 4294967295LL: // REPEAT //LL (long long) to avoid unsigned warning
        //reapeat if volume change, holding the vol button
        if ( prevAct == VOLUME_DOWN || prevAct == VOLUME_UP ){
          action = prevAct;
        }else{
          action = NONE;
        }
      break;
      case 2011287696: // UP
      case 2011255020:
        action = VOLUME_UP;
      break;
      case 2011279504: // DOWN
      case 2011246828 :
        action = VOLUME_DOWN;
      break;
      case 2011291792: // RIGHT
      case 2011259116:
        action = CHANNEL_RIGHT;
      break;
      case 2011238544: // LEFT
      case 2011271404:
        action = CHANNEL_LEFT;
      break;
      case 2011282064: // MIDDLE  or 77E1206C
      case 2011275500:
        action = VOLUME_MUTE;
      break;
      case 2011250832:  // MENU
      case 2011283692:
        action = FILTER;
      break;
      case 2011265680: // PLAY or 77E1206C
      case 2011265644:
        action = NONE;
      break;
      default:
        action = NONE;
      break;
      }

    LOG("Remote action: " + actionToString(action) );

    irrecv.resume();
    prevAct = action;    // Remember the key in case we want to use the repeat code

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
