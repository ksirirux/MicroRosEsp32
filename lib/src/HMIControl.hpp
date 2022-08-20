


#include <Arduino.h>
#include <ModbusMaster.h>

#include <rs485Param.hpp>
#include <hmiParam.hpp>


ModbusMaster mbScreen;
struct HMIPowerState
{
    bool powerState;
    bool manualState;
    bool resetState;
};

struct ManualSpeedControl
{
    float manual_linear_x;
    float manual_angular_z;
    int direction;
};



class HMIControl
{
private:
    int8_t deviceId;

    bool hmiDebug = false;
    bool hmiState = false;
    HMIPowerState powerState;
    ManualSpeedControl manualSpeed;
    String direction[10] = {"OFF", "LEFT DOWN", "DOWN", "RIGHT DOWN", "LEFT", "STOP", "RIGHT", "LEFT UP", "UP", "RIGHT UP"};


    

    
public:
    HMIControl(int8_t _deviceId){
        this->deviceId = _deviceId;

    }
    void Init(bool _debug){

        this->hmiDebug = _debug;
        Serial.println("HMI Screen initialize");
        Serial.printf("HMI Device id => %d \n",this->deviceId);
    
       
        postTransmission();
        delay(100);
        
        mbScreen.begin(this->deviceId, Serial1);
        mbScreen.preTransmission(preTransmission);
        mbScreen.postTransmission(postTransmission);

    }

    static void preTransmission()
    {
        digitalWrite(RS485_EN_PIN, HIGH);
    }
    static void postTransmission()
    {
        digitalWrite(RS485_EN_PIN, LOW);
    }

    bool readPowerState(){
        uint8_t result;
        result = mbScreen.readCoils(HMI_POWER_ADD,HMI_NUM_COIL);
        if (result = mbScreen.ku8MBSuccess){
            uint8_t data[HMI_NUM_COIL];
            uint16_t coil = mbScreen.getResponseBuffer(0);
            int pos = HMI_NUM_COIL;
            while (pos >=0)
            {
                if ((coil >>pos) & 1)
                    data[pos] =1;
                else
                    data[pos] = 0;
                pos--;
            }
            this->powerState.powerState = data[0];
            this->powerState.manualState = data[1];
            this->powerState.resetState = data[2];
            this->hmiDebug ? Serial.printf("POWER STATE : %s \n", this->powerState.powerState ? "ON" : "OFF") : NULL;
            this->hmiDebug ? Serial.printf("Manual : %s \n", this->powerState.manualState ? "ON" : "OFF") : NULL;

            this->hmiState = true;
        }else{
            Serial.println("HMI READ POWER STATE ERROR");
            this->hmiState = false;
        }   
        return this->hmiState;    
    }
    HMIPowerState getHMIPowerState(){
        return this->powerState;
    }
    bool readControlCmd(){
        //Read Speed
        float holdData[HMI_HOLD_ADD];
        uint8_t result;
        result = mbScreen.readHoldingRegisters(HMI_HOLD_ADD,HMI_NUM_HOLD);
        if (result == mbScreen.ku8MBSuccess){
            for (size_t i = 0; i < HMI_NUM_HOLD; i++)
            {
                holdData[i] = mbScreen.getResponseBuffer(i)/10.0; //rpm

            }
            this->manualSpeed.manual_linear_x = holdData[0];
            this->manualSpeed.manual_angular_z = holdData[1];

            this->hmiState = true;
            
        }else{
            Serial.println("HMI READ CONTROL COMMAND ERROR");
            this->hmiState = false;
        } 
            
        
        //read Direction
        result = mbScreen.readCoils(ADD_DIRECTION_CONTROL,NUM_DIRECTION_CONTROL);
        if (result == mbScreen.ku8MBSuccess){
            uint16_t coil = mbScreen.getResponseBuffer(0);
            int pos = NUM_DIRECTION_CONTROL;
            while (pos >= 0)
            {   
                if((coil >> pos )& 1){
                    this->manualSpeed.direction = pos+1;
                    break;
                }
                pos--;
            }
            Serial.printf("Robot go => %s VEL-X => %1.2f  ANGULAR-Z => %1.2f",direction[this->manualSpeed.direction],this->manualSpeed.manual_linear_x,this->manualSpeed.manual_angular_z);
            this->hmiState = true;
            
        }else{
            Serial.println("READ DIRECTION ERROR");
            this->hmiState = false;
        }
            
        return this->hmiState;

    }

    ManualSpeedControl getManualControl(){
        return this->manualSpeed;
    }

    
    
};

