char command;
char value;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    command = Serial.read();
    Serial2.write(command);
    delay(500);
  }
  if(Serial2.available()){
    value = Serial2.read();
    Serial.println(value,HEX);
  }

}
