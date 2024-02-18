#include <pgmspace.h>
 
#define SECRET
 
const char WIFI_SSID[] = //your SSID goes here;               
const char WIFI_PASSWORD[] = //your network password goes here;  
 
#define THINGNAME //name of registered device(thing) in AWS goes here
 
#define TIME_ZONE 0//GMT
 
const char MQTT_HOST[] = //AWS endpoint(can be gotten from console);;
 
 
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";
 

// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
 
)KEY";
 
 
// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
 
)KEY";
