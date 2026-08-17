from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess, OpaqueFunction
import subprocess
import time

def wait_for_can0_up(context, *args, **kwargs):
    # Poll until can0 is up
    subprocess.run(['sudo','ip', 'link', 'set', 'can0', 'up', 'type', 'can', 'bitrate', '1000000'], capture_output=True, text=True)
    print("Waiting for CAN interface can0 to come up...")
    for _ in range(10):  # Timeout after 30 seconds
        result = subprocess.run(['ip', 'link', 'show', 'can0'], capture_output=True, text=True)
        if 'state UP' in result.stdout:
            print("CAN interface can0 is up.")
            break
        time.sleep(1)
    else:
        print("Timeout: CAN interface can0 is not up after 30 seconds.")

    # Return the drivetrain node to be added to the launch sequence
    return [
        Node(
           package="drivetrain",
            executable="drivetrain_node",
        )
    ]

def generate_launch_description():
    controller_teleop_node = Node(
        package="teleop",
        executable="teleop_node",
    )
    shovel_node = Node(
        package="shovel",
        executable="shovel_node",
    )

    start_server = ExecuteProcess(
        cmd=["python", "-m", "http.server", "8080", "-d" "/home/lunabot/lunabot-2026/controller"],
        output="screen",    )
    
    run_web_bridge = ExecuteProcess(
        cmd=["python", "/home/lunabot/lunabot-2026/controller/web_joy_bridge.py"],
        output="screen",)

    start_camera1 = Node(package="usb_cam", executable="usb_cam_node_exe",
        arguments=['--ros-args', '--params-file' , '/home/lunabot/lunabot-2026/controller/params_1.yaml'], 
        remappings=[('__ns','/usb_cam_0')])
    """
    start_cameras = ExecuteProcess(
        cmd=[

            "ros2",
            "launch",
            "realsense2_camera",
            "rs_multi_camera_launch.py",
            "serial_no1:=_213522252891",
            "serial_no2:=_324422300049",
            "rgb_camera.profile:=_320x240x10",
        ],
        output="screen",
    )
    """
    canable_start_process = ExecuteProcess(
        cmd=[
            "bash",
            "-c",
            "/home/lunabot/lunabot-2026/src/drivetrain/canable_start.sh",
        ],
        output="screen",
    )

    ld = LaunchDescription()

    # Run shell script to start CAN
    #ld.add_action(canable_start_process)


    ld.add_action(shovel_node)
    #ld.add_action(start_server)
    # ld.add_action(run_web_bridge)
    # ld.add_action(start_camera1)
    # Add the controller node
    ld.add_action(controller_teleop_node)
#    ld.add_action(start_cameras)
    # Wait for CAN to come up before launching drivetrain node
    ld.add_action(OpaqueFunction(function=wait_for_can0_up))
    return ld

