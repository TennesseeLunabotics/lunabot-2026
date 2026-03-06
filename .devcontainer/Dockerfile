ARG ROS_DISTRO=humble
ARG ROS_DOMAIN_ID=1
ARG ROS_VERSION=2

# ARG BASE_IMAGE=nvcr.io/nvidia/isaac/ros:x86_64-ros2_humble_f247dd1051869171c3fc53bb35f6b907
ARG BASE_IMAGE=althack/ros2:humble-dev

#################################
#   Librealsense Builder Stage  #
#################################
FROM $BASE_IMAGE AS librealsense-builder


# Nav2 Installation
WORKDIR $ROS_WS

RUN apt update \
    && apt install -qq -y --no-install-recommends \
         ros-${ROS_DISTRO}-nav2-bringup \
         ros-${ROS_DISTRO}-navigation2 \
         ros-${ROS_DISTRO}-turtlebot3-gazebo

ENV DEBIAN_FRONTEND=noninteractive
ENV ROS_WS=/opt/ros2_ws

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



ARG LIBRS_VERSION=2.56.4
# Make sure that we have a version number of librealsense as argument
RUN test -n "$LIBRS_VERSION"

# To avoid waiting for input during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Builder dependencies installation
RUN apt update \
    && apt install -qq -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libusb-1.0-0-dev \
    pkg-config \
    libgtk-3-dev \
    libglfw3-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libudev-dev \
    udev \
    v4l-utils \
    libx11-dev \
    curl \
    python3 \
    python3-dev \
    ca-certificates \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    mesa-common-dev \
    && rm -rf /var/lib/apt/lists/*

# Download sources
WORKDIR /usr/src
RUN curl https://codeload.github.com/realsenseai/librealsense/tar.gz/refs/tags/v$LIBRS_VERSION -o librealsense.tar.gz 
RUN tar -zxf librealsense.tar.gz \
    && rm librealsense.tar.gz 
RUN ln -s /usr/src/librealsense-$LIBRS_VERSION /usr/src/librealsense

# Build and install
RUN cd /usr/src/librealsense \
    && mkdir build && cd build \
    && cmake \
    -DCMAKE_C_FLAGS_RELEASE="${CMAKE_C_FLAGS_RELEASE} -s" \
    -DCMAKE_CXX_FLAGS_RELEASE="${CMAKE_CXX_FLAGS_RELEASE} -s" \
    -DCMAKE_INSTALL_PREFIX=/opt/librealsense \
    -DBUILD_GRAPHICAL_EXAMPLES=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DCMAKE_BUILD_TYPE=Release ../ \
    && make -j$(($(nproc))) all \
    && make install 

######################################
#   librealsense Base Image Stage    #
######################################
FROM ${BASE_IMAGE} AS librealsense

# Copy binaries from builder stage
COPY --from=librealsense-builder /opt/librealsense /usr/local/
COPY --from=librealsense-builder /usr/lib/python3/dist-packages/pyrealsense2 /usr/lib/python3/dist-packages/pyrealsense2
COPY --from=librealsense-builder /usr/src/librealsense/config/99-realsense-libusb.rules /etc/udev/rules.d/
COPY --from=librealsense-builder /usr/src/librealsense/config/99-realsense-d4xx-mipi-dfu.rules /etc/udev/rules.d/
ENV PYTHONPATH=$PYTHONPATH:/usr/local/lib

# Install dep packages
RUN rm -f /etc/apt/sources.list.d/yarn.list && \
    rm -f /usr/share/keyrings/yarn-archive-keyring.gpg && \
    curl -fsSL https://dl.yarnpkg.com/debian/pubkey.gpg | gpg --dearmor -o /usr/share/keyrings/yarn-archive-keyring.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/yarn-archive-keyring.gpg] https://dl.yarnpkg.com/debian stable main" > /etc/apt/sources.list.d/yarn.list

RUN apt update \
    && apt install -y --no-install-recommends \
    libusb-1.0-0 \
    yarn \
    udev \
    apt-transport-https \
    ca-certificates \
    curl \
    software-properties-common \
    git \
    build-essential \
    cmake \
    python3-colcon-common-extensions \
    usbutils \
    net-tools \
    vim \
    ros-humble-pcl-conversions \
    ros-humble-pcl-ros \
    && rm -rf /var/lib/apt/lists/*



CMD ["bash"]
