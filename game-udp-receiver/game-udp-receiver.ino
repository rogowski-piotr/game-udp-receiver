#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <string.h>

unsigned const int localPort = 20777;       // define UDP PORT
const char ssid[] = "ssid";
const char password[] = "password";
char packetBuffer[2000];                    // buffer to hold incoming packet
WiFiUDP Udp;


#pragma pack(1)
typedef struct _PacketHeader
{
    uint16_t    m_packetFormat;             // 2021
    uint8_t     m_gameMajorVersion;         // Game major version - "X.00"
    uint8_t     m_gameMinorVersion;         // Game minor version - "1.XX"
    uint8_t     m_packetVersion;            // Version of this packet type, all start from 1
    uint8_t     m_packetId;                 // Identifier for the packet type, see below
    uint64_t    m_sessionUID;               // Unique identifier for the session
    float       m_sessionTime;              // Session timestamp
    uint32_t    m_frameIdentifier;          // Identifier for the frame the data was retrieved on
    uint8_t     m_playerCarIndex;           // Index of player's car in the array
    uint8_t     m_secondaryPlayerCarIndex;  // Index of secondary player's car in the array (splitscreen)
                                            // 255 if no second player
} PacketHeader, *P_PacketHeader, *LP_PacketHeader;
#pragma pack()



void setup() {                              // setup -> connect to WiFi and start listening udp port
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

/* DEBUG ONLY METHOD */
void print_uint64_t(uint64_t num) {
  char rev[128]; 
  char *p = rev+1;
  while (num > 0) {
    *p++ = '0' + ( num % 10);
    num/= 10;
  }
  p--;
  while (p > rev) {
    Serial.print(*p--);
  }
}

void loop() {
  
  int packetSize = Udp.parsePacket();
  Serial.println(packetSize);
  
  if (packetSize) {
    
    Udp.read(packetBuffer, packetSize);

    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    
    LP_PacketHeader pack = (LP_PacketHeader)&packetBuffer;
    
    Serial.println(pack->m_packetFormat);
    Serial.println(pack->m_gameMajorVersion);
    Serial.println(pack->m_gameMinorVersion);
    Serial.println(pack->m_packetId);
    print_uint64_t(pack->m_sessionUID);
    Serial.println(pack->m_sessionTime);
    Serial.println(pack->m_frameIdentifier);
    Serial.println(pack->m_playerCarIndex);
    Serial.println(pack->m_secondaryPlayerCarIndex);
    Serial.println("\n");
  }
}
