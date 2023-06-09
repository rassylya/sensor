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
def move_repeated():
    g = FollowJointTrajectoryGoal()
    g.trajectory = JointTrajectory()
    g.trajectory.joint_names = JOINT_NAMES
    Q1 = [0.11232423477845364, -1.6391863357528837, 2.334893684105548, -2.2652865416977965, -1.5760013632924865, 3.246503670547497]
    Q2 = [0.11232423477845364, -1.6986591032616971, 2.28879126571678, -2.1597028358152217, -1.5760013632924865, 3.246485914719877]
    # g.trajectory.points = [
    #     JointTrajectoryPoint(positions=Q1, velocities=[0]*6, time_from_start=rospy.Duration(2.0)),
    #     # JointTrajectoryPoint(positions=Q2, velocities=[0]*6, time_from_start=rospy.Duration(3.0)),
    #     # JointTrajectoryPoint(positions=Q3, velocities=[0]*6, time_from_start=rospy.Duration(4.0))
	# ]    
    # client.send_goal(g)
    # states = g.trajectory.points[0].positions
    # print(states)
    
    # sh = states[1]
    # el = states[2]
    # w = states[3]
    # sh1 =  sh + 0.03
    # sh2 =  sh - 0.03
    # el1 =  el + 0.02
    # el2 =  el - 0.02
    # w1 = w + 0.04
    # w2 = w - 0.04
    # sols = inverse(np.array(states), 0.02)
    # print(sols)

    # Q1 = list(states)
    # Q2 = list(states)


    # Q1[1] = sh1
    # Q2[1] = sh2
    # Q1[3] = w1
    # Q2[1] = sh2
    # Q1[2] = el1
    # Q2[3] = w2
    d = 5.0
    g.trajectory.points = []
    for i in range(1):
        g.trajectory.points.append(
            JointTrajectoryPoint(positions=Q1, velocities=[0]*6, time_from_start=rospy.Duration(d)))
        d += 5
        g.trajectory.points.append(
            JointTrajectoryPoint(positions=Q2, velocities=[0]*6, time_from_start=rospy.Duration(d)))
        d += 5
        # print(g.trajectory.velocities)
        # g.trajectory.points.append(
        #     JointTrajectoryPoint(positions=Q3, velocities=[0]*6, time_from_start=rospy.Duration(d)))
        # d += 2
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
        # move1()
        move_repeated()
        move1()
        # #move_disordered()
        #move_interrupt()
    except KeyboardInterrupt:
        rospy.signal_shutdown("KeyboardInterrupt")
        raise

if __name__ == '__main__': main()
