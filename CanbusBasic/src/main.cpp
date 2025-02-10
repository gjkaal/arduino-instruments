#include "main.h"

// Define the gauge type and pins
//Gauge::AutoGauge gauge(Gauge::X27_168, 5,6,7,8);
//int stepsPerRev = gauge.StepsPerRevolution(Gauge::X27_168);
//int startMillis = millis();

void remoteRequest(int event, int param)
{
  // Remote request for data
}

void dataLost(int event, int param)
{
  // Data lost in the fifo stack of the canbus
  Serial.println("Data lost");
}

void dataReceived(int event, int param)
{
  // Data available in the fifo stack of the canbus
  // Read the data and process it
  while(param > 0)
  {
    unsigned char* data = canSensor.readMessage();
    if(data != NULL)
    {
      uint8_t dataType = data[1];
      // Process the data
      switch (data[0])
      {
        case Canbus::kMessageTypeInteger:
          int value = (data[2] << 8) | data[3];
          switch (dataType)
          {
            case SENSOR_ALTIMETER:
            /* code */
            break;          
          default:
            break;
          }
          break;
        case Canbus::kMessageTypeFloat:
          /* code */
          break;
        case Canbus::kMessageTypeFailure:
          /* code */
          break;
        case Canbus::kMessageTypeString:
          /* code */
          break;
        default:
          break;
      }

      Serial.println("Type: " + String(data[0]));
      for(int i = 1; i < 8; i++)
      {
        unsigned char c = data[i];
        Serial.print(" ");
        Serial.print(c, HEX);
      }
      Serial.println();
      Serial.println();
    }
    else
    {
      // No data available
      Serial.println("No data available");
    }
    param--;
  }  
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // find the id or define it here
  // Now 0x01 is the id for this item

  // On nano, clock is 8Mhz
  canSensor.begin(0x02, Canbus::Clock8MHZ);

  // Initialize the gauge with a speed of 100
  //gauge.begin();

  canSensor.eventManager.addListener(EventManager::EventType::kEventRequestData, remoteRequest);
  canSensor.eventManager.addListener(EventManager::EventType::kEventCanDataReceived, dataReceived);
  canSensor.eventManager.addListener(EventManager::EventType::kEventCanDataLost, dataLost);
  
  // use noise to seed the random number generator
  randomSeed(analogRead(0));
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

      canSensor.sendSensorFloat(SENSOR_ALTIMETER, currentMillis / 7);

      digitalWrite(LED_BUILTIN, HIGH);   
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);   
  }

  // print a random number from 10 to 19
  //int randNumber = random(0, stepsPerRev * 2 - stepsPerRev);
  //gauge.setTargetValue(randNumber);
}

void handleEvents()
{
  canSensor.eventManager.processAllEvents();
  //gauge.eventManager.processAllEvents();
}

void raiseEvents()
{
  canSensor.loop();
  //gauge.loop();
}