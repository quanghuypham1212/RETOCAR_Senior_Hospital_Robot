from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os
import xacro

def generate_launch_description():

    # 1. Định nghĩa đường dẫn tới file xacro
    package_name = 'my_robot_description'
    pkg_share = get_package_share_directory(package_name)
    xacro_file = xacro_file = '/home/huy/ros2_ws/src/my_robot_description/urdf/robot.urdf.xacro'

    # 2. Xử lý file xacro thành dữ liệu robot_description (dạng XML)
    robot_description_config = xacro.process_file(xacro_file)
    robot_desc = robot_description_config.toxml()

    #rviz_config_path = os.path.join(pkg_share, 'rviz', 'view_robot.rviz')

    return LaunchDescription([
        
    # 3. Node robot_state_publisher: Nhận dữ liệu URDF để phát các frame (TF)
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            output='screen',
            parameters=[{'robot_description': robot_desc}] # Truyền dữ liệu vào đây
        ),

    # 4. Node joint_state_publisher_gui: Để bạn có thanh trượt điều khiển các khớp (nếu cần)
        Node(
            package='joint_state_publisher_gui',
            executable='joint_state_publisher_gui'
        ),

       # 5. Node RViz2
       # Node(
       #     package='rviz2',
      #      executable='rviz2',
        #    output='screen',
         #   arguments=['-d', rviz_config_path]

       # )
    ])
