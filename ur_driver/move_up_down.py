#!/usr/bin/env python
import time
import roslib; roslib.load_manifest('ur_driver')
import rospy
import actionlib
from control_msgs.msg import *
from trajectory_msgs.msg import *
from deserialize import *
roslib.load_manifest("ur_kinematics")
import sys
sys.path.append("/home/rass/Rassul/sensor_ws/src/universal_robot/ur_kinematics/src/ur_kinematics")
from test_analytical_ik import *


JOINT_NAMES = ['shoulder_pan_joint', 'shoulder_lift_joint', 'elbow_joint',
               'wrist_1_joint', 'wrist_2_joint', 'wrist_3_joint']
# Q1 = [0.12219549702229426, -1.7903310223303484, 2.294240751640502, -2.084082012341981, -1.5709994258510083, 3.261806346503907]
# Q2 = [-0.10748985597764804, -1.8572946782206419, 2.376675319634319, -2.1275165345402662, -1.570584186839107, 3.0477936496491074] #experiment
# Q2 = [-0.11455381193951286, -1.858989004741968, 2.3777864841843703, -2.126888216009548, -1.5702912060960008, 3.0407332442136057] #experiment2
# Q2 = [-0.10784547746538653, -1.8438403883935655, 2.364048324820909, -2.097040746479742, -1.5708771579948335, 3.0817785817473786] #calibration green
# Q3 = [-0.10150641690800999, -1.8468791340269135, 2.3669899055545174, -2.0983437960473914, -1.577831028464617, 3.0559314081414612] #calibration red
client = None
robot_hostname = '192.168.1.103'
port = 50001

def move1():
    Q1 = [0.12219549702229426, -1.7903310223303484, 2.294240751640502, -2.084082012341981, -1.5709994258510083, 3.261806346503907]
    g = FollowJointTrajectoryGoal()
    g.trajectory = JointTrajectory()
    g.trajectory.joint_names = JOINT_NAMES
    g.trajectory.points = [
        JointTrajectoryPoint(positions=Q1, velocities=[0]*6, time_from_start=rospy.Duration(2.0)),
        # JointTrajectoryPoint(positions=Q2, velocities=[0]*6, time_from_start=rospy.Duration(3.0)),
        # JointTrajectoryPoint(positions=Q3, velocities=[0]*6, time_from_start=rospy.Duration(4.0))
	]    
    client.send_goal(g)
    states = g.trajectory.points

    first_pos = states[0].positions
    shoulder = first_pos[1]
    elbow = first_pos[2]
    new_shoulder = shoulder + 0.01
    new_elbow = elbow + 0.03
    first_pos[1] = new_shoulder
    first_pos[2] = new_elbow

def move_repeated():
    g = FollowJointTrajectoryGoal()
    g.trajectory = JointTrajectory()
    g.trajectory.joint_names = JOINT_NAMES
    Q1 = [0.12219549702229426, -1.7903310223303484, 2.294240751640502, -2.084082012341981, -1.5709994258510083, 3.261806346503907]
    Q2 = [0.1221214824492788, -1.7695614082229651, 2.3162178834661753, -2.1262807596175453, -1.5708840129714794, 3.2618152244177168]
    g.trajectory.points = [
        JointTrajectoryPoint(positions=Q1, velocities=[0]*6, time_from_start=rospy.Duration(2.0)),
        # JointTrajectoryPoint(positions=Q2, velocities=[0]*6, time_from_start=rospy.Duration(3.0)),
        # JointTrajectoryPoint(positions=Q3, velocities=[0]*6, time_from_start=rospy.Duration(4.0))
	]    
    client.send_goal(g)
    states = g.trajectory.points[0].positions
    print(states)
    
    sh = states[1]
    el = states[2]
    w = states[3]
    sh1 =  sh + 0.03
    sh2 =  sh - 0.03
    el1 =  el + 0.02
    el2 =  el - 0.02
    w1 = w + 0.04
    w2 = w - 0.04
    sols = inverse(np.array(states), 0.02)
    print(sols)

    Q1 = list(states)
    Q2 = list(states)


    Q1[1] = sh1
    Q2[1] = sh2
    Q1[3] = w1
    Q2[1] = sh2
    Q1[2] = el1
    Q2[3] = w2
    d = 2.0
    g.trajectory.points = []
    for i in range(10):
        g.trajectory.points.append(
            JointTrajectoryPoint(positions=Q1, velocities=[0]*6, time_from_start=rospy.Duration(d)))
        d += 1
        g.trajectory.points.append(
            JointTrajectoryPoint(positions=Q2, velocities=[0]*6, time_from_start=rospy.Duration(d)))
        d += 1
        # g.trajectory.points.append(
        #     JointTrajectoryPoint(positions=Q3, velocities=[0]*6, time_from_start=rospy.Duration(d)))
        # d += 2
    # client.send_goal(g)
    try:
        client.wait_for_result()
    except KeyboardInterrupt:
        client.cancel_goal()
        raise
    
def main():
    global client
    try:
        rospy.init_node("test_move", anonymous=True, disable_signals=True)
        client = actionlib.SimpleActionClient('follow_joint_trajectory', FollowJointTrajectoryAction)
        print "Waiting for server..."
        client.wait_for_server()
        print "Connected to server"
        # move1()
        move_repeated()
        #move_disordered()
        #move_interrupt()
    except KeyboardInterrupt:
        rospy.signal_shutdown("KeyboardInterrupt")
        raise

if __name__ == '__main__': main()
