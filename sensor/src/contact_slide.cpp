#include <iostream>
#include <fstream>
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/String.h>
#include <ourSensor_msgs/Accel.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <wittenstein_msgs/wittenstein.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#define PY_SSIZE_T_CLEAN
#include <python2.7/Python.h>

using namespace std;

ourSensor_msgs::Accel message;
wittenstein_msgs::wittenstein global_msg;

int ch = 1;
float curr = 20.0;
float poss = 20.0;
double accel1_y, accel2_y;
int trial = 1;
// string name = "c";
string name;

void accelCallback(const ourSensor_msgs::Accel::ConstPtr& msg){
    accel1_y = msg->accel1_y;
    accel2_y = msg->accel2_y;
}
void nameCallback(const std_msgs::String::ConstPtr& msg){
    name = msg->data;
    // cout<<name;
}
int main(int argc, char **argv)
{

    ros::init(argc, argv, "listener_global");
    ros::NodeHandle n;
    ros::Rate rate(500); // *5 = 2500Hz
    int count = 1;
    double prev = 0.0;
    string objT = "";
    std_msgs::Float32MultiArray grip;
    grip.data.resize(2);
    grip.data[0] = 20;
    grip.data[1] = 20;
    count++;
    ros::Subscriber sub_accel = n.subscribe("accel_data", 1, accelCallback);
    ros::Subscriber file_name = n.subscribe("filename_topic", 1, nameCallback);

    ros::Publisher pub = n.advertise<std_msgs::Float32MultiArray>("setData", 1);   
    pub.publish(grip);
    ros::spinOnce();
    Py_Initialize();
    Py_SetProgramName(argv[0]);
    PySys_SetArgv(argc, argv);
    PyObject* module = PyImport_ImportModule("rospy");
     if (!module) {
            std::cerr << "Failed to import the rospy module\n";
            return 1;
        }

        FILE* script_filehom = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/home_pos.py","r");

        if (script_filehom) {
            PyRun_SimpleFile(script_filehom, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/home_pos.py");
            fclose(script_filehom);
        } else {
            std::cerr << "Failed to open the Python script file 1\n";
        }
    time_t t = time(0);
    struct tm * now = localtime( & t );
    float thresh;
    cout<<"Enter threshold: "<<endl;
    cin>>thresh;
    cout<<name<<endl;
    ofstream myfile;
    string filename;
    while (ros::ok()) {
        ros::spinOnce();
        break;
    }
    filename = name+"/main.csv";
    cout<<filename<<endl;
    myfile.open(filename.c_str(), ios::out | ios::app | ios::binary);
    double dif = 0;
    prev = accel1_y;
    ros::Time contact, slide, lift, slip;
    while (ros::ok() && poss > 0) {
        myfile << ros::Time::now() << "," << poss << "\n";
        poss = curr - 0.001 * count;
        grip.data[0] = 20;
        grip.data[1] = poss;
        dif = abs(abs(accel1_y) - abs(prev));
        // cout<<dif<<endl;
        if (abs(accel1_y) >= thresh) {
            cout << "contact detected, running UR at " << ros::Time::now() << endl;
            contact = ros::Time::now();
            break;
        }
        prev = accel1_y;
        count++;
        pub.publish(grip);
        ros::spinOnce();
        rate.sleep();
    }

    poss = poss + 0.001;
    grip.data[1] = poss;
    pub.publish(grip);
    ros::spinOnce();
    rate.sleep();
    curr = poss;
    count = 1;

    while (ros::ok()) {
        myfile << ros::Time::now() << "," << poss << "\n";
        
        if (!module) {
            std::cerr << "Failed to import the rospy module\n";
            return 1;
        }

        FILE* script_file1 = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py","r");

        if (script_file1) {
            PyRun_SimpleFile(script_file1, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py");
            fclose(script_file1);
        } else {
            std::cerr << "Failed to open the Python script file 1\n";
        }
        slide = ros::Time::now();
        cout<<"Ready to grasp"<<endl;
        
        
        //Grasps for 10 secs

        while (count<=10000 && ros::ok()){				
            myfile <<ros::Time::now()<<poss<<"\n";
            poss = curr-0.001*count;
            // poss = curr-0.001*trial;
            grip.data[0] = 20;
            grip.data[1] = poss;
            pub.publish(grip);
            count++;

            ros::spinOnce();
            rate.sleep();
	    }
    
        lift = ros::Time::now();
        // Run the second Python script
        FILE* script_file2 = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/up_pos.py", "r");
        if (script_file2) {
            PyRun_SimpleFile(script_file2, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/up_pos.py");
            fclose(script_file2);
        } else {
            std::cerr << "Failed to open the Python script file 2\n";
        }

        // Clean up and exit
        
    break;
    }
    Py_DECREF(module);
    Py_Finalize();
    slip = ros::Time::now();
    grip.data[0] = 8;
    grip.data[1] = 20;
    pub.publish(grip);
    count++;
    ros::spinOnce();
    cout<<"CONTACT: "<<contact<<"SLIDE ENG: "<<slide<<"LIFT: "<<lift<<"SLIP: "<<slip<<endl;
    myfile<<"CONTACT,"<<contact<<",SLIDE ENG,"<<slide<<",LIFT,"<<lift<<",SLIP,"<<slip<<endl;
    myfile.close();
	return 0;
}
