bool xbeeSend(uint32_t TargetSL,uint8_t* payload){
  XBeeAddress64 TargetAddress = XBeeAddress64(UniSH,TargetSL);
  ZBTxRequest zbTx = ZBTxRequest(TargetAddress, payload, xbeeSendBufSize); //Assembles Packe
  xbee.send(zbTx);              //Sends packet
  memset(xbeeSendBuf, 0, xbeeSendBufSize);
  if (xbee.readPacket(500)) {                                       //Checks Reception
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        Serial.println("SuccessfulTransmit");
        return true;
      } else {
        Serial.println("TxFail");
        return false;
      }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");
    Serial.println(xbee.getResponse().getErrorCode());
  }
  return false;
}

void xbeeRead(){
  xbee.readPacket(); //read buffer
    if (xbee.getResponse().isAvailable()) { //got something
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) { //got a TxRequestPacket
        xbee.getResponse().getZBRxResponse(rx);
        uint32_t incominglsb = rx.getRemoteAddress64().getLsb();
        logprint("IncPckFrom ");
        logprintln32(incominglsb);
        if(rx.getPacketLength()>=xbeeRecBufSize){
          logprint("Oversized Message: ");
          logprintln(rx.getPacketLength());
        }
        memset(xbeeRecBuf, 0, xbeeRecBufSize); // Clears old buffer
        memcpy(xbeeRecBuf,rx.getData(),rx.getPacketLength());
        if(incominglsb == TardisSL){
          processTardisMessage();
        }
        if(incominglsb == MarsSL){
          processMarsMessage();
        }
        if(incominglsb == GroundSL){
          processGroundMessage();
        }    
      }
    } 
}

void processTardisMessage(){
  if(strstr((char*)xbeeRecBuf,"pleasedrop")){
      OutputSerial.println("TardisDropRequest");
      logprintln("TardisDropRequest");
  }else if(strstr((char*)xbeeRecBuf,"holdme")){
      OutputSerial.println("TardisHoldRequest");
      logprintln("TardisHoldRequest");
  }
}
void processMarsMessage(){
  
}

void processGroundMessage(){
  Serial.println("GroundMSG");
  if(strstr((char*)xbeeRecBuf,"test")){
      OutputSerial.println("things");
      logprintln("things");
      String("test").getBytes(xbeeSendBuf,xbeeSendBufSize);
      xbeeSend(GroundAddress,xbeeSendBuf);
  }
}
