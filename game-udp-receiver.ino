#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <string.h>

unsigned const int localPort = 5606;        // define UDP PORT
const char ssid[] = "ssid";
const char pass[] = "password";
char packetBuffer[1343];              // buffer to hold incoming packet
WiFiUDP Udp;


int vRpm, vGear, vSpeed, vTemp = 0, vAbs = 0, vFuel = 0, vEngineState = 0, vLevel = 0, vTime = 0, vSplit = 0;
int LEDSTATE = 0;
char incomingByte = 0;
char data[50];
String sSpeed;
String sRpm;
int counter = 0;
int button = 1;
int state = 0;
String ver = "1.3.2";
String module = "1";


void setup() {                        // setup -> connect to WiFi and start listening udp port
  Serial.begin(9600);
  while (!Serial) ;
  
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("starting listen UDP on PORT: ");
  Serial.println(localPort);
  Udp.begin(localPort);
}

void loop() {
  int packetSize = Udp.parsePacket();             // if recived any data then read a packet
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    int len = Udp.read(packetBuffer, 1343);       // read the packet into packetBufffer
    if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    
    String stringOne = packetBuffer;
    
    if (stringOne.substring(0, 2) == "CC") {
        sRpm = stringOne.substring(2, 6);
        vRpm = sRpm.toInt();

        //here the right display for RPM,LAP,Water and Oil temp,position

    } else if (stringOne.substring(0, 3) == "VER") {
        //to version check
        Serial.println(ver + ";" + module + ";");

    } else if (stringOne.substring(0, 5) == "state") {
        //button state
        state = stringOne.substring(5, 6).toInt();

      if (state == 1) {
          String button_s = String(button);
          Serial.println(button_s + ":0:0:");
      }
    } else if (stringOne.substring(0, 2) == "DD") {
        sSpeed = stringOne.substring(2, 6);
        vSpeed = sSpeed.toInt();
    }
    else if (stringOne.substring(0, 2) == "BB") {
        vGear = stringOne.substring(2).toInt();
    }
    else if (stringOne.substring(0, 2) == "CH") {
        vLevel = stringOne.substring(2).toInt();
    }
    else if (stringOne.substring(0, 2) == "CA") {
        vTime = stringOne.substring(2).toInt();
    }
    else if (stringOne.substring(0, 2) == "CE") {
        vSplit = stringOne.substring(2).toInt();
    }
    else if (stringOne.substring(0, 2) == "AA") {
        vTemp = stringOne.substring(2, 3).toInt(); //0 or 1 , 0 is off and 1 is on
        vAbs = stringOne.substring(3, 4).toInt(); //0 or 1
        vEngineState = stringOne.substring(4, 5).toInt(); //0 or 1
        vFuel = stringOne.substring(5, 6).toInt(); //0 or 1
        LEDSTATE = stringOne.substring(6, 7).toInt(); //0 - 8
   }

   Serial.print("stringOne = ");    Serial.println(stringOne);
   Serial.println("converted values:");
   Serial.print("vRpm = ");         Serial.println(vRpm);
   Serial.print("vGear = ");        Serial.println(vGear);
   Serial.print("vSpeed = ");       Serial.println(vSpeed);
   Serial.print("vTemp = ");        Serial.println(vTemp);
   Serial.print("vAbs = ");         Serial.println(vAbs);
   Serial.print("vFuel = ");        Serial.println(vFuel);
   Serial.print("vEngineState = "); Serial.println(vEngineState);
   Serial.print("vLevel = ");       Serial.println(vLevel);
   Serial.print("vTime = ");        Serial.println(vTime);
   Serial.print("vSplit = ");       Serial.println(vSplit);
   Serial.print("LEDSTATE = ");     Serial.println(LEDSTATE);
  }
}
