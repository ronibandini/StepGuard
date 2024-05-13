#!/usr/bin/env python3

# Step Guard 3.0
# Roni Bandini, May 2024, @RoniBandini
# MIT License
# Read BLE server to enable sounds and send Telegram notifications


import datetime
from bluepy import btle
from bluepy.btle import Scanner, Peripheral, Characteristic, ScanEntry, UUID

from unihiker import GUI
from unihiker import Audio
from pinpong.board import Board, Pin
import time
import requests


def writeLog(myLine):
    now = datetime.datetime.now()
    dtFormatted = now.strftime("%Y-%m-%d %H:%M:%S")
    with open('log.txt', 'a') as f:
        myLine=str(dtFormatted)+","+myLine
        f.write(myLine+"\n")

def telegramAlert(message):

	apiToken = ''
	chatID = ''
	apiURL = f'https://api.telegram.org/bot{apiToken}/sendMessage'

	try:
		response = requests.post(apiURL, json={'chat_id': chatID, 'text': message})
		print(response.text)
	except Exception as e:
		print(e)


mac	 	=""
afterDelay     	= 15
step		= 0
lastDetection   = time.time()
stepSeconds	= 10
noConnectionCounter=0
noConnectionBootLimit=10
okConnection=0

Board().begin()
gui     = GUI()
audio   = Audio()

# relay settings
relay = Pin(Pin.P23, Pin.OUT)

# green boton, used to disable sounds
buttonWhite = Pin(Pin.P22, Pin.IN)

# white button`
buttonGreen = Pin(Pin.P21, Pin.IN)

scanner = None
publicdevices = []

# wait until WiFi connection has been established
time.sleep(10)

print("Step Guard 3.0 started")
print("@RoniBandini - May 2024 - MIT License")
print("")

telegramAlert("Step Guard started")
writeLog("Started 3.0")

img = gui.draw_image(w=240, h=320, image='/root/images/stepguard.png')

counter=0

connected = False

while not connected:
	try:
		peri = Peripheral()
		peri.connect(mac)
            
		while True:
			noConnectionCounter=0

			if okConnection==0:
				img = gui.draw_image(w=240, h=320, image='/root/images/stepguard.png')
				okConnection=1
				
			print("Recibiendo datos...")
			chars = peri.getCharacteristics()
			for c in chars:
				if c.uuid=="beb5483e-36e1-4688-b7f5-ea07361b26a8":
					
					if "Detected" in str(c.read()):

						print("Hay una persona")						
						
						now = time.time()
						secondsSinceLastDetection = now - lastDetection
						print("Step: "+str(step))
						print("Segundos desde la ultima deteccion: "+str(secondsSinceLastDetection))

						if step==0:
							lastDetection=now
							telegramAlert("Alguien en la puerta")
							step=1


							# if not muted
							if buttonGreen.read_digital()==0:

								if buttonWhite.read_digital()==1:
									img = gui.draw_image(w=240, h=320, image='/root/images/dog.png')

									relay.write_digital(1)
									audio.start_play('/root/sounds/dog.mp3')								
									writeLog("Dog")
									time.sleep(3)
									relay.write_digital(0)

								else:
									img = gui.draw_image(w=240, h=320, image='/root/images/horn.png')

									relay.write_digital(1)
									audio.start_play('/root/sounds/ring.mp3')								
									writeLog("Ring")
									time.sleep(3)
									relay.write_digital(0)
							else:
								print("Muted")
								writeLog("Muted")


						elif secondsSinceLastDetection<stepSeconds:								
																					
							if step==1:
								print("Ya sono ring y sigue ahi")	
								lastDetection=now
								step=2
								img = gui.draw_image(w=240, h=320, image='/root/images/horn.png')

								if buttonGreen.read_digital()==0:
									relay.write_digital(1)
									audio.start_play('/root/sounds/despeje.mp3')
									writeLog("Despeje")
									time.sleep(5)
									relay.write_digital(0)
								else:
									print("Muted")
									writeLog("Muted")
								

							elif step==2:
								print("Ya sono despeje y sigue ahi")
								lastDetection=now

								img = gui.draw_image(w=240, h=320, image='/root/images/horn.png')


								if buttonGreen.read_digital()==0:
									relay.write_digital(1)
									audio.start_play('/root/sounds/lrad.mp3')
									writeLog("Horn")
									time.sleep(5)
									relay.write_digital(0)
								else:
									print("Muted")
									writeLog("Muted")


						else:
							print("Mucho tiempo desde ultima deteccion, reseteo")
							step=0
							lastDetection=now							


		connected = True


	except Exception as e:
		noConnectionCounter=noConnectionCounter+1

		if noConnectionCounter==noConnectionBootLimit:	
			#writeLog("Rebooting...")
			#time.sleep(3)		
			#os.system('systemctl reboot -i')
			telegramAlert("BLE unit connection lost")
			
		print("connectandread: Error,", e)
		writeLog(str(e))
		img = gui.draw_image(w=240, h=320, image='/root/images/nolink.png')
		okConnection=0

		time.sleep(10)
		pass
			


