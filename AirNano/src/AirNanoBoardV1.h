#pragma once
#include <Arduino.h>

#define LedRed A3
#define LedYellow A2
#define LedGreen A1

#define pushButton A0
#define RotaryClock 3   // interrupt 1
#define RotaryData 4
#define RotaryEncoderButton 9
#define DIRECTION_CW 0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

void SetupBoard(int8_t ledValues);
void encoderChange();
void SetLed(int8_t ledValues);

void SetRed(bool on);
void SetYellow(bool on);
void SetGreen(bool on);

void BoardLoop();
bool SelectButtonPressed();
bool RotaryButtonPressed();
uint8_t RotaryValue();