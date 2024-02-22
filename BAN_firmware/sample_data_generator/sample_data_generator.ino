#include <AWSIotConnect.h>

void setup() {
  Serial.begin(115200);
  connectAWS(); // Connect to AWS IoT
}


void loop() {
  // Sample data
  float temperature = random(20, 40); // Random temperature between 20 and 40
  int BPM = random(60, 100); // Random heart rate between 60 and 100
  int SpO2 = random(90, 100); // Random SpO2 between 90 and 100
  int systolicPressure = random(100, 140); // Random systolic pressure between 100 and 140
  int diastolicPressure = random(60, 90); // Random diastolic pressure between 60 and 90
  sendData(temperature, BPM, SpO2, systolicPressure, diastolicPressure); // Publish sample data
  delay(2000); // Delay for 5 seconds before publishing next message
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
  Serial.print(temperature);
  Serial.print(" ");
  Serial.print(avgBPM);
  Serial.print(" ");
  Serial.print(avgSpO2);
  Serial.print(" ");
  Serial.print(systolicPressure);
  Serial.print(" ");
  Serial.println(diastolicPressure);
}
}