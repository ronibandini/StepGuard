# Step Guard 2.0
# Roni Bandini, March 2024, https://bandini.medium.com/ @RoniBandini
# MIT License
# Monitor Telegram communication channel to enable relay and audio playback

from unihiker import GUI
from unihiker import Audio
from pinpong.board import Board, Pin
from telethon.sync import TelegramClient, events
import time
import datetime

# settings
api_id      = ""
api_hash    = ""
myDelay     = 5
disable     = 0

Board().begin()
gui     = GUI()
audio   = Audio()

# pin settings
relay = Pin(Pin.P23, Pin.OUT)

# green boton, used to disable sounds
buttonWhite = Pin(Pin.P21, Pin.IN)

# white button`
buttonGreen = Pin(Pin.P22, Pin.IN)

def writeLog(myLine):
    now = datetime.datetime.now()
    dtFormatted = now.strftime("%Y-%m-%d %H:%M:%S")
    with open('log.txt', 'a') as f:
        myLine=str(dtFormatted)+","+myLine
        f.write(myLine+"\n")

def action(state):

    if buttonGreen.read_digital()==1:
        print("Disabled with button")
        disable=1

    if buttonGreen.read_digital()==0:
        print("Enabled with button")
        disable=0

    if state=='horn' and disable==0:
        print("Horn")
        audio.start_play('/home/server/sounds/lrad.mp3')
        relay.write_digital(1)
        img = gui.draw_image(w=240, h=320, image='images/horn.png')
        writeLog("Horn")
        time.sleep(5)
        relay.write_digital(0)
        img.config(w=240, h=320, image='/root/images/start.png')

    if state=='despeje' and disable==0:
        print("Despeje")
        audio.start_play('/root/sounds/despeje.mp3')
        relay.write_digital(1)
        img = gui.draw_image(w=240, h=320, image='images/horn.png')
        writeLog("clear")
        time.sleep(7)
        relay.write_digital(0)
        img.config(w=240, h=320, image='/root/images/start.png')

    if state=='ring' and disable==0 and buttonWhite.read_digital()==0:
        print("Ring")
        audio.start_play('/root/sounds/ring.mp3')
        relay.write_digital(1)
        img = gui.draw_image(w=240, h=320, image='images/ring.png')
        writeLog("Ring")
        time.sleep(3)
        relay.write_digital(0)
        img.config(w=240, h=320, image='/root/images/start.png')

    # print log
    time.sleep(2)
    img = gui.draw_image(w=240, h=320, image='images/background.png')
    count = 0
    myY=80
    # read log file in reverse order
    for line in reversed(list(open("log.txt"))):
        myLine=line.rstrip()
        myAction    =myLine[20:]
        myTime      =myLine[:20]
        text_1 = gui.draw_text(x=30, y=myY, color="black", font_size=8, text=myTime)
        myY=myY+10
        text_2 = gui.draw_text(x=40, y=myY, color="black", font_size=8, text=myAction)
        myY=myY+10
        count+=1
        if count==10:
            break

# wait until WiFi connection has been established
time.sleep(10)

print("Step Guard started")
print("@RoniBandini - March 2024 - MIT License")
print("")

writeLog("Started")

client = TelegramClient('StepGuard', api_id, api_hash)
client.start()

img = gui.draw_image(w=240, h=320, image='images/stepguard.png')

@client.on(events.NewMessage())
async def handle(event):

    print(event.text)

    if event.text=='horn':
        action('horn')

    if event.text=='despeje':
        action('despeje')

    if event.text=='ring':
        action('ring')

    if event.text=='disable':
        disable=1

    if event.text=='enable':
        disable=0

client.run_until_disconnected()
