#include <Arduino.h>
#include <math.h>

//#define R_2 1000 // Resistance of the resistor in series with the thermistor
//Script intended to b run on an arduino device connected to a thermistor in order to record changes in heat 


class NTC_Thermistor{
public:
    double R_therm;
    double R_res;
    double A;
    double B;
    double C;

    NTC_Thermistor(double thermistorResistance, double resistorResistance, double a, double b, double c){
        R_therm = thermistorResistance;
        R_res = resistorResistance;
        A = a;
        B = b;
        C = c;
    }


    //Solves for current temperature based on resistance of the thermistor using Steinhart-Hart equation
    double getTemperature(double resistance){
        double lnR = log(resistance / R_res);
        double invT = A + B * lnR + C * (lnR * lnR * lnR);
        return 1.0 / invT - 273.15; // Convert Kelvin to Celsius
    }

    //
    double getTemperatureBeta(double resistance){
      double T_0 = 298.1;
      double R_0 = 10000;
      double B = -113546.480773;
      return 1/(1/T_0 + (1/B)*log(resistance/10000));
    }
    //Solve for resistance of R1 based on Voltage output from thermisor
    double getResistance(double V_in, double V_out, int R_2){
        return (R_2 * V_out) / (V_in - V_out);
    }
};

void setup(){
    Serial.begin(9600);
    Serial.println("Bootin'....");
}

void loop(){
    pinMode(A0,INPUT);
    double voltage = analogRead(A0); // Read the voltage from the thermistor
    Serial.print("Voltage: ");
    Serial.println(voltage);
    double V_in = 5.0; // Assuming a 5V supply
    double V_out = voltage * (5.0 / 1023.0); // Convert analog reading to voltage
    NTC_Thermistor thermistor(10000.0, 1000, 0.025, -0.0025, 2e-6); // Example coefficients for a 10k thermistor
    double resistance = thermistor.getResistance(V_in, V_out, 1000); // Calculate the resistance of the thermistor
    double temperature = thermistor.getTemperature(resistance); // Calculate the temperature in Celsius
    double temperature_beta = thermistor.getTemperatureBeta(resistance);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Temperature(BETA): ");
    Serial.print(temperature_beta);
    Serial.println(" °K");

    delay(1000); // Wait for a second before the next reading
}
