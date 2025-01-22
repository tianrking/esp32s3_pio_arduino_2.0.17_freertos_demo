#!/usr/bin/env python3
import sys
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32MultiArray
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QSlider, QPushButton
from PyQt5.QtCore import Qt

class CustomSlider(QSlider):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.is_dragging = False

    def mousePressEvent(self, event):
        super().mousePressEvent(event)
        self.is_dragging = True

    def mouseReleaseEvent(self, event):
        super().mouseReleaseEvent(event)
        self.is_dragging = False
        # 通知父窗口滑块已释放
        self.parent().sliderReleased.emit(self.value())

class ArmControlGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()
        
        # Initialize ROS node
        rclpy.init()
        self.node = Node('arm_control_gui')
        
        # Create publisher and subscriber
        self.joint_pub = self.node.create_publisher(Int32MultiArray, 'xiao/joint_commands', 10)
        self.joint_sub = self.node.create_subscription(Int32MultiArray, 'xiao/joint_states', self.joint_state_callback, 10)
        
        # Timer for processing ROS messages
        self.node.create_timer(0.1, self.timer_callback)  # 10Hz timer

    def initUI(self):
        self.setWindowTitle('XIAO Arm Control')
        self.setGeometry(100, 100, 800, 400)

        # Create central widget and layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout(central_widget)

        # Create sliders
        self.sliders = []
        self.value_labels = []
        
        for i in range(6):
            # Create horizontal layout for each joint control
            h_layout = QHBoxLayout()
            
            # Joint label
            joint_label = QLabel(f'Joint {i+1}:')
            h_layout.addWidget(joint_label)
            
            # Slider container widget for handling release signal
            slider_container = QWidget()
            
            # Slider
            slider = CustomSlider(Qt.Horizontal)
            slider.setMinimum(0)
            slider.setMaximum(4095)
            slider.setValue(2048)  # Center position
            slider.setTickPosition(QSlider.TicksBelow)
            slider.setTickInterval(500)
            # 仅在值改变时更新显示，不发送命令
            slider.valueChanged.connect(lambda v, i=i: self.update_value_label(i, v))
            # 在释放时发送命令
            slider.sliderReleased.connect(self.send_joint_commands)
            self.sliders.append(slider)
            h_layout.addWidget(slider)
            
            # Value label
            value_label = QLabel('2048')
            self.value_labels.append(value_label)
            h_layout.addWidget(value_label)
            
            layout.addLayout(h_layout)

        # Control buttons layout
        button_layout = QHBoxLayout()
        
        # Add center position button
        center_button = QPushButton('Center All Joints')
        center_button.clicked.connect(self.center_all_joints)
        button_layout.addWidget(center_button)
        
        # Add reset button
        reset_button = QPushButton('Reset to Current Position')
        reset_button.clicked.connect(self.reset_to_current)
        button_layout.addWidget(reset_button)
        
        layout.addLayout(button_layout)

        # Status label
        self.status_label = QLabel('Status: Ready')
        layout.addWidget(self.status_label)

    def update_value_label(self, index, value):
        """只更新显示的数值，不发送命令"""
        self.value_labels[index].setText(str(value))

    def send_joint_commands(self):
        """发送关节命令到ROS话题"""
        msg = Int32MultiArray()
        msg.data = [slider.value() for slider in self.sliders]
        self.joint_pub.publish(msg)
        self.status_label.setText('Status: Command Sent - ' + str(msg.data))

    def joint_state_callback(self, msg):
        """更新显示当前关节状态"""
        for i, value in enumerate(msg.data):
            if i < len(self.sliders):
                self.sliders[i].blockSignals(True)
                self.sliders[i].setValue(value)
                self.value_labels[i].setText(str(value))
                self.sliders[i].blockSignals(False)

    def center_all_joints(self):
        """将所有关节移动到中心位置"""
        for slider in self.sliders:
            slider.setValue(2048)
        self.send_joint_commands()
        self.status_label.setText('Status: All Joints Centered')

    def reset_to_current(self):
        """重置滑块到当前实际位置"""
        msg = Int32MultiArray()
        msg.data = [slider.value() for slider in self.sliders]
        self.joint_pub.publish(msg)
        self.status_label.setText('Status: Reset to Current Position')

    def timer_callback(self):
        rclpy.spin_once(self.node, timeout_sec=0)

    def closeEvent(self, event):
        self.node.destroy_node()
        rclpy.shutdown()
        event.accept()

def main():
    app = QApplication(sys.argv)
    gui = ArmControlGUI()
    gui.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()