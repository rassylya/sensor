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
// #include <conio.h>
#define PY_SSIZE_T_CLEAN
#include <python2.7/Python.h>

using namespace std;	

ourSensor_msgs::Accel message;

wittenstein_msgs::wittenstein global_msg;
// sensor_msgs::JointState schunk_st;
// float position
float hall1, hall2;
float witt;
int start_time = 0;
int ch = 1;
float curr = 20.0;
float poss = 20.0;
double accel1_x, accel1_y, accel2_x, accel2_y;
int trial = 1;

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
	message.accel1_x = msg->accel1_x;
	message.accel1_y = msg->accel1_y;
    message.accel2_x = msg->accel2_x;
	message.accel2_y = msg->accel2_y;
	accel1_x = msg->accel1_x;
	accel1_y = msg->accel1_y;
	accel2_x = msg->accel2_x;
	accel2_y = msg->accel2_y;
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
	
    ros::Subscriber sub_teensy = n.subscribe("chatter", 1, chatterCallback);
	ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
    ros::Subscriber sub_accel = n.subscribe("accel_data", 1, accelCallback);
	ros::Publisher pub = n.advertise<std_msgs::Float32MultiArray>("setData", 1);   

    time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );
    while(true){
        cout<<"Enter type of the object (cylinder, plain, textured): "<<endl;
        cin>>objT;
        if(objT=="cylinder" || objT=="plain" || objT=="textured") break;
    }
    float thresh;
    cout<<"Enter threshold: "<<endl;
    cin>>thresh;
	stringstream ss;
	ofstream myfile;
	ss << "/home/rass/Rassul/experiments/grasp_slide/"<<now->tm_mday <<"_"<<now->tm_hour <<":"<<now->tm_min<<"_"<<objT<<"grasp_slide.csv";
	myfile.open (ss.str(), ios::out | ios::app | ios::binary);
	double dif = 0;
    prev = accel1_x;

    while(ros::ok() and poss>0){
        myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<accel1_x<<","<<accel1_y<<","<<accel2_x<<","<<accel2_y<<","<<poss<<"\n";
        poss = curr-0.001*count;
        grip.data[0] = 20;
        grip.data[1] = poss;
        // cout<<accel1_x<<" "<<accel1_y<<endl;
        dif = abs(abs(accel1_x) - abs(prev));
        if(dif>=thresh && dif<2000){
            cout<<"contact detected, running UR at "<<start_time<<endl;
            break;
        }
        prev = accel1_x;
        count++;
        start_time++;
        pub.publish(grip);
        ros::spinOnce();
        rate.sleep();
    }   
    poss = poss + 0.001;
    pub.publish(grip);
    ros::spinOnce();
    rate.sleep();
    curr = poss;
    count = 1;
    while(ros::ok()){
        myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<accel1_x<<","<<accel1_y<<","<<accel2_x<<","<<accel2_y<<","<<poss<<"\n";
        Py_Initialize();

    // Set the program name (equivalent to sys.argv[0])
        Py_SetProgramName(argv[0]);

        // Set sys.argv
        PySys_SetArgv(argc, argv);

        // Import the ROS Python module
        PyObject* module = PyImport_ImportModule("rospy");
        if (!module) {
            std::cerr << "Failed to import the rospy module\n";
            return 1;
        }

        // Run the first Python script
        FILE* script_file1 = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py", "r");
        if (script_file1) {
            PyRun_SimpleFile(script_file1, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py.py");
            fclose(script_file1);
        } else {
            std::cerr << "Failed to open the Python script file 1\n";
        }
        
        cout<<"Ready to grasp"<<endl;

        while (start_time<60000 && ros::ok()){				
            myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<accel1_x<<","<<accel1_y<<","<<accel2_x<<","<<accel2_y<<","<<poss<<"\n";
            poss = curr-0.001*count;
            // poss = curr-0.001*trial;
            grip.data[0] = 20;
            grip.data[1] = poss;
            pub.publish(grip);
            count++;
            start_time++;
            ros::spinOnce();
            rate.sleep();
	    }


        // Run the second Python script
        FILE* script_file2 = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/up_pos.py", "r");
        if (script_file2) {
            PyRun_SimpleFile(script_file2, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/up_pos.py");
            fclose(script_file2);
        } else {
            std::cerr << "Failed to open the Python script file 2\n";
        }

        // Clean up and exit
        Py_DECREF(module);
        Py_Finalize();
    }
        while (start_time<5000 && ros::ok()){				

            myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<accel1_x<<","<<accel1_y<<","<<accel2_x<<","<<accel2_y<<","<<poss<<"\n";

        }
	return 0;
}
