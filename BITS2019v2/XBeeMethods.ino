bool xbeeSend(XBeeAddress64 addr64,uint8_t* payload){
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload)); //Assembles Packe
  xbee.send(zbTx);                                                  //Sends packet
  if (xbee.readPacket(500)) {                                       //Checks Reception
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        logprintln("SuccessXbeeTransmit");
        return true;
      } else {
        //MissionFail
      }
    }
  }
  return false;
}

void xbeeRead(){
  xbee.readPacket(); //read buffer
    if (xbee.getResponse().isAvailable()) { //got something
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) { //got a TxRequestPacket
        xbee.getResponse().getZBRxResponse(rx);
        
        uint32_t incominglsb = rx.getRemoteAddress64().getLsb();
        logprint("ReceivedXbeePacketFrom");
        logprintln32(incominglsb);
        Serial.print("Incoming Packet From: ");
        Serial.println(incominglsb,HEX);
        int incomingLength = rx.getPacketLength();
        memset(xbeeBuf, 0, 250); // Clears old buffer
        memcpy(xbeeBuf,rx.getData(),rx.getPacketLength());
        if(incominglsb == TardisSL){
          processTardisMessage();
        }else if(incominglsb == MarsSL){
          processMarsMessage();
        }else if(incominglsb == GroundSL){
          processGroundMessage();
        }    
      }
    } else if (xbee.getResponse().isError()) {
      Serial.print("error code:");
      Serial.println(xbee.getResponse().getErrorCode());
    }
}

void processTardisMessage(){
  if(strstr((char*)xbeeBuf,"pleasedrop")){
        OutputSerial.println("TardisDropRequest");
        logprintln("TardisDropRequest");
  }else if(strstr((char*)xbeeBuf,"holdme")){
        OutputSerial.println("TardisHoldRequest");
        logprintln("TardisHoldRequest");
  }
}
void processMarsMessage(){
  
}
void processGroundMessage(){
  Serial.println("GroundMSG");
  if(strstr((char*)xbeeBuf,"test")){
        OutputSerial.println("things");
        logprintln("things");
        String("test").getBytes(xbeeBuf2,49);
    xbeeSend(GroundAddress,xbeeBuf2);
  }
}
