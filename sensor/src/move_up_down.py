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


def move1():
    Q2 = [0.10471039783682476, -2.2269130439652605, 2.536406291099133, -1.880754258665199, -1.5784593469953352, 3.222408283826585]
    
    g = FollowJointTrajectoryGoal()
    g.trajectory = JointTrajectory()
    g.trajectory.joint_names = JOINT_NAMES
    g.trajectory.points = [
        JointTrajectoryPoint(positions=Q2, velocities= [0]*6, time_from_start=rospy.Duration(4.0)),
      
	]    
    client.send_goal(g)
    try:
        client.wait_for_result()
    except KeyboardInterrupt:
        client.cancel_goal()
        raise
def move_repeated():
    g = FollowJointTrajectoryGoal()
    g.trajectory = JointTrajectory()
    g.trajectory.joint_names = JOINT_NAMES
    Q1 = [0.10472171094513541, -2.2544273862409625, 2.4759325563851178, -1.7927457062277288, -1.5784169803634498, 3.222406260889421]
    Q2 = [0.10472171094513541, -2.187318954522226, 2.5939707865078176, -1.977894598732954, -1.5784169803634498, 3.222406260889421]
    d = 4.0
    g.trajectory.points = []
    for i in range(2):
        g.trajectory.points.append(
            JointTrajectoryPoint(positions=Q1, velocities=[0]*6, time_from_start=rospy.Duration(d)))
        d += 4
        g.trajectory.points.append(
            JointTrajectoryPoint(positions=Q2, velocities=[0]*6, time_from_start=rospy.Duration(d)))
        d += 4
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
        print ("Waiting for server...")
        client.wait_for_server()
        print ("Connected to server")
        move1()
        move_repeated()
        move1()
    except KeyboardInterrupt:
        rospy.signal_shutdown("KeyboardInterrupt")
        raise

if __name__ == '__main__': main()
