// #include <Arduino.h>



// #define MAX_CELLS 12

// double cell_const[MAX_CELLS] = 
// {
//   1.0000,
//   2.1915,
//   2.6970,
//   4.1111,
//   4.7333,
//   6.6000,
//   6.6000,
//   7.8293,
//   8.4667,
//   9.2353,
//   11.0000,
//   11.0000
// };

// double battery_voltage; 
// double chargePercentage;





// void setup() 
// {
//   // Initialize the analog pin for reading battery voltage.
//   pinMode(A2, INPUT);
//   // Initialize serial communication.
//   Serial.begin(9600);
 
// }

// void loop() 
// {
 

//     if(Serial.available()){
//         char input = Serial.read();
//         if (input == 'p' || input == 'P'){
//             double cellVoltage = analogRead(A2) * 0.00472199 * cell_const[0]; // Reading from A2 and using the first cell_const value

//             battery_voltage = cellVoltage *7.5;
//             Serial.print("Battery Voltage: ");
//             Serial.println(battery_voltage);


//             // Check if battery is attached.
//             if (battery_voltage >= 2.0) {
//                 Serial.println("Battery: Present");
//             } else {
//                 Serial.println("Battery: Not Present");
//             }

//             // Update battery health.
//             if (battery_voltage >= 14.8) {
//                 Serial.println("Battery Health: Overvoltage");
//             } else if (battery_voltage < 10) {
//                 Serial.println("Battery Health: Dead");
//             } else {
//                 Serial.println("Battery Health: Good");
//             }
//             double minVoltage = 10; // what is the minin vollatge
//             double maxVoltage = 14.8; // what is thhe maximu voltage 

//             double voltageRange = maxVoltage - minVoltage;
//             double relativeVoltage = battery_voltage - minVoltage;
//             chargePercentage = (relativeVoltage / voltageRange) * 100.0;
            
//             if (chargePercentage > 2){
//                 Serial.print("Battery Charge Percentage: ");
//                 Serial.print(chargePercentage);
//                 Serial.println("%");
//             }else{
//                 Serial.println("Battery Charge Percentage: 0%");
//             }

//         }

 
//     }
 


  
// }

// ////////////////////////////////////////////////////////////////
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

//double battery_voltage; 
//double chargePercentage;

double cellVoltage = analogRead(A2) * 0.00472199 * cell_const[0]; // Reading from A2 and using the first cell_const value
double battery_voltage = cellVoltage * 7.5;

    //Battery Charge percentage 
      double minVoltage = 10; // Set your minimum voltage
      double maxVoltage = 14.8; // Set your maximum voltage
      double voltageRange = maxVoltage - minVoltage;
      double relativeVoltage = battery_voltage - minVoltage;
      double chargePercentage = (relativeVoltage / voltageRange) * 100.0;

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

      snprintf(msg.data.data, 100, "Battery Voltage: %.2f", battery_voltage);
      RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));

      // Check if battery is attached.
      if (battery_voltage >= 2.0) {
        snprintf(msg.data.data, 100, "Battery Present");
         RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
      } else {
        snprintf(msg.data.data, 100, "Battery Not Present");
         RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
      }
     

      // Update battery health.
      if (battery_voltage >= 14.8) {
        snprintf(msg.data.data, 100, "Battery Health: Overvoltage");
         RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
      } else if (battery_voltage < 10) {
        snprintf(msg.data.data, 100, "Battery Health: Dead");
         RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
      } else {
        snprintf(msg.data.data, 100, "Battery Health: Good");
         RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
      }

    
      if (chargePercentage > 2) {
        snprintf(msg.data.data, 100, "Battery Charge Percentage: %.2f%%", chargePercentage);
         RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
      } else {
        snprintf(msg.data.data, 100, "Battery Charge Percentage: 0%%");
         RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
      }
   

  delay(1000);
}
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

// #include <Arduino.h>
// #include <micro_ros_platformio.h>

