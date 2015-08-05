/*
   MyRadioPacket unified classes for MyMeteo project
   Author: Odinchenko Aleksey (aleksey.clx@gmail.com)
   Last changes: 2015-08-05
*/

#ifndef _MyRadioProtocol_h
#define _MyRadioProtocol_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//
// PROTOCOL DESCRIPRION
//

// myradio_packet_t.prot_id:
#define RADIO_PROT_ID  0x36     // radio_packet_t.prot_id

#define RADIO_CMD_PING    1     // ping request for response
//   values[0..3] timestamp ms

#define RADIO_CMD_PONG    2     // pong response
//   values[0..3] sender timestamp

#endif