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
// #include<windows.h>


using namespace std;	

ourSensor_msgs::Accel message;

wittenstein_msgs::wittenstein global_msg;
// float position
float hall1, hall2;
float witt;
int start_time = 0;
int ch = 1;
float curr = 10.0;
float poss = 0.0;
float accel2_x,accel2_y;
double thresh;
int trial = 1;
float poss1, poss2;
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
void schunkCallback(const std_msgs::Float32MultiArray::ConstPtr& msg){

    poss = msg->data[0];
    // poss2 = msg->data[1];
}

// Function to run Python code


int main(int argc, char **argv)
{
    // ros::Time contact, slide, ungrasp, lift;   
    Py_Initialize();

    // Set the program name (equivalent to sys.argv[0])
    Py_SetProgramName(argv[0]);

        // Set sys.argv
    PySys_SetArgv(argc, argv);

    PyObject* module = PyImport_ImportModule("rospy");

    // PythonRunner runner;

	ros::init(argc, argv, "listener_global");
	ros::NodeHandle n;
	ros::Rate rate(1000); // *5 = 2500Hz
    int count = 1;
    double prev = 0.0;
    string objT = "";
	std_msgs::Float32MultiArray grip;
	grip.data.resize(2);
    ros::Subscriber sub_teensy = n.subscribe("chatter", 1, chatterCallback);
	ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
    ros::Subscriber sub_accel = n.subscribe("accel_data", 1, accelCallback);
    ros::Subscriber sub_schunk = n.subscribe("float_grip", 1, schunkCallback);

	ros::Publisher pub = n.advertise<std_msgs::Float32MultiArray>("setData", 1);  

    time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );
	stringstream ss;
	ofstream myfile;
	ss << "/home/rass/Rassul/experiments/hri/"<<now->tm_mday <<"_"<<now->tm_hour <<":"<<now->tm_min<<"_"<<"hri.csv";
	myfile.open (ss.str(), ios::out | ios::app | ios::binary);
    float prevX = 0, prevY = 0, difX = 0, difY = 0;
    // grip.data[0] = 20;
    // grip.data[1] = 15;
    // pub.publish(grip);
    // ros::spin();
    // rate.sleep();
    poss1 = poss;
    int velocity;
    float contact = 0.02, slip = 0.04;
    cout<<"Ready to grasp"<<endl;
    // cout<<"Enter velocity: ";
    // cin>>velocity;
    cout<<endl;
    int i = 0;
    while (i<1000){
        cout<<i<<endl;
        i++; 
        ros::spinOnce();
        rate.sleep();
    }
    float limitHX = accel2_x, limitHY = accel2_y;
    float limitLX = accel2_x, limitLY = accel2_y;
    i = 0;
    prevX = accel2_x;
    prevY = accel2_y;
    while (i<1000){
        cout<<i<<endl;
        if (accel2_x > limitHX) limitHX = accel2_x;
        if (accel2_y > limitHY) limitHY = accel2_y;
        if (accel2_x < limitLX) limitLX = accel2_x;
        if (accel2_y < limitLY) limitLY = accel2_y;
        i++;
        ros::spinOnce();
        rate.sleep();    
    }
  
    
    
    cout<<"Limits: "<<limitHX<<"    "<<limitHY<<";"<<limitLX<<"    "<<limitLY<<endl;
    cout<<"Enter contact thresh: ";
    cin>>contact;
    // // cout<<endl<<"Enter slip thresh: ";
    // // cin>>slip;
    // cout<<endl;
    bool dir = true;
    int j = 0;

    grip.data[0]=10;

    while (ros::ok()){				
        myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<accel2_x<<","<<accel2_y<<","<<poss<<"\n";
        // cout<<"waiting..."<<endl;
        // cout<<accel2_x<<endl;
        // || (accel2_y>prevY+contact || accel2_y<prevY-contact)
        if ((accel2_x>prevX + contact|| accel2_x<prevX-contact) ){
            dir = !dir;

        }
       
        if(dir==true){
            
            cout<<"Open"<<endl;
            
            grip.data[1]=20;
            pub.publish(grip);
            ros::spinOnce();
            rate.sleep(); 
        }
        else{
            
            cout<<"Close"<<endl;
            poss1 = poss1-count*0.01;
            grip.data[1]=0;
            pub.publish(grip);
            ros::spinOnce();
            rate.sleep();

        }
        prevX = accel2_x;
        prevY = accel2_y;
        ros::spinOnce();
        rate.sleep();    
    }
       

    myfile.close();
	return 0;
}