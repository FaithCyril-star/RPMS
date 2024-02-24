# RPMS
A code repo for a remote patient monitoring system, includes the microcontroller firmware and application software(backend/frontend)

* Place AWSIotConnect module and secrets folder in the Arduino library folder
* Modify secrets header file and place your credentials in accordingly
* Connect as shown in circuit_schematic.png
* Upload the slave code to your arduino nano
* Upload firmware code to the esp8266
* Enter digits from keypad connected to arduino nano representing the blood pressure values, with "*" representing the end of a pressure value and "#" to erase the last digit
* Wait to connect to AWS
* After connected successfully, place DS18B20 on underarm, and finger on pulse oximeter for while, preferrably 6 mins
* Once values have stabilised as displayed on OLED, press push button to send data to AWS IoT Core

* Ensure rule has been set in AWS IoT Core to forward data from IoT Core to Dynamodb after specified table has been created
  

  


