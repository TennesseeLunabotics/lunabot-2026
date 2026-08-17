#include <memory>

#include <cstdio>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <termios.h>
#include <unistd.h>

#include <lunabot_constants/controller.hpp>
#include <lunabot_constants/teleop.hpp>

using namespace lunabot_constants::controller;
using namespace lunabot_constants::teleop;
using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class Keyboard : public rclcpp::Node
{
  public:
    Keyboard()
    : Node("keyboard_node"), count_(0)
    {
      publisher_ = this->create_publisher<sensor_msgs::msg::Joy>("joy", 10);
      timer_ = this->create_wall_timer(
      50ms, std::bind(&Keyboard::timer_callback, this));
    }

  private:
  
    char getch() {
        struct termios oldt, newt;
        char ch;
    
        // Get the terminal settings for stdin
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        
        // Disable canonical mode and echoing
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
        // Read a single character
        ch = getchar();
    
        // Restore the old terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
        return ch;
    }
    
    void timer_callback()
    {
      auto message = sensor_msgs::msg::Joy();
      char ch = getch();
	message.axes.resize(8);
      if (ch == 'w' ) {
      message.axes[AXIS_LEFTY] = 1;
      }
      else if (ch == 'x'){
      message.axes[AXIS_LEFTY] = -1;
      }
      else if (ch == 'i') {
        message.axes[AXIS_RIGHTY] = 1;
      }
      else if (ch == 'n') {
        message.axes[AXIS_RIGHTY] = -1;
      }else if (ch == 's'){
	message.axes[AXIS_LEFTY] = 0;
      }else if (ch == 'j'){
      	message.axes[AXIS_RIGHTY] = 0;
      }
      std::cout << ch << std::endl;
      publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr publisher_;
    size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Keyboard>());
  rclcpp::shutdown();
  return 0;
}
