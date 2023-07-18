import pyaudio
import numpy as np
import rospy
import time
import csv
# import sys
# sys.path.append('../sensor')
from ourSensor_msgs.msg import Accel

accel_data = Accel()

def talker():
    # pub = rospy.Publisher('accel_data', Accel, queue_size = 100)
    rospy.init_node('talker', anonymous=True)
    
    rate = rospy.Rate(1000) # 10hz
    FORMAT = pyaudio.paInt16
    CHANNELS = 4
    RATE = 8000
    CHUNK = 1
    audio = pyaudio.PyAudio()
        
    stream = audio.open(format=FORMAT, channels=CHANNELS,
            rate=RATE, input=True,
            frames_per_buffer=CHUNK,input_device_index=15)
    
    sample = np.zeros([CHANNELS, CHUNK])
    threshold = np.zeros([CHANNELS, 1])
    while(True):
        obj = input("Object name? Use either one of this: plain, bumpsB, bumpsS: ")
        if obj=='plain' or obj=='bumpsB' or obj=='bumpsS':
            break

    timestamp = time.strftime("%m_%d_%H-%M-%S")
    filename = f"{obj}_{timestamp}.csv"
    file = open('./texture_recognition/'+filename, 'w')
    writer = csv.writer(file)
    i = 0
    print("recording...")
    while not rospy.is_shutdown():
        	
        data = stream.read(CHUNK, exception_on_overflow = False)
        for i in range (CHUNK):
            for j in range(CHANNELS):   
                sample[j,i]=int.from_bytes([data[j*2+i*8],data[j*2+i*8+1]], "little", signed=True)
       

        sample = sample/32768    
        accel_data.accel1_x = sample[0][0]
        accel_data.accel1_y = sample[1][0]
        accel_data.accel2_x = sample[2][0]
        accel_data.accel2_y = sample[3][0]
        writer.writerow([rospy.Time.now(),  accel_data.accel2_x, accel_data.accel2_y])
        # pub.publish(accel_data)
        print(accel_data.accel1_x)
        sample = np.zeros([CHANNELS, CHUNK])
        rate.sleep()
    # stop Recording
    stream.stop_stream()
    stream.close()
    file.close()

    # audio.terminate()

if __name__ == '__main__':
    
    # pub = rospy.Publisher('accel_data', Accel, queue_size = 100)
    # rate = rospy.Rate(8000) #10hz

    try:
        talker()
    except rospy.ROSInterruptException:
        pass
    rospy.spin()
