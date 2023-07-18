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

#define BUFFER_SIZE 1000

using namespace std;

ourSensor_msgs::Accel accel_msg;
wittenstein_msgs::wittenstein witt_msg;
std_msgs::Float32MultiArray hall_msg;

float buffer[BUFFER_SIZE][7];
int buffer_idx = 0;
std::mutex buffer_mutex;
std::condition_variable buffer_cv;

std::ofstream myfile;
ros::Timer timer;

void timerCallback(const ros::TimerEvent& event) {
    std::unique_lock<std::mutex> lock(buffer_mutex);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        myfile << buffer[i][0] << "," << buffer[i][1] << ","
               << buffer[i][2] << "," << buffer[i][3] << ","
               << buffer[i][4] << "," << buffer[i][5] << ","
               << buffer[i][6] << "\n";
    }
    buffer_idx = 0;
    buffer_cv.notify_all();
}

void accelCallback(const ourSensor_msgs::Accel::ConstPtr& msg) {
    std::unique_lock<std::mutex> lock(buffer_mutex);
    buffer[buffer_idx][0] = msg->accel1_x;
    buffer[buffer_idx][1] = msg->accel1_y;
    buffer[buffer_idx][2] = msg->accel2_x;
    buffer[buffer_idx][3] = msg->accel2_y;
    buffer_idx++;
    if (buffer_idx == BUFFER_SIZE) {
        buffer_cv.wait(lock);
    }
}

void wittCallback(const wittenstein_msgs::wittenstein::ConstPtr& msg) {
    std::unique_lock<std::mutex> lock(buffer_mutex);
    buffer[buffer_idx][4] = msg->fz;
    buffer_idx++;
    if (buffer_idx == BUFFER_SIZE) {
        buffer_cv.wait(lock);
    }
}

void hallCallback(const std_msgs::Float32MultiArray::ConstPtr& msg) {
    std::unique_lock<std::mutex> lock(buffer_mutex);
    buffer[buffer_idx][5] = msg->data[0];
    buffer[buffer_idx][6] = msg->data[1];
    buffer_idx++;
    if (buffer_idx == BUFFER_SIZE) {
        buffer_cv.wait(lock);
    }
}

void write_data_to_file(ros::NodeHandle& nh) {
    ros::Rate rate(500); // *5 = 2500Hz
    ros::Subscriber accel_sub = nh.subscribe("accel_data", 1, accelCallback);
    ros::Subscriber witt_sub = nh.subscribe("wittenstein_topic", 1, wittCallback);
    ros::Subscriber hall_sub = nh.subscribe("chatter", 1, hallCallback);

    timer = nh.createTimer(ros::Duration(0), timerCallback);

    ros::spin();
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
    // is_running = false;
    // cv.notify_one();
}


int main(int argc, char** argv) {
    ros::init(argc, argv, "data_logger");
    ros::NodeHandle nh;

    // open file for writing data
    myfile.open("/home/rass/Rassul/experiments/grasp_slide/TEST.csv");
    Py_Initialize();
    Py_SetProgramName(argv[0]);
    PySys_SetArgv(argc, argv);

    PyObject* module = PyImport_ImportModule("rospy");
    if (!module) {
        std::cerr << "Failed to import the Python module\n";
        Py_Finalize();
        return 1;
    }
    std::thread python_thread(slide, module);

    // start logging data
    std::thread data_logger_thread(write_data_to_file, std::ref(nh));

    // wait for thread to finish
    
    data_logger_thread.join();
    python_thread.join();
    Py_DECREF(module);
    Py_Finalize();
    // close file
    myfile.close();

    return 0;
}