
#include "hw02/Move_service.h"
#include "ros/ros.h"
#include <geometry_msgs/Twist.h>

#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using geometry_msgs::Twist;
std::string prev = "";
ros::Publisher pub;

Twist doCircle(int step)
{
  Twist msg;
  msg.linear.x = 2;
  msg.angular.z = 2;
  return msg;
}

Twist doSquare(int step)
{
  Twist msg;
  msg.linear.x = 0.5;
  if (step % 3 == 0)
  {
    msg.linear.x = 0;
    msg.angular.z = (90 / 360.0) * 2 * 3.1415926;
  }
  return msg;
}

Twist doTriangle(int step)
{
  Twist msg;
  msg.linear.x = 0.5;
  if (step % 3 == 0)
  {
    msg.linear.x = 0;
    msg.angular.z = (120 / 360.0) * 2 * 3.1415926;
  }
  return msg;
}

Twist doRandom(int step)
{
  Twist msg;
  msg.linear.x = (double)(rand() / (double)RAND_MAX);
  msg.angular.z = (double)((rand() / (double)RAND_MAX) - 0.5) * 2;
  return msg;
}

bool handle(hw02::Move_service::Request &req,
            hw02::Move_service::Response &res)
{
  std::cout << "Message: " << res.message << std::endl;
  res.message = prev.c_str();
  prev = req.message;

  Twist (*fun)(int);

  if (req.message == "circle")
  {
    fun = doCircle;
  }
  if (req.message == "square")
  {
    fun = doSquare;
  }
  if (req.message == "triangle")
  {
    fun = doTriangle;
  }
  if (req.message == "random")
  {
    fun = doRandom;
  }

  ros::Rate rate(1);
  Twist msg;
  for (int i = 0; i < req.duration; i++)
  {
    msg = fun(i);
    pub.publish(msg);
    rate.sleep();
  }

  return true;
}

int main(int argc, char **argv)
{
  double scale_linear, scale_angular;
  ros::init(argc, argv, "move_service_node");
  ros::NodeHandle n;
  srand(time(nullptr));

  pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);
  ros::ServiceServer service = n.advertiseService("move_service_node/move", handle);
  ROS_INFO("Movement service up");
  ros::spin();

  return 0;
}
