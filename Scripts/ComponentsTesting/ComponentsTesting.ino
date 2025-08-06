#include "Arduino.h"
#include "MPU9250.h"
#include "Adafruit_VL53L0X.h"
#include <SoftwareWire.h>

#define lox_SDA A2
#define lox_SCL A3



const int DIGITAL_GND_PORTS[] = {3, 4, 5}; 

bool dig_to_gnd(){
    for(int port: DIGITAL_GND_PORTS){
        pinMode(port,INPUT);
    }
}


// Compoennt class to represent simple parts where a voltage is just run through the device and you feel it(Cooler, Heating Pad, Thermistor)
class Component
{
    int pin;
    bool isAnal;
    // boolean variable describing if the pin is a sensor or actuator
    bool isSensor;

public:
    Component(int pin, bool isAnal, bool isSensor) : pin(pin), isAnal(isAnal), isSensor(isSensor)
    {
        if (isSensor)
            pinMode(pin, OUTPUT);
        else
            pinMode(pin, INPUT);
    }
    // method to turn on component, given analogue voltage[0,255], which can be -1 if the device is simply digital
    void turnOn(int analVol)
    {
        if (isAnal)
        {
            analogWrite(pin, analVol);
        }
        if (!isAnal)
        {
            digitalWrite(pin, HIGH);
        }
    }

    void turnOff()
    {
        if (isAnal)
        {
            analogWrite(pin, 0);
        }
        if (!isAnal)
        {
            digitalWrite(pin, LOW);
        }
    }

    int getPin() const { return pin; }
};


// Class designed for checking existence of MPU9250
//based off library here - https://github.com/hideakitai/MPU9250/blob/master/examples/connection_check/connection_check.ino
class MPU9250_Check
{
    uint8_t addrs[7] = {0};
    uint8_t device_count = 0;
    template <typename WireType = TwoWire>
    void scan_mpu(WireType &wire = Wire)
    {
        Serial.println("Searching for i2c devices...");
        device_count = 0;
        Serial.println("To Loop through Adresses...");
        for (uint8_t i = 0x68; i < 0x70; ++i)
        {
            Serial.println("DEBUG: Iterating Through Address");
            wire.beginTransmission(i);
            Serial.println("DEBUG: Begun Transmission @ Address");
            if (wire.endTransmission() == 0)
            {
                addrs[device_count++] = i;
                delay(10);
                Serial.println("DEBUG:  Found Device");
            }
        }
        Serial.print("Found ");
        Serial.print(device_count, DEC);
        Serial.println(" I2C devices");

        Serial.print("I2C addresses are: ");
        for (uint8_t i = 0; i < device_count; ++i)
        {
            Serial.print("0x");
            Serial.print(addrs[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    template <typename WireType = TwoWire>
    uint8_t readByte(uint8_t address, uint8_t subAddress, WireType &wire = Wire)
    {
        uint8_t data = 0;
        wire.beginTransmission(address);
        wire.write(subAddress);
        wire.endTransmission(false);
        wire.requestFrom(address, (size_t)1);
        if (wire.available())
            data = wire.read();
        return data;
    }

    public: 
    void check_connection()
    {
        //Serial.begin(115200);
        Serial.flush();
        Wire.begin();
        delay(2000);

        scan_mpu();

        if (device_count == 0)
        {
            Serial.println("No device found on I2C bus. Please check your hardware connection");
            while(1);
        }

        // check WHO_AM_I address of MPU
        for (uint8_t i = 0; i < device_count; ++i)
        {
            Serial.print("I2C address 0x");
            Serial.print(addrs[i], HEX);
            byte ca = readByte(addrs[i], WHO_AM_I_MPU9250);
            if (ca == MPU9250_WHOAMI_DEFAULT_VALUE)
            {
                Serial.println(" is MPU9250 and ready to use");
            }
            else if (ca == MPU9255_WHOAMI_DEFAULT_VALUE)
            {
                Serial.println(" is MPU9255 and ready to use");
            }
            else if (ca == MPU6500_WHOAMI_DEFAULT_VALUE)
            {
                Serial.println(" is MPU6500 and ready to use");
            }
            else
            {
                Serial.println(" is not MPU series");
                Serial.print("WHO_AM_I is ");
                Serial.println(ca, HEX);
                Serial.println("Please use correct device");
            }
            static constexpr uint8_t AK8963_ADDRESS{0x0C}; //  Address of magnetometer
            static constexpr uint8_t AK8963_WHOAMI_DEFAULT_VALUE{0x48};
            byte cb = readByte(AK8963_ADDRESS, AK8963_WHO_AM_I);
            if (cb == AK8963_WHOAMI_DEFAULT_VALUE)
            {
                Serial.print("AK8963 (Magnetometer) is ready to use");
            }
            else
            {
                //Serial.print("AK8963 (Magnetometer) was not found"); //This shpuld be uncommented later. Commented for DEBUG
            }
        }
    }

};

bool vl53l0x_check(){   
    SoftwareWire loxWire(lox_SDA, lox_SCL); 
    Adafruit_VL53L0X lox = Adafruit_VL53L0X();
    if (!lox.begin(0x29, &loxWire)) {
        Serial.println(F("Failed to boot VL53L0X"));
        return false;
    }
    Serial.print("Succesfully Booted VL53L0X;");
    for(int i = 0; i< 10; i++){
        VL53L0X_RangingMeasurementData_t measure;
            
        Serial.print("Reading a measurement... ");
        lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

        if (measure.RangeStatus != 4) {  // phase failures have incorrect data
            Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
        } else {
            Serial.println(" out of range ");
        }
            
        delay(100);
    }
    return true;

}

void setup()
{
    Serial.begin(9600);
    dig_to_gnd();
    // wait until serial port opens for native USB devices
    while (! Serial) {
        delay(1);
    }

    Serial.println("Components Testing Setup Complete... Bootin' up!");

    Serial.println("Beginning to Check for MPU9250...");
    MPU9250_Check mpu_check;
    mpu_check.check_connection();

    Serial.println("Beginning to Check for VL53L0X");
    vl53l0x_check();
}

void loop(){}