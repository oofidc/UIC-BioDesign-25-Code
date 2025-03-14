void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  int time_0 = millis();
  Serial.println("STARTING");
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();  //gets one byte from serial buffer
      Serial.print(c);
    }
  }
}
