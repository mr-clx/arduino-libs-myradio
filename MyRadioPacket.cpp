#include "MyRadioPacket.h"
#include "MyRadioProtocol_Common.h"

// MyRadioPacket radio_packet(); // for external use

MyRadioPacket::MyRadioPacket(uint16_t deviceId, radio_mode_e radioMode, uint8_t protId) {
    this->device_id=deviceId;
    this->mode=radioMode;
    this->prot_id=protId;
	/*if (radioMode==radio_mode_e.master) {
		radio_type_send = RADIO_TYPE_S_TO_M;
		radio_type_rcv = RADIO_TYPE_M_TO_S;
    } else {
		radio_type_send = RADIO_TYPE_M_TO_S;
		radio_type_rcv = RADIO_TYPE_S_TO_M;
	}*/
}

void MyRadioPacket::prepare(uint16_t devTo, uint8_t cmd) {
    idx=0;
    memset(&p, 0, sizeof(radio_packet_t)); // ?
    p.prot_id= prot_id;
    p.type = mode==master? RADIO_TYPE_M_TO_S : RADIO_TYPE_S_TO_M;
    p.cmd = cmd;
    p.dev_from=device_id;
    p.dev_to=devTo;
}


bool MyRadioPacket::isValid() {
   return (p.prot_id==prot_id) && 
          ( (mode==master && p.type==RADIO_TYPE_S_TO_M) || (mode==slave && p.type==RADIO_TYPE_M_TO_S) ) &&
          (!p.dev_to || p.dev_to==device_id);
}

//
//  Read values
//

byte MyRadioPacket::valueReadByte() {
  uint8_t v= p.values[idx];
  idx +=sizeof(uint8_t);
  return v; 
}

uint16_t MyRadioPacket::valueReadUInt16() {
  uint16_t v;
  memcpy(&v, &p.values[idx], sizeof(uint16_t));
  idx +=sizeof(uint16_t);
  return v; 
}

uint32_t MyRadioPacket::valueReadUInt32() {
  uint32_t v;
  memcpy(&v, &p.values[idx], sizeof(uint32_t));
  idx +=sizeof(uint32_t);
  return v;
}


float MyRadioPacket::valueReadFloat() {
  float v;
  memcpy(&v, &p.values[idx], sizeof(float));
  idx +=sizeof(float);
  return v;
}

//
//  Write values
//

void MyRadioPacket::valueWriteByte(byte v) {
  p.values[idx]=v;
  idx +=sizeof(uint8_t);
}

void MyRadioPacket::valueWriteUInt16(uint16_t v) {
  memcpy(&p.values[idx], &v, sizeof(uint16_t));
  idx +=sizeof(uint16_t);
}
void MyRadioPacket::valueWriteUInt32(uint32_t v) {
  memcpy(&p.values[idx], &v, sizeof(uint32_t));
  idx +=sizeof(uint32_t);
}

void MyRadioPacket::valueWriteFloat(float v) {
  memcpy(&p.values[idx], &v, sizeof(float)); // & v?
  idx +=sizeof(float);
}

//
//  Send/receive 
//

void MyRadioPacket::send(RF24 radio) {
    //if (listening) 
    radio.stopListening();
    radio.write(&p, sizeof(radio_packet_t));
    //if (listening) 
    radio.startListening();
    //leds_blink(1, 50,20);
}

void MyRadioPacket::sendPacket(RF24 radio, radio_packet_t *packet) {
    //if (listening) 
    radio.stopListening();
    radio.write(packet, sizeof(radio_packet_t));
    //if (listening) 
    radio.startListening();
    //leds_blink(1, 50,20);
}

void MyRadioPacket::sendPing(RF24 radio, uint16_t devTo) {
    prepare(devTo, RADIO_CMD_PING);
    valueWriteUInt32(millis());
    printActivity(true, "PING\r\n");
    send(radio);
}

void MyRadioPacket::sendPingResponse(RF24 radio) {
    printActivity(true, "PONG\r\n");
    p.cmd=RADIO_CMD_PONG;
    p.type = mode==master? RADIO_TYPE_M_TO_S : RADIO_TYPE_S_TO_M; // RADIO_TYPE_SEND;
    p.dev_to = p.dev_from;
    p.dev_from = device_id;
    send(radio);
}


uint8_t MyRadioPacket::receive(RF24 radio, bool onlyValid, bool autoPingPong, uint32_t *lasTickRcvd) {
    uint8_t pipeNum=0xff;
    if (radio.available(&pipeNum))
    {
        uint8_t len = radio.getDynamicPayloadSize();
	// all packet in buffer ?
        if (len!=sizeof(radio_packet_t)) return 0xff; 

        radio.read(&p, sizeof(radio_packet_t));
        idx=0;

        uint32_t ms = millis();

        if (!isValid()) {
            if (onlyValid) {
                printActivity(false, "Invalid packet\r\n");
                printInfo(true, pipeNum);
                return 0xff;
            } else 
              return pipeNum;
        }

        // todo: only if success processed 
        *lasTickRcvd=ms;

        if(autoPingPong){

            if (p.cmd == RADIO_CMD_PING){
                printActivity(false, "PING\r\n");
                sendPingResponse(radio);
                return 0xff;
            }

            else if (p.cmd == RADIO_CMD_PONG) {
                printActivity(false, "PONG ");
                uint32_t tsPing = valueReadUInt32(); //
                uint32_t rt = ms - tsPing;
                Serial.print(F("RoundTrip=")); Serial.println(abs(rt));
                return 0xff;
            }
        }
        return pipeNum;
    }
    return 0xff;
}


void MyRadioPacket::printActivity(bool isOut, char* msg) {
  Serial.print(F("[RF:"));
  Serial.print(isOut? F("OUT] ") : F("IN] "));
  printPath();
  if (msg) Serial.print(msg);
}

void MyRadioPacket::printInfo(bool printValues, uint8_t fromPipe) {
	Serial.println(F("###"));
	if (fromPipe!=0xff)
	   Serial.print(F("Pipe=")); Serial.println(fromPipe);
	//printf("PIPE 0x%02x\r\n", fromPipe);
	Serial.print(F("prot_id=0x")); Serial.print(p.prot_id, HEX);
	Serial.print(F("\ttype="));  Serial.println(p.type);
	//printf("prot_id=0x%02x\t", p.prot_id); printf("type=0x%02x\r\n", p.type);
	//printf("path: "); radio_packet_print_path(p);
	Serial.print(F("from=0x")); Serial.print(p.dev_from, HEX);
	Serial.print(F("\tto=0x")); Serial.println(p.dev_to, HEX);
	// printf("from=0x%04x\t", p.dev_from); printf("to=0x%04x\r\n", p.dev_to);
	Serial.print(F("cmd=")); Serial.print(p.cmd);
	//printf("cmd=0x%02x\r\n", p.cmd);
	if (printValues)
	{
		Serial.print(F("\r\nValues: "));
		for (uint8_t i=0; i<RADIO_PACKET_VALUES_CAPACITY; i++)
		{
			Serial.print(p.values[i]); Serial.print(F("\t"));
			//printf("0x%02x\t", p.values[i]);
			// Serial.print(p->values[i], HEX); Serial.print(" ");
		}
		Serial.println();
		if (p.prot_id!=RADIO_PROT_ID) 
			Serial.println(F("*Invalid prot_id!"));
	}
	Serial.println(F("\r\n###"));

}

void MyRadioPacket::printPath() {
	Serial.print(F("(0x")); Serial.print(p.dev_from, HEX); Serial.print(F("->0x"));
	Serial.print(p.dev_to, HEX); Serial.print(F(") "));
}
