
#include "ros/ros.h"
#include "package01/Greeting.h"

//#include <cstdlib>
#include <sstream>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "our_client_node");
  ros::NodeHandle n;

  ros::Publisher pub = n.advertise<package01::Greeting>("our_pub1/chat1", 1000); // the message tyoe is inside the angled brackets

  std::stringstream ss;
  package01::Greeting req; // = package01::Greeting();

  ros::Rate rate(1);

  ss << "RInS is the best course in Mordor";
  while (ros::ok())
  {

    req.content = ss.str();
    req.number = 12;

    ROS_INFO("Sending: %s", ss.str().c_str());
    pub.publish(req);
    rate.sleep();
  }

  return 0;
}