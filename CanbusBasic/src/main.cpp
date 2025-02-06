#include <Arduino.h>
#include <EventManager.h>
#include <CanSensor.h>
#include <GaugeDisplay.h>

void setup();
void loop();
void handleEvents();
void raiseEvents();

void readAltitude(int event, int param);

// Define the canbus object
Canbus::CanSensor canSensor;

// Define the gauge type and pins
Gauge::AutoGauge gauge(Gauge::X27_168, 5,6,7,8);
int stepsPerRev = gauge.StepsPerRevolution(Gauge::X27_168);
int startMillis = millis();

unsigned long lastTimerEvent = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // find the id or define it here
  // Now 0x01 is the id for this item
  canSensor.begin(0x01, Canbus::Clock16MHZ);

  // Initialize the gauge with a speed of 100
  gauge.begin();

  if(!canSensor.eventManager.addListener( EventManager::EventType::kEventRequestData, readAltitude))
  {
    Serial.println("Failed to add readBarometer listener");
  }

  // use noise to seed the random number generator
  randomSeed(analogRead(0));
}

void readAltitude(int event, int param){
  // read the barometer
  // send the data
  float data = 0.0;
  canSensor.sendSensorFloat(Canbus::Altimeter, data);
}

void loop() {
  // put your main code here, to run repeatedly
  unsigned long currentMillis = millis();
  handleEvents();
  raiseEvents();

  // Output data every second and when data is requested
  if(currentMillis - lastTimerEvent > 1000)
  {
      lastTimerEvent = currentMillis;

      canSensor.sendSensorFloat(Canbus::Altimeter, currentMillis / 7);

      digitalWrite(LED_BUILTIN, HIGH);   
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);   
  }

    // print a random number from 10 to 19
  int randNumber = random(0, stepsPerRev * 2 - stepsPerRev);
  gauge.setTargetValue(randNumber);
}

void handleEvents()
{
  canSensor.eventManager.processAllEvents();
  gauge.eventManager.processAllEvents();
}

void raiseEvents()
{
  canSensor.loop();
  gauge.loop();
}