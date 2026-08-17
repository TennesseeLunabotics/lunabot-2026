#include <memory>
#include <string>
#include <cmath>

#include <cstdio>
#include <iostream>

#include <rclcpp/rclcpp.hpp>

#include <sensor_msgs/msg/joy.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/string.hpp>

#include <lunabot_constants/controller.hpp>
#include <lunabot_constants/teleop.hpp>

#include "interfaces/srv/set_teleop.hpp"
#include "Timer.hpp"

using std::placeholders::_1;
using namespace std;

using namespace lunabot_constants::controller;
using namespace lunabot_constants::teleop;

class Teleop : public rclcpp::Node
{
public:
    Teleop()
        : Node("Teleop")
    {
        subscription_ =
            this->create_subscription<sensor_msgs::msg::Joy>(
                "joy",
                10,
                std::bind(&Teleop::topic_callback, this, _1));

        drivetrainPub =
            this->create_publisher<sensor_msgs::msg::JointState>(
                "drivetrain_cmd_vel", 10);

        armPub =
            this->create_publisher<std_msgs::msg::String>(
                "shovel/arm_cmd", 10);

        scoopPub =
            this->create_publisher<std_msgs::msg::String>(
                "shovel/scoop_cmd", 10);

        bucketPub =
            this->create_publisher<std_msgs::msg::String>(
                "shovel/bucket_cmd", 10);

        service =
            this->create_service<interfaces::srv::SetTeleop>(
                "set_teleop",
                [this](
                    const std::shared_ptr<
                        interfaces::srv::SetTeleop::Request> request,
                    std::shared_ptr<
                        interfaces::srv::SetTeleop::Response> response)
                {
                    this->set_teleop(request, response);
                });
    }

private:
    void topic_callback(const sensor_msgs::msg::Joy::SharedPtr raw)
    {
        drivetrain_states.velocity.resize(2);
        drivetrain_states.velocity[0] = 0;
        drivetrain_states.velocity[1] = 0;

        bucket_state.data = "";
        arm_state.data = "";
        scoop_state.data = "";

        // Mode switching
        if (raw->buttons[BUTTON_LSTICK]) {
            robotState = 1;
        }

        if (raw->axes[AXIS_DPAD_X] < -0.5 ||
            raw->buttons[BUTTON_RSTICK]) {
            robotState = 0;
        }

        if (raw->axes[AXIS_DPAD_Y] > 0.5) {
            robotState = 2;
        }

        switch (robotState) {

        case 1:

            // Driving
            if (!raw->buttons[BUTTON_B]) {
                drivetrain_states.velocity[0] =
                    raw->axes[AXIS_LEFTY] * NORMAL_MODE;

                drivetrain_states.velocity[1] =
                    raw->axes[AXIS_RIGHTY] * NORMAL_MODE;
            }
            else {
                drivetrain_states.velocity[0] =
                    raw->axes[AXIS_LEFTY] *
                    ARHAN_MODE *
                    NORMAL_MODE;

                drivetrain_states.velocity[1] =
                    raw->axes[AXIS_RIGHTY] *
                    ARHAN_MODE *
                    NORMAL_MODE;
            }

            // Scoop
            if (raw->buttons[BUTTON_RBUMPER]) {
                scoop_state.data = "f";
            }
            else if (raw->axes[AXIS_RTRIGGER] < 0) {
                scoop_state.data = "b";
            }

            // Arm
            if (raw->axes[AXIS_LTRIGGER] < 0) {
                arm_state.data = "b";
            }
            else if (raw->buttons[BUTTON_LBUMPER]) {
                arm_state.data = "f";
            }

            // Bucket
            if (raw->buttons[BUTTON_Y]) {
                bucket_state.data = "b";
            }
            else if (raw->buttons[BUTTON_X]) {
                bucket_state.data = "f";
            }

            break;

        case 2:

            if (raw->axes[AXIS_DPAD_Y] < -0.5) {
                autoState = "dumping";
                autoTimer.start();
            }

            if (raw->axes[AXIS_DPAD_X] > 0.5) {
                autoState = "mining";
                autoTimer.start();
            }

            break;
        }

        // Autonomous handling
        if (autoState == "dumping") {
            dump();
        }
        else if (autoState == "mining") {
            mine();
        }
        else {
            drivetrainPub->publish(drivetrain_states);
            scoopPub->publish(scoop_state);
            armPub->publish(arm_state);
            bucketPub->publish(bucket_state);
        }
    }

