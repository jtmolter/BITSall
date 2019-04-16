//EZgpsmethods for adding simple preconfigured gps magic -Jonathan

const char hexList[] PROGMEM = {
  //RATES
  0xB5, 0x62, 0x06, 0x08 ,0x06 ,0x00 ,0xE8 ,0x03 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x39, //1Hz
  //0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0xF4 ,0x01 ,0x01 ,0x00 ,0x01 ,0x00 ,0x0B ,0x77, //2Hz
  //0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0xFA ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x10 ,0x96, //4Hz
  //0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0xC8 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0xDE ,0x6A, //5Hz
  //0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0x64 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x7A ,0x12, //10Hz
  //BAUD
  0xB5 ,0x62 ,0x06 ,0x00 ,0x14 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0xD0 ,0x08 ,0x00 ,0x00 ,0x80 ,0x25 ,0x00 ,0x00 ,0x07 ,0x00 ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xA2 ,0xB5, //9600
  //0xB5 ,0x62 ,0x06 ,0x00 ,0x14 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0xD0 ,0x08 ,0x00 ,0x00 ,0x00 ,0xC2 ,0x01 ,0x00 ,0x07 ,0x00 ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xC0 ,0x7E, //115200
  //AIRBORNE 4g
  //NEW
  0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0xDB,
  //0xB5 ,0x62 ,0x06 ,0x24 ,0x24 ,0x00 ,0xFF ,0xFF ,0x08 ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x10 ,0x27 ,0x00 ,0x00 ,0x05 ,0x00 ,0xFA ,0x00 ,0xFA ,0x00 ,0x64 ,0x00 ,0x2C ,0x01 ,0x00 ,0x3C ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x54 ,0x2C,
  //SAVE
  0xB5 ,0x62 ,0x06 ,0x09 ,0x0D ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0xFF ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x03 ,0x1D ,0xAB
};

void GPSINIT(){
  //pinMode(6,OUTPUT); //GPS transistor pin
  //digitalWrite(6,HIGH);
  //delay(1000);
	for(int i = 0;i<sizeof(hexList);i++){
		Serial3.write(pgm_read_byte(hexList+i));
		delay(10);
	}
}

GPSdata getGPS(){
  GPSdata gpsInfo;
  unsigned long chars;
  unsigned short sentences, failed;
  
    float GPSLat, GPSLon;
    int GPSSats;
    long GPSAlt;
    unsigned long date,fix_age,GPSTime;
    gps.f_get_position(&GPSLat, &GPSLon, &fix_age);
    GPSSats = gps.satellites();
    gps.get_datetime(&date, &GPSTime, &fix_age);
    GPSAlt = gps.altitude()/100.;

    gpsInfo.GPSLat = GPSLat;
    gpsInfo.GPSLon = GPSLon;
    gpsInfo.GPSTime = GPSTime/100;
    gpsInfo.GPSSats = GPSSats;
    gpsInfo.GPSAlt = GPSAlt;
  
  return gpsInfo;
}

void output(){
  String gpspacket;
  if(gpsInfo.GPSSats!=-1){
    gpspacket = String(gpsInfo.GPSTime)+","+String(gpsInfo.GPSLat,6) + "," + String(gpsInfo.GPSLon,6)+","+gpsInfo.GPSAlt+","+gpsInfo.GPSSats;
  }else{
    //gpspacket = String(preserve.GPSTime/100)+","+String(preserve.GPSLat,6) + "," + String(preserve.GPSLon,6)+","+preserve.GPSAlt+","+preserve.GPSSats;
    gpspacket = "err" + String(gpsInfo.GPSTime/100)+","+String(gpsInfo.GPSLat,6) + "," + String(gpsInfo.GPSLon,6)+","+gpsInfo.GPSAlt+","+gpsInfo.GPSSats;
  }
  Serial.println(gpspacket);
}
