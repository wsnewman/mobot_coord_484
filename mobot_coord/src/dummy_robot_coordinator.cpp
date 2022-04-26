#include <ros/ros.h>
#include <mobot_coord/MobotCoord.h> // this message type is defined in the robot_coord package
#include <geometry_msgs/PoseStamped.h>
#include <iostream>
#include <string>
using namespace std;


int main(int argc, char **argv) {
    ros::init(argc, argv, "dummy_mobot_coordinator");
    ros::NodeHandle n;
    ros::ServiceClient perception_client = n.serviceClient<mobot_coord::MobotCoord>("perception_service");
    ros::ServiceClient navigation_client = n.serviceClient<mobot_coord::MobotCoord>("navigation_service");
    ros::ServiceClient manipulation_client = n.serviceClient<mobot_coord::MobotCoord>("manipulation_service");    
        
    mobot_coord::MobotCoord mobot_srv;
    
    ROS_INFO("attempting to send request to navigation service to go to station 1");
    mobot_srv.request.request_code= mobot_coord::MobotCoordRequest::GO_TO_STATION_1;
    navigation_client.call(mobot_srv);
    int response = mobot_srv.response.response_code;
    if (!response) {ROS_WARN("navigator responded with failure!"); }
    else {ROS_INFO("navigator responded with success");}
    
    
    ROS_INFO("attempting request to perception service: ");
    mobot_srv.request.request_code= mobot_coord::MobotCoordRequest::GET_BLOCK_COORDS; //not really necessary; perception service can assume this request
    double x,y,z;

   if (perception_client.call(mobot_srv)) {
   	x = mobot_srv.response.perceived_block_pose.pose.position.x;
   	y = mobot_srv.response.perceived_block_pose.pose.position.y;
   	z = mobot_srv.response.perceived_block_pose.pose.position.z;   	
   	ROS_INFO("perception service returned values x,y,z = %f, %f, %f",x,y,z);

        } else {
            ROS_ERROR("Failed to call service perception_service");
            return 1; //crash and burn...could use better error handling!
        }
    response = mobot_srv.response.response_code;
    if (!response) {ROS_WARN("perception responded with failure!"); }
    else {ROS_INFO("perception responded with success"); }     
        
        
    ROS_INFO("attempting request to manipulation service to grab the block");
        mobot_srv.request.request_code = mobot_coord::MobotCoordRequest::GRAB_BLOCK; 
        mobot_srv.request.grasp_block_pose = mobot_srv.response.perceived_block_pose; //just copy over the perceived block coords as grasp coords
        manipulation_client.call(mobot_srv);
    
    return 0;
}
