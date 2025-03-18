#include <Arduino.h>
#include <AirNanoBoardV1.h>

// include library for LCD
// Using wire library for I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// put declarations here:
bool on = true;
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  // put your setup code here, to run once:
  SetupBoard(0x00000001);

  lcd.init();        // initialize the lcd
  lcd.backlight();   // turn on the backlight

  lcd.setCursor(0, 0);
  // 16 positions per line
  //         0123456789ABCDEF
  lcd.print("AirNanoBoardV1  ");
  lcd.setCursor(0, 1);
  lcd.print("(c) 2025 AirNano");
  delay(2000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:

  BoardLoop();
  if (SelectButtonPressed()) {    
    SetGreen(true);
  }
  else {
    SetGreen(false);
  }

  if (RotaryButtonPressed()) {    
    SetRed(true);
  }
  else {
    SetRed(false);
  }

  lcd.setCursor(0, 1);

  //         0123456789ABCDEF
  lcd.print("Rotary:");
  lcd.print(RotaryValue());
  lcd.print("  ");
}

