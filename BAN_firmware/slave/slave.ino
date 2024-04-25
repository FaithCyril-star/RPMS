#include <Keypad.h>
#include<Wire.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char key;
long integer = 0;
int flag = 0;

char keys[ROWS][COLS] = {
{'1', '4', '7', '*'},
{'2', '5', '8', '0'},
{'3', '6', '9', '#'},
{'A', 'B', 'C', 'D'}
};
byte rowPins[ROWS] = {11, 10, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 5, 4, 3}; //connect to the column pinouts of the keypad
//Create an object of keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
void setup(){
  Serial.begin(115200); 
  Wire.begin(8);  ////nano-Slave Address
  Wire.onRequest(sendEvent);
}

void loop(){
  key = keypad.getKey();// Read the key

  // Print if key pressed
  if (key){
    if (key != 'A' and key != 'B' and key != 'C' and key != 'D'){
      if(key=='*'){
        integer=64;
      }
      else if(key == '#'){
        integer=32;
      }
      else{
        integer = String(key).toInt();
      }
    Serial.print("Key Pressed: ");
    Serial.println(key);
    Serial.print("Decimal Number: ");
    Serial.println(integer);
    }
    flag = 1; //new data
  }
}


//a4 -sda, A5 -SCl
void sendEvent()
{ 
  byte data = (flag << 7) | (byte)integer;
  Wire.write(data);
  flag = 0; //data is old
}

/*
S1 - 1
S5 - 2
S9 - 3
S2 - 4
S6 - 5
S10 - 6
S3 - 7
S7 - 8
S11 - 9
S4 - move to next value
S8 - 0
S12 - Clear
*/