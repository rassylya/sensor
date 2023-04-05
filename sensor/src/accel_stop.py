import pyaudio
import numpy as np
import rospy
import time
import csv
# import sys
# sys.path.append('../sensor')
from ourSensor_msgs.msg import Accel

# accel_data = Accel()
# publisher
accel_data = Accel()

def talker():
    pub = rospy.Publisher('accel_data', Accel, queue_size = 100)
    rospy.init_node('talker', anonymous=True)
    rate = rospy.Rate(1000) # 10hz

    FORMAT = pyaudio.paInt16
    CHANNELS = 2
    RATE = 8000
    CHUNK = 1
    audio = pyaudio.PyAudio()
        
    stream = audio.open(format=FORMAT, channels=CHANNELS,
            rate=RATE, input=True,
            frames_per_buffer=CHUNK,input_device_index=15)
    
    sample = np.zeros([CHANNELS, CHUNK])
    
    print("recording...")
    file = open('/home/rass/Rassul/experiments/test_acc.csv', 'w')
    writer = csv.writer(file)
    i = 0
    prev = 0
    while not rospy.is_shutdown():
    # while counter<1000:
        data = stream.read(CHUNK, exception_on_overflow = False)

        # writer.writerow(data)
        
        # read data from stream
        # for i in range (CHUNK):
        #     for j in range(CHANNELS):
        #         sample[j,i]=int.from_bytes([data[j*2+i*4],data[j*2+i*4+1]], "little", signed=True)

        # print(int.from_bytes([data[2], data[3]], "little", signed=True))
        for j in range(CHANNELS):
            # s1 = int.from_bytes([data[j*2+i*4],data[j*2+i*4+1]], "little", signed=True)
            sample[j,i]=int.from_bytes([data[j*2+i*4],data[j*2+i*4+1]], "little", signed=True)
            
            accel_data.accel1_x = sample[0][0]
            accel_data.accel1_y = sample[1][0]
            
            dif = abs(abs(accel_data.accel1_y) - abs(prev))
            writer.writerow([accel_data.accel1_x, accel_data.accel1_y,dif])
            if dif>500:
                print('contact detected')
                return 0
            prev = accel_data.accel1_y
            pub.publish(accel_data)
            print(accel_data.accel1_y)
        rate.sleep()
   
    # stop Recording
    # stream.stop_stream()
    file.close()
    stream.close()
    # audio.terminate()

if __name__ == '__main__':
    
    # pub = rospy.Publisher('accel_data', Accel, queue_size = 100)
    # rate = rospy.Rate(8000) #10hz

    try:
        talker()
    except rospy.ROSInterruptException:
        pass
    rospy.spin()
