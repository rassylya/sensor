#include <iostream>
#include <fstream>      // std::ifstream
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <ourSensor_msgs/Accel.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <wittenstein_msgs/wittenstein.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <thread>
#include <condition_variable>
// #include "contact_slide.h"
#define PY_SSIZE_T_CLEAN
#include <python2.7/Python.h>
#include <mutex>


using namespace std;	

ourSensor_msgs::Accel message;

wittenstein_msgs::wittenstein global_msg;
// float position
float hall1, hall2;
float witt;
int start_time = 0;
int ch = 1;
float curr = 20.0;
float poss = 20.0;
float accel2_x,accel2_y;
double thresh;
int trial = 1;

std::mutex file_mutex;
std::condition_variable cv;
bool is_running = true;

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

void accelCallback(const ourSensor_msgs::Accel::ConstPtr& msg){

    accel2_x = msg->accel2_x;
    accel2_y = msg->accel2_y;
}

// Function to run Python code


int main(int argc, char **argv)
{
    ros::Time contact, slide, ungrasp, lift;   
    Py_Initialize();

    // Set the program name (equivalent to sys.argv[0])
    Py_SetProgramName(argv[0]);

        // Set sys.argv
    PySys_SetArgv(argc, argv);

    PyObject* module = PyImport_ImportModule("rospy");

    // PythonRunner runner;

	ros::init(argc, argv, "listener_global");
	ros::NodeHandle n;
	ros::Rate rate(500); // *5 = 2500Hz
    int count = 1;
    double prev = 0.0;
    string objT = "";
	std_msgs::Float32MultiArray grip;
	grip.data.resize(2);
    ros::Subscriber sub_teensy = n.subscribe("chatter", 1, chatterCallback);
	ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
    ros::Subscriber sub_accel = n.subscribe("accel_data", 1, accelCallback);
	ros::Publisher pub = n.advertise<std_msgs::Float32MultiArray>("setData", 1);   
    time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );

    float limit;
    cout<<"Enter threshold: "<<endl;
    cin>>limit;
	stringstream ss;
	ofstream myfile;
	ss << "/home/rass/Rassul/experiments/hri/"<<now->tm_mday <<"_"<<now->tm_hour <<":"<<now->tm_min<<"_"<<"hri.csv";
	myfile.open (ss.str(), ios::out | ios::app | ios::binary);
    float limitX = 0, limitY = 0;
    float prevX = 0, prevY = 0;
    cout<<"Enter threshold: "<<endl;
    cout<<"Ready to grasp"<<endl;
    for (int i = 0; i<1000; i++){
        prevX = accel2_x;
        prevY = accel2_y;
        if (prevX > limitX) limitX = prevX;
        if (prevY > limitY) limitY = prevY;
    }
    while (ros::ok()){				
        // myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<*accel1_x<<","<<*accel1_y<<","<<*accel2_x<<","<<*accel2_y<<","<<poss<<"\n";
        if (accel2_x>limitX && accel2_y>limitY){
            limitX = accel2_x;
            limitY = accel2_y;
            while(start_time<5000){
                poss = curr-0.001*count;
        // poss = curr-0.001*trial;
                grip.data[0] = 20;
                grip.data[1] = poss;
                pub.publish(grip);
                    if(accel2_x>=limitX*2 && accel2_y>=limitY*2){
                        grip.data[0] = 20;
                        grip.data[1] = 9;
                        pub.publish(grip);
                        break;
                    }
                count++;
                start_time++;
                // cout<<start_time<<endl;
                ros::spinOnce();
                rate.sleep();

            }    
        }
        
    }
       
    // while(ros::ok()){
    //     myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<*accel1_x<<","<<*accel1_y<<","<<*accel2_x<<","<<*accel2_y<<","<<poss<<"\n";
    //     // Run the second Python script
    //     lift = ros::Time::now();
    //     FILE* script_file2 = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/up_pos.py", "r");
    //     if (script_file2) {
    //         PyRun_SimpleFile(script_file2, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/up_pos.py");
    //         fclose(script_file2);
    //     } else {
    //         std::cerr << "Failed to open the Python script file 2\n";
    //     }
    //     // Clean up and exit
    //     release
    //     grip.data[0] = 10;
    //     grip.data[1] = 20;
    //     pub.publish(grip);      
    //     ros::spinOnce();
    //     rate.sleep();
    //     break;
    // }
    // cout<<contact<<" "<<slide<<" "<<lift<<endl;
    myfile.close();
	return 0;
}