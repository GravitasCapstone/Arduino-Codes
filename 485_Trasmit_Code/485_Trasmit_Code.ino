char input[10];

char command;

uint16_t temp;

void resetinput() {
  int k = 0;
  while (k != 10) {
    input[k] = '\0';
    k++;
  }
}


void readSerial() { // check if data detected on input buffer of serial monitor
  if (Serial2.available()) {
    Serial2.readBytesUntil('\0', input, 8);
  }
  //input[0] = 'n'; // if no input detected ; acts as flag for no input
  //Serial.println(int(input));
  return;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    command = Serial.read();
    Serial.println(command);
    Serial2.write(command);
    delay(1000);
  }

  if(Serial2.available())
  {
    readSerial();
    Serial.println(uint8_t(input[0]));
    Serial.println(uint8_t(input[1]));
    Serial.println(uint8_t(input[2]));
    Serial.println(uint8_t(input[3]));
    Serial.println(uint8_t(input[4]));
    temp = input[1] << 8 | input[0];
    Serial.println(temp);
    delay(1000);
    resetinput();
    delay(1000);
  }

}
