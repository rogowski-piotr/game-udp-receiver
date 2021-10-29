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

#pragma pack(1)
typedef struct _CarTelemetryData
{
    uint16_t  m_speed;                    // Speed of car in kilometres per hour
    float     m_throttle;                 // Amount of throttle applied (0.0 to 1.0)
    float     m_steer;                    // Steering (-1.0 (full lock left) to 1.0 (full lock right))
    float     m_brake;                    // Amount of brake applied (0.0 to 1.0)
    uint8_t   m_clutch;                   // Amount of clutch applied (0 to 100)
    int8_t    m_gear;                     // Gear selected (1-8, N=0, R=-1)
    uint16_t  m_engineRPM;                // Engine RPM
    uint8_t   m_drs;                      // 0 = off, 1 = on
    uint8_t   m_revLightsPercent;         // Rev lights indicator (percentage)
    uint16_t  m_revLightsBitValue;        // Rev lights (bit 0 = leftmost LED, bit 14 = rightmost LED)
    uint16_t  m_brakesTemperature[4];     // Brakes temperature (celsius)
    uint8_t   m_tyresSurfaceTemperature[4]; // Tyres surface temperature (celsius)
    uint8_t   m_tyresInnerTemperature[4]; // Tyres inner temperature (celsius)
    uint16_t  m_engineTemperature;        // Engine temperature (celsius)
    float     m_tyresPressure[4];         // Tyres pressure (PSI)
    uint8_t   m_surfaceType[4];           // Driving surface, see appendices
} CarTelemetryData, *P_CarTelemetryData, *LP_CarTelemetryData;

#pragma pack(1)
typedef struct _PacketCarTelemetryData
{
    _PacketHeader       m_header;              // Header
    _CarTelemetryData   m_carTelemetryData[22];
    uint8_t             m_mfdPanelIndex;       // Index of MFD panel open - 255 = MFD closed
                                               // Single player, race – 0 = Car setup, 1 = Pits
                                               // 2 = Damage, 3 =  Engine, 4 = Temperatures
                                               // May vary depending on game mode
    uint8_t             m_mfdPanelIndexSecondaryPlayer;   // See above
    int8_t              m_suggestedGear;       // Suggested gear for the player (1-8)
                                               // 0 if no gear suggested
} PacketCarTelemetryData, *P_PacketCarTelemetryData, *LP_PacketCarTelemetryData;


#pragma pack(1)
typedef struct _CarMotionData
{
    float           m_worldPositionX;           // World space X position
    float           m_worldPositionY;           // World space Y position
    float           m_worldPositionZ;           // World space Z position
    float           m_worldVelocityX;           // Velocity in world space X
    float           m_worldVelocityY;           // Velocity in world space Y
    float           m_worldVelocityZ;           // Velocity in world space Z
    int16_t         m_worldForwardDirX;         // World space forward X direction (normalised)
    int16_t         m_worldForwardDirY;         // World space forward Y direction (normalised)
    int16_t         m_worldForwardDirZ;         // World space forward Z direction (normalised)
    int16_t         m_worldRightDirX;           // World space right X direction (normalised)
    int16_t         m_worldRightDirY;           // World space right Y direction (normalised)
    int16_t         m_worldRightDirZ;           // World space right Z direction (normalised)
    float           m_gForceLateral;            // Lateral G-Force component
    float           m_gForceLongitudinal;       // Longitudinal G-Force component
    float           m_gForceVertical;           // Vertical G-Force component
    float           m_yaw;                      // Yaw angle in radians
    float           m_pitch;                    // Pitch angle in radians
    float           m_roll;                     // Roll angle in radians
} CarMotionData, *P_CarMotionData, *LP_CarMotionData;

#pragma pack(1)
typedef struct _PacketMotionData
{
    _PacketHeader    m_header;                  // Header
    _CarMotionData   m_carMotionData[22];       // Data for all cars on track
                                                // Extra player car ONLY data
    float         m_suspensionPosition[4];      // Note: All wheel arrays have the following order:
    float         m_suspensionVelocity[4];      // RL, RR, FL, FR
    float         m_suspensionAcceleration[4];  // RL, RR, FL, FR
    float         m_wheelSpeed[4];              // Speed of each wheel
    float         m_wheelSlip[4];               // Slip ratio for each wheel
    float         m_localVelocityX;             // Velocity in local space
    float         m_localVelocityY;             // Velocity in local space
    float         m_localVelocityZ;             // Velocity in local space
    float         m_angularVelocityX;           // Angular velocity x-component
    float         m_angularVelocityY;           // Angular velocity y-component
    float         m_angularVelocityZ;           // Angular velocity z-component
    float         m_angularAccelerationX;       // Angular velocity x-component
    float         m_angularAccelerationY;       // Angular velocity y-component
    float         m_angularAccelerationZ;       // Angular velocity z-component
    float         m_frontWheelsAngle;           // Current front wheels angle in radians
} PacketMotionData, *P_PacketMotionData, *LP_PacketMotionData;

