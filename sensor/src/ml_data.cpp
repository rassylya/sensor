#include <iostream>
#include <fstream>      // std::ifstream
#include <ros/ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <ourSensor_msgs/Accel.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <thread>
#include <condition_variable>
// #include "contact_slide.h"
#define PY_SSIZE_T_CLEAN
#include <python2.7/Python.h>



using namespace std;	

ourSensor_msgs::Accel message;
float curr = 20.0;
float poss = 20.0;
int trial = 1;
std::mutex file_mutex;
std::condition_variable cv;
bool is_running = true;
std::ofstream myfile;


void slide(PyObject* module) {
    FILE* script_file = fopen("/home/rass/Rassul/sensor_ws/src/sensor/src/move_up_down.py", "r");
    if (script_file) {
        PyRun_SimpleFile(script_file, "/home/rass/Rassul/sensor_ws/src/sensor/src/move_up_down.py");
        fclose(script_file);
    } else {
        std::cerr << "Failed to open the Python script file\n";
    }
    is_running = false;
    cv.notify_one();
}
//add lift

// void record(PyObject* module, PyObject* pyad) {
//     FILE* script_file = fopen("/home/rass/Rassul/sensor_ws/src/sensor/src/accel_ml.py", "r");
//     if (script_file) {
//         PyRun_SimpleFile(script_file, "/home/rass/Rassul/sensor_ws/src/sensor/src/accel_ml.py");
//         fclose(script_file);
//     } else {
//         std::cerr << "Failed to open the Python script file\n";
//     }
// }
void run_data()
{
    int data = system("python /home/rass/Rassul/sensor_ws/src/sensor/src/accel_ml.py");
}

int main(int argc, char** argv) {
    // Open a file for writing
//    myfile.open("/home/rass/Rassul/experiments/texture_recognition/TEST.csv");
    // if (!myfile.is_open()) {
    //     std::cerr << "Failed to open file for writing\n";
    //     return 1;
    // }
    Py_Initialize();
    Py_SetProgramName(argv[0]);
    PySys_SetArgv(argc, argv);

    PyObject* module = PyImport_ImportModule("rospy");
    // PyObject* pyaudio = PyImport_ImportModule("pyaudio");

    if (!module) {
        std::cerr << "Failed to import the Python module\n";
        Py_Finalize();
        return 1;
    }

    
    ros::init(argc, argv, "listener_global"); 
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<std_msgs::Float32MultiArray>("setData", 1);   

    std_msgs::Float32MultiArray grip;
    grip.data.resize(2);
    grip.data[0] = 20;
    grip.data[1] = 20;
    pub.publish(grip);
    ros::spinOnce();
    // int home_pos = system("python home_pos.py");
    while(true){
        cout<<"Enter poss between 13 and 20"<<endl;
        cin>>poss;
        if(poss>=13 || poss<=20) break;
    }
    
    grip.data[0] = 20;
    grip.data[1] = poss;
    pub.publish(grip);
    ros::spinOnce();
    std::thread data_thread(run_data);
    std::thread slide_thread(slide, module);
    // int data = system("python /home/rass/Rassul/sensor_ws/src/sensor/src/accel_ml.py");
    
    slide_thread.join();
    data_thread.join();
    Py_DECREF(module);
    Py_Finalize();
    myfile.close();
    return 0;
}
