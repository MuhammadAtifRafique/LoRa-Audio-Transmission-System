import io
from datetime import datetime
import serial
import wave
import audioop
import numpy as np
import scipy.io.wavfile
from scipy.io.wavfile import write

##    Working Perfectly


class SerialRead:
    def __init__(self):
        self.ser = serial.Serial(port='COM10',baudrate=115200)
        self.dateTime = datetime.now().strftime("%d-%m-%Y %H-%M-%S")
        self.wavFile = None
        self.bytes = None
        self.totalData = []
        pass  # end of SerialRead class constructor

    def convertToWav(self):
        with wave.open(f'{self.dateTime}.wav','wb') as self.wavFile:
            self.wavFile.setnchannels(1) # Mono audio
            self.wavFile.setsampwidth(2) # 16-bit audio
            self.wavFile.setframerate(10000) # Sample rate: 441 kHz
            print(f"Port status : {self.ser.is_open}")
            buffer_data = []
            timer = 0
            is_loop = True
            while is_loop:
                data= self.ser.read(254)   
                if data:
                    increased_frames = audioop.mul(data, 2, 60)
                    timer = timer + 1
                    print(timer)
                    self.wavFile.writeframes(increased_frames)  
                    
                if timer > 1000:
                    is_loop = False
                
        pass   # end of convertToWav function
    pass # End of SerialRead Function

if __name__ == "__main__":
    serRead = SerialRead()
    serRead.convertToWav()
