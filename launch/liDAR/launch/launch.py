from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():
    return LaunchDescription([

        Node(
            package='tf2_ros',
            executable='static_transform_publisher',
            name='static_transform_publisher',
            arguments=['--x', '0', '--y', '0', '--z', '0',
                      '--yaw', '0', '--pitch', '0', '--roll', '0',
                      '--frame-id', 'odom', '--child-frame-id', 'base_link']
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                '/opt/ros2_ws/install/unitree_lidar_ros2/share/unitree_lidar_ros2/launch.py'
            )
        ),

        Node(
            package='lunabot_bringup',
            executable='cloud_to_scan',
            name='cloud_to_scan',
            output='screen'
        ),

        Node(
            package='slam_toolbox',
            executable='async_slam_toolbox_node',
            name='slam_toolbox',
            output='screen',
            parameters=[{
                'odom_frame': 'odom',
                'base_frame': 'base_link',
                'map_frame': 'map',
                'scan_topic': '/scan',
                'use_sim_time': False,
                'transform_timeout': 2.0,
                'tf_buffer_duration': 60.0,
                'map_update_interval': 1.0,
                'minimum_travel_distance': 0.0,
                'minimum_travel_heading': 0.0,
                'throttle_scans': 1
            }]
        ),

    ])
