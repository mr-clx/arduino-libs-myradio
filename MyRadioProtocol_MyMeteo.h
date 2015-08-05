/*
   MyRadioPacket protocol for MyMeteo project
   Author: Odinchenko Aleksey (aleksey.clx@gmail.com)
   Last changes: 2015-08-06
*/

#ifndef _MyRadioProtocol_MyMeteo_h
#define _MyRadioProtocol_MyMeteo_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define RADIO_CMD_MANUAL  3         // serial input
// *not implemented yet

#define RADIO_CMD_SENS_VALUES_Q 4   // M->S query sensor values
//   values[0] = sensor.id (0=all)

#define RADIO_CMD_SENS_VALUES_R 5   // S->M sensor values :
//   values[0]=sensor.id (local sensor N for device)
// < values[1]=value.kind, values[2]=value.id, values[3..6]=value > * N (=6 bytes per value, max 3 vaues)
//     value.kind==0 => end of list

#define RADIO_CMD_DISP_LED_TXT  6  // M->S CHANGE_DISP_MODE
//    values[0]=disp_id
//    values[1]: mask(b7=beep)
//    values[2]=duration(sec)
//    values[3..6] = text to show

#define RADIO_CMD_SET_DISP_MODE 7  // M->S DISPLAY LED TEXT
//    values[0]=disp_id
//    values[1] mask(b7=beep)
//    values[2]=duration(sec)
//    values[3]=mode
//    values[4]=next mode (0XFF for previous mode)

#define RADIO_CMD_RTC 8 // M->S broadcast Realtime Clock Signal
//    values[0]=year
//    values[1]=month
//    values[2]=day
//    values[3]=hour
//    values[4]=minutes
//    values[5]=seconds
//    values[6..9] = unix timestamp

#endif