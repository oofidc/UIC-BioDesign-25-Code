// This is supposed to be  a simple sketch which runs the selected module(Peltier, or heating pad/heater fabric) in order for the other 
// Sketch to read the temperature and plot the thermal curve 
#include <Arduino.h>

#define PELTIER_ANAL_PIN A0
#define PELTIER_GND_PIN 3 // Optional ground pin for the Peltier module

#define FABRIC_ANAL_PIN A1
#define TESTING_TIME 30*1000 //Time for testing in 


class Module{

       
    private:
        int pin;
        int gndPin; // Optional ground pin, if needed for the module

    public:
        Module(int pin,int gndPin = -1) : pin(pin),gndPin(gndPin) {
            pinMode(pin, OUTPUT);
            if(gndPin != -1) {
                pinMode(gndPin, OUTPUT);
                digitalWrite(gndPin, LOW); // Set ground pin to LOW
            }
        }
    
        void turnOn() {
            analogWrite(pin, HIGH);
        }
    
        void turnOff() {
            analogWrite(pin, LOW);
        }
 
};


Module peltier(PELTIER_ANAL_PIN,PELTIER_GND_PIN);
Module fabric(FABRIC_ANAL_PIN);

void setup() {
    Serial.begin(9600);
    Serial.println("Module setup complete... Bootin' up!");
    peltier.turnOff();
    fabric.turnOff();
}


void loop() {
    // Example usage
    Module* TESTING_MODULE = &peltier;

    if(Serial.available() != 0) {
        char command = Serial.read();
        if(command == 'p') {
            Serial.println("Peltier module turned ON");
            TESTING_MODULE->turnOn();
            delay(TESTING_TIME);
            TESTING_MODULE->turnOff();
            Serial.println("Peltier module turned OFF"); 
        } 
    }

}