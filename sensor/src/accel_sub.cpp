#include <iostream>
#include <fstream>      // std::ifstream
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <ourSensor_msgs/Accel.h>

using namespace std;	

ourSensor_msgs::Accel message;
// std_msgs::Float64 accel_x, accel_y;
double accel_x, accel_y;
void callback(const ourSensor_msgs::Accel::ConstPtr& msg){
	message.accel1_x = msg->accel1_x;
	message.accel1_y = msg->accel1_y;
	accel_x = msg->accel1_x;
	accel_y = msg->accel1_y;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "listener_global");
	ros::NodeHandle n;
	ros::Rate rate(500); // *5 = 2500Hz

	int count = 0;
	
	ros::Subscriber sub_accel = n.subscribe("accel_data", 1, &callback);

	ofstream myfile("accel_data.csv", ios::out | ios::binary);
	
	while(ros::ok()){
		// cout<<accel_x<<"  "<<accel_y;
		myfile <<ros::Time::now()<<","<<accel_x<<","<<accel_y<<"\n";
		// if(count == 1000){
		// 	myfile.close();
		// 	return 0;
		// }
		// count++;
		ros::spinOnce();
		rate.sleep();			
	}
	myfile.close();
	return 0;
}
