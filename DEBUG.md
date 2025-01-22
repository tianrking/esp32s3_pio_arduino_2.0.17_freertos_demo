picocom  /dev/ttyACM0 -b 115200

mkdir demo_{11..30}

mkdir demo_{11..30} && touch demo_{11..30}/demo_{11..30}.{cpp,h}


docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm --privileged --net=host dockerp.w0x7ce.eu/microros/micro-ros-agent:$ROS_DISTRO serial --dev /dev/ttyACM1 -v6

# 发送关节命令

# 查看所有话题
ros2 topic list

# 监听心跳消息
ros2 topic echo /xiao/heartbeat

# 监听关节状态
ros2 topic echo /xiao/joint_states

# 发送关节命令
ros2 topic pub /xiao/joint_commands std_msgs/msg/Int32MultiArray "data: [2000, 2000, 2000, 2000, 2000, 2000]"

