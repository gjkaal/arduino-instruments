#pragma once
#include <EventManager.h>

// CanSensor class is used as a wrapper for the Canbus library
// It is used to send sensor data over the Canbus network
// Each sensor has a unique identifier
namespace Canbus {
    class CanSensor {   
        public:
            CanSensor();
            bool begin(uint8_t identifier, byte clockSpeed);
            bool initialized();
            bool errorOccured();
            void loop();
            void sendSensorInteger(unsigned char dataType, int data);
            void sendSensorFloat(unsigned char dataType, float data);
            void sendFailureCode(int code);
            EventManager eventManager;
        private:
            // Identifier for the sensor
            // It is typically set using DIP switches
            // on at compile time
            unsigned long sensorId;
            bool initFailed = false;
            bool error = false;
            unsigned long lastTimerEvent = 0;

    };

    enum SensorType{
        Altimeter = 0x01,
    };

    enum ClockSpeed{
        Clock20MHZ = 0x00,
        Clock16MHZ = 0x01,
        Clock8MHZ = 0x02,
    };
}