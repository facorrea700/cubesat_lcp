

#include <SoftwareSerial.h>

#define SIM800_TX_PIN 2 //tx
#define SIM800_RX_PIN 3 //rx

SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  serialSIM800.begin(9600);
  delay(500);
  serialSIM800.println("AT+CMGF=1");
  delay(500);
  serialSIM800.println("AT+CNMI=2,2,0,0,0\r");
  delay(1000);
  //Serial.println("Setup Complete!");
  //Serial.println("Recept Ready SMS ...");
}
void loop() {
  String str_Payload;

  if (serialSIM800.available()) {
    str_Payload = serialSIM800.readString();
    byte Payload[str_Payload.length()];
    int LengthFrameAPI = 40 + sizeof(Payload);
    int LengthPayload = sizeof(Payload);
    delay(500);
    Serial.println(str_Payload);
  }
}

//void updateSMS() {
//  delay(500);
//  while (Serial.available())
//  {
//    serialSIM800.write(Serial.read());
//  }
//  while (serialSIM800.available())
//  {
//    Serial.write(serialSIM800.read());
//  }
//}
