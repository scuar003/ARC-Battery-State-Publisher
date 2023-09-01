#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/string.h>
#include <std_msgs/msg/header.h>
//#include <srv/batterystate.hpp>


#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
#error This example is only available for Arduino framework with serial transport.
#endif

rcl_publisher_t publisher;
std_msgs__msg__String voltage_msg;
std_msgs__msg__String charge_msg;
std_msgs__msg__String health_msg;
std_msgs__msg__String attached_msg;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_support_t support; 


#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}
#define MAX_CELLS 12

double cell_const[MAX_CELLS] = 
{
  1.0000,
  2.1915,
  2.6970,
  4.1111,
  4.7333,
  6.6000,
  6.6000,
  7.8293,
  8.4667,
  9.2353,
  11.0000,
  11.0000
};


// global variable for battery calculations 
double battery_voltage, chargePercentage, cellVoltage, minVoltage, maxVoltage, voltageRange, relativeVoltage;



// Error handle loop
void error_loop() {
  while(1) {
    delay(100);
  }
}

void setup() 
{
  // Initialize the analog pin for reading battery voltage.
  pinMode(A2, INPUT);
  
  // Configure serial transport
  Serial.begin(115200);
  set_microros_serial_transports(Serial);
  delay(2000);

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));  // Initialize support
  RCCHECK(rclc_node_init_default(&node, "arduino_battery_node", "", &support));

  // create battery state publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "battery_state"));
 
 // initialize messages for each battery state
  voltage_msg.data.data = (char *) malloc(100);
  charge_msg.data.data = (char*) malloc(100);
  health_msg.data.data = (char *) malloc(100);
  attached_msg.data.data = (char*) malloc(100);
  
}

void loop() 
{
      //Battery Volatge 
      cellVoltage = analogRead(A2) * 0.00472199 * cell_const[0]; // Reading from A2 and using the first cell_const value
      battery_voltage = cellVoltage * 7.5;
      snprintf(voltage_msg.data.data, 100, "Battery Voltage: %.2f", battery_voltage);
      RCSOFTCHECK(rcl_publish(&publisher, &voltage_msg, NULL));

      //battery charge percentage 
      minVoltage = 10; // Set your minimum voltage
      maxVoltage = 13.6; // Set your maximum voltage
      voltageRange = maxVoltage - minVoltage;
      relativeVoltage = battery_voltage - minVoltage;
      chargePercentage = (relativeVoltage / voltageRange) * 100.0;
      if (chargePercentage > 2) {
        snprintf(charge_msg.data.data, 100, "Battery Charge Percentage: %.2f%%", chargePercentage);
      } else {
        snprintf(charge_msg.data.data, 100, "Battery Charge Percentage: 0%%");
      }
      RCSOFTCHECK(rcl_publish(&publisher, &charge_msg, NULL));

            // // Check if battery is attached.
      if (battery_voltage > 2.0) {
        snprintf(attached_msg.data.data, 100, "Battery Present");
      } else  {
        snprintf(attached_msg.data.data, 100, "Battery Not Present");
      }
      RCSOFTCHECK(rcl_publish(&publisher, &attached_msg, NULL));

      // Update battery health.
        if (battery_voltage >= 14.8) {
        snprintf(health_msg.data.data, 100, "Battery Health: Overvoltage");
      } else if (battery_voltage < 10) {
        snprintf(health_msg.data.data, 100, "Battery Health: Dead");
      } else {
        snprintf(health_msg.data.data, 100, "Battery Health: Good");
      }
      RCSOFTCHECK(rcl_publish(&publisher, &health_msg, NULL));



  delay(5000);
}
