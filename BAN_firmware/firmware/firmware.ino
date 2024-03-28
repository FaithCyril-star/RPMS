#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>

#include <AWSIotConnect.h>
#include <DatabaseConnector.h>

#define DEBUG //uncomment for serial printing

// D6 which is GPIO12
#define ONE_WIRE_BUS 12

OneWire oneWire(ONE_WIRE_BUS);	
MAX30105 particleSensor;
PubSubClient device; 

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
unsigned long currentTime;
unsigned long lastReadingTime = 0;

uint32_t irBuffer[100]; // infrared LED sensor data
uint32_t redBuffer[100]; // red LED sensor data

int32_t bufferLength;      // data length
int32_t spo2;              // SPO2 value
int8_t validSPO2;          // indicator to show if the SPO2 calculation is valid
int32_t heartRate;         // heart rate value
int8_t validHeartRate;     // indicator to show if the heart rate calculation is valid

byte pulseLED = 14; // Must be on PWM pin pin IRD D5
byte readLED = 13;   // Blinks with each data read pin D7
byte buttonPin = 16;

float temperature;
int avgBPM;
int avgSpO2;
int systolicPressure = 0;
int diastolicPressure = 0;
int endCount = 0;

bool isHeartRateMeasuring = false;
unsigned long lastMillis = 0;

int sendSignal;

const unsigned long databaseTimeOut = 30000;  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);


void setup()
{
  Serial.begin(115200);

  Wire.begin(4, 5); //SDA=4, SCL=5
  //setup button
  pinMode(buttonPin, INPUT);
  setUpOLED();
  setUpOximeter();
  setUpTempSensor();
  displayMessage("Connecting to AWS...");
  connectAWS();

  startTime = millis();
  while(!isDeviceRegistered()){
    currentTime = millis();
    if (currentTime - startTime < databaseTimeOut){
          displayMessage("Checking if device has been registered...");
    }
    else{
      while(true){
        displayMessage("Device is not registered. Please signup with device on the RPMS website and restart device");
      }
    }
  }
  
  readBloodPressure();
  startTime = millis();
}

void loop()
{
  readTemperature();
  currentTime = millis();

  // Check if it's time to switch from oxygen saturation to heart rate
  if (currentTime - startTime < oxygenSaturationInterval) {
    // Read oxygen saturation during the first three minutes
    readSpO2();
  } else {
    if(!isHeartRateMeasuring){reSetUpOximeter();};
    isHeartRateMeasuring = true;
    // Read heart rate for the rest of the time
    readHeartRate();
    }
  displayParams(temperature,systolicPressure,diastolicPressure,beatAvg,spo2);
  sendSignal = digitalRead(buttonPin);
  if(sendSignal){
    sendData();
  }
}



//set up functions
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

  #ifdef DEBUG
  Serial.print(F("red="));
  Serial.print(redBuffer[i], DEC);
  Serial.print(F(", ir="));
  Serial.println(irBuffer[i], DEC);
  #endif
  displayMessage("Setting oximeter up ...");
}
}


void reSetUpOximeter(){
  memset(rates, '\0', sizeof rates); //Array of heart rates
  byte rateSpotHR = 0;
  long lastBeatHR = 0; //Time at which the last beat occurred
  float beatsPerMinuteHR = 0;
  int beatAvgHR = 0;

  particleSensor.setup(); 
  digitalWrite(readLED, LOW);
}


//display functions
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
if(avgBPM >= 50) {
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
display.print(String(systolicPressure)+"/"+String(diastolyicPressure)+"mmHg");
display.display();
}


void displayMessage(String message){
  display.clearDisplay();
  display.setCursor(0, 28);
  display.print(message);
  display.display();
}


//reading functions
void readTemperature() {
  if (millis() - lastTemperatureRead >= temperatureInterval) {
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    lastTemperatureRead = millis();
  }
}


void readSpO2(){
  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }


    // take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data

      digitalWrite(readLED, !digitalRead(readLED)); //Blink onboard LED with every data read

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample

      //send samples and calculation result to terminal program through UART
      #ifdef DEBUG
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
      #endif
    }

    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }


void readHeartRate(){
    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  #ifdef DEBUG
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);


  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
  #endif
}


void readBloodPressure(){
  displayMessage("Get ready to enter blood pressure values...");
  delay(2000);
  while(endCount<2){
    Wire.requestFrom(8, 1);    // request 6 bytes from slave device #8
    byte data = Wire.read();
    bool isNewData = data & (1 << 7);
    if(isNewData){
      int8_t number  = data & ~(1 << 7);
      readValue(number);
    }

    #ifdef DEBUG
    Serial.print("sysPressure: ");
    Serial.println(systolicPressure);
    Serial.print("diaPressure: ");
    Serial.println(diastolicPressure);
    #endif

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("systolic Pressure: ");
    display.setCursor(110, 0);
    display.print(systolicPressure);
    display.setCursor(0, 28);
    display.print("diastolic Pressure: ");
    display.setCursor(110, 28);
    display.print(diastolicPressure);
    display.display();
    delay(500);
  }
}


void readValue(int number)
{
  if(number==64){
    endCount++;
  }
  else if(number==32){
    if(endCount == 0){
      systolicPressure /= 10;
    }
    else{
      diastolicPressure /= 10;
    }
  }
  else{
    if(!endCount){
    systolicPressure = systolicPressure*10 + number;
    }
    else if(endCount==1){
    diastolicPressure = diastolicPressure*10 + number;
    }
  }
}


//data sending functions
void sendData(){
device = getClient();
if (!device.connected())
{
  connectAWS();
}
else
{
  device.loop();
  publishMessage(temperature,avgBPM,avgSpO2,systolicPressure,diastolicPressure);
  #ifdef DEBUG
  Serial.print(temperature);
  Serial.print(" ");
  Serial.print(avgBPM);
  Serial.print(" ");
  Serial.print(avgSpO2);
  Serial.print(" ");
  Serial.print(systolicPressure);
  Serial.print(" ");
  Serial.println(diastolicPressure);
  #endif
}
}
