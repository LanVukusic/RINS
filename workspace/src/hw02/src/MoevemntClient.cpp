#include "ros/ros.h"
#include "hw02/Move_service.h"

//#include <cstdlib>
#include <sstream>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "our_client_node");
  ros::NodeHandle n;

  ros::ServiceClient client = n.serviceClient<hw02::Move_service>("move_service_node/move");

  hw02::Move_service srv;

  srv.request.duration = 15;
  //  "circle", "rectangle", "triangle" or "random"
  srv.request.message = "random";

  ros::service::waitForService("move_service_node/move", 1000);
  ROS_INFO("Sending...");

  if (client.call(srv))
  {
    std::cout << "The service returned: " << srv.response.message << std::endl;
  }
  else
  {
    ROS_ERROR("Failed to call service");
    return 1;
  }

  return 0;
}