    // Auto dump
    void dump()
    {
        cout << "Auto Dump Engaged" << endl;

        bucket_state.data = "f";
        scoop_state.data = "b";
        arm_state.data = "f";

        autoTime = autoTimer.elapsedSeconds();

        if (autoTime < 14) {

            scoopPub->publish(scoop_state);
            armPub->publish(arm_state);
            bucketPub->publish(bucket_state);

        }
        else if (autoTime < 15) {

            drivetrain_states.velocity[0] = -NORMAL_MODE;
            drivetrain_states.velocity[1] = -NORMAL_MODE;

            drivetrainPub->publish(drivetrain_states);
        }
        else {
            autoState = "";
        }
    }

    // Auto mine
    void mine()
    {
        cout << "Auto mine engaged" << endl;

        autoTime = autoTimer.elapsedSeconds();

        drivetrain_states.velocity[0] = NORMAL_MODE;
        drivetrain_states.velocity[1] = NORMAL_MODE;

        if (autoTime < 1) {

            drivetrainPub->publish(drivetrain_states);

        }
        else if (autoTime < 1.5) {

            scoop_state.data = "f";
            scoopPub->publish(scoop_state);

        }
        else if (autoTime < 2) {

            drivetrainPub->publish(drivetrain_states);

        }
        else if (autoTime < 16) {

            arm_state.data = "f";

            if (autoTime >= 9 && autoTime <= 10) {
                scoop_state.data = "b";
                scoopPub->publish(scoop_state);
            }

            if (autoTime >= 15) {
                scoop_state.data = "f";
                scoopPub->publish(scoop_state);
            }

            armPub->publish(arm_state);

        }
        else if (autoTime < 19) {

            arm_state.data = "b";

            direction =
                ((fmod(autoTime, shakePeriod)) <
                 (shakePeriod / 2.0))
                    ? 1
                    : -1;

            drivetrain_states.velocity[0] =
                direction * NORMAL_MODE;

            drivetrain_states.velocity[1] =
                direction * NORMAL_MODE;

            armPub->publish(arm_state);
            drivetrainPub->publish(drivetrain_states);

        }
        else {

            autoState = "";
        }
    }

    void set_teleop(
        const std::shared_ptr<
            interfaces::srv::SetTeleop::Request> request,
        std::shared_ptr<
            interfaces::srv::SetTeleop::Response> response)
    {
        string enabled;

        robotState = request->teleop_enabled;

        if (robotState == 1) {

            RCLCPP_INFO(
                get_logger(),
                "\033[1;35mMANUAL CONTROL:\033[0m "
                "\033[1;32mENABLED\033[0m");

            enabled = "enabled";
        }
        else {

            RCLCPP_INFO(
                get_logger(),
                "\033[1;35mMANUAL CONTROL:\033[0m "
                "\033[1;31mDISABLED\033[0m");

            enabled = "disabled";
        }

        response->message = "Teleop: " + enabled;
        response->success = true;
    }

    // ROS interfaces
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;

    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr drivetrainPub;

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr bucketPub;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr armPub;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr scoopPub;

    rclcpp::Service<interfaces::srv::SetTeleop>::SharedPtr service;

    // Robot state
    int robotState = 0;
    string autoState = "";

    // Auto variables
    Timer autoTimer;
    double autoTime = 0.0;

    int direction = 1;
    double shakePeriod = 0.5;

    // Shared messages
    sensor_msgs::msg::JointState drivetrain_states;

    std_msgs::msg::String bucket_state;
    std_msgs::msg::String arm_state;
    std_msgs::msg::String scoop_state;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<Teleop>());

    rclcpp::shutdown();

    return 0;
}
