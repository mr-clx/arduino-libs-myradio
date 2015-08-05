/*
   MyRadioPacket unified classes for MyMeteo project
   Author: Odinchenko Aleksey (aleksey.clx@gmail.com)
   Last changes: 2015-08-05
*/

#ifndef _MyRadioPacket_h
#define _MyRadioPacket_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "nRF24L01.h" // RF24L01+ RADIO I2C
#include "RF24.h"     // http://tmrh20.github.io/RF24/index.html


#define RADIO_TYPE_OTHER  0   // other
#define RADIO_TYPE_M_TO_S 1   // master to slave
#define RADIO_TYPE_S_TO_M 2   // slave to master
#define RADIO_TYPE_M_TO_M 3   // master to master

#define RADIO_REMOTE_BROADCAST 0x0FFFF

#define RADIO_PACKET_VALUES_CAPACITY 25


typedef struct {// 32 bytes
	uint8_t prot_id;      // protocol ID
	uint8_t type;         // packet type (RADIO_TYPE_...)
	uint16_t dev_from;    // device_id from (0=broarcast)
	uint16_t dev_to;      // device_id to (0=broarcast)
	uint8_t cmd;          // packet command (RADIO_CMD_...)
	uint8_t values[RADIO_PACKET_VALUES_CAPACITY];
} radio_packet_t;

// radio mode
typedef enum radio_mode_e {
  unknown=(0), 
  master =(1),
  slave  =(2)
};


//
//  Radio Packet class helper
//
class MyRadioPacket {
	public:
	MyRadioPacket(uint16_t deviceId, radio_mode_e radioMode, uint8_t protId);

	uint8_t idx;
	radio_packet_t p;

	// uint8_t (*on_receive_cmd)(MyRadioPacket* sender, uint8_t cmd); // not implemented yet
	
	void prepare(uint16_t devTo, uint8_t cmd);
	bool isValid();

	byte     valueReadByte();
	uint16_t valueReadUInt16();
	uint32_t valueReadUInt32();
	float    valueReadFloat();

	void valueWriteByte(byte v);
	void valueWriteUInt16(uint16_t v);
	void valueWriteUInt32(uint32_t v);
	void valueWriteFloat(float v);

	void send(RF24 radio);
	void sendPacket(RF24 radio, radio_packet_t *packet);
	void sendPing(RF24 radio, uint16_t devTo);
	void sendPingResponse(RF24 radio);
	uint8_t receive(RF24 radio, bool onlyValid, bool autoPingPong, uint32_t *lasTickRcvd);

	void printActivity(bool isOut, char* msg);
	void printInfo(bool printValues, uint8_t fromPipe);
	void printPath();

protected:
	radio_mode_e mode;  // radio mode (master, slave)
	uint8_t prot_id;    // protocol_id
	uint16_t device_id; // local device id
};

// extern MyRadioPacket radio_packet();

#endif
