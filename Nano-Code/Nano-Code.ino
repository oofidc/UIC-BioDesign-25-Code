#define vib_port A2

int blinkDelay = 500; // default blink delay in milliseconds

void setup() {
  Serial.begin(9600);
  pinMode(vib_port, OUTPUT); // Set the orange LED pin as output
}

void loop() {
  int number;
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n'); // Read until Enter key
    input.trim(); // Remove any spaces or newlines

    if (input.length() > 0) {
    number = input.toInt(); // Convert to integer
      if (number >= 0 && number <= 1023) {
        Serial.print("This is the correct number: ");
        Serial.println(number);

        // Map 0-1023 to a reasonable blink speed, like 50ms to 1000ms
        analogWrite(vib_port,number);
      } else {
        Serial.println("Number out of range (0-1023)");
      }
    }
  }


  
  // Blink the LED

}
