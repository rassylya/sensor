#include <ros/ros.h>
#include <tf/transform_broadcaster.h> //header for TransformBroadcaster object
#include <tf/transform_listener.h>
#include <std_msgs/Float64.h>
#include "ros/time.h"
#include "tactile_servo_msgs/ContsFeats.h"
#include "tactile_servo_msgs/OneContFeats.h"
#include <time.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include "geometry_msgs/WrenchStamped.h"
#include <trajectory_msgs/JointTrajectory.h>
#include <sensor_msgs/JointState.h>
#include "kinematic_control.h"

//Initial variables
double Z_TEMP;
double Y_TEMP;
double X_TEMP;
double Z_INIT;
double Y_INIT;
double X_INIT;
double Z_NOW;
double Z_DOWN;
double Y_NOW;
double X_NOW;
double Z_TARGET; // Target position for Z-coordinate of end-effector

PoseShow::PoseShow()
{
  is_transform_tool0_ = false;
  not_first_time = true;
}

PoseShow::~PoseShow(){}


URutil::URutil()
{
  is_new_msg_ = false;

  pub_velocities = n.advertise<geometry_msgs::Twist>("/desired_Vx_Vy_Vz", 1000);

  pub_point = n.advertise<geometry_msgs::PointStamped>("/topic_point", 100);

  pub_mode = n.advertise<std_msgs::Float64>("/topic_mode", 100);

  pub_state = n.advertise<std_msgs::Float64>("/topic_state", 100);

  pub_angleZ = n.advertise<std_msgs::Float64>("/topic_angleZ", 100);


  fb_feats_sub = n.subscribe("/fb_feats", 1,  &URutil::cb_fb_feats_sub,this);

  is_init_exper_ = false;
  is_at_home_ = false;
  boollet_ = false;
  couner_ = 0;
  recieved_joint_state_ = false;
  trjp_.velocities.resize(6);
  trj_.points.push_back(trjp_);
  trj_.points.push_back(trjp_);

//     ROS_INFO("initializing joint names");
  trj_.joint_names.push_back("shoulder_pan_joint");
  trj_.joint_names.push_back("shoulder_lift_joint");
  trj_.joint_names.push_back("elbow_joint");
  trj_.joint_names.push_back("wrist_1_joint");
  trj_.joint_names.push_back("wrist_2_joint");
  trj_.joint_names.push_back("wrist_3_joint");

}
URutil::~URutil(){}

void URutil::cb_fb_feats_sub(const tactile_servo_msgs::ContsFeatsConstPtr& msg_){
    if (msg_->control_features.size() == 1){
        is_in_contact_ur_ = msg_->control_features[0].is_contact;
        pixels_in_contact_ur_ = msg_->control_features[0].num_pixels_contact;
        highest_force_cell_ur_ = msg_->control_features[0].highest_force_cell;
        real_total_force_ur_ = msg_->control_features[0].real_total_force;

  is_new_msg_ = true;
    }
}

void URutil::send_velocity(geometry_msgs::Twist in_vel)
{
  in_vel_.angular = in_vel.angular;
  in_vel_.linear = in_vel.linear;
  pub_velocities.publish(in_vel_);

}
// void PoseShow::pose_init_create(){
//   if (is_transform_tool0_== false)
//   {
//     listener.waitForTransform("tool0", ros::Time(0), "robot_base",ros::Time(0), "robot_base", ros::Duration(10));
//     try
//     {
//       listener.lookupTransform("robot_base", "tool0", ros::Time(0), stransform);
//       is_transform_tool0_ = true;
//     }
//     catch (tf::TransformException &ex)
//     {
//       ROS_ERROR("%s", ex.what());
//     }

//     init_position = stransform.getOrigin();
//     //ROS_INFO("%lf,%lf,%lf",init_position[0],init_position[1],init_position[2]);
//     X_now_test_.header.frame_id = stransform.frame_id_.c_str();
//     X_now_test_.pose.position.x = init_position[0];
//     X_now_test_.pose.position.y = init_position[1];
//     X_now_test_.pose.position.z = init_position[2];

//     Z_INIT = init_position[2];
//     X_INIT = init_position[0];
//     Y_INIT = init_position[1];

//     Z_TEMP = init_position[2];
//     X_TEMP = init_position[0];
//     Y_TEMP = init_position[1];
//     not_first_time = false;
//   }
//   tf::Quaternion q_init_pose = stransform.getRotation();
//   geometry_msgs::Quaternion quat;
//   quat.w = q_init_pose.w();
//   quat.w = q_init_pose.x();
//   quat.w = q_init_pose.y();
//   quat.w = q_init_pose.z();
//   X_now_test_.pose.orientation = quat;
// }
void PoseShow::pose_now()
{
  /*
  The listener's buffer is literally empty immediately after creating tf listener,
  therefore it is good practice to wait some time after the listener has been created,
  so that the buffer can fill up
  */
  listener_pose_now.waitForTransform("tool0", ros::Time(0), "robot_base",ros::Time(0), "robot_base", ros::Duration(10));
  try
  {
    //we request (query) the listener for a specific transformation
    listener_pose_now.lookupTransform("robot_base", "tool0", ros::Time(0), nowtransform);
  }
  catch (tf::TransformException &ex)
  {
    ROS_ERROR("%s", ex.what());
  }
  Z_NOW = nowtransform.getOrigin().getZ(); // get Z position of tool0 relative to base frame
  X_NOW = nowtransform.getOrigin().getX(); // get X position of tool0 realtive to world (base frame)
  Y_NOW = nowtransform.getOrigin().getY(); // get Y position of tool0 realtive to world (base frame)

}

int main(int argc, char** argv)

{
  ros::Time::init();
  geometry_msgs::Twist in_velocityzero;
  in_velocityzero.linear.x = 0;
  in_velocityzero.linear.y = 0;
  in_velocityzero.linear.z = 0.0;
  in_velocityzero.angular.x = 0;
  in_velocityzero.angular.y = 0;
  in_velocityzero.angular.z = 0;

  geometry_msgs::Twist in_velocitydown_slow;
  in_velocitydown_slow.linear.x = 0;
  in_velocitydown_slow.linear.y = 0;
  in_velocitydown_slow.linear.z = 0.007;
  in_velocitydown_slow.angular.x = 0;
  in_velocitydown_slow.angular.y = 0;
  in_velocitydown_slow.angular.z = 0;

  ros::init(argc, argv, "init_before_any_exploration_node");
  ros::NodeHandle nh; //NodeHandle is the main access point to communications with the ROS system
ros::Rate loop_rate(140); 
  PoseShow PoseShow_test;
  URutil Experiment;
  ros::spinOnce();
  PoseShow_test.pose_now(); // getting X,Y,Z coordinates of end-effector relative to base frame
Z_TARGET = 0.2;
while( (ros::ok()) && ((Z_NOW)<(Z_TARGET)) )
    {
      ROS_INFO("Z_NOW: %f", Z_NOW);

    PoseShow_test.pose_now();
    Experiment.send_velocity(in_velocitydown_slow);
    }
  Experiment.send_velocity(in_velocityzero);
  
    
    return 0;
}