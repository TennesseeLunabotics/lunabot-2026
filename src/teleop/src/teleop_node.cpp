#include <memory>

#include <cstdio>
#include <chrono>
#include <thread>
#include <iostream>
#include <joybuttons.h>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include "string"
#include "constants.h"
#include "std_msgs/msg/bool.hpp"
#include "std_msgs/msg/string.hpp"
#include "interfaces/srv/set_teleop.hpp"


using std::placeholders::_1;
using namespace std;

class Teleop : public rclcpp::Node
{
 public:
    Teleop()
    : Node("Teleop")
    {
      subscription_ = this->create_subscription<sensor_msgs::msg::Joy>("joy", 10, std::bind(&Teleop::topic_callback, this, _1));
      drivetrainPub = this->create_publisher<sensor_msgs::msg::JointState>("drivetrain_cmd_vel", 10);
      armPub = this->create_publisher<std_msgs::msg::String>("shovel/arm_cmd", 10);
      scoopPub = this->create_publisher<std_msgs::msg::String>("shovel/scoop_cmd", 10);
      bucketPub = this->create_publisher<std_msgs::msg::String>("shovel/bucket_cmd", 10);
      service = this->create_service<interfaces::srv::SetTeleop>("set_teleop", [this](const std::shared_ptr<interfaces::srv::SetTeleop::Request> request,
                  std::shared_ptr<interfaces::srv::SetTeleop::Response> response) { this->set_teleop(request, response); });
    }

