picocom  /dev/ttyACM0 -b 115200

mkdir demo_{11..30}

mkdir demo_{11..30} && touch demo_{11..30}/demo_{11..30}.{cpp,h}


docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm --privileged --net=host dockerp.w0x7ce.eu/microros/micro-ros-agent:$ROS_DISTRO serial --dev /dev/ttyACM1 -v6

