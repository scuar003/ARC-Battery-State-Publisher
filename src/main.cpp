#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/string.h>

#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
#error This example is only available for Arduino framework with serial transport.
#endif

rcl_publisher_t publisher;
std_msgs__msg__String msg;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_support_t support;  // Add this line

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

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "battery_info"));

  msg.data.data = (char *) malloc(100);
}

void loop() 
{
      //Battery Volatge 
      cellVoltage = analogRead(A2) * 0.00472199 * cell_const[0]; // Reading from A2 and using the first cell_const value
      battery_voltage = cellVoltage * 7.5;
      snprintf(msg.data.data, 100, "Battery Voltage: %.2f", battery_voltage);
      //RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));

      //battery charge percentage 
      minVoltage = 10; // Set your minimum voltage
      maxVoltage = 14.8; // Set your maximum voltage
      voltageRange = maxVoltage - minVoltage;
      relativeVoltage = battery_voltage - minVoltage;
      chargePercentage = (relativeVoltage / voltageRange) * 100.0;

      if (chargePercentage > 2) {
        snprintf(msg.data.data, 100, "Battery Charge Percentage: %.2f%%", chargePercentage);
      } else {
        snprintf(msg.data.data, 100, "Battery Charge Percentage: 0%%");
      }
      RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));

  delay(1000);
}
