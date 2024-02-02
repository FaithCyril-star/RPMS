/*
  Optical Heart Rate Detection (PBA Algorithm) is by: Nathan Seidle @ SparkFun Electronics
  
  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/
//libraries for temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

// D6 which is GPIO12
#define ONE_WIRE_BUS 12

OneWire oneWire(ONE_WIRE_BUS);	
MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
unsigned long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
unsigned long lastTemperatureRead = 0;
unsigned long temperatureInterval = 1000;

uint32_t irBuffer[100]; // infrared LED sensor data
uint32_t redBuffer[100]; // red LED sensor data

int32_t bufferLength;      // data length
int32_t spo2;              // SPO2 value
int8_t validSPO2;          // indicator to show if the SPO2 calculation is valid
int32_t heartRate;         // heart rate value
int8_t validHeartRate;     // indicator to show if the heart rate calculation is valid

byte pulseLED = 14; // Must be on PWM pin pin IRD D5
byte redLED = 13;   // Blinks with each data read pin D7

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  //start temperature sensor
  sensors.begin();
  sensors.setWaitForConversion(false);

  pinMode(pulseLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  // read the first 100 samples, and determine the signal range
  for (byte i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false) // do we have new data?
      particleSensor.check(); // Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); // We're finished with this sample so move to the next sample

    Serial.print(F("Continue placing your finger on the sensor"));
  }

  //take blood pressure here

  //
  // byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  // byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  // byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  // byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  // int pulseWidth = 411; //Options: 69, 118, 215, 411
  // int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
  // particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
}

void loop()
{
  readTemperature();
  readHeartRate();
}


void readTemperature() {
  if (millis() - lastTemperatureRead >= temperatureInterval) {
    sensors.requestTemperatures();
    Serial.print("Temperature: ");
    Serial.print(sensors.getTempCByIndex(0));
    Serial.print("ºC");
    Serial.println();
    lastTemperatureRead = millis();
  }
}

void readHeartRate() {
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  // readOxygenSaturation();
  Serial.println();
}


void readOxygenSaturation(){
  bufferLength = 100; // buffer length of 100 stores 4 seconds of samples running at 25sps

  // calculate heart rate and SpO2 after the first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
    // dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++) {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    // take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++) {
      while (particleSensor.available() == false) // do we have new data?
        particleSensor.check(); // Check the sensor for new data

      digitalWrite(redLED, !digitalRead(redLED)); // Blink onboard LED with every data read

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); // We're finished with this sample so move to the next sample

      // send samples and calculation result to the terminal program through UART
      Serial.print(F("red="));
      Serial.print(redBuffer[i], DEC);
      Serial.print(F(", ir="));
      Serial.print(irBuffer[i], DEC);

      Serial.print(F(", HR="));
      Serial.print(heartRate, DEC);

      Serial.print(F(", HRvalid="));
      Serial.print(validHeartRate, DEC);

      Serial.print(F(", SPO2="));
      Serial.print(spo2, DEC);

      Serial.print(F(", SPO2Valid="));
      Serial.print(validSPO2, DEC);
    }

    // After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
