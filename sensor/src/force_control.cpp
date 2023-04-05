#include <iostream>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int16MultiArray.h>
#include <wittenstein_msgs/wittenstein.h>
#include <rosbag/bag.h>
#include <sensor_msgs/JointState.h>
using namespace std;

wittenstein_msgs::wittenstein global_msg;
float witt;
float f_interm = 0.0;
int thr = 0;
int activate = 0;
float f_desired;
int frst = 0;      
int achv = 0;
int time_exp = 0;
int i_grip = 0;
int forw = 0;


float old_poss = 20.0, new_poss = 0.0, err = 0.0, step_b = 0.0;

void callback(const wittenstein_msgs::wittenstein::ConstPtr& msg){
	global_msg.fx = msg->fx;
	global_msg.fy = msg->fy;
	global_msg.fz = msg->fz;
	witt = msg->fz;
}

void rec_callback(const std_msgs::Float32::ConstPtr& msg_f){
	f_interm = msg_f->data;
	activate = 1;
}

int main(int argc, char **argv)
{	
	ros::init(argc, argv, "listener_global");
	ros::NodeHandle n;
	ros::Rate rate(500); 
	sensor_msgs::JointState schunk_st;

	std_msgs::Float32MultiArray grip;
	grip.data.resize(2);

	std_msgs::Int16MultiArray exp_s;
	exp_s.data.resize(1);

	ros::Subscriber sub_wittenstein = n.subscribe("wittenstein_topic", 1, &callback);
	ros::Subscriber sub_force = n.subscribe("send_force", 1, &rec_callback); 
	ros::Publisher pub = n.advertise<std_msgs::Float32MultiArray>("setData", 1);
	ros::Publisher exp_stat = n.advertise<std_msgs::Int16MultiArray>("exp_status",1);
		
	ROS_INFO("Force control launched successfully");
	while(ros::ok()){


		// If the value is recieved start force_control
		if(activate == 1 && f_interm > 0.0){
			if(frst == 0){
				f_desired = f_interm;
				step_b = f_desired-f_desired/3;
				f_desired = -f_desired;
				frst = 1;
			}

			if(witt < -30 || f_desired < -30){
				ROS_INFO("URGENT STOP| pos: %.5f", new_poss);
				return 0;
			} else if (witt > -0.5){
				new_poss = old_poss - step_b;
			
				grip.data[0] = 5;
				grip.data[1] = new_poss;

				pub.publish(grip);

				old_poss = new_poss;
				if(step_b > 0.001){
					step_b = step_b/10;
				} else {
					step_b = 0.001;
				}
			}else{
				err = (abs(witt)-abs(f_desired))/abs(f_desired);			
				new_poss = old_poss + err*0.001;
				//ROS_INFO("POS: %.5f | ERROR: %.5f | FORCE: %.5f | %d", new_poss, err, f_interm, thr);

				grip.data[0] = 0.5;
				grip.data[1] = new_poss;

				pub.publish(grip);
				old_poss = new_poss;
			
				if(abs(abs(witt)-abs(f_desired))<=0.01){
					thr++;
				}
				if(thr >= 200){
					ROS_INFO("GOAL ACHIEVED - END FORCE = %.5f N", witt);
					f_desired = f_desired-0.25;
					thr = 0;
					activate = 0;
					frst = 0;
				}
			}
		} else if (activate == 1 && f_interm == -10.0){ // If the value recieved is negative - release
			grip.data[0] = 11.75;
			grip.data[1] = 20;

			pub.publish(grip);
			activate = 0;
			frst = 0;
			ROS_INFO("GRIPPER RELEASED");
		} else if (activate == 1 && f_interm == -5.0){
			if(frst == 0){
				f_desired = 2.0;
				step_b = f_desired-f_desired/3;
				f_desired = -f_desired;
				frst = 1;
			}

			if(witt < -30 || f_desired < -30){
				ROS_INFO("URGENT STOP| pos: %.5f", new_poss);
				return 0;
			} else if (witt > -0.5){
				new_poss = old_poss - step_b;
			
				grip.data[0] = 2;
				grip.data[1] = new_poss;

				pub.publish(grip);

				old_poss = new_poss;
				if(step_b > 0.001){
					step_b = step_b/10;
				} else {
					step_b = 0.001;
				}
			}else{
				if(achv == 0){
					err = (abs(witt)-abs(f_desired))/abs(f_desired);			
					new_poss = old_poss + err*0.001;
					//ROS_INFO("POS: %.5f | ERROR: %.5f | FORCE: %.5f | %d", new_poss, err, f_interm, thr);

					grip.data[0] = 0.5;
					grip.data[1] = new_poss;

					pub.publish(grip);
					old_poss = new_poss;
			
					if(abs(abs(witt)-abs(f_desired))<=0.01){
						thr++;
					}
					if(thr >= 200){
						ROS_INFO("GOAL ACHIEVED - END FORCE = %.5f N | STARTING THE PROCEDURE", witt);
						achv = 1;
					}
				} else if(achv == 1){
					if (abs(witt) < 5.0 && time_exp == 0){

						grip.data[0] = 0.25;
						grip.data[1] = new_poss-5;

						pub.publish(grip);
						time_exp = 1;
						exp_s.data[0] = 2;
						exp_stat.publish(exp_s);
					}else if (abs(abs(witt)-5.0) < 0.1 && time_exp == 1){

						ROS_INFO("DONE, GOING BACK %.5f", abs(abs(witt)-5.0));
						grip.data[0] = 0.25;
						grip.data[1] = new_poss;

						pub.publish(grip);
						time_exp = 2;
					}else if (abs(abs(witt)-2.0) < 0.1 && time_exp == 2) {
						exp_s.data[0] = 1;
						ROS_INFO("EXP. FINISHED");
						exp_stat.publish(exp_s);
						thr = 0;
						activate = 0;
						frst = 0;
						achv = 0;
						time_exp = 0;
					}
				}
			}
		} else if (activate == 1 && f_interm == -15.0){
			if (forw == 0){ // establishing touch
				if (i_grip == 0){
					ROS_INFO("STARTING THE CONFIGURATION EXPERIMENT");
					grip.data[1] = 20;
					grip.data[0] = 20;
					pub.publish(grip);
					exp_s.data[0] = 2;
					exp_stat.publish(exp_s);
				}

				i_grip = i_grip + 1;
				grip.data[1] = 20.0 - i_grip*0.01;
				grip.data[0] = 5; 
				
				pub.publish(grip);
				ROS_INFO("PUBLISHED: %.5f", 20.0 - i_grip*0.01);
				sleep(1);
				if (i_grip == 235){
					forw = 1;
					i_grip = 0;
				}
			} else{ // releasing from touch event
				if (i_grip == 0){
					ROS_INFO("GOING BACK");
					//usleep(100000);
				}
				i_grip = i_grip + 1;
				grip.data[1] = 17.65 + i_grip*0.01;
				grip.data[0] = 5;

				pub.publish(grip);
				ROS_INFO("PUBLISHED: %.5f", 17.65 + i_grip*0.01);
				sleep(1);
				if (i_grip == 235){
					forw = 0;
					activate = 0;
					exp_s.data[0] = 1;
					exp_stat.publish(exp_s);
				
					ROS_INFO("EXP. FINISHED");
					i_grip = 0;
				}
			}
		}
		ros::spinOnce();
		rate.sleep();	
	
	}
}
