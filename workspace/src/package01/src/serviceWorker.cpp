#include "package01/Myserv.h"
#include "ros/ros.h"

#include <string>
#include <iostream>
#include <algorithm>

bool sum(package01::Myserv::Request &req,
         package01::Myserv::Response &res)
{
  int sum = 0;
  for (int i = 0; i < req.numbers.size(); i++)
  {
    sum += req.numbers[i];
  }

  res.sum = sum;

  ROS_INFO("response: %d", res.sum);
  return true;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "our_service_node");
  ros::NodeHandle n;

  ros::ServiceServer service = n.advertiseService("our_service_node/string", sum);
  ROS_INFO("Summation service ready");
  ros::spin();

  return 0;
}
