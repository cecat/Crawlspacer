/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/charlescatlett/CODE/Crawlspacer/Crawlspacer.ino"
//new crawlspace monitor system for Photon
// 7/9/19 CeC
// 11/9/19 CeC - added degreesF variable

#include <OneWire.h>

void setup();
void loop();
void checkDanger();
void initializeSensor();
void checkTemperature();
#line 7 "/Users/charlescatlett/CODE/Crawlspacer/Crawlspacer.ino"
OneWire ds = OneWire(D4);  // 1-wire signal on pin D4

unsigned long lastUpdate = 0;
float lastTemp;
byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
float celsius, fahrenheit;
double degreesF;


// intervals
int sampleInterval  = 60000;    // check temperature every minute (60000ms)
int publishInterval = 300000;   // report temperature every 5 minutes (900,000ms)
int evalInterval    = 180000;   // check for trouble every 3 minutes (300,000ms)
float lastPublish   = 0;
float lastEval      = 0;

String tString;                 // string buffer to hold timestamp

// danger tripwire settings
float danger        = 35.00;    // this is the threshold we define as "pipe freeze eminent"
float allGood       = 37.00;    // not gonna relax until we are seeing temperature go back solid up above danger
float Freezing      = 32.00;    // now we are really in trouble
bool inDanger       = false;    // start with a clean slate!


void setup() {
    Particle.variable("LastEval", tString);
    Particle.variable("CrawlTemperature", degreesF);
    delay(10000);
    initializeSensor();
    checkTemperature(); 
    String temperature = String(fahrenheit); // store temp in "temperature" string
    Particle.publish("crawlTemp", temperature, PRIVATE); // publish on startup
    Time.zone(-5);
}

void loop() {

    checkTemperature();
    degreesF = fahrenheit;
    if ( (millis()-lastPublish) > publishInterval ) {
        String temperature = String(fahrenheit); // store temp in "temperature" string
        Particle.publish("crawlTemp", temperature, PRIVATE); // publish to cloud
        lastPublish = millis();
    }
    if ( (millis()-lastEval) > evalInterval ) {
        checkDanger();
        lastEval = millis();
    }
    
    delay(sampleInterval);   
}

//
// Check to see if we are in danger, then report it once for IFTTT to deal with
// Report again if/when we get out of danger

void checkDanger() {
    time_t time = Time.now(); // get the current time
    Time.format(time, TIME_FORMAT_DEFAULT); // format the string
    tString = Time.timeStr();  // update the exposed variable

    if (inDanger) {
        if (fahrenheit < Freezing) { Particle.publish("CRAWLSPACE", "FREEZING!", PRIVATE); }
        if (fahrenheit > allGood) {
            inDanger = false;
            Particle.publish("CRAWLSPACE", "OK", PRIVATE);
        }
    }
    else {
        if (fahrenheit < danger) {
            inDanger = true;
            Particle.publish("CRAWLSPACE", "DANGER", PRIVATE);
            if (fahrenheit < Freezing) { Particle.publish("CRAWLSPACE", "RAPID FREEZING!", PRIVATE); }
        }
    }  

}

//
// search for sensor; figure out what kind we have; and report out
// Code here is from Particle.io
// https://docs.particle.io/tutorials/hardware-projects/maker-kit/
//
void initializeSensor() {

  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
         Particle.publish("diag", "CRC is not valid", PRIVATE);
      return;
  }
 
  // what kind of chip do we have?
  // we will set a type_s value for known types or just return

  switch (addr[0]) {
    case 0x10:
        Particle.publish("diag","Chip = DS1820/DS18S20", PRIVATE);
        type_s = 1;
        break;
    case 0x28:
        Particle.publish("diag","Chip = DS18B20", PRIVATE);
        type_s = 0;
        break;
    case 0x22:
        Particle.publish("diag","Chip = DS1822", PRIVATE);
        type_s = 0;
      break;
    case 0x26:
        Particle.publish("diag","Chip = DS2438", PRIVATE);
        type_s = 2;
        break;
    default:
        Particle.publish("diag","Unknown device type.", PRIVATE);
        return;
  }


}

// the following code is from particle.io
// https://docs.particle.io/tutorials/hardware-projects/maker-kit/
//
void checkTemperature() {

  ds.reset();               // first clear the 1-wire bus
  ds.select(addr);          // now select the device we just found
  ds.write(0x44, 0);        // or start conversion in powered mode (bus finishes low)

  delay(1000);     // wait 1 sec for conversion

  // first make sure current values are in the scratch pad

  present = ds.reset();
  ds.select(addr);
  ds.write(0xB8,0);         // Recall Memory 0
  ds.write(0x00,0);         // Recall Memory 0

  // now read the scratch pad

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE,0);         // Read Scratchpad
  if (type_s == 2) {
    ds.write(0x00,0);       // The DS2438 needs a page# to read
  }

  // transfer the values

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s == 2) raw = (data[2] << 8) | data[1];
  byte cfg = (data[4] & 0x60);

  switch (type_s) {
    case 1:
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
      celsius = (float)raw * 0.0625;
      break;
    case 0:
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      // default is 12 bit resolution, 750 ms conversion time
      celsius = (float)raw * 0.0625;
      break;

    case 2:
      data[1] = (data[1] >> 3) & 0x1f;
      if (data[2] > 127) {
        celsius = (float)data[2] - ((float)data[1] * .03125);
      }else{
        celsius = (float)data[2] + ((float)data[1] * .03125);
      }
  }

  // remove random errors
  if((((celsius <= 0 && celsius > -1) && lastTemp > 5)) || celsius > 125) {
      celsius = lastTemp;
  }

  fahrenheit = celsius * 1.8 + 32.0;
  lastTemp = celsius;

}
