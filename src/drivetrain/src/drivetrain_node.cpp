#include <memory>
#include <cstdio>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include "string"
#include "SparkMax.hpp"

#include <lunabot_constants/drivetrain.hpp>

using std::placeholders::_1;
using namespace std;
using namespace lunabot_constants::drivetrain;

class Drivetrain : public rclcpp::Node
{
 public:
    Drivetrain()
    : Node("Drivetrain")
    {
      subscription_ = this->create_subscription<sensor_msgs::msg::JointState>(
      "drivetrain_cmd_vel", 10, std::bind(&Drivetrain::topic_callback, this, _1));
      sensorPub = this->create_publisher<sensor_msgs::msg::JointState>("drivetrain_sensor_output", 10);

      motors[0].SetInverted(false);
      motors[1].SetInverted(true);
      for(int i = 0; i < NUM_MOTORS; i++){
        motors[i].SetIdleMode(IdleMode::kBrake);
        motors[i].SetMotorType(MotorType::kBrushless);
      }
    }

  private:
    SparkMax motors[NUM_MOTORS] = {SparkMax("can0", MOTOR_LEFT), SparkMax("can0", MOTOR_RIGHT)};    
    std::string locations[NUM_MOTORS] = {"Left", "Right"};
    void topic_callback(const sensor_msgs::msg::JointState &drivetrain_states) {
      //set motor values
      SparkMax::Heartbeat();
      for(int i = 0; i < NUM_MOTORS; i++){
        motors[i].SetVoltage(drivetrain_states.velocity[i] * MOTOR_MAX);
      }

      sensor_msgs::msg::JointState motor_states;
      motor_states.name.resize(2);
      motor_states.velocity.resize(2);
      motor_states.position.resize(2);
      motor_states.effort.resize(2);
      
      for(int i = 0; i < NUM_MOTORS; i++) {
        motor_states.name[i] = locations[i];
        motor_states.velocity[i] = motors[i].GetVelocity();
        motor_states.position[i] = motors[i].GetPosition();
        motor_states.effort[i] = motors[i].GetVoltage();
      }
      sensorPub->publish(motor_states);
    }
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr sensorPub;
};

int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Drivetrain>());
  rclcpp::shutdown();
  return 0;
}