#pragma pack(1)
typedef struct _LapData
{
    uint32_t    m_lastLapTimeInMS;           // Last lap time in milliseconds
    uint32_t    m_currentLapTimeInMS;        // Current time around the lap in milliseconds
    uint16_t    m_sector1TimeInMS;           // Sector 1 time in milliseconds
    uint16_t    m_sector2TimeInMS;           // Sector 2 time in milliseconds
    float       m_lapDistance;               // Distance vehicle is around current lap in metres – could
                                          // be negative if line hasn’t been crossed yet
    float       m_totalDistance;             // Total distance travelled in session in metres – could
                                          // be negative if line hasn’t been crossed yet
    float       m_safetyCarDelta;            // Delta in seconds for safety car
    uint8_t     m_carPosition;               // Car race position
    uint8_t     m_currentLapNum;             // Current lap number
    uint8_t     m_pitStatus;                 // 0 = none, 1 = pitting, 2 = in pit area
    uint8_t     m_numPitStops;               // Number of pit stops taken in this race
    uint8_t     m_sector;                    // 0 = sector1, 1 = sector2, 2 = sector3
    uint8_t     m_currentLapInvalid;         // Current lap invalid - 0 = valid, 1 = invalid
    uint8_t     m_penalties;                 // Accumulated time penalties in seconds to be added
    uint8_t     m_warnings;                  // Accumulated number of warnings issued
    uint8_t     m_numUnservedDriveThroughPens;  // Num drive through pens left to serve
    uint8_t    m_numUnservedStopGoPens;        // Num stop go pens left to serve
    uint8_t    m_gridPosition;           // Grid position the vehicle started the race in
    uint8_t    m_driverStatus;           // Status of driver - 0 = in garage, 1 = flying lap
                                          // 2 = in lap, 3 = out lap, 4 = on track
    uint8_t    m_resultStatus;              // Result status - 0 = invalid, 1 = inactive, 2 = active
                                          // 3 = finished, 4 = didnotfinish, 5 = disqualified
                                          // 6 = not classified, 7 = retired
    uint8_t    m_pitLaneTimerActive;        // Pit lane timing, 0 = inactive, 1 = active
    uint16_t   m_pitLaneTimeInLaneInMS;      // If active, the current time spent in the pit lane in ms
    uint16_t   m_pitStopTimerInMS;           // Time of the actual pit stop in ms
    uint8_t    m_pitStopShouldServePen;      // Whether the car should serve a penalty at this stop
} LapData, *P_LapData, *LP_LapData;

#pragma pack(1)
typedef struct _PacketLapData
{
    _PacketHeader    m_header;              // Header
    _LapData         m_lapData[22];         // Lap data for all cars on track
} PacketLapData, *P_PacketLapData, *LP_PacketLapData;
#pragma pack()



void setup() {                              // setup -> connect to WiFi and start listening udp port
  Serial.begin(9600);
  while (!Serial);
  
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
  int packetSize = Udp.parsePacket();
  
  if (packetSize) {
    Udp.read(packetBuffer, packetSize);

    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    if (packetSize == 970) {    
      _PacketLapData* pack = (LP_PacketLapData)&packetBuffer;
      _LapData lap_data = pack->m_lapData[0];
      Serial.println(lap_data.m_currentLapNum);
      Serial.println("\n");
      
    } else if(packetSize == 1464) {
      _PacketMotionData* pack = (LP_PacketMotionData)&packetBuffer;
      _CarMotionData car_motion_data = pack->m_carMotionData[0];
      Serial.println(car_motion_data.m_worldPositionX);
      Serial.println(car_motion_data.m_worldPositionY);
      Serial.println(car_motion_data.m_worldPositionZ);     
      Serial.println("\n");
      
    } else if (packetSize == 1347) {
      _PacketCarTelemetryData* pack = (LP_PacketCarTelemetryData)&packetBuffer;
      _CarTelemetryData car_telemetry_data = pack->m_carTelemetryData[0];
      Serial.println(car_telemetry_data.m_speed);
      Serial.println(car_telemetry_data.m_throttle);
      Serial.println(car_telemetry_data.m_steer);     
      Serial.println(car_telemetry_data.m_brake); 
      Serial.println(car_telemetry_data.m_clutch); 
      Serial.println(car_telemetry_data.m_gear); 
      Serial.println(car_telemetry_data.m_engineRPM); 
      Serial.println(car_telemetry_data.m_drs); 
      Serial.println(car_telemetry_data.m_revLightsPercent); 
      Serial.println(car_telemetry_data.m_revLightsBitValue); 
      Serial.println(car_telemetry_data.m_brakesTemperature[0]); 
      Serial.println(car_telemetry_data.m_brakesTemperature[1]); 
      Serial.println(car_telemetry_data.m_brakesTemperature[2]); 
      Serial.println(car_telemetry_data.m_brakesTemperature[3]); 
      Serial.println(car_telemetry_data.m_engineTemperature); 
      Serial.println("\n");
    }
    
  }
}
