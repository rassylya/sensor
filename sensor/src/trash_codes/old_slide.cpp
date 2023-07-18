// #include <iostream>
// #include <fstream>      // std::ifstream
// #include <ros/ros.h>
// #include <std_msgs/Float64.h>
// #include <ourSensor_msgs/Accel.h>
// #include <std_msgs/Float32.h>
// #include <std_msgs/Float32MultiArray.h>
// #include <wittenstein_msgs/wittenstein.h>
// #include <sensor_msgs/JointState.h>
// #include <string>
// #include <stdio.h>
// #include <cstdlib>
// #include <vector>
// #include <thread>
// #include <condition_variable>
// // #include "contact_slide.h"
// #define PY_SSIZE_T_CLEAN
// #include <python2.7/Python.h>
// #include <mutex>


// using namespace std;	

// ourSensor_msgs::Accel message;

// wittenstein_msgs::wittenstein global_msg;
// // float position
// float hall1, hall2;
// float witt;
// int start_time = 0;
// int ch = 1;
// float curr = 20.0;
// float poss = 20.0;
// float accel1_x;
// // [50], accel1_y[50], accel2_x[50], accel2_y[50];
// double thresh;
// int trial = 1;

// std::mutex file_mutex;
// std::condition_variable cv;
// bool is_running = true;

// void chatterCallback(const std_msgs::Float32MultiArray::ConstPtr& msg2)
// {
// 	hall1 = msg2->data[0];
//     hall2 = msg2->data[1];
// }	

// void callback(const wittenstein_msgs::wittenstein::ConstPtr& msg){
// 	global_msg.fx = msg->fx;
// 	global_msg.fy = msg->fy;
// 	global_msg.fz = msg->fz;
// 	witt = msg->fz;
// }

// void accelCallback(const ourSensor_msgs::Accel::ConstPtr& msg){

//     accel1_x = msg->accel1_x;
// }

// void write_data_to_file(ofstream& myfile){
//     // float* accel1_y, float* accel2_x, float* accel2_y, float thresh) {
//     std::unique_lock<std::mutex> lock(file_mutex);

//     while (is_running) {
//         // myfile << ros::Time::now() << "," << witt << "," << hall1 << "," << hall2 << "," << *accel1_x << "," << *accel1_y << "," << *accel2_x << "," << *accel2_y << "," << poss << "\n";
//         myfile << ros::Time::now() <<","<<accel1_x<<"\n";
//         // Adjust sleep duration to control data recording frequency
//     }
//     lock.unlock(); // unlock access to file
//     myfile.flush(); // flush buffer to ensure data is written to file
//     std::this_thread::sleep_for(std::chrono::milliseconds(25));
// }

// void run_sliding(PyObject* module) {

//     if (!module) {
//         std::cerr << "Failed to import the rospy module\n";
//         Py_Finalize();
//         return;
//     }
//     // Run the Python script
//     FILE* script_file = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py", "r");
//     if (script_file) {
//         PyRun_SimpleFile(script_file, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py");
//         fclose(script_file);
//     } else {
//         std::cerr << "Failed to open the Python script file\n";
//     }
//     // Notify the data recording thread to stop running
//     is_running = false;
//     cv.notify_one();
// }

// // Function to run Python code


// int main(int argc, char **argv)
// {
//     ros::Time contact, slide, ungrasp, lift;   
//     Py_Initialize();

//     // Set the program name (equivalent to sys.argv[0])
//     Py_SetProgramName(argv[0]);

//         // Set sys.argv
//     PySys_SetArgv(argc, argv);

//     PyObject* module = PyImport_ImportModule("rospy");

//     // PythonRunner runner;

// 	ros::init(argc, argv, "listener_global");
// 	ros::NodeHandle n;
// 	ros::Rate rate(500); // *5 = 2500Hz
//     int count = 1;
//     double prev = 0.0;
//     string objT = "";
// 	std_msgs::Float32MultiArray grip;
// 	grip.data.resize(2);
//     ros::Subscriber sub_teensy = n.subscribe("chatter", 1, chatterCallback);
// 	ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
//     ros::Subscriber sub_accel = n.subscribe("accel_data", 1, accelCallback);
// 	ros::Publisher pub = n.advertise<std_msgs::Float32MultiArray>("setData", 1);   
//     time_t t = time(0);   // get time now
// 	struct tm * now = localtime( & t );

