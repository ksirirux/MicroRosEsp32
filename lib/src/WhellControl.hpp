#include <Arduino.h>

#include <rs485Param.hpp>
#include <ModbusMaster.h>

ModbusMaster mbMotor;
struct MOTOR_STATE
{
    float battVol;
    float leftTemp;
    float rightTemp;
    float deviceTemp;
    bool leftState;
    bool rightState;
    uint16_t leftError;
    uint16_t rightError;

};

struct MOTOR_RPM
{
    int rpmRight;
    int rpmLeft;
} ;

class WheelControl
{
private:
    int8_t deviceId;
    int8_t leftDir;
    int8_t rightDir;
    bool motorAvaliable = false;
    MOTOR_STATE motorState;
    MOTOR_RPM motorRpm;

public:
    WheelControl(int8_t _diviceId)
    {
        this->deviceId = _diviceId;
    }

    void Init(int8_t _leftDir, int8_t _rightDir)
    {

        this->leftDir = _leftDir;
        this->rightDir = _rightDir;

        
        postTransmission();
        delay(100);
        Serial.print(" Device Id =>");
        Serial.printf("%d %d %d \n", this->deviceId, this->leftDir, this->rightDir);

        mbMotor.begin(this->deviceId, Serial1);
        mbMotor.preTransmission(preTransmission);
        mbMotor.postTransmission(postTransmission);
    }
    static void preTransmission()
    {
        digitalWrite(RS485_EN_PIN, HIGH);
    }
    static void postTransmission()
    {
        digitalWrite(RS485_EN_PIN, LOW);
    }

    bool InitMotor(bool testMotor)
    {
        bool res;
        // Set Vel mode
        res = sendControlCommand(CMD_SET_VELMODE, ADD_MOTORMODE, VEL_MODE);
        // Set Accelation Time 500 ms
        res = sendControlCommand(CMD_SET_ACCLEFT, ADD_ACCLEFT, 500);
        res = sendControlCommand(CMD_SET_ACCRIGHT, ADD_ACCRIGHT, 500);
        // Set Deaccelation time 500 ms
        res = sendControlCommand(CMD_SET_DEACCLEFT, ADD_DEACCLEFT, 500);
        res = sendControlCommand(CMD_SET_DEACCRIGHT, ADD_DEACCRIGHT, 500);
        // Enable Motor
        res = sendControlCommand(CMD_ENABLE_MOTOR, ADD_ENABLE_MOTOR, ENABLE_MOTOR);
        if (testMotor)
        {

            motorVelControl(ADD_VEL_RIGHT, 100);
            motorVelControl(ADD_VEL_LEFT, 100);
            delay(5000);
        }
        return res ? false : true;
    }

    void stopMotor()
    {
        mbMotor.writeSingleRegister(ADD_ENABLE_MOTOR, DISABLE_MOTOR);
    }

    void enableMotor()
    {
        motorAvaliable = sendControlCommand(CMD_ENABLE_MOTOR, ADD_ENABLE_MOTOR, ENABLE_MOTOR);
    }

    bool sendControlCommand(char *cmd_name, uint16_t add, int cmd)
    {
        Serial.println(cmd);
        Serial.println(add);
        bool res = mbMotor.writeSingleRegister(add, cmd);
        delay(5);
        Serial.printf(" %s : %s \n ", cmd_name, res == 0 ? "Suceess" : "Fail");
        return res;
    }

    bool motorVelControl(uint16_t add, int rpm)
    {
        int _rpm = 0;
        if (add == ADD_VEL_LEFT)
        {
            _rpm = leftDir ? rpm : -rpm;
        }
        else if (add == ADD_VEL_RIGHT)
        {
            _rpm = rightDir ? rpm : -rpm;
        }

        bool res = mbMotor.writeSingleRegister(add, (uint16_t)_rpm);
        return res;
    }
    uint16_t readStateUnsigned(uint16_t add)
    {
        uint8_t res = mbMotor.readHoldingRegisters(add, 1);
        // uint16_t result = 0x0000;
        long start = millis();
        while (res != mbMotor.ku8MBSuccess && millis() - start < 100)
        {
            Serial.print(".");
        }
        if (res == mbMotor.ku8MBSuccess)
        {
            uint16_t data = mbMotor.getResponseBuffer(0);
            return data;
        }
        else
        {
            Serial.println("ERROR! read State");
            return false;
        }
    }

