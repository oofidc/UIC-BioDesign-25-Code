int voltage;
  int voltage_0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  voltage = 240;
}

void loop() {
  // put your main code here, to run repeatedly:
  String input = "";

  //Serial.println("RUNNING THE LOOP");
  while(voltage_0==voltage){
  if(Serial.available()){
    input = Serial.readString();
    if(input.length()>0){
      
      voltage = input.toInt();
      Serial.println(voltage);
    }
  }
  }
  analogWrite(A2,voltage);
  voltage_0 = voltage;
  
}