//     while(true){
//         cout<<"Enter type of the object (cylinder, plain, textured): "<<endl;
//         cin>>objT;
//         if(objT=="cylinder" || objT=="plain" || objT=="textured") break;
//     }

//     float limit;
//     cout<<"Enter threshold: "<<endl;
//     cin>>limit;
// 	stringstream ss;
// 	ofstream myfile;
// 	ss << "/home/rass/Rassul/experiments/grasp_slide/"<<now->tm_mday <<"_"<<now->tm_hour <<":"<<now->tm_min<<"_"<<objT<<"grasp_slide.csv";
// 	myfile.open (ss.str(), ios::out | ios::app | ios::binary);
// 	/
//     float limit;
//     cout<<"Enter threshold: "<<endl;
//     cin>>limit;
// 	stringstream ss;
// 	ofstream myfile;
// 	ss << "/home/rass/Rassul/experiments/grasp_slide/"<<now->tm_mday <<"_"<<now->tm_hour <<":"<<now->tm_min<<"_"<<objT<<"grasp_slide.csv";
// 	myfile.open (ss.str(), ios::out | ios::app | ios::binary);
// 	// double dif = 0;
//     // prev = thresh;
//     //     ros::spinOnce();
//     //     rate.sleep();
//     // }   
//     contact = ros::Time::now();
//     poss = poss + 0.002;
//     pub.publish(grip);
//     ros::spinOnce();
//     rate.sleep();
//     curr = poss;
//     count = 1;
//     while(ros::ok()){
        
//         // Import the ROS Python module
//         // PyObject* module = PyImport_ImportModule("rospy");
//         // if (!module) {
//         //     std::cerr << "Failed to import the rospy module\n";
//         //     return 1;
//         // }

//         // // Run the first Python script
//         // FILE* script_file1 = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py", "r");
//         // if (script_file1) {
//         //     PyRun_SimpleFile(script_file1, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py.py");
//         //     fclose(script_file1);
//         // } else {
//         //     std::cerr << "Failed to open the Python script file 1\n";
//         // }
     
//         std::thread data_thread(write_data_to_file, std::ref(myfile));
//         // accel1_y, accel2_x, accel2_y, thresh);

//     // Start the Python thread
 
//         std::thread py_thread(run_sliding, module);

//         // Wait for the Python thread to finish
//         py_thread.join();

//         // Notify the data recording thread to stop running
//         is_running = false;
//         cv.notify_one();

//         data_thread.join();

//     // // Close the file


//     //     slide = ros::Time::now();
//     //     cout<<"Ready to grasp"<<endl;

//         // while (start_time<20000 && ros::ok()){				
//         //     // myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<*accel1_x<<","<<*accel1_y<<","<<*accel2_x<<","<<*accel2_y<<","<<poss<<"\n";
//         //     poss = curr-0.001*count;
//         //     // poss = curr-0.001*trial;
//         //     grip.data[0] = 20;
//         //     grip.data[1] = poss;
//         //     pub.publish(grip);
//         //     count++;
//         //     start_time++;
//         //     // cout<<start_time<<endl;
//         //     ros::spinOnce();
//         //     rate.sleep();
// 	    // }
//         break;
//     }
//     // while(ros::ok()){
//     //     myfile <<ros::Time::now()<<","<<witt<<","<<hall1<<","<<hall2<<","<<*accel1_x<<","<<*accel1_y<<","<<*accel2_x<<","<<*accel2_y<<","<<poss<<"\n";
//     //     // Run the second Python script
//     //     lift = ros::Time::now();
//     //     FILE* script_file2 = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/up_pos.py", "r");
//     //     if (script_file2) {
//     //         PyRun_SimpleFile(script_file2, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/up_pos.py");
//     //         fclose(script_file2);
//     //     } else {
//     //         std::cerr << "Failed to open the Python script file 2\n";
//     //     }
//     //     // Clean up and exit
//     //     release
//     //     grip.data[0] = 10;
//     //     grip.data[1] = 20;
//     //     pub.publish(grip);      
//     //     ros::spinOnce();
//     //     rate.sleep();
//     //     break;
//     // }
//     // cout<<contact<<" "<<slide<<" "<<lift<<endl;
//     myfile<<contact<<","<<slide;
//     Py_DECREF(module);
//     Py_Finalize();
//     myfile.close();
// 	return 0;
// }