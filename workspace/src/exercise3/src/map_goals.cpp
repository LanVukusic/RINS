#include "ros/ros.h"

#include <nav_msgs/GetMap.h>
// #include <move_base_msgs/.h>
#include <actionlib_msgs/GoalStatusArray.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf2/transform_datatypes.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void moveTo(int x, int y);

Mat cv_map;
float map_resolution = 0;
geometry_msgs::TransformStamped map_transform;

ros::Publisher goal_pub;
ros::Subscriber map_sub;
ros::Subscriber state_sub;

uint8_t status = 0;

void stateCallback(const actionlib_msgs::GoalStatusArrayPtr &msg)
{
    status = msg->status_list[0].status;
    // cout << "Status: " << status << endl;
}

void mapCallback(const nav_msgs::OccupancyGridConstPtr &msg_map)
{
    int size_x = msg_map->info.width;
    int size_y = msg_map->info.height;

    if ((size_x < 3) || (size_y < 3))
    {
        ROS_INFO("Map size is only x: %d,  y: %d . Not running map to image conversion", size_x, size_y);
        return;
    }

    // resize cv image if it doesn't have the same dimensions as the map
    if ((cv_map.rows != size_y) && (cv_map.cols != size_x))
    {
        cv_map = cv::Mat(size_y, size_x, CV_8U);
    }

    map_resolution = msg_map->info.resolution;
    map_transform.transform.translation.x = msg_map->info.origin.position.x;
    map_transform.transform.translation.y = msg_map->info.origin.position.y;
    map_transform.transform.translation.z = msg_map->info.origin.position.z;

    map_transform.transform.rotation = msg_map->info.origin.orientation;

    // tf2::poseMsgToTF(msg_map->info.origin, map_transform);

    const std::vector<int8_t> &map_msg_data(msg_map->data);

    unsigned char *cv_map_data = (unsigned char *)cv_map.data;

    // We have to flip around the y axis, y for image starts at the top and y for map at the bottom
    int size_y_rev = size_y - 1;

    for (int y = size_y_rev; y >= 0; --y)
    {

        int idx_map_y = size_x * (size_y - y);
        int idx_img_y = size_x * y;

        for (int x = 0; x < size_x; ++x)
        {

            int idx = idx_img_y + x;

            switch (map_msg_data[idx_map_y + x])
            {
            case -1:
                cv_map_data[idx] = 127;
                break;

            case 0:
                cv_map_data[idx] = 255;
                break;

            case 100:
                cv_map_data[idx] = 0;
                break;
            }
        }
    }
}

void mouseCallback(int event, int x, int y, int, void *data)
{

    if (event != EVENT_LBUTTONDOWN || cv_map.empty())
        return;

    moveTo(x, y);
}

void moveTo(int x, int y)
{

    int v = (int)cv_map.at<unsigned char>(y, x);

    if (v != 255)
    {
        ROS_WARN("Unable to move to (x: %d, y: %d), not reachable", x, y);
        return;
    }

    ROS_INFO("resolution is %f", map_resolution);
    geometry_msgs::Point pt;
    geometry_msgs::Point transformed_pt;

    pt.x = (float)x * map_resolution;
    pt.y = (float)(cv_map.rows - y) * map_resolution;
    pt.z = 0.0;

    tf2::doTransform(pt, transformed_pt, map_transform);

    // geometry_msgs::Point transformed = map_transform * pt;

    geometry_msgs::PoseStamped goal;

    goal.header.frame_id = "map";
    goal.pose.orientation.w = 1;
    goal.pose.position.x = transformed_pt.x;
    goal.pose.position.y = transformed_pt.y;
    goal.header.stamp = ros::Time::now();

    ROS_INFO("Moving to (x: %f, y: %f)", transformed_pt.x, transformed_pt.y);

    goal_pub.publish(goal);
}

int xs[] = {304, 276, 246, 233, 265};
int ys[] = {276, 255, 217, 260, 284};

int main(int argc, char **argv)
{

    ros::init(argc, argv, "map_goals");
    ros::NodeHandle n;
    cout << "hey" << endl;

    state_sub = n.subscribe("/move_base/status", 1, &stateCallback);
    map_sub = n.subscribe("map", 10, &mapCallback);
    goal_pub = n.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal", 10);

    // gui window
    // namedWindow("Map");
    // setMouseCallback("Map", mouseCallback, NULL);

    bool run = true;
    ros::Rate rate(10);
    int i = 0;
    bool in_move = false;
    while (ros::ok())
    {
        ros::spinOnce();

        if (status == 3 && !in_move)
        {
            if (i == 5)
            {
                exit(0);
            }
            in_move = true;
            moveTo(xs[i], ys[i]);
            i++;
        }

        if (in_move && status == 1)
        {
            in_move = false;
        }

        rate.sleep();
    }
    return 0;
}
