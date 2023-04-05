import pyaudio
import wave
import numpy as np


FORMAT = pyaudio.paInt16
CHANNELS = 4
RATE = 8000
CHUNK = 50
RECORD_SECONDS = 5
WAVE_OUTPUT_FILENAME = "file.wav"

sample =np.zeros([4, CHUNK])

audio = pyaudio.PyAudio()
 
# start Recording
stream = audio.open(format=FORMAT, channels=CHANNELS,
                rate=RATE, input=True,
                frames_per_buffer=CHUNK,input_device_index=2)
print("recording...")
frames = []
 

data = stream.read(CHUNK)

for i in range (CHUNK):
	for j in range(4):
		sample[j,i]=int.from_bytes(data[j*2+i*8],data[j*2+i*8+1], "little", signed=True)

sample=sample/32768
print(sample)
# stop Recording
stream.stop_stream()
stream.close()
audio.terminate()
