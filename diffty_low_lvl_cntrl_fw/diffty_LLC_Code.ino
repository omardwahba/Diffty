
#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Twist.h>


#define SONAR_NUM     3 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

//defining the parameter to control the motors
#define left_motor_forward 6
#define left_motor_backward 9 
#define right_motor_forward 10
#define right_motor_backward 11
#define HSpeed 200
#define Lspeed 140
#define vel 0.0 
#define ang 0.0
#define zeroo 0.0
//defineing the the HEAd light
#define headLight_pin 12

/*unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array. left >> back >> right
  NewPing(A1, A0, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(A4, A5, MAX_DISTANCE),
  NewPing(A2, A3, MAX_DISTANCE)
};*/


ros::NodeHandle nh ;

/*void sensor_msg_init(sensor_msgs::Range &range_name, char *frame_id_name)
{
  range_name.radiation_type = sensor_msgs::Range::ULTRASOUND;
  range_name.header.frame_id = frame_id_name;
  range_name.field_of_view = 0.26;
  range_name.min_range = 0.0;
  range_name.max_range = 2.0;
}

//Create three instances for range messages.
sensor_msgs::Range range_left;
sensor_msgs::Range range_center;
sensor_msgs::Range range_right;

//Create publisher onjects for all sensors
ros::Publisher pub_range_left("/diffty/ultrasound_left", &range_left);
ros::Publisher pub_range_center("/diffty/ultrasound_center", &range_center);
ros::Publisher pub_range_right("/diffty/ultrasound_right", &range_right);

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
    range_left.range   = cm[0];
    range_center.range = cm[1];
    range_right.range  = cm[2];

    range_left.header.stamp = nh.now();
    range_center.header.stamp = nh.now();
    range_right.header.stamp = nh.now();

    pub_range_left.publish(&range_left);
    pub_range_center.publish(&range_center);
    pub_range_right.publish(&range_right);
    nh.spinOnce();
}*/
/*this part is to control the head light of the robot through a std bool message using led_cmd topic lisner */
void light_cb ( const std_msgs::Bool& cmd ){
        if (cmd.data == true ) digitalWrite(headLight_pin, LOW );
        else digitalWrite(headLight_pin,HIGH);
    digitalWrite(13, cmd.data);
}


void motorControl_init(){
    pinMode(left_motor_backward,OUTPUT);
    pinMode(left_motor_forward,OUTPUT);
    pinMode(right_motor_forward,OUTPUT);
    pinMode(right_motor_backward,OUTPUT);
}
void forward (uint8_t speed){
    analogWrite(left_motor_forward,speed);
    analogWrite(right_motor_forward,speed);
    analogWrite(left_motor_backward,0);
    analogWrite(right_motor_backward,0);
}

void back (uint8_t speed){
    analogWrite(left_motor_forward,0);
    analogWrite(right_motor_forward,0);
    analogWrite(left_motor_backward,speed);
    analogWrite(right_motor_backward,speed);
}
void turnLeft (uint8_t speed , bool pureRotation){
    if (pureRotation){
        analogWrite(left_motor_forward,0);
        analogWrite(right_motor_forward,speed);
        analogWrite(left_motor_backward,speed);
        analogWrite(right_motor_backward,0);
    }
    else {
        analogWrite(left_motor_forward,Lspeed);
        analogWrite(right_motor_forward,HSpeed);
        analogWrite(left_motor_backward,0);
        analogWrite(right_motor_backward,0);
    }
}
void turnRight (uint8_t speed , bool pureRotation){
    if (pureRotation){
        analogWrite(left_motor_forward,speed);
        analogWrite(right_motor_forward,0);
        analogWrite(left_motor_backward,0);
        analogWrite(right_motor_backward,speed);
    }
    else {
        analogWrite(left_motor_forward,Lspeed);
        analogWrite(right_motor_forward,HSpeed);
        analogWrite(left_motor_backward,0);
        analogWrite(right_motor_backward,0);
    }
}
void motorControl_cb (const geometry_msgs::Twist& velocity ){
    float linearV = velocity.linear.x ;
    float rotationV = velocity.angular.z ;
    if ( linearV > vel && rotationV == zeroo ) forward(HSpeed);
    else if ( linearV < vel && rotationV == zeroo ) back(HSpeed);
    else if ( linearV == zeroo && rotationV > ang ) turnLeft(HSpeed,true);
    else if ( linearV == zeroo && rotationV < ang) turnRight(HSpeed,true);
    else if ( linearV > vel && rotationV > vel ) turnLeft(HSpeed, false);
    else if ( linearV > vel && rotationV < vel ) turnRight(HSpeed, false);
    else forward(0);
}

ros::Subscriber<geometry_msgs::Twist> motorControl("/diffty/cmd_vel", &motorControl_cb);
ros::Subscriber<std_msgs::Bool> lightControl_sub("/diffty/light_cmd", &light_cb);

void setup() {
  /*pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;*/

  motorControl_init();
  pinMode(headLight_pin,OUTPUT);
  digitalWrite(headLight_pin,HIGH);
  pinMode(13,OUTPUT);

  nh.initNode();
  /*nh.advertise(pub_range_left);
  nh.advertise(pub_range_center);
  nh.advertise(pub_range_right);

  sensor_msg_init(range_left, "/diffty/ultrasound_left");
  sensor_msg_init(range_center, "/diffty/ultrasound_center");
  sensor_msg_init(range_right, "/diffty/ultrasound_right");*/

  nh.subscribe(lightControl_sub);
  nh.subscribe(motorControl);
}

void loop() {
   nh.spinOnce();
   delay(1);
  /*for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }*/

 //Handle ROS events
}
