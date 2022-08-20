#include <Arduino.h>
#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/int64.h>


//Robot control
#include "WhellControl.hpp"
#include "HMIControl.hpp"

#include <rosParam.hpp>

#if defined(MICRO_ROS_TRANSPORT_ARDUINO_WIFI) 

#endif


//Motor
int8_t rs485MotorId = 10;
int8_t leftMotorDir = 1; // change 0 or 1 to direction motor
int8_t rightMotorDir = 1;
WheelControl MOTOR(rs485MotorId);
bool  motorState = false;
//HMI
int8_t hmiDeviceId = 4;
HMIControl HMI(hmiDeviceId);
HMIPowerState powerState;
ManualSpeedControl manualSpeedControl;


rcl_publisher_t publisher;
rcl_publisher_t publisher2;
std_msgs__msg__Int32 msg;
std_msgs__msg__Int64 num;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;





#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}




//Time 

long beforeReadMotorState = 0;
long beforeReadHmiState = 0;
long beforeMotorControl = 0;

//ros speed
double ros_linear_vel_x = 0.0;
double ros_linear_vel_y = 0.0;
double ros_angular_vel_z = 0;

//Function

void error_loop();
void timer_callback(rcl_timer_t * timer, int64_t last_call_time);
void manualMoveRobot();
void stopMove();
void rosMoveBase(float linear_vel, float pan_vel, float angle_vel);



void setup() {
  
  Serial.begin(57600);
  Serial.print("START!!!!");
  
  Serial1.begin(RS485_BAUD_RATE, SERIAL_8N1, RS485_RXD2_PIN, RS485_TXD2_PIN);
  pinMode(RS485_EN_PIN, OUTPUT);
  IPAddress agent_ip(192, 168, 1, 35);
  size_t agent_port = 8888;

  char ssid[] = "tawechai 2.4";
  char psk[]= "0819732784";

  set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);
  
  Serial.printf("Connect to => ssid: %s",ssid);
  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_platformio_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "micro_ros_pub_32"));
  RCCHECK(rclc_publisher_init_default(
    &publisher2,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int64),
    "micro_ros_pub_64"));

  // create timer,
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  
  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
  msg.data = 0;


  //Initialize HMI Screen
  Serial.println("Initialize HMI Screen");
  HMI.Init(true);

  //Initialize Motor
  Serial.println("Initialize MOTOR");
  
  MOTOR.Init(leftMotorDir,rightMotorDir);
  motorState = MOTOR.InitMotor(true);
  
  MOTOR.stopMotor();
  
  Serial.println("MOTOR STOP");
  MOTOR.enableMotor();

}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
  if (millis()- beforeReadMotorState > MOTOR_READ_STATE_INTERVAL){
    MOTOR_STATE motorState = MOTOR.getMotorState();
    MOTOR_RPM  motorRpm = MOTOR.getMotorRpm();
    beforeReadMotorState = millis();
  }

  if(millis() - beforeReadHmiState > HMI_READ_INTERVAL){
    bool res = HMI.readPowerState();
    if (res){
      powerState = HMI.getHMIPowerState();
      if (powerState.manualState){
        res = HMI.readControlCmd();
        if (res) {
          manualSpeedControl = HMI.getManualControl();
        }
      }
    }else{
      Serial.println("HMI is not Connected");
    } 
    beforeReadHmiState = millis();
  }

  if(millis() - beforeMotorControl > MOTOR_CONTROL_INTERVAL){// Control Motor
    beforeMotorControl = millis();
    if (powerState.powerState){ // ROBOT ON
      if (!motorState) Serial.print(" Try to connect Motor ");
      while (!motorState)
      {
        Serial.print(".");
      }
      Serial.println(" connected");
      
      if(powerState.manualState){
        manualMoveRobot();
      }else{
        rosMoveBase(ros_linear_vel_x, ros_linear_vel_y, ros_angular_vel_z);
      }

    }else{// Robot Turn Off
      stopMove();
    }

  }


}

// ROS=>Error handle loop
void error_loop() {
  while(1) {
    delay(100);
  }
}
// ROS=> timerCallback
void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.data++;
    RCSOFTCHECK(rcl_publish(&publisher2, &num, NULL));
    num.data+=2;
  }
}




void stopMove(){
  
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
void manualMoveRobot()
{

    switch (manualSpeedControl.direction)
    {

    case OFF:
        rosMoveBase(0, 0, 0);
        break;
    case LEFTDOWN:
        rosMoveBase(-manualSpeedControl.manual_linear_x, 0, -manualSpeedControl.manual_angular_z);
        break;
    case DOWN:
        rosMoveBase(-manualSpeedControl.manual_linear_x, 0, 0);
        break;
    case RIGHTDOWN:
        rosMoveBase(-manualSpeedControl.manual_linear_x, 0, manualSpeedControl.manual_angular_z);
        break;
    case LEFT:
        rosMoveBase(0, 0, manualSpeedControl.manual_angular_z);
        break;
    case STOP:
        rosMoveBase(0, 0, 0);
        break;
    case RIGHT:
        rosMoveBase(0, 0, -manualSpeedControl.manual_angular_z);
        break;
    case LEFTUP:
        rosMoveBase(manualSpeedControl.manual_linear_x, 0, -manualSpeedControl.manual_angular_z);
        break;
    case UP:
        rosMoveBase(manualSpeedControl.manual_linear_x, 0, 0);
        break;
    case RIGHTUP:
        rosMoveBase(manualSpeedControl.manual_linear_x, 0, manualSpeedControl.manual_angular_z);
        break;
    default:
        rosMoveBase(0, 0, 0);
        break;
    }
}

void rosMoveBase(float linear_vel, float pan_vel, float angle_vel)
{
    // linear_vel *=cos(45);
    // pan_vel *=sin(45);
    int LF_Wheel_Spd = constrain((linear_vel - angle_vel * 0.25) * 60 / (0.13 * PI), -MAX_RPM, MAX_RPM);
    int RF_Wheel_Spd = constrain((linear_vel + angle_vel * 0.25) * 60 / (0.13 * PI), -MAX_RPM, MAX_RPM);
    char msg[100];
    // Serial.printf("linear_vel %1.2f angular_vel %1.2f \n",linear_vel,angle_vel);
    // sprintf(msg,"Left Wheel spd : %d Right Wheel spd : %d \n",LF_Wheel_Spd,RF_Wheel_Spd);
    // Serial.print(msg);
    bool res = MOTOR.moveMotor(LF_Wheel_Spd,RF_Wheel_Spd);
    if (res) Serial.println("ERROR TO MOVE ROBOT");

    
}