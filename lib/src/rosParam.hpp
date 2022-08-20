
//FREQ to control 1000/val

#define COMMAND_RATE 1 //every 1 sec
#define VEL_PUBLISH_RATE 10 //every 10 hz
#define IMU_PUBLISH_RATE 10 // 10 hz
#define MOTOR_STATE_PUBLISH_RATE 2 // 2 hz
#define DEBUG_RATE 1 
float baseWidth = 0.55;
float wheelDiameter = 0.13; //unit in metres

int MAX_RPM =300; // จำนวนรอบสูงสุดที่ยอมรับได้จากการคำนวณ
float WHELL_DIAMETER = 0.13; // unit in m.
float ROBOT_WIDTH = 0.55; //uint in m 