  private:
    void topic_callback(const sensor_msgs::msg::Joy &raw) {
        //Variable declare
	bool breakout = false;
	sensor_msgs::msg::JointState drivetrain_states;
        drivetrain_states.velocity.resize(2);
        drivetrain_states.velocity[0] = 0;
        drivetrain_states.velocity[1] = 0;
        std_msgs::msg::String bucket_state;
        std_msgs::msg::String arm_state;
        std_msgs::msg::String scoop_state;

        if (raw.buttons[BUTTON_LSTICK]) {
          teleopEnabled = true;
        }

        if (teleopEnabled){
            cout << "This is working!" << endl;

  	    if (!raw.buttons[BUTTON_B]) {
              drivetrain_states.velocity[0] = (raw.axes[AXIS_LEFTY])*MOTOR_MAX;
              drivetrain_states.velocity[1] = (raw.axes[AXIS_RIGHTY])*MOTOR_MAX;
              cout << "Normal speed" << endl;
            } else {
              drivetrain_states.velocity[0] = (raw.axes[AXIS_LEFTY])*ARHAN_MODE * MOTOR_MAX;
              drivetrain_states.velocity[1] = (raw.axes[AXIS_RIGHTY])*ARHAN_MODE * MOTOR_MAX;
              cout << "Arhan Mode engaged" << endl;
	    } 

            if (raw.buttons[BUTTON_RBUMPER]) {
              bucket_state.data = "f";
            } else if (raw.axes[AXIS_RTRIGGER] > 0.5) {
              bucket_state.data = "b";
            }

            if (raw.axes[AXIS_LTRIGGER] > 0.5) {
              arm_state.data = "f";
            } else if (raw.buttons[BUTTON_LBUMPER]) {
              arm_state.data = "b";
            }

            if (raw.buttons[BUTTON_Y]) {
              scoop_state.data = "f";
            } else if (raw.buttons[BUTTON_X]) {
              scoop_state.data = "b";
            }

	    //Test for auto dump
	    if (raw.buttons[BUTTON_DPAD_DOWN]) {
	      cout << "Auto Dump engaged" << endl;
	      //Setting all of the systems to forward to dump
	      bucket_state.data = "f";
	      scoop_state.data = "b";
	      arm_state.data = "f";
	    
	      //Sleeps for 14 seconds
	      //If we press any of the buttons we use to activte it, we break out
	      for (int i = 0; i < 14 * 10; i++) {	
		    this_thread::sleep_for(chrono::milliseconds(100));
        	scoopPub->publish(scoop_state);
            armPub->publish(arm_state);
            bucketPub->publish(bucket_state);
	    	if (raw.buttons[BUTTON_DPAD_DOWN]) {
		      breakout = true;
		      break;
		    }
	      }
		  if (breakout) break;
		  
	      //Go backward for 1 second
          drivetrain_states.velocity[0] = (raw.axes[AXIS_LEFTY])*MOTOR_MAX;
          drivetrain_states.velocity[1] = (raw.axes[AXIS_RIGHTY])*MOTOR_MAX;
	      for (int i = 0; i < 10; i++) {	
		    this_thread::sleep_for(chrono::milliseconds(100));
            drivetrainPub->publish(drivetrain_states);
		    if (raw.buttons[BUTTON_DPAD_DOWN]) {
		      breakout = true;
		      break;
		    }
	      }
		  if (breakout) break;
		  
	      //Resetting everything
	      scoop_state.data = "f";
	      bucket_state.data = "b";
	      arm_state.data = "b";
	      for (int i = 0; i < 13 * 10; i++) {	
		    this_thread::sleep_for(chrono::milliseconds(100));
            scoopPub->publish(scoop_state);
            armPub->publish(arm_state);
            bucketPub->publish(bucket_state);
		    if (raw.buttons[BUTTON_DPAD_DOWN) {
		      breakout = true;
		      break;
		    }
	     }
	      
	      breakout = true;
	    }

	    //Test for auto mine
	    if (raw.buttons[BUTTON_A] && raw.buttons[BUTTON_B]) {
	      cout << "Auto mine engaged" << endl;
		
	      //Assume the arm is at the lowest point
	      //and the scoop is pointed to the ground (at the lowest point)

	      //Move forward for 1 second
          drivetrain_states.velocity[0] = (raw.axes[AXIS_LEFTY])*MOTOR_MAX;
          drivetrain_states.velocity[1] = (raw.axes[AXIS_RIGHTY])*MOTOR_MAX;
	      for (int i = 0; i < 10; i++) {	
		    this_thread::sleep_for(chrono::milliseconds(100));
            drivetrainPub->publish(drivetrain_states);
		    if (raw.buttons[BUTTON_A] || raw.buttons[BUTTON_B]) {
		      breakout = true;
		      break;
		    }
	      }
		  if (breakout) break;

          //Rase scoop for .5 seconds
	      scoop_state.data = "f";
	      for (int i = 0; i < 5; i++) {
		    this_thread::sleep_for(chrono::milliseconds(100));
            scoopPub->publish(scoop_state);
		    if (raw.buttons[BUTTON_A] || raw.buttons[BUTTON_B]) {
		      breakout = true;
		      break;
		    }
	      }
	      if (breakout) break;

		  //Move forward for 0.5 seconds
		  drivetrain_states.velocity[0] = (raw.axes[AXIS_LEFTY])*MOTOR_MAX;
          drivetrain_states.velocity[1] = (raw.axes[AXIS_RIGHTY])*MOTOR_MAX;
		  for (int i = 0; i < 5; i++) {
		    this_thread::sleep_for(chrono::milliseconds(100));
		    scoopPub->publish(scoop_state);
		    if (raw.buttons[BUTTON_A] || raw.buttons[BUTTON_B]) {
		      breakout = true;
		      break;
		    }
		  }
		  if (breakout) break;

		  //Assume that the scoop is full now
		  //Store the mined material in the dump

		  //Rasie the scoop, while keeping it level
		  arm_state.data = "f";
		  for (int i = 0; i < 14 * 10; i++) {
		    this_thread::sleep_for(chrono::milliseconds(100));
		    //Halfway through, move scoop 
			if (i >= 70 && i <= 80) {
				scoop_state = "b";
				scoopPub->publish(scoop_state);
			}
            //When at peak, dump material into dump
			if (i >= 130) {
				scoop_state = "f";
				scoopPub->publish(scoop_state);
			}
			armPub->publish(arm_state);
		    if (raw.buttons[BUTTON_A] || raw.buttons[BUTTON_B]) {
		      breakout = true;
		      break;
		    }
		  }
		  if (breakout) break;

		  //"Shakes" the bot to even out the material
		  arm_state = "b";
		  for (int i = 0; i < 13 * 10; i++) {
            this_thread::sleep_for(chrono::milliseconds(100));
			if (i % 2 == 0) {
		      drivetrain_states.velocity[0] = (raw.axes[AXIS_LEFTY])*MOTOR_MAX;
              drivetrain_states.velocity[1] = (raw.axes[AXIS_RIGHTY])*MOTOR_MAX;
			} else {
			  drivetrain_states.velocity[0] = 0;
			  drivetrain_states.velocity[1] = 0;
			}
			armPub->publish(arm_state);
		    drivetrainPub->publish(drivetrain_states);
		  }
		  if (breakout) break;
			
	      breakout = true;
	    }

        }

	//If we breakout of the autodump/automine we don't need to have sudden movement once we breakout
	if (!breakout) {
          drivetrainPub->publish(drivetrain_states);
          scoopPub->publish(scoop_state);
          armPub->publish(arm_state);
          bucketPub->publish(bucket_state);
	}

    }

    void set_teleop(const std::shared_ptr<interfaces::srv::SetTeleop::Request> request, std::shared_ptr<interfaces::srv::SetTeleop::Response> response){
    	string enabled;
      teleopEnabled = request->teleop_enabled;

      if(teleopEnabled) {
        RCLCPP_INFO(get_logger(), "\033[1;35mMANUAL CONTROL:\033[0m \033[1;32mENABLED\033[0m");
        enabled = "enabled";
      } else {
        RCLCPP_INFO(get_logger(), "\033[1;35mMANUAL CONTROL:\033[0m \033[1;32mDISABLED\033[0m");
        enabled = "disabled";
      }

      response->message = "Teleop: " + enabled;
      response->success = true;
    }

    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr drivetrainPub;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr bucketPub;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr armPub;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr scoopPub;
    rclcpp::Service<interfaces::srv::SetTeleop>::SharedPtr service;
    bool teleopEnabled = false;
};

int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Teleop>());
  rclcpp::shutdown();
  return 0;
}
