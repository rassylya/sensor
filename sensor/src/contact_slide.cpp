#include <iostream>
#include <fstream>      // std::ifstream
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <ourSensor_msgs/Accel.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <sensor_msgs/JointState.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
// #include <conio.h>
#include <python2.7/Python.h>

using namespace std;	

ourSensor_msgs::Accel message;
int ch = 1;
float curr = 20.0;
float poss = 20.0;
double accel_x, accel_y;

void accelCallback(const ourSensor_msgs::Accel::ConstPtr& msg){
	message.accel1_x = msg->accel1_x;
	message.accel1_y = msg->accel1_y;
	accel_x = msg->accel1_x;
	accel_y = msg->accel1_y;
}

// void urCallback(const sensor_msgs::JointState::ConstPtr& msg){
// 	message.accel1_x = msg->accel1_x;
// 	message.accel1_y = msg->accel1_y;
// 	accel_x = msg->accel1_x;
// 	accel_y = msg->accel1_y;
// }
	
int main(int argc, char **argv)
{
	ros::init(argc, argv, "listener_global");
	ros::NodeHandle n;
	ros::Rate rate(500); // *5 = 2500Hz
    int count = 1;
    double prev = 0.0;
	std_msgs::Float32MultiArray grip;
	grip.data.resize(2);
	
    // ros::Subscriber sub_teensy = n.subscribe("chatter", 1, chatterCallback);
	// ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
	ros::Publisher pub = n.advertise<std_msgs::Float32MultiArray>("setData", 1);   
	ros::Subscriber sub_accel = n.subscribe("accel_data", 1, accelCallback);

    time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );

	stringstream ss;
	ofstream myfile;
	ss << "/home/rass/Rassul/experiments/"<<now->tm_mday <<"_"<<now->tm_hour <<":"<<now->tm_min<<"_contact-slide-experiment.csv";
	myfile.open (ss.str(), ios::out | ios::app | ios::binary);
	double dif = 0;
    prev = accel_y;

    while(ros::ok()){
        while (poss>=0){
            myfile <<accel_x<<","<<accel_y<<"\n";
            poss = curr-0.001*count;
            grip.data[0] = 20;
            grip.data[1] = poss;
            // cout<<accel_y<<endl;
            // dif = abs(abs(accel_y) - abs(prev));
            cout<<accel_x<<" "<<accel_y<<endl;
            if(abs(accel_y)>=3000){
                cout<<"contact detected, running UR"<<endl;
                break;
            }
            // prev = accel_y;
            count++;
            pub.publish(grip);
            ros::spinOnce();
            rate.sleep();
        }
        char filename[] = "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/test_move.py";
        FILE *cp = fopen(filename, "r");    
        if (!cp)
        {
            cout<<"Not Found"<<endl;
            return 0;
        }

        Py_Initialize();
        PyObject* PyFileObject = PyFile_FromString(filename, (char *)"r");
        PySys_SetArgv(argc, argv);
        PyRun_SimpleFile(PyFile_AsFile(PyFileObject), filename);
        Py_Finalize();
    }
	return 0;
}