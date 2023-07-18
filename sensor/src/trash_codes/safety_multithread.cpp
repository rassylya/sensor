#include <iostream>
#include <fstream>      // std::ifstream
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <ourSensor_msgs/Accel.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <wittenstein_msgs/wittenstein.h>
#include <sensor_msgs/JointState.h>
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
float hall1, hall2;
float witt;
int start_time = 0;
int ch = 1;
float curr = 20.0;
float poss = 20.0;
int trial = 1;
std::mutex file_mutex;
std::condition_variable cv;
bool is_running = true;
std::ofstream myfile;
float accel1_x, accel1_y, accel2_x, accel2_y;

// std::vector<float> data;

void accelCallback(const ourSensor_msgs::Accel::ConstPtr& msg) {
    ros::Rate rate(750); // *5 = 2500Hz

    cout<<"Accel Callback"<<endl;
    accel1_x = msg->accel1_x;
    accel1_y = msg->accel1_y;
    accel2_x = msg->accel2_x;
    accel2_y = msg->accel2_y;
    std::unique_lock<std::mutex> lock(file_mutex);  
    cout<<msg->accel1_x<<endl;
    // Check if the file is open before writing data
    if (myfile.is_open()) {
        myfile << accel1_x <<"," << accel1_y <<"," << accel2_x <<"," << accel2_y << ";";
        rate.sleep();

        myfile.flush(); 
    } else {
        std::cerr << "Failed to write data to file: file is not open\n";
    }

    lock.unlock();

}

void wittCallback(const wittenstein_msgs::wittenstein::ConstPtr& msg){
        ros::Rate rate(500); // *5 = 2500Hz

    cout<<"Witt Callback";
	global_msg.fz = msg->fz;
	witt = msg->fz;
    std::unique_lock<std::mutex> lock(file_mutex);  
    if (myfile.is_open()) {
        myfile << witt << ";";
        // data.push_back(witt)
        rate.sleep();

        myfile.flush();
    }else{ 
        std::cerr << "Failed to write data to file: file is not open\n";
    }
    lock.unlock();

}
////add chatter callback
void hallCallback(const std_msgs::Float32MultiArray::ConstPtr& msg2){
    ros::Rate rate(500); // *5 = 2500Hz
    hall1 = msg2->data[0];
    hall2 = msg2->data[1];
    cout<<"Hall Callback";
    std::unique_lock<std::mutex> lock(file_mutex);  
    if (myfile.is_open()) {
        myfile << hall1 <<","<<hall2<<"\n";
        rate.sleep();
        myfile.flush();
    }else{ 
        std::cerr << "Failed to write data to file: file is not open\n";
    }
    lock.unlock();

}

void write_data_to_file(ros::NodeHandle& nh) {
    if (!myfile.is_open()) {
        std::cerr << "Failed to open file for writing\n";
        return;
    }
    cout<<"Write to file"<<endl;
    ros::Subscriber acc = nh.subscribe("accel_data", 1, accelCallback);
    ros::Subscriber wit = nh.subscribe("wittenstein_topic", 1, wittCallback);
    ros::Subscriber hall = nh.subscribe("chatter", 1, hallCallback);
    ros::spin();
    while (ros::ok() && is_running) { 
        std::unique_lock<std::mutex> lock(file_mutex);
        cv.wait(lock); 
    }
}

void slide(PyObject* module) {
    if (!module) {
        std::cerr << "Failed to import the Python module\n";
        Py_Finalize();
        return;
    }
    FILE* script_file = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py", "r");
    if (script_file) {
        PyRun_SimpleFile(script_file, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py");
        fclose(script_file);
    } else {
        std::cerr << "Failed to open the Python script file\n";
    }
    is_running = false;
    cv.notify_one();
}
//add lift

int main(int argc, char** argv) {
    // Open a file for writing
   myfile.open("/home/rass/Rassul/experiments/grasp_slide/TEST.csv");
    if (!myfile.is_open()) {
        std::cerr << "Failed to open file for writing\n";
        return 1;
    }
    Py_Initialize();
    Py_SetProgramName(argv[0]);
    PySys_SetArgv(argc, argv);

    PyObject* module = PyImport_ImportModule("rospy");
    if (!module) {
        std::cerr << "Failed to import the Python module\n";
        Py_Finalize();
        return 1;
    }

    ros::init(argc, argv, "listener_global"); 
    ros::NodeHandle nh;
    std::thread python_thread(slide, module);
    write_data_to_file(nh);
    python_thread.join();

    //add grasp and file wring
    
    //add lift thread

    //add release



    // Clean up Python interpreter and close file
    Py_DECREF(module);
    Py_Finalize();
    myfile.close();
    return 0;
}
