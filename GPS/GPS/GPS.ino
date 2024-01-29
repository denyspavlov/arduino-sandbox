#include <TinyGPS++.h>
#include <TinyGPSPlus.h>

#include <SoftwareSerial.h>

// VCC + GRN must be connected to Arduino (both 5V and 3.3V work)
#define NEO_6M_TX 3
#define NEO_6M_RX 4

// When NEO 6M starts up it does not light any LEDs (this is searching for Satelites state)
// One the satelites are found the red LED will start blinking (usually needs a few minutes for the first time)
SoftwareSerial neo6m(NEO_6M_TX, NEO_6M_RX);

int gpsCmdBufferI = 0;
bool gpsCmdBufferComplete = false;
char gpsCmdBuffer[255];

// e.g. $GPGGA,210230,3855.4487,N,09446.0071,W,1,07,1.1,370.5,M,-29.5,M,,*7A
struct Position {
  // 0 The sentence type
  // $GPGGA
  // 1 Current time (if available; UTC)
  long  utcTime;
  // 2 Latitude (in DDMM.MMM format)
  double lat;
  // 3 Latitude compass direction
  char latD;
  // 4 Longitude (in DDDMM.MMM format)
  double lon;
  // 5 Longitude compass direction
  char lonD;
  // 6 Fix type (0 for no fix, 1 for GPS, 2 for DGPS)
  int fix;
  // 7 Number of satellites used for fix
  int satCnt;
  // 8 Horizontal dilution of precision
  float altSea;  
  // 9 Altitude above mean sea level
  float altAboveSea;  
  // 10 Altitude units (M for meters)
  float alt;  
  // 11 Height of mean sea level above WGS-84 earth ellipsoid
  // 12 Units of the above geoid seperation (M for meters)
  // 13 Time since last differential correction (ignored if inactive)
  // 14 Differential station ID (ignored if inactive)
  // 15 The checksum validation value (in hexadecimal)
};

bool gpsCmdRead = false;
struct Position last;
struct Position current;

// The TinyGPSPlus object
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  neo6m.begin(9600);
  Serial.println("Serial started");
}

void loop() {
  
  readTinyGPS();
  //readNeo6M();
  //parsePosition();
}

void readTinyGPS() {
  // This sketch displays information every time a new sentence is correctly encoded.
  while (neo6m.available() > 0)
    if (gps.encode(neo6m.read()))
      displayInfo();
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);

  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.print(F("   Altitude: ")); 
  if (gps.altitude.isValid())
  {
    Serial.print(gps.altitude.meters(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Satelites: "));
  if (gps.satellites.isValid())
  {
    Serial.print(gps.satellites.value());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}


void readNeo6M() {

  while (neo6m.available() > 0) {
    char c = neo6m.read();
    if (c == '$') {
      // if we received a new command but no \n from previous
      if (gpsCmdBufferI > 0 && !gpsCmdBufferComplete) {
        gpsCmdBuffer[gpsCmdBufferI++] = '\0'; 
        gpsCmdBufferComplete = true;
        gpsCmdRead = false;
  //      Serial.println(" <<<<<<<<< End");
      } else {
        // new command start
    //    Serial.print("Start >>>>>>>>>");
        gpsCmdBufferI = 0;
        gpsCmdBufferComplete = false;
      }
    } else if (c == '\n') {
      // received 
      gpsCmdBuffer[gpsCmdBufferI++] = '\0'; 
      gpsCmdBufferComplete = true;
      gpsCmdRead = false;
      //Serial.println(" <<<<<<<<< End");
      break; 
    } else {
      gpsCmdBuffer[gpsCmdBufferI++] = c;
      //Serial.print(c);
    }
  }

}

void parsePosition() {
  if (gpsCmdBufferComplete && !gpsCmdRead) {


    if (strncmp(gpsCmdBuffer, "GPGGA", 5) == 0) {

      Serial.print("Command received: ");
      Serial.println(gpsCmdBuffer);

      int i = 0;
      int pos = 0;
      int chunkStart = 0;
      char *dptr;

      char temp[25];

      while (gpsCmdBuffer[i] != '\0') {
        if (gpsCmdBuffer[i] == ',') {

          memcpy(temp, gpsCmdBuffer + chunkStart, i - chunkStart);
          temp[i - chunkStart] = '\0';

          Serial.print("Command chunk: ");
          Serial.println(temp);

          switch (pos) {
            // 0 The sentence type
            // $GPGGA
            // 1 Current time (if available; UTC)
            case 1:
              current.utcTime = atol(temp);
              break;
            // 2 Latitude (in DDMM.MMM format)
            case 2:
              current.lat = strtod(temp, &dptr);
              break;
            // 3 Latitude compass direction
            case 3:
              current.latD = temp[0];
              break;
            // 4 Longitude (in DDDMM.MMM format)
            case 4:
              current.lon = strtod(temp, &dptr);
              break;
            // 5 Longitude compass direction
            case 5:
              current.lonD = temp[0];
              break;
            // 6 Fix type (0 for no fix, 1 for GPS, 2 for DGPS)
            case 6:
              current.fix = atoi(temp);
              break;
            // 7 Number of satellites used for fix
            case 7:
              current.satCnt = atoi(temp);
              break;
            // 8 Horizontal dilution of precision
            case 8:
              current.altSea = atoi(temp);
              break;
            // 9 Altitude above mean sea level
            case 9:
              current.altAboveSea = atoi(temp);
              break;
            // 10 Altitude units (M for meters)
            case 10:
              current.alt = atoi(temp);
              break;
            // 11 Height of mean sea level above WGS-84 earth ellipsoid
            // 12 Units of the above geoid seperation (M for meters)
            // 13 Time since last differential correction (ignored if inactive)
            // 14 Differential station ID (ignored if inactive)
            // 15 The checksum validation value (in hexadecimal)
            default:
            // move to next chunk
              break;
          }
          pos++;
          chunkStart = ++i;
        } else {
          i++;
        }
      }

      Serial.print("Parsed >>>>>>>>>");
      Serial.print("Lat:");
      Serial.print(current.lat);
      Serial.print(",Lon:");
      Serial.print(current.lon);
      Serial.print(",LatD:");
      Serial.print(current.latD);
      Serial.print(",LonD:");
      Serial.print(current.lonD);
      Serial.print(",Alt(m):");
      Serial.print(current.alt);
      Serial.print(",Sats:");
      Serial.println(current.satCnt);
    }

    gpsCmdRead = true;
  }
}
