#include <iostream>
#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <wittenstein_msgs/wittenstein.h>
using namespace std;

wittenstein_msgs::wittenstein global_msg;
float witt;

void callback(const wittenstein_msgs::wittenstein::ConstPtr& msg){
	global_msg.fx = msg->fx;
	global_msg.fy = msg->fy;
	global_msg.fz = msg->fz;
	witt = msg->fz;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "force_pub");
	ros::NodeHandle n;
	ros::Rate rate(1000); 

	ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
	ros::Publisher force_pub = n.advertise<std_msgs::Float32MultiArray>("force_pub", 1000);

	ROS_INFO("Force publisher launched successfully");

	while(ros::ok()){

		std_msgs::Float32MultiArray w_force;
		w_force.data.resize(3);

		w_force.data[0] = global_msg.fx;
		w_force.data[1] = global_msg.fy;
		w_force.data[2] = global_msg.fz;
		force_pub.publish(w_force);

		ros::spinOnce();
		rate.sleep();	
	
	}
	return 0;
}