// #include <rcl/rcl.h>
// #include <rclc/rclc.h>
// #include <rclc/executor.h>
// #include <std_msgs/msg/string.h>

// #if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
// #error This example is only available for Arduino framework with serial transport.
// #endif

// rcl_publisher_t publisher;
// std_msgs__msg__String msg;
// rcl_allocator_t allocator;
// rcl_node_t node;
// rclc_support_t support;
// rcl_subscription_t subscription;

// #define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
// #define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}
// #define MAX_CELLS 12

// double cell_const[MAX_CELLS] = 
// {
//   1.0000,
//   2.1915,
//   2.6970,
//   4.1111,
//   4.7333,
//   6.6000,
//   6.6000,
//   7.8293,
//   8.4667,
//   9.2353,
//   11.0000,
//   11.0000
// };

// double battery_voltage; 
// double chargePercentage;
// // Error handle loop
// void error_loop() {
//   while(1) {
//     delay(100);
//   }
// }

// // Callback function for subscription
// void on_subscription_callback(const void *msgin)
// {
//   const std_msgs__msg__String *msg_sub = (const std_msgs__msg__String *)msgin;
//   if (strcmp(msg_sub->data.data, "p") == 0 || strcmp(msg_sub->data.data, "P") == 0) {
//     double cellVoltage = analogRead(A2) * 0.00472199 * cell_const[0]; // Reading from A2 and using the first cell_const value

//     battery_voltage = cellVoltage * 7.5;
//     snprintf(msg.data.data, 100, "Battery Voltage: %.2f", battery_voltage);
//     RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));

//     // Check if battery is attached.
//     if (battery_voltage >= 2.0) {
//       snprintf(msg.data.data, 100, "Battery Present");
//     } else {
//       snprintf(msg.data.data, 100, "Battery Not Present");
//     }
//     RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));

//     // Update battery health.
//     if (battery_voltage >= 14.8) {
//       snprintf(msg.data.data, 100, "Battery Health: Overvoltage");
//     } else if (battery_voltage < 10) {
//       snprintf(msg.data.data, 100, "Battery Health: Dead");
//     } else {
//       snprintf(msg.data.data, 100, "Battery Health: Good");
//     }
//     RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));

//     double minVoltage = 10; // Set your minimum voltage
//     double maxVoltage = 14.8; // Set your maximum voltage
//     double voltageRange = maxVoltage - minVoltage;
//     double relativeVoltage = battery_voltage - minVoltage;
//     chargePercentage = (relativeVoltage / voltageRange) * 100.0;

//     if (chargePercentage > 2) {
//       snprintf(msg.data.data, 100, "Battery Charge Percentage: %.2f%%", chargePercentage);
//     } else {
//       snprintf(msg.data.data, 100, "Battery Charge Percentage: 0%%");
//     }
//     RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
//   }
// }

// void setup() 
// {
//   // Initialize the analog pin for reading battery voltage.
//   pinMode(A2, INPUT);
  
//   // Configure serial transport
//   Serial.begin(115200);
//   set_microros_serial_transports(Serial);
//   delay(2000);

//   allocator = rcl_get_default_allocator();
//   RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
//   RCCHECK(rclc_node_init_default(&node, "arduino_battery_node", "", &support));

//   // create publisher
//   RCCHECK(rclc_publisher_init_default(
//     &publisher,
//     &node,
//     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
//     "arduino_battery_publisher"));

//   // create subscription
//   RCCHECK(rclc_subscription_init_default(
//     &subscription,
//     &node,
//     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
//     "micro_ros_agent_command_sub"));

//   msg.data.data = (char *) malloc(100);
// }

// void loop() 
// {
//   rclc_executor_t executor;  // Declare the executor
//   RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));  // Initialize the executor
//   RCCHECK(rclc_executor_add_subscription(&executor, &subscription, &msg, &on_subscription_callback, ON_NEW_DATA));
  
//   rclc_executor_spin(&executor);  // Spin the executor
//   delay(100);
// }

