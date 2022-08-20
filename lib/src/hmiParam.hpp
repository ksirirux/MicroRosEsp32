

//HMI
#define HMI_READ_INTERVAL 300


#define HMI_HOLD_ADD 50
#define HMI_NUM_HOLD 2


//COILs 
/*
POWER Address 10
Manual Address 11
Reset Coil 12


*/
#define HMI_POWER_ADD 10
#define HMI_NUM_COIL 3




#define HMI_REG_VEL_X 60
#define HMI_REG_ANGLE_Z 61
#define HMI_REG_LEFT_TEMP 62
#define HMI_REG_RIGHT_TEMP 63
#define HMI_REG_DEVICE_TEMP 64

#define HMI_REG_VOL_BAT 65
/*
ฺBit state setting จะไม่สามารถ Read ค่าในตำแหน่งนี่น
แต่ bit state switch จะ Read ค่าต่ำแหน่งนั้นมาด้วย

*/
#define ADD_ERROR_CODE 100
#define ADD_ERROR_STATE 101

#define ADD_DIRECTION_CONTROL 101
#define NUM_DIRECTION_CONTROL 9