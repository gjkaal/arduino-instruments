#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>
#include <EventManager.h>
#include "CanSensor.h"

namespace Canbus {

    const int SPI_CS_PIN = 10;
    MCP_CAN CAN(SPI_CS_PIN); // Set CS pin

    void sendInteger(int id, unsigned char messageType, unsigned char dataType, int data)
    {
        byte response;
        unsigned char msg[4] = {
            messageType,
            dataType,
            (uint8_t)(data >> 8),
            (uint8_t)data
        };
        response = CAN.sendMsgBuf(id, 0, 4, msg);
        if(response != CAN_OK)
        {
            Serial.println("Failed to send integer");
        }
        Serial.println("Type : " + messageType);
        Serial.println("Value: " + String(data));
        Serial.println("Response: " + String(response));
    }

    CanSensor::CanSensor() {       
    }

    bool CanSensor::begin(uint8_t identifier, byte clockSpeed)
    {
        int retryCount = 0;
        this->sensorId = 0x100 | (identifier & 0x0F);
        Serial.println("Starting CAN with id: " + String(sensorId));

        // Initialize the CAN bus using the correct clock (16 / 8)
        while (CAN_OK != CAN.begin(MCP_ANY, CAN_100KBPS, clockSpeed) && retryCount<10) { 
            retryCount++;
            Serial.println("CAN BUS Shield init fail");
            Serial.println(" Retry " + String(retryCount));
            delay(300);
        }

        // Change to normal mode to allow messages to be transmitted
        CAN.setMode(MCP_NORMAL);

        if(retryCount>=10)
        {
            Serial.println("Starting CAN failed!");
            initFailed = true;
        }
        else
        {            
            Serial.println("Starting CAN succeeded at 100KBPS");
            unsigned char data[5] = { 0xAA, 'H', 'E', 'L', 'O'};
            // send data:  id = 0x00, standrad frame, data len = 4, data: data buf
            CAN.sendMsgBuf(sensorId, 0, 5, data);
        }
        delay(100);
        return !initFailed;
    }

    bool CanSensor::initialized() 
    {
        return !initFailed;
    }

    bool CanSensor::errorOccured()
    {
        return error;
    }

    void CanSensor::sendSensorInteger(unsigned char dataType, int data)
    {
        sendInteger(sensorId, 'I', dataType, data);
    }    

/*
Float has size of 4 bytes. Therefore you need 4 bytes in your array 
to store each float. If you want to pass 4 floats you need 16 bytes

You can cast a pointer to float to a byte array:

   float a = 12.34
   byte dataArray[4] = {
      ((uint8_t*)&a)[0],
      ((uint8_t*)&a)[1],
      ((uint8_t*)&a)[2],
      ((uint8_t*)&a)[3]
   };

and make opposite operation on the receiver side:

    float a;
    ((uint8_t*)&a)[0] = dataArray[0];
    ((uint8_t*)&a)[1] = dataArray[1];
    ((uint8_t*)&a)[2] = dataArray[2];
    ((uint8_t*)&a)[3] = dataArray[3];
*/

    void CanSensor::sendSensorFloat(unsigned char dataType, float data)
    {
        byte response;
        unsigned char msg[6] = {
            'F',
            dataType,
            ((uint8_t*)&data)[0],
            ((uint8_t*)&data)[1],
            ((uint8_t*)&data)[2],
            ((uint8_t*)&data)[3]    
        };

        // Normal message, 6 bytes
        response = CAN.sendMsgBuf(sensorId, 0, 6, msg);
        if(response != CAN_OK)
        {
            Serial.println("Failed with code:" + String(response));
            Serial.println("Verify connections and can bus termination.");

        }
        // CAN_GETTXBFTIMEOUT = 6
        // CAN_SENDMSGTIMEOUT = 7
    }

    void CanSensor::sendFailureCode(int code)
    {
        sendInteger(sensorId, 0xEE, 0, code);
    }

    void CanSensor::loop() {
        // prevent the loop from running if the sensor was not initialized
        if(initFailed)
        {
            return;
        }

        unsigned long currentMillis = millis();
        // Do something every interval
        if(currentMillis - lastTimerEvent > 100)
        {
            lastTimerEvent = currentMillis;

            unsigned char msg[8];
            unsigned char len = 8;
            unsigned long canId = 0;            
            unsigned long rxId;
            bool extended = false;
            bool remoteRequest = false;

            // Do your stuff here
            int frameReceived = CAN.checkReceive();
            // Result value is 3 or 4, CAN_MSGAVAIL or CAN_NOMSG
            // Both values are positive
            if (frameReceived == CAN_MSGAVAIL) {

                CAN.readMsgBuf(&rxId, &len, msg);

                if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
                {
                    extended = true;
                    canId = (rxId & 0x1FFFFFFF);
                }
                else
                {
                    extended = false;
                    canId = rxId;
                }

                if(extended == false && canId != sensorId)
                {
                    if((rxId & 0x40000000) == 0x40000000)
                    {
                        remoteRequest = true;
                    }
                    else
                    {
                        remoteRequest = false;
                    }

                    if (remoteRequest) {
                        Serial.println("Remote request");
                        if(!eventManager.queueEvent(EventManager::EventType::kEventRequestData, 0)){
                            Serial.println("Failed to queue Remote request");
                        };
                    } 
                    else 
                    if (len > 0) 
                    {
                        Serial.print("Data: ");
                        for (int i = 0; i < len; i++) {
                            Serial.print(msg[i], HEX);
                            Serial.print(" ");
                        }
                        Serial.println();
                    }           
                    else{
                        Serial.println("Length is 0");
                    }                    
                    delay(100);
                }            
            }
        }
    }
}