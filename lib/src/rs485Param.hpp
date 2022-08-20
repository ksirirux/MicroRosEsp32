




#define DEBUG_BAUD_RATE 57600
#define RS485_BAUD_RATE 57600

#define RS485_RXD2_PIN 25
#define RS485_TXD2_PIN 32
#define RS485_EN_PIN 33
#define RS485_DELAY_TIME 50
#define LED_PIN 02


#define highByte(x) ( (x) >> (8) ) // keep upper 8 bits
#define lowByte(x) ( (x) & (0xff) ) // keep lower 8 bits


#define PROJECT_NAME "ROBOT_CONTROL"




//MOTOR CONTROL
#define MOTOR_CONTROL_INTERVAL 100


//MOTOR ADDRESS
#define CMD_SET_VELMODE "Set Velocity mode"
#define ADD_MOTORMODE 0x200D
#define VEL_MODE 3

#define CMD_SET_ACCLEFT "Set Accelation Left whell"
#define ADD_ACCLEFT 0x2080

#define CMD_SET_ACCRIGHT "Set Accelation Right whell"
#define ADD_ACCRIGHT 0x2081

#define CMD_SET_DEACCLEFT "Set Deaccelation Left whell"
#define ADD_DEACCLEFT 0x2082

#define CMD_SET_DEACCRIGHT "Set Deaccelation Right whell"
#define ADD_DEACCRIGHT 0x2083

#define CMD_ENABLE_MOTOR "Enable Motor"
#define ADD_ENABLE_MOTOR 0x200E
#define ENABLE_MOTOR 8
#define DISABLE_MOTOR 7


#define ADD_VEL_LEFT 0x2088
#define ADD_VEL_RIGHT 0x2089


#define ROBOT_WHEEL_SIZE 0


//READ MOTOR STATE
#define MOTOR_READ_STATE_INTERVAL 1000
#define ADD_STATE_BUS_VOL 0x20A1
#define ADD_STATE_MOTOR_TEMP 0x20A4
#define ADD_STATE_DRIVER_TEMP 0x20B0
#define ADD_STATE_VEL_LEFT 0x20A8
#define ADD_STATE_VEL_RIGHT 0x20AC
#define ADD_STATE_TOR_LEFT 0x20AD
#define ADD_STATE_TOR_RIGHT 0x20AE
#define ADD_STATE_MOTOR_STATE 0x20A2
#define ADD_STATE_LEFT_ERROR 0x20A5
#define ADD_STATE_RIGHT_ERROR 0x20A6


#define ADD_ACT_VEL_LEFT 0x20AB //0.1 rpm
#define ADD_ACT_VEL_RIGHT 0x20AC //0.1 rpm


//CONTROL WORLD
#define ADD_ERROR_CMD   0x200E
#define EMERGENCY_STOP  0x05
#define CLEAR_FAULT     0x06
#define STOP_CONTROL    0x07
#define ENABLE_CONTROL  0x08
#define START_SYNC      0x10
#define START_LEFT      0x11
#define START_RIGHT     0x12    


//ERROR CODE
#define NO_ERROR                0x0000
#define OVER_VOLTAGE            0x0001
#define UNDER_VOLTAGE           0x0002
#define OVER_CURRENT            0x0004
#define OVER_LOAD               0x0008
#define CURRENT_FOLLOW_ERROR    0x0010
#define POSITION_FOLLOW_ERROR   0x0020
#define VELOCITY_FOLLOW_ERROR   0x0040
#define REFERENCE_VOL_ERROR     0x0080
#define EEPROM_ERROR            0x0100
#define HALL_ERROR              0x0200
#define MOTOR_TEMO_OVER         0x0400


//ERROR STRING

#define CODE_OVER_VOLTAGE            "Over Voltage"
#define CODE_UNDER_VOLTAGE           "Under Voltage"
#define CODE_OVER_CURRENT            "Over Current"
#define CODE_OVER_LOAD                "Over Load"
#define CODE_CURRENT_FOLLOW_ERROR    "Current Following Error "
#define CODE_POSITION_FOLLOW_ERROR   "Position Following Error"
#define CODE_VELOCITY_FOLLOW_ERROR   "Velocity Following Error"
#define CODE_REFERENCE_VOL_ERROR     "Reference Voltage Error"
#define CODE_EEPROM_ERROR            "EEPROM Error"
#define CODE_HALL_ERROR              "HALL Error"
#define CODE_MOTOR_TEMO_OVER         "Motor Temperature Over"

    



//#define READ_CMD 0x03
//#define SEND_CMD 0x06
