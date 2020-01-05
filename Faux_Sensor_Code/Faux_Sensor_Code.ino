const int  switchOnePin = 2;    // digital in 2 (pin the switch one is attached to)
const int  switchTwoPin = 3;    // digital in 3 (pin the switch two is attached to)
const int  switchThreePin = 4;  // digital in 4 (pin the switch three is attached to)

const int red = 8;
const int green = 9;
const int blue = 10;

int switchOneState = 0;         // current state of the switch

int switchTwoState = 0;

int switchThreeState = 0;

uint8_t temp_data[2];
uint8_t humid_data[2];
int select = 0;

char input;

void setup() {
    //initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    Serial2.begin(9600);

    int switchOneState = 0;         // current state of the switch
    int lastSwitchOneState = 0;     // previous state of the switch switch pins as an input
    pinMode(switchOnePin, INPUT);
    pinMode(switchTwoPin, INPUT);
    //pinMode(switchThreePin, INPUT);

    pinMode(red, OUTPUT);
    pinMode(green, OUTPUT);
    pinMode(blue, OUTPUT);
}

void loop() {
    if(Serial.available())
    {
      input = Serial.read();
      switch(input)
      {
        case 'h':
        digitalWrite(red, HIGH);
        digitalWrite(green, LOW);
        digitalWrite(blue, LOW);
        break;
  
        case 's':
        digitalWrite(red, LOW);
        digitalWrite(green, HIGH);
        digitalWrite(blue, LOW);
        break;
  
        case 'c':
        digitalWrite(red, LOW);
        digitalWrite(green, LOW);
        digitalWrite(blue, HIGH);
        break;
  
        case 'r':
        // read the switch input pins:
        switchOneState   = digitalRead(switchOnePin);
        switchTwoState   = digitalRead(switchTwoPin);
        switchThreeState = digitalRead(switchThreePin);
        
    
        if (switchOneState == LOW && switchTwoState == LOW && switchThreeState == LOW)
        {
          select = 0;
        }
        else if (switchOneState == LOW && switchTwoState == LOW && switchStateThree == HIGH)
        {
          select = 1;
        }
        else if (switchOneState == LOW && switchTwoState == HIGH && switchStateThree == LOW)
        {
          select = 2;
        }
        else if (switchOneState == LOW && switchTwoState == HIGH && switchStateThree == HIGH)
        {
          select = 3;
        }
        else if (switchOneState == HIGH && switchTwoState == LOW && switchStateThree == LOW)
        {
          select = 4;
        }
        else if (switchOneState == HIGH && switchTwoState == LOW && switchStateThree == HIGH)
        {
          select = 5;
        }
        else if (switchOneState == HIGH && switchTwoState == HIGH && switchStateThree == LOW)
        {
          select = 6;
        }
        else if (switchOneState == HIGH && switchTwoState == HIGH && switchStateThree == HIGH)
        {
          select = 7;
        }
    
        switch(select)
        {
          case 0:
          //45F - 45%
          temp_data[0] = 0x44;
          temp_data[1] = 0x49;
          humid_data[0] = 0x33;
          humid_data[1] = 0x73;
          
          Serial2.write(temp_data[0]);
          Serial2.write(temp_data[1]);
          Serial2.write(humid_data[0]);
          Serial2.write(humid_data[1]);
          Serial.print(select);
          break;
    
          case 1:

          //50F - 48%
          temp_data[0] = 0x93;
          temp_data[1] = 0x4D;
          humid_data[0] = 0xE1;
          humid_data[1] = 0x7A;
          
          Serial2.write(temp_data[0]);
          Serial2.write(temp_data[1]);
          Serial2.write(humid_data[0]);
          Serial2.write(humid_data[1]);
          Serial.print(select);
          break;
    
          case 2:
          //55F - 51%%
          temp_data[0] = 0xE3;
          temp_data[1] = 0x51;
          humid_data[0] = 0x8F;
          humid_data[1] = 0x82;
          
          Serial2.write(temp_data[0]);
          Serial2.write(temp_data[1]);
          Serial2.write(humid_data[0]);
          Serial2.write(humid_data[1]);
          Serial.print(select);
          break;
    
          case 3:
          //60F - 54%
          temp_data[0] = 0x32;
          temp_data[1] = 0x56;
          humid_data[0] = 0x3D;
          humid_data[1] = 0x8A;
    
          Serial2.write(temp_data[0]);
          Serial2.write(temp_data[1]);
          Serial2.write(humid_data[0]);
          Serial2.write(humid_data[1]);
          Serial.print(select);
          break;

          case 4:
          //65F - 57%
          temp_data[0] = 0x81;
          temp_data[1] = 0x5A;
          humid_data[0] = 0xEC;
          humid_data[1] = 0x91;
    
          Serial2.write(temp_data[0]);
          Serial2.write(temp_data[1]);
          Serial2.write(humid_data[0]);
          Serial2.write(humid_data[1]);
          Serial.print(select);
          break;

          case 5:
          //70F - 60%
          temp_data[0] = 0xD1;
          temp_data[1] = 0x5E;
          humid_data[0] = 0x9A;
          humid_data[1] = 0x99;
    
          Serial2.write(temp_data[0]);
          Serial2.write(temp_data[1]);
          Serial2.write(humid_data[0]);
          Serial2.write(humid_data[1]);
          Serial.print(select);
          break;

          case 6:
          //75F - 63%
          temp_data[0] = 0x20;
          temp_data[1] = 0x63;
          humid_data[0] = 0x48;
          humid_data[1] = 0xA1;
    
          Serial2.write(temp_data[0]);
          Serial2.write(temp_data[1]);
          Serial2.write(humid_data[0]);
          Serial2.write(humid_data[1]);
          Serial.print(select);
          break;

          case 7:
          //80F - 66%
          temp_data[0] = 0x6F;
          temp_data[1] = 0x67;
          humid_data[0] = 0xF6;
          humid_data[1] = 0xA8;
    
          Serial2.write(temp_data[0]);
          Serial2.write(temp_data[1]);
          Serial2.write(humid_data[0]);
          Serial2.write(humid_data[1]);
          Serial.print(select);
          break;
    
          default:
          Serial.print("Error");
          break;
        }
        break;
  
        default:
        Serial.print(input);
        Serial.println(" is not a valid input");
        break;
      }
    }
}
