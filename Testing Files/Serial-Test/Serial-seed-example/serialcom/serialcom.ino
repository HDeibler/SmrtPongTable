// Need this for the lower level access to set them up.
#include <HardwareSerial.h>

//Define two Serial devices mapped to the two internal UARTs
HardwareSerial MySerial0(0);


void setup()
{
    // For the USB, just use Serial as normal:
    Serial.begin(9600);
  
    // Configure MySerial0 on pins TX=6 and RX=7 (-1, -1 means use the default)
    MySerial0.begin(115200, SERIAL_8N1, -1, -1);
    MySerial0.print("MySerial0");
}

void loop(){
  MySerial0.println("MySerial0");
  Serial.println('sent');
   if (MySerial0.available()) {
    String str  = MySerial0.readStringUntil('\r');
    Serial.println(str);
  }
  delay(5000);



}