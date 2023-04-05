#include <iostream>
#include <fstream>      // std::ifstream
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <ourSensor_msgs/Accel.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <wittenstein_msgs/wittenstein.h>
#include <sensor_msgs/JointState.h>
#include <ourSensor_msgs/Accel.h>
#include <string>
#include <stdio.h>
#include <fstream>

using namespace std;	

ourSensor_msgs::Accel message;
wittenstein_msgs::wittenstein global_msg;
float hall1, hall2;
float witt;
int start_time = 0;
int ch = 1;
float curr = 0.0;
int trial = 1;
float poss = 0.0;

double accel_x, accel_y;

void accelCallback(const ourSensor_msgs::Accel::ConstPtr& msg){
	message.accel1_x = msg->accel1_x;
	message.accel1_y = msg->accel1_x;
	accel_x = msg->accel1_x;
	accel_y = msg->accel1_y;
}

void chatterCallback(const std_msgs::Float32MultiArray::ConstPtr& msg2)
{
	hall1 = msg2->data[0];
    hall2 = msg2->data[1];
}	

void callback(const wittenstein_msgs::wittenstein::ConstPtr& msg){
	global_msg.fx = msg->fx;
	global_msg.fy = msg->fy;
	global_msg.fz = msg->fz;
	witt = msg->fz;
}


int main(int argc, char **argv)
{
	ros::init(argc, argv, "listener_global");
	ros::NodeHandle n;
	ros::Rate rate(500); // *5 = 2500Hz
    int count = 0;
	std_msgs::Float32MultiArray grip;
	grip.data.resize(2);
	
    ros::Subscriber sub_teensy = n.subscribe("chatter", 1, chatterCallback);
	ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
	ros::Publisher pub = n.advertise<std_msgs::Float32MultiArray>("setData", 1);   
	ros::Subscriber sub_accel = n.subscribe("accel_data", 1, accelCallback);

    time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );
	stringstream ss;

	ofstream myfile;
	ss << "/home/rass/Rassul/experiments/"<<now->tm_mday <<"_"<<now->tm_hour <<":"<<now->tm_min<<"_grasp-experiment.csv";
	myfile.open (ss.str(), ios::out | ios::app | ios::binary);
	
    while(ros::ok()){
		
		curr = 20-0.005*ch;
		grip.data[0] = 20;
		grip.data[1] = curr;
		ch++;
		pub.publish(grip);
		ros::spinOnce();
		rate.sleep();
		if(curr<=8){
			break;
		}
	}
		
	cout<<"Collecting Data"<<endl;
			// cout<<"Start time"<<endl;
	while (start_time<50000 && ros::ok()){				
		myfile <<ros::Time::now()<<","<<start_time<<","<<witt<<","<<hall1<<","<<hall2<<","<<accel_x<<","<<accel_y<<"\n";
		poss = curr-0.0001*trial;
		// poss = curr-0.001*trial;
		grip.data[0] = 20;
		grip.data[1] = poss;
		pub.publish(grip);
		trial++;
		start_time++;
		ros::spinOnce();
		rate.sleep();
	}
	cout<<"Grip release"<<endl;
	start_time = 0;
	trial = 1;
	curr = poss	;
	cout<<poss<<endl;   
	while(start_time<20000 && ros::ok())
	{
		cout<<start_time<<endl;
		myfile <<ros::Time::now()<<","<<start_time<<","<<witt<<","<<hall1<<","<<hall2<<","<<accel_x<<","<<accel_y<<"\n";
		poss = curr+0.0001*trial;
		// poss = curr+0.001*trial;
		grip.data[0] = 20;
		grip.data[1] = poss;
		pub.publish(grip);
		trial++;
		start_time++;
		ros::spinOnce();
		rate.sleep();
		if(poss>=6){
			myfile.close();
			break;
		}
	}
	cout<<"Data collected"<<endl;
	grip.data[0] = 20;
	grip.data[1] = 20;
	pub.publish(grip);
	return 0;
}