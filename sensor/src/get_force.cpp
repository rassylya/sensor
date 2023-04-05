#include <iostream>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <wittenstein_msgs/wittenstein.h>
#include <sensor_msgs/JointState.h>

using namespace std;

wittenstein_msgs::wittenstein global_msg;
float witt;
//int f = 0;

void callback(const wittenstein_msgs::wittenstein::ConstPtr& msg){
	global_msg.fx = msg->fx;
	global_msg.fy = msg->fy;
	global_msg.fz = msg->fz;
	witt = msg->fz;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "get_force");
	ros::NodeHandle n;
	ros::Rate rate(500);
 
	ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
	
	while(ros::ok()){
		std_msgs::Float32 data_out;
		data_out.data= witt;	
		//if(f == 0){
			ROS_INFO_STREAM(witt);
			//f = 1;
			//return 0;
		//}
		ros::spinOnce();
		rate.sleep();	
	}

	return 0;
}