    int16_t readStateSigned(uint16_t add)
    {
        uint8_t res = mbMotor.readHoldingRegisters(add, 1);
        // uint16_t result = 0x0000;
        long start = millis();
        while (res != mbMotor.ku8MBSuccess && millis() - start < 100)
        {
            Serial.print(".");
        }
        if (res == mbMotor.ku8MBSuccess)
        {
            int16_t data = mbMotor.getResponseBuffer(0);
            return data;
        }
        else
        {
            Serial.println("ERROR! read State");
            return false;
        }
    }

    void readMotorState()
    {
        uint16_t motorTemp = readStateUnsigned(ADD_STATE_MOTOR_TEMP);
        this->motorState.leftTemp = highByte(motorTemp);
        this->motorState.rightTemp = lowByte(motorTemp);
        this->motorState.deviceTemp = readStateUnsigned(ADD_STATE_DRIVER_TEMP);

        // Motor Voltage
        uint16_t batV = readStateUnsigned(ADD_STATE_BUS_VOL);
        this->motorState.battVol = batV / 100.0;
        Serial.printf("Motor temp Left : %1.2f  Right : %1.2f  Batt %1.2f v. \n", motorState.leftTemp, motorState.rightTemp, motorState.battVol);

        // Motor State
        int8_t state = readStateUnsigned(ADD_STATE_MOTOR_STATE);
        this->motorState.leftState = highByte(state);
        this->motorState.rightState = lowByte(state);
        // Error State
        this->motorState.leftError = readStateUnsigned(ADD_STATE_LEFT_ERROR);
        if (this->motorState.leftError != NO_ERROR)
        {
            Serial.print(" MOTOR LEFT ERROR => ");
            decodeMotorError(this->motorState.leftError);
        }
        this->motorState.rightError = readStateUnsigned(ADD_STATE_RIGHT_ERROR);
        if (this->motorState.rightError != NO_ERROR)
        {
            Serial.print(" MOTOR RIGHT ERROR => ");
            decodeMotorError(this->motorState.rightError);
        }

        
    }
    void readMotorRpm(){
        this->motorRpm.rpmLeft = readStateSigned(ADD_ACT_VEL_LEFT) / 10;
        this->motorRpm.rpmRight = readStateSigned(ADD_ACT_VEL_RIGHT) / 10;
    }

    MOTOR_STATE getMotorState(){
        this->readMotorState();
        return this->motorState;
    }

    MOTOR_RPM getMotorRpm(){
        this->readMotorRpm();
        return this->motorRpm;
    }
    void decodeMotorError(uint16_t code)
    {

        switch (code)
        {
        case OVER_VOLTAGE:
            Serial.println(CODE_OVER_CURRENT);
            break;
        case UNDER_VOLTAGE:
        {
            Serial.println(CODE_UNDER_VOLTAGE);
        }
        break;
        case OVER_CURRENT:
        {
            Serial.println("Over Current Error ");
        }
        break;
        case OVER_LOAD:
            Serial.println(CODE_OVER_LOAD);
            break;

        case CURRENT_FOLLOW_ERROR:
            Serial.println("Current Following Error ");
            break;
        case POSITION_FOLLOW_ERROR:
            Serial.println("Position Following Error ");
            break;
        case VELOCITY_FOLLOW_ERROR:
            Serial.println("Velocity Following Error ");
            break;
        case REFERENCE_VOL_ERROR:
            Serial.println("Reference Voltage Error ");
            break;
        case EEPROM_ERROR:
            Serial.println("EEPROM Error ");
            break;
        case HALL_ERROR:
            Serial.println("HALL Error ");
            break;
        case MOTOR_TEMO_OVER:
            Serial.println("Motor Temp Over ");
            break;
        }

        /*char word[]= "ERROR 12345678910";


        for (size_t i = 0; i < sizeof(word)-1 ; i++)
        {
           mbHMI.writeSingleRegister(ADD_ERROR_STATE+i,word[i]);
        }*/
    }

    
    enum moveD
    {
        OFF,
        LEFTDOWN,
        DOWN,
        RIGHTDOWN,
        LEFT,
        STOP,
        RIGHT,
        LEFTUP,
        UP,
        RIGHTUP
    };


    bool moveMotor(int leftRpm,int rightRpm){
        bool resL = mbMotor.writeSingleRegister(ADD_VEL_LEFT,(uint16_t)leftRpm);
        bool resR = mbMotor.writeSingleRegister(ADD_VEL_RIGHT,(uint16_t)rightRpm);
        if(resL && resR) return true;
        else return false;

    }
};
