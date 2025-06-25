#include "Arduino.h"
// Ensure Baud Rate is the same between the different microcontrollers
#define BAUD_RATE 9600 //Increase the Baud Rate in the future
#define START_KEY "$/"
#define END_KEY "/$"
#define CONNECTION_CHECK "$C"
#define ELEMENT_SEPERATOR "|"

class Nano_Communication{
    public:
    String readSerialFor(){
        
    }
    Nano_Communication::Nano_Communication(){
        //Initialize Serial For Communication
        Serial.begin(BAUD_RATE);
        if(Serial.available()){
            String input = Serial.readStringUntil('\n'); // Read until Enter key
            input.trim(); // Remove any spaces or newlines
            Serial.println(CONNECTION_CHECK);
            if (input.length() > 0) {
    Serial.println("Input Lenth is greater than 0");
    number = input.toInt(); // Convert to integer
      if (number >= 0 && number <= 1023) {
        Serial.print("This is the correct number: ");
        Serial.println(number);
    

        // Map 0-1023 to a reasonable blink speed, like 50ms to 1000ms
        analogWrite(vib_port,number);
        if(number>200){
          
        }
      } else {
        Serial.println("Number out of range (0-1023)");
      }
    }
        }
        
        

        
        
    }
};

