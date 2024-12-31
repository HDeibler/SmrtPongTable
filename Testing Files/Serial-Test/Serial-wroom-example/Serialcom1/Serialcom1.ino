
#include <HardwareSerial.h>
#define RXp2 16
#define TXp2 17
HardwareSerial MySerial2(2);

void setup()
{
    // For the USB, just use Serial as normal:
    Serial.begin(9600);
  
    // Configure MySerial0 on pins TX=6 and RX=7 (-1, -1 means use the default)
    MySerial2.begin(115200, SERIAL_8N1, RXp2, TXp2);

}

void loop(){

  Serial.println('looping');
   if (MySerial2.available()) {
    String str  = MySerial2.readStringUntil('\r');
    Serial.println(str);
  }
  MySerial2.println("received");
  delay(5000);



}