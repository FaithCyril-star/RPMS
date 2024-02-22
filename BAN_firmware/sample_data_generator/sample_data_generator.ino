#include <AWSIotConnect.h>

void setup() {
  Serial.begin(115200);
  connectAWS(); // Connect to AWS IoT
}


void loop() {
  sendTestData(); // Publish sample data
  delay(2000); // Delay for 5 seconds before publishing next message
}


//data sending functions
void sendTestData(){
device = getClient();
if (!device.connected())
{
  connectAWS();
}
else
{
  device.loop();
  float temperature = random(20, 40); // Random temperature between 20 and 40
  int BPM = random(60, 100); // Random heart rate between 60 and 100
  int SpO2 = random(90, 100); // Random SpO2 between 90 and 100
  int systolicPressure = random(100, 140); // Random systolic pressure between 100 and 140
  int diastolicPressure = random(60, 90); // Random diastolic pressure between 60 and 90
  publishMessage(temperature,avgBPM,avgSpO2,systolicPressure,diastolicPressure);
  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("BPM: "); Serial.println(BPM);
  Serial.print("SpO2: "); Serial.println(SpO2);
  Serial.print("Systolic Pressure: "); Serial.println(systolicPressure);
  Serial.print("Diastolic Pressure: "); Serial.println(diastolicPressure);
}
}