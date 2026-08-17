"""Hardware mappings and fixed geometry/control constants for the shovel."""
# I/O pins are chosen based off of trial and error. Note than many esp32 pins have dual purposes/may not work.

# Linear actuator I/O pins.
ARM_PWM = [26, 27]
ARM_DIR = [16, 17]
ARM_ADC = [35, 32]

# Linear actuator feedback/control conversion.
ARM_INCHES_PER_BIT = 6 / 4096
ARM_P = 4 * 255 * ARM_INCHES_PER_BIT

# Scoop I/O pins.
SCOOP_PWM = 13
SCOOP_DIR = 14
SCOOP_ADC = 33

# Bucket I/O pins.
BUCKET_PWM = 25
BUCKET_DIR = 23

# Default PWM duty cycles.
ARM_DUTY_CYCLE = 200
BUCKET_DUTY_CYCLE = 255
SCOOP_DUTY_CYCLE = 255

# Shovel geometry, in inches.
ARM_LENGTH = 31.5
ARM_LIN_DIST = 6.5
LIN_UNEXTENDED_DIST = 16

# Command values used by shovel topics.
ARM_FORWARD = "f"
ARM_BACKWARD = "b"
BUCKET_FORWARD = "f"
BUCKET_BACKWARD = "b"
SCOOP_FORWARD = "f"
SCOOP_BACKWARD = "b"
