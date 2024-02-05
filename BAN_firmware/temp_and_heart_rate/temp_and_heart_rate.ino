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

#include <PulseSensorPlayground.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>

#include <Keypad.h>

// D6 which is GPIO12
#define ONE_WIRE_BUS 12

OneWire oneWire(ONE_WIRE_BUS);	
MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
unsigned long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
unsigned long lastTemperatureRead = 0;
unsigned long temperatureInterval = 1000;

const unsigned long oxygenSaturationInterval = 60000;  
unsigned long startTime;
unsigned long lastReadingTime = 0;

uint32_t irBuffer[100]; // infrared LED sensor data
uint32_t redBuffer[100]; // red LED sensor data

int32_t bufferLength;      // data length
int32_t spo2;              // SPO2 value
int8_t validSPO2;          // indicator to show if the SPO2 calculation is valid
int32_t heartRate;         // heart rate value
int8_t validHeartRate;     // indicator to show if the heart rate calculation is valid

const byte RATE_SIZE_HR = 4; //Increase this for more averaging. 4 is good.
byte rates_hr[RATE_SIZE_HR]; //Array of heart rates
byte rateSpotHR = 0;
long lastBeatHR = 0; //Time at which the last beat occurred
float beatsPerMinuteHR;
int beatAvgHR;

byte pulseLED = 14; // Must be on PWM pin pin IRD D5
byte readLED = 13;   // Blinks with each data read pin D7

int pulsePin = 0;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 580;       // Determine which Signal to "count as a beat", and which to ingore.
int pulse;

float temperature;

int avgBPM;
int avgSpO2;
int systolicPressure;
int diastolicPressure;

/*ADD YOUR PASSWORD BELOW*/
const char *ssid = "Faith";
const char *password = "testConnection";

//Wi-Fi Client
WiFiClient client;
//create an instance of Pulse sensor
PulseSensorPlayground pulseSensor;

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(115200);
  // enterBloodPressure();
  setUpOLED();
  setUpOximeter();
  setUpTempSensor();
  setUpPulseSensor();
  connectToWiFi();

  startTime = millis();
}

void loop()
{
  readTemperature();
  unsigned long currentTime = millis();

  // Check if it's time to switch from oxygen saturation to heart rate
  if (currentTime - startTime < oxygenSaturationInterval) {
    // Read oxygen saturation during the first three minutes
    readHeartRateAndSpO2();
  } else {
    // Read heart rate for the rest of the time
      readHR();
    }
  displayParams(temperature,80,120,beatAvgHR,spo2);
}


void readTemperature() {
  if (millis() - lastTemperatureRead >= temperatureInterval) {
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    Serial.println(temperature);
    lastTemperatureRead = millis();
  }
}


void readHeartRateAndSpO2(){

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data

      digitalWrite(readLED, !digitalRead(readLED)); //Blink onboard LED with every data read

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample

      //send samples and calculation result to terminal program through UART
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
      Serial.println(validSPO2, DEC);
    }

    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }


void readHR(){
    particleSensor.setup(); //Configure sensor with default settings
    digitalWrite(readLED, LOW); //Blink onboard LED with every data read
    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
    //We sensed a beat!
    long delta = millis() - lastBeatHR;
    lastBeatHR = millis();

    beatsPerMinuteHR = 60 / (delta / 1000.0);

    if (beatsPerMinuteHR < 255 && beatsPerMinuteHR > 20)
    {
      rates_hr[rateSpotHR++] = (byte)beatsPerMinuteHR; //Store this reading in the array
      rateSpotHR %= RATE_SIZE_HR; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE_HR ; x++)
        beatAvgHR += rates_hr[x];
      beatAvgHR /= RATE_SIZE_HR;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinuteHR);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvgHR);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
}


  void displayParams(float temperature, int systolicPressure, int diastolyicPressure, int heartRate, int sp02){
  if(validSPO2) {
    avgSpO2 = max(avgSpO2,spo2);
    }
  avgBPM = max(heartRate,avgBPM);

  // Display Text
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.setCursor(85, 0);
  display.print(String(temperature));
  display.setCursor(120, 0);
  display.println("C");
  display.setCursor(0, 15);
  display.print("Heart rate: ");
  if(avgBPM) {
    display.setCursor(85, 15);
    display.print(String(avgBPM));
    display.setCursor(110, 15);
    display.println("bpm");
    }
  else{
    display.setCursor(70, 15);
    display.print("Reading..");
    }

  display.setCursor(0, 35);
  display.print("Sp02: ");
  display.setCursor(85, 35);
  display.print(String(avgSpO2));
  display.setCursor(100, 35);
  display.print("%");
  display.setCursor(0, 50);
  display.print("Blood Pressure:");
  display.setCursor(90, 50);
  display.print(String(systolicPressure)+"/");
  display.setCursor(105, 50);
  display.print(String(diastolyicPressure));
  display.display();
  }


  void setUpOLED(){
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  }


  void setUpTempSensor(){
  sensors.begin();
  sensors.setWaitForConversion(false);
  }


  void setUpPulseSensor(){
  pulseSensor.analogInput(pulsePin);   
  pulseSensor.setThreshold(Threshold);  
  }


  void setUpOximeter(){
  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps

  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC);
    displayMessage("Setting oximeter up ...");
  }
  }


  void setUpKeyPad(){
//     int n_rows = 4;
//     int n_cols = 4;
//     char keys[n_rows][n_cols] = {
//   {'1','2','3','A'},
//   {'4','5','6','B'},
//   {'7','8','9','C'},
//   {'*','0','#','D'}
// };

//   byte colPins[n_rows] = {D3, D2, D1, D0};
//   byte rowPins[n_cols] = {D7, D6, D5, D4};

//   Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, n_rows, n_cols);
  }


void connectToWiFi() {
//Connect to WiFi Network
   Serial.println();
   Serial.println();
   Serial.println("Connecting to WiFi..");
   WiFi.enableInsecureWEP(true);
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);

  displayMessage("Connecting to Wi-Fi...");
  while ((WiFi.status() != WL_CONNECTED)) {
   delay(500);
   Serial.println("Connecting...");
}

if (WiFi.status() == WL_CONNECTED) {
    displayMessage("WiFi connected!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(1000);
}
    Serial.println(F("Setup ready"));
}


void enterBloodPressure(){
  Serial.println("Enter systolic pressure: ");
  while (Serial.available() == 0) {
  }
  int systolicPressure = Serial.parseInt();
  Serial.println("Enter distolic pressure: ");
  while (Serial.available() == 0) {
  }
  int diastolicPressure = Serial.parseInt();
}


void displayMessage(String message){
  display.clearDisplay();
  display.setCursor(0, 28);
  display.print(message);
  display.display();
}
