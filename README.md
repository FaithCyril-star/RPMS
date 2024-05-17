# RPMS
A code repo for a remote patient monitoring system includes the microcontroller firmware and application software(backend).
The code repo for frontend can be found here - https://github.com/FaithCyril-star/iot-dashboard/tree/auth

This project involved the design and development of a circuit to accurately measure and wirelessly transmit temperature, heart rate, oxygen saturation and blood pressure data to AWS.
The transmitted data is stored using an event-driven architecture in AWS, into an EC2-hosted timescale database (a Postgres database extension for storing time series data).
A user-friendly web interface to enable effortless access to and interpretation of the transmitted health data was developed as well. 

The code repo is labelled as follows:
* BAN_firmware - Contains all code related to the Body Area Network (BAN) development, that is the hardware of the project. 
* Lambda_to_Timescale - Contains the AWS Lambda event-handler code.
* Application/backend -  Contains all code related to the backend of the web interface.
  
![alt text](https://github.com/FaithCyril-star/RPMS/blob/main/architecture.png?raw=true)
  

**System Design:**
Data is transmitted from the ESP8266, the main microcontroller of the BAN, via Wi-Fi to AWS IoT Core. AWS IoT Core then forwards the received data to an AWS Lambda service, which inserts the data into a TimescaleDB hosted on an EC2 instance. All client (frontend) requests are routed to the backend, which interacts with the database.

