#include "ros/ros.h"
#include "package01/Greeting.h"

#include <string>
#include <iostream>
#include <algorithm>

void messageReceived(const package01::Greeting::ConstPtr &msg)
{ // The parameter is the message type
  ROS_INFO("I recieved: %d:: %s", msg->number, msg->content.c_str());
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "our_service_node");
  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("our_pub1/chat1", 1000, &messageReceived); // First parameter is the topic

  ROS_INFO("Receiver up");
  ros::spin();

  return 0;
}