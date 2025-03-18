#include <AirNanoBoardV1.h>
#include <ezButton.h>

/// @brief  The current state of the LEDs
/// @details 0b00000010 means that the yellow LED is on
int8_t LedValues = 0b00000010;

ezButton selectButton(pushButton);

ezButton rotaryButton(RotaryEncoderButton);
uint8_t counter = 0;
int8_t direction = DIRECTION_CW;
long lastTime = 0;

void SetupBoard(int8_t ledValues) {
    Serial.begin(9600);

    pinMode(LedGreen, OUTPUT);
    pinMode(LedYellow, OUTPUT);
    pinMode(LedRed, OUTPUT);
    SetLed(0b00000111);
    delay(300);
    SetLed(0b00000000);
    delay(300);
    LedValues = ledValues;
    SetLed(ledValues);

    selectButton.setDebounceTime(25);          // set debounce time to 50 milliseconds
    rotaryButton.setDebounceTime(25);          // set debounce time to 50 milliseconds
   
    pinMode(RotaryData, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RotaryClock), encoderChange, RISING);

    Serial.println("Board setup done");
}

bool SelectButtonPressed() {
    return selectButton.getState() != 0;
}

bool RotaryButtonPressed() {
    return rotaryButton.getState() == 0;
}

uint8_t RotaryValue() {
    return counter;
}

void encoderChange() {
    if ((millis() - lastTime) < 20)  // debounce time is 20ms
      return;
  
    if (digitalRead(RotaryData) == LOW) {
      // the encoder is rotating in counter-clockwise direction => decrease the counter
      counter--;
      direction = DIRECTION_CCW;
    } else {
      // the encoder is rotating in clockwise direction => increase the counter
      counter++;
      direction = DIRECTION_CW;
    }
  
    lastTime = millis();
  }

void BoardLoop() {
    selectButton.loop();    
    rotaryButton.loop();    
}

void SetLed(int8_t ledValues) 
{
    LedValues = ledValues;
    digitalWrite(LedGreen, (LedValues & 0b00000001) ? HIGH : LOW);
    digitalWrite(LedYellow, (LedValues & 0b00000010) ? HIGH : LOW);
    digitalWrite(LedRed, (LedValues & 0b00000100) ? HIGH : LOW);
}

void SetRed(bool on) {
    if (on) {
        LedValues |= 0b00000100;
    } else {
        LedValues &= 0b11111011;
    }
    SetLed(LedValues);
}

void SetYellow(bool on) {
    if (on) {
        LedValues |= 0b00000010;
    } else {
        LedValues &= 0b11111101;
    }
    SetLed(LedValues);
}

void SetGreen(bool on) {
    if (on) {
        LedValues |= 0b00000001;
    } else {
        LedValues &= 0b11111110;
    }
    SetLed(LedValues);
}