#!/usr/bin/env python3
import rclpy
from rclpy.node import Node 
from example_interfaces.msg import String

class Subsciber(Node):
    def __init__(self):
          super().__init__("My_subscriber")
          self.subscriber_ = self.create_subscription(String,"data",self.callback_data,10)
    
    def callback_data(self, msg: String):
         self.get_logger().info(msg.data)
    

def main(args=None):
     rclpy.init(args=args)
     node = Subsciber()
     rclpy.spin(node)
     rclpy.shutdown()
    
if __name__=="__main__":
     main()

