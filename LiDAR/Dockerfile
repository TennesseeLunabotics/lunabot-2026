FROM osrf/ros:humble-desktop

ENV DEBIAN_FRONTEND=noninteractive
ENV ROS_WS=/opt/ros2_ws

# Install extra tools you added manually
RUN apt update && apt install -y \
    git \
    build-essential \
    cmake \
    python3-colcon-common-extensions \
    usbutils \
    net-tools \
    vim \
    && rm -rf /var/lib/apt/lists/*

# workspace
RUN mkdir -p $ROS_WS/src
WORKDIR $ROS_WS/src

# clone sdk
RUN git clone https://github.com/unitreerobotics/unilidar_sdk2.git

# build
WORKDIR $ROS_WS
RUN /bin/bash -c "source /opt/ros/humble/setup.bash && colcon build --packages-select unitree_lidar_ros2"

# Auto source
RUN echo "source /opt/ros/humble/setup.bash" >> /root/.bashrc && \
    echo "source $ROS_WS/install/setup.bash" >> /root/.bashrc && \
    echo "export FASTDDS_BUILTIN_TRANSPORTS=UDPv4" >> /root/.bashrc && \
    echo "alias ros2launch='ros2 launch unitree_lidar_ros2 launch.py'" >> /root/.bashrc

WORKDIR $ROS_WS

CMD ["bash"]
