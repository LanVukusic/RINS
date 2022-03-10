#include "ros/ros.h"
#include "package01/Myserv.h"

//#include <cstdlib>
#include <sstream>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "our_client_node");
  ros::NodeHandle n;

  ros::ServiceClient client = n.serviceClient<package01::Myserv>("our_service_node/string");

  package01::Myserv srv;

  srv.request.numbers = {1, 2, 3, 4, 5, 6};

  ros::service::waitForService("our_service_node/string", 1000);

  ROS_INFO("Sending...");

  if (client.call(srv))
  {
    ROS_INFO("The service returned: %d", srv.response.sum);
  }
  else
  {
    ROS_ERROR("Failed to call service");
    return 1;
  }

  return 0;
}
