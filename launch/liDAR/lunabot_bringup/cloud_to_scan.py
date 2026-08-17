import rclpy
from rclpy.node import Node
from sensor_msgs.msg import PointCloud2, LaserScan
from rclpy.clock import Clock
import math
import struct

class CloudToScan(Node):
    def __init__(self):
        super().__init__('cloud_to_scan')
        self.sub = self.create_subscription(PointCloud2, '/unilidar/cloud', self.callback, 10)
        self.pub = self.create_publisher(LaserScan, '/scan', 10)

    def callback(self, msg):
        scan = LaserScan()
        scan.header.stamp = self.get_clock().now().to_msg()
        scan.header.frame_id = 'base_link'
        scan.angle_min = -math.pi
        scan.angle_max = math.pi
        scan.angle_increment = math.pi / 180.0
        scan.range_min = 0.1
        scan.range_max = 100.0
        num_ranges = int((scan.angle_max - scan.angle_min) / scan.angle_increment)
        ranges = [float('inf')] * num_ranges
        
        point_step = msg.point_step
        for i in range(msg.width):
            offset = i * point_step
            x = struct.unpack_from('f', msg.data, offset)[0]
            y = struct.unpack_from('f', msg.data, offset + 4)[0]
            angle = math.atan2(y, x)
            dist = math.sqrt(x*x + y*y)
            idx = int((angle - scan.angle_min) / scan.angle_increment)
            if 0 <= idx < num_ranges:
                if dist < ranges[idx]:
                    ranges[idx] = dist
        
        scan.ranges = ranges
        self.pub.publish(scan)

def main():
    rclpy.init()
    node = CloudToScan()
    rclpy.spin(node)

if __name__ == '__main__':
    main()
