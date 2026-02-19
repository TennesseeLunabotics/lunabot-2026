# Run
You will need to forward display aka
`xhost +si:localuser:$(whoami)`

## Podman
- build `podman build -t unitree_lidar_ros2 .`
- running it ```podman run -it --rm \
  --privileged \
  --network=host \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  unitree_lidar_ros2
```

## Docker
- build `docker build -t unitree_lidar_ros2 .`
- running it ```
docker run -it --rm \
  --privileged \
  --network=host \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  unitree_lidar_ros2
 
## Launch it
Within the container there is a alias that launches it for you. run `ros2launch`
