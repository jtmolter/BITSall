//An overly complicated piece of code for and overly simple little groundstation box
//Maybe at some point this can have a GUI, but this is still WIP
//Jonathan

#include <XBee.h>
#include <SoftwareSerial.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
SoftwareSerial xbeeSerial(2, 3);
//#define xbeeSerial Serial2 //How you would do it if you wanted to use harware serial


const uint32_t BitsSL = 0x417B4A3B;  //Specific to the XBee on Bits (the Serial Low address value)
const uint32_t GroundSL = 0x417B4A36;
const uint32_t HeliosSL = 0x417B4A3A;
const uint32_t UniSH = 0x0013A200; //Common across any and all XBees

ZBTxStatusResponse txStatus = ZBTxStatusResponse(); //What lets the library check if things went through
ZBRxResponse rx = ZBRxResponse();                   //Similar to above
ModemStatusResponse msr = ModemStatusResponse();    //And more
const int xbeeRecBufSize = 50; //Rec must be ~15bytes larger than send because
const int xbeeSendBufSize = 35;//there is overhead in the transmission that gets parsed out
uint8_t xbeeRecBuf[xbeeRecBufSize];
uint8_t xbeeSendBuf[xbeeSendBufSize];

void setup() {
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  delay(500);
  xbee.setSerial(xbeeSerial); //Sets which serial the xbee object listens to
  Serial.println("Startup");
  Serial.println("Enter Message Target (1 BITS, 2 Helios)");
}

void loop() {
  if(Serial.available()>0){ //Allows someone to send serial commands to the box
      if(Serial.read()=='1'){
        Serial.println("ToBits");
        while(!Serial.available()){}
        if(Serial.available()>0){
          Serial.print("Sending: ");
          Serial.readBytes((char*)xbeeSendBuf,xbeeSendBufSize);
          Serial.write(xbeeSendBuf,xbeeSendBufSize);
          xbeeSend(BitsSL,xbeeSendBuf);
        }
      }else if(Serial.read()=='2'){
        Serial.println("ToHelios"); //Really should color code these XBees instead of using payload names
        while(!Serial.available()){}
        if(Serial.available()>0){
          Serial.print("Sending: ");
          Serial.readBytes((char*)xbeeSendBuf,xbeeSendBufSize);
          Serial.write(xbeeSendBuf,xbeeSendBufSize);
          xbeeSend(HeliosSL,xbeeSendBuf);
        }
      }
  }
  
  
  xbeeRead(); //Checks buffer, does stuff if there are things to do
}

bool xbeeSend(uint32_t TargetSL,uint8_t* payload){
  XBeeAddress64 TargetAddress = XBeeAddress64(UniSH,TargetSL);      //The full address, probably could be done more efficiently, oh well
  ZBTxRequest zbTx = ZBTxRequest(TargetAddress, payload, xbeeSendBufSize); //Assembles Packet
  xbee.send(zbTx);                                                  //Sends packet
  memset(xbeeSendBuf, 0, xbeeSendBufSize);                          //Nukes buffer
  if (xbee.readPacket(500)) {                                       //Checks Reception
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {   //If rec
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      if (txStatus.getDeliveryStatus() == SUCCESS) {                //If positive transmit response
        Serial.println("SuccessfulTransmit");
        Serial.println("Enter Message Target (1 BITS, 2 Helios)");
        return true;
      } else {
        Serial.println("TxFail");
        Serial.println("Enter Message Target (1 BITS, 2 Helios)");
        return false;
      } 
    }
  } else if (xbee.getResponse().isError()) { //Stil have yet to see this trigger, might be broken...
    Serial.print("Error reading packet.  Error code: ");
    Serial.println(xbee.getResponse().getErrorCode());
  }
  return false;
}

void xbeeRead(){
  xbee.readPacket(); //read serial buffer
    if (xbee.getResponse().isAvailable()) { //got something
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) { //got a TxRequestPacket
        xbee.getResponse().getZBRxResponse(rx);
        
        uint32_t incominglsb = rx.getRemoteAddress64().getLsb(); //The SL of the sender
        Serial.print("Incoming Packet From: ");
        Serial.println(incominglsb,HEX);
        if(rx.getPacketLength()>=xbeeRecBufSize){                //Probably means something is done broke
          Serial.print("Oversized Message: ");
          Serial.println(rx.getPacketLength());
        }
        memset(xbeeRecBuf, 0, xbeeRecBufSize); // Nukes old buffer
        memcpy(xbeeRecBuf,rx.getData(),rx.getPacketLength());
        if(incominglsb == BitsSL){ //Seperate methods to handle messages from different senders
          processBitsMessage();    //prevents one payload from having the chance to be mistaken as another
        }
        if(incominglsb == HeliosSL){
          processHeliosMessage();
        }    
      }
    }
}

void processBitsMessage(){ //Just print things to the monitor
  Serial.println("RecFromBits");
  Serial.write(xbeeRecBuf,xbeeRecBufSize);
  Serial.println("");
}
/**
void processGroundMessage(){ //But THIS IS THE GROUND
  Serial.print("RecFromGND: ");
  Serial.write(xbeeRecBuf,xbeeRecBufSize);
  
  if(strstr((char*)xbeeRecBuf,"test")){ //Checks if "test" is within buffer
      Serial.println("");
      Serial.println("ackTest");
      String("PacketAck").getBytes(xbeeSendBuf,xbeeSendBufSize);
      xbeeSend(GroundSL,xbeeSendBuf);
  }
  if(strstr((char*)xbeeRecBuf,"TG")){ //Checks if "test" is within buffer
      Serial.println("");
      Serial.println("ToGround");
      String("ToGNDAck").getBytes(xbeeSendBuf,xbeeSendBufSize);
      xbeeSend(GroundSL,xbeeSendBuf);
  }  
}*/
void processHeliosMessage(){ //Just print things to the monitor
  Serial.println("RecFromHelios");
  Serial.write(xbeeRecBuf,xbeeRecBufSize);
  Serial.println("");
}
