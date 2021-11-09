
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#define GPSECHO false
#define TX 4
#define RX 5
#define SIM800_TX_PIN 2  //tx
#define SIM800_RX_PIN 3  //rx


SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);
SoftwareSerial mySerial(TX, RX);
Adafruit_GPS GPS(&mySerial);


void setup() {
  Serial.println("Cubesat GPS");
  Serial.begin(115200);
  while (!Serial);
  serialSIM800.begin(9600);
  GPS.begin(9600);

  delay(500);
  serialSIM800.println("AT+CMGF=1");                // Activamos la funcion de envio de SMS
  updateSMS();
  serialSIM800.println("AT+CNMI=2,2,0,0,0");
  updateSMS();
  serialSIM800.println("AT+COPS?\r");  // Obtiene el numero de celda GSM en el cual se registro
  updateSMS();
  serialSIM800.println("AT+CMGL=\"ALL\",0\r");
  updateSMS();
  serialSIM800.println("AT+CMGF=1\r"); // Formato de texto string
  updateSMS();
  delay(200);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);     // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);  // uncomment this line to turn on only the "minimum recommended" data

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);        // 1 Hz update rate
  // GPS.sendCommand(PGCMD_ANTENNA);

  delay(600);
  mySerial.println(PMTK_Q_RELEASE);
}

uint32_t timer = millis();

void loop() {
  String str_Payload;

  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))  // this also sets the newNMEAreceived() flag to false
      return;                        // we can fail to parse a sentence in which case we should just wait for another
  }


  if (millis() - timer > 8000) {
    timer = millis();  // reset the timer
    /*
        Serial.print("\nTime: ");
        if (GPS.hour < 10) {
          Serial.print('0');
        }
        Serial.print(GPS.hour, DEC);
        Serial.print(':');
        if (GPS.minute < 10) {
          Serial.print('0');
        }
        Serial.print(GPS.minute, DEC);
        Serial.print(':');
        if (GPS.seconds < 10) {
          Serial.print('0');
        }
        Serial.print(GPS.seconds, DEC);
        Serial.print('.');
        if (GPS.milliseconds < 10) {
          Serial.print("00");
        } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
          Serial.print("0");
        }
        Serial.println(GPS.milliseconds);
        Serial.print("Date: ");
        Serial.print(GPS.day, DEC);
        Serial.print('/');
        Serial.print(GPS.month, DEC);
        Serial.print("/20");
        Serial.println(GPS.year, DEC);*/
    Serial.print("Fix: ");
    Serial.print((int)GPS.fix);
    Serial.print(" quality: ");
    Serial.println((int)GPS.fixquality);

    if (GPS.fix) {

      String latitud = String (GPS.latitude / 100, 6);
      String latitudDiv = latitud.substring(3);
      latitudDiv = ((latitudDiv.toDouble()) / 60 * 1000);
      latitudDiv = latitudDiv.substring(0, 6);
      latitud = latitud.substring(0, 3) + (String (latitudDiv));

      String longitd = String (GPS.longitude / 100, 6);
      String longitdDiv = longitd.substring(3);
      longitdDiv = ((longitdDiv.toDouble()) / 60 * 1000);
      longitdDiv = longitdDiv.substring(0, 6);
      longitd = longitd.substring(0, 3) + (String (longitdDiv));

      String Velocidad = String (GPS.speed * 0.514444);
      String Angulo = String (GPS.angle);
      String Altitud = String (GPS.altitude);
      String Satelites = String ((int)GPS.satellites);

      String senial = String (serialSIM800.println("AT+CSQ"));
      senial = senial.substring(0, 20);
      String Batt = String (serialSIM800.println("AT+CBC"));
      Batt = senial.substring(0, 20);

      str_Payload += "-" + latitud;
      str_Payload += ",";
      str_Payload += "-" + longitd;
      str_Payload += "," ;
      str_Payload +=  Velocidad;
      str_Payload += "," ;
      str_Payload +=  Batt;
      str_Payload += "," ;
      str_Payload +=  Satelites;
      str_Payload += "," ;
      str_Payload +=  senial;
      str_Payload += "," ;
      str_Payload +=  Altitud;
      str_Payload += "," ;
      str_Payload +=  "1024";
      str_Payload += "," ;
      str_Payload +=  "60";
      str_Payload += "," ;
      str_Payload +=  "40";
      str_Payload += "," ;
      str_Payload +=  Angulo;

      byte Payload[str_Payload.length()];
      int LengthFrameAPI = 60 + sizeof(Payload);
      int LengthPayload = sizeof(Payload);
      delay(1500);
      //EnviaSMS(str_Payload);
      Serial.println(str_Payload);

    }
  }
}
void updateSMS() {
  delay(500);
  while (Serial.available())
  {
    serialSIM800.write(Serial.read());
  }
  while (serialSIM800.available())
  {
    Serial.write(serialSIM800.read());
  }
}

void EnviaSMS(String str_Payload) {                       // Recive las trazas a enviar
  delay(100);
  //serialSIM800.println("AT+CMGS=\"+59899586757\"");
  //serialSIM800.println("AT+CMGS=\"+59897388126\"");       // Definimos el numero del destinatario en formato internacional
  //serialSIM800.println("AT+CMGS=\"+59892213525\"");
  //serialSIM800.println("AT+CMGS=\"+59898320207\"");
  serialSIM800.println("AT+CMGS=\"+59892968348\"");
  delay(100);
  serialSIM800.println(str_Payload);                      // MENSAJE
  serialSIM800.print(char(26));                           // Enviamos el equivalente a Control+Z
}
