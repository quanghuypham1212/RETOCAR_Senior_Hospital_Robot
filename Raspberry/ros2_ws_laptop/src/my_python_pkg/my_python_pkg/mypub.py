#!/usr/bin/env python3
import rclpy
from rclpy.node import Node 
from example_interfaces.msg import String

class Publisher(Node):
    def __init__(self):
          super().__init__("My_publisher")
          self.publishers_ = self.create_publisher(String,"data",10)
          self.timer_ = self.create_timer(0.5, self.publish_new)
          self.get_logger().info("Transmit is started")
    
    def publish_new(self):
         msg = String()
         msg.data = "Hello, I am transmitting"
         self.publishers_.publish(msg)
    

def main(args=None):
     rclpy.init(args=args)
     node = Publisher()
     rclpy.spin(node)
     rclpy.shutdown()
    
if __name__=="__main__":
     main()

