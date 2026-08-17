import rclpy
from rclpy.node import Node
from lunabot_constants import shovel as constants
from . import arduino_client
from std_msgs.msg import String


class shovel_node(Node):

    def __init__(self):
        super().__init__('shovel_node')

        self.arduino = arduino_client.ArduinoClient(port="/dev/ttyUSB0")

        self.arduino.reset()
        self.arm_subscription = self.create_subscription(
            String,
            '/shovel/arm_cmd',
            self.arm_callback,
            10)
        self.scoop_subscription = self.create_subscription(
            String,
            '/shovel/scoop_cmd',
            self.scoop_callback,
            10)
        self.bucket_subscription = self.create_subscription(
            String,
            '/shovel/bucket_cmd',
            self.bucket_callback,
            10)

        self.bucket_subscription  # prevent unused variable warning
        self.arm_subscription  # prevent unused variable warning

    def arm_callback(self, msg):
        feedback = self.arduino.read_analog();

        if (len(feedback) >= 3) and feedback[0] == 'A':
            arm_diff = int(feedback[3]) - int(feedback[1])
            #arm_diff = 0
            #compensation_value = constants.ARM_DUTY_CYCLE
            compensation_value = min(abs(arm_diff) * constants.ARM_P + constants.ARM_DUTY_CYCLE, 255)
        else:
            arm_diff = 0
            compensation_value = constants.ARM_DUTY_CYCLE
        if(bool(arm_diff > 0) ^ bool(msg.data == constants.ARM_BACKWARD)):
            pwm_values = [compensation_value, constants.ARM_DUTY_CYCLE]
        else:
            pwm_values = [constants.ARM_DUTY_CYCLE, compensation_value]
        
        if(msg.data == constants.ARM_FORWARD):
            self.arduino.digital_write(constants.ARM_DIR, [1, 1]);
        elif(msg.data == constants.ARM_BACKWARD):
            self.arduino.digital_write(constants.ARM_DIR, [0, 0]);
        else:
            pwm_values = [0, 0];
        self.arduino.pwm_write(constants.ARM_PWM, pwm_values);

    def bucket_callback(self, msg):
        if(msg.data == constants.BUCKET_FORWARD):
            self.arduino.pwm_write(constants.BUCKET_PWM, constants.BUCKET_DUTY_CYCLE);
            self.arduino.digital_write(constants.BUCKET_DIR, 1);
        elif(msg.data == constants.BUCKET_BACKWARD):
            self.arduino.pwm_write(constants.BUCKET_PWM, constants.BUCKET_DUTY_CYCLE);
            self.arduino.digital_write(constants.BUCKET_DIR, 0);
        else:
            self.arduino.pwm_write(constants.BUCKET_PWM, 0)

    def scoop_callback(self, msg):
        if(msg.data == constants.SCOOP_FORWARD):
            self.arduino.pwm_write(constants.SCOOP_PWM, constants.SCOOP_DUTY_CYCLE);
            self.arduino.digital_write(constants.SCOOP_DIR, 0);
        elif(msg.data == constants.SCOOP_BACKWARD):
            self.arduino.pwm_write(constants.SCOOP_PWM, constants.SCOOP_DUTY_CYCLE);
            self.arduino.digital_write(constants.SCOOP_DIR, 1);
        else:
            self.arduino.pwm_write(constants.SCOOP_PWM, 0);

def main(args=None):
    rclpy.init(args=args)
    shovelNode = shovel_node()

    rclpy.spin(shovelNode)
    self.arduino.shutdown()
    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    shovelNode.destroy_node()


if __name__ == '__main__':
    main()
