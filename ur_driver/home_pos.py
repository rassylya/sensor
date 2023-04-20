#!/usr/bin/env python
import time
import roslib; roslib.load_manifest('ur_driver')
import rospy
import actionlib
from control_msgs.msg import *
from trajectory_msgs.msg import *
# from deserialize import *
# roslib.load_manifest("ur_kinematics")
# import sys
# sys.path.append("/home/rass/Rassul/sensor_ws/src/universal_robot/ur_kinematics/src/ur_kinematics")
# from test_analytical_ik import *


JOINT_NAMES = ['shoulder_pan_joint', 'shoulder_lift_joint', 'elbow_joint',
               'wrist_1_joint', 'wrist_2_joint', 'wrist_3_joint']
client = None
robot_hostname = '192.168.1.103'
port = 50001

def move1():
    # Q1 = [0.36910516608044464, -1.648588551412895, 2.149498216971902, -2.069142391288544, -1.5755909563200676, 3.503362071075151]
    Q2 = [0.11232423477845364, -1.6567479038806292, 2.3227691063496896, -2.2356343862718075, -1.576010241206296, 3.246477036806067]
    # v1 = [0.001, 0.001, 0.001, 0.001, 0.001, 0.001]
    # v1 = [0.1, 0.1, 0.1, 0.1, 0.1, 0.1]
    g = FollowJointTrajectoryGoal()
    g.trajectory = JointTrajectory()
    g.trajectory.joint_names = JOINT_NAMES
    g.trajectory.points = [
        JointTrajectoryPoint(positions=Q2, velocities= [0]*6, time_from_start=rospy.Duration(4.0)),
        # JointTrajectoryPoint(positions=Q2, velocities= [0]*6, time_from_start=rospy.Duration(6.0)),
        # JointTrajectoryPoint(positions=Q3, velocities=[0]*6, time_from_start=rospy.Duration(4.0))
	]    
    client.send_goal(g)
    try:
        client.wait_for_result()
    except KeyboardInterrupt:
        client.cancel_goal()
        raise

    
def main():
    global client
    try:
        rospy.init_node("slide_move", anonymous=True, disable_signals=True)
        client = actionlib.SimpleActionClient('follow_joint_trajectory', FollowJointTrajectoryAction)
        print "Waiting for server..."
        client.wait_for_server()
        print "Connected to server"
        move1()
        
    except KeyboardInterrupt:
        rospy.signal_shutdown("KeyboardInterrupt")
        raise

if __name__ == '__main__': main()
