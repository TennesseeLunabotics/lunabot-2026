# Lunabot Constants

This package is the central location for robot-wide fixed constants.

- `include/lunabot_constants/controller.hpp` — controller button/axis mappings
- `include/lunabot_constants/drivetrain.hpp` — drivetrain CAN IDs and fixed limits
- `include/lunabot_constants/teleop.hpp` — teleoperation scaling constants
- `lunabot_constants/shovel.py` — shovel I/O mappings, geometry, and fixed control values

Keep implementation-specific constants with the implementation that owns them. For example, Spark CAN protocol values remain in the drivetrain Spark driver. Values expected to change during robot tuning should preferably become ROS parameters rather than constants.
