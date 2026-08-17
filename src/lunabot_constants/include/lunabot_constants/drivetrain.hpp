#pragma once

namespace lunabot_constants::drivetrain {

/// Maximum voltage sent to a drivetrain motor controller. Do not change. If speed needs to be adjusted, change the constant in teleop.hpp
inline constexpr double MOTOR_MAX = 12.0;

/// CAN device IDs for drivetrain motor controller.
inline constexpr int MOTOR_LEFT = 3;
inline constexpr int MOTOR_RIGHT = 2;

inline constexpr int NUM_MOTORS = 2;

}  // namespace lunabot_constants::drivetrain
