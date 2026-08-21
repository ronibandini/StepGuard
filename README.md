# 🚪🔊 Step Guard

**An indoor/outdoor doorway monitoring system that evolved from ultrasonic sensing to Edge AI person detection, combining ESP32 hardware, UNIHIKER, BLE, Telegram, logging, remote control, and progressive audio alerts.**

Step Guard was created to keep a street-facing doorstep clear when people remain seated or loitering there for extended periods.

The project evolved through several iterations:

```text
Step Guard 1.0
Ultrasonic sensing
      │
      ▼
Step Guard 2.0
Remote outdoor sensor
+ indoor UNIHIKER
+ Telegram
+ logs
+ progressive alerts
      │
      ▼
Step Guard 3.0
Computer vision
+ person detection
+ BLE
+ dog mode
```

The current repository contains the original sensor-based implementation and the later **AI-powered Step Guard 3.0**.

---

## ✨ Features

- 🚪 Doorstep occupancy monitoring
- 📡 Separate indoor and outdoor units
- ⚡ ESP32-based outdoor hardware
- 🖥️ UNIHIKER indoor controller
- 📷 Edge AI person detection in version 3.0
- 🧠 Grove Vision AI Module V2
- 📶 BLE communication in version 3.0
- 🌐 Telegram communication in earlier versions
- 📲 Remote notifications
- 📝 Local activity logging
- 🔘 Physical illuminated controls
- 🔊 Progressive audio alerts
- 🐕 Optional dog-bark first warning
- 📢 Recorded voice warning
- 🔔 Manual sound triggering
- 🔇 Remote mute / enable controls
- 🔄 OTA firmware updates
- 🖨️ Custom 3D-printed indoor enclosure
- 📜 MIT licensed

---

# 🧠 Concept

The basic behavior is:

```text
Outdoor area
     │
     ▼
Detect continued presence
     │
     ▼
Indoor controller
     │
     ├── Update screen
     ├── Write log
     ├── Notify remotely
     └── Start alert sequence
                │
                ▼
          First warning
                │
                ▼
        Still occupied?
                │
                ▼
         Stronger warning
```

The project intentionally separates:

```text
detection
```

from:

```text
notification / response
```

so the sensing hardware can evolve without replacing the indoor interface.

---

# 🧬 Project evolution

## Step Guard 1.0 — Ultrasonic detection

The first implementation used:

```text
ESP32
+
ultrasonic distance sensor
```

mounted near the doorstep.

If an object remained within the configured distance for several seconds, the device triggered a prerecorded request to clear the entrance.

The delay avoided activating the system during normal short events such as:

```text
opening the door
entering the house
leaving the house
```

Conceptually:

```text
HC-SR04
   │
   ▼
distance
   │
   ▼
occupied > threshold?
   │
   ▼
wait several seconds
   │
   ▼
play warning
```

---

# Step Guard 2.0 — Indoor / outdoor architecture

The next major implementation separated the system into two physical devices.

```text
OUTDOOR UNIT
┌────────────────────┐
│ Beetle ESP32-C3    │
│ Ultrasonic sensor  │
│ Wi-Fi              │
└─────────┬──────────┘
          │
          │ Telegram
          ▼
┌────────────────────┐
│     UNIHIKER       │
│                    │
│ Python             │
│ Color display      │
│ Buttons            │
│ Logs               │
│ Audio              │
│ Relay              │
└─────────┬──────────┘
          │
          ▼
     Amplifier
          │
          ▼
       Speaker
```

This architecture moved the sensor to a higher and more protected position while keeping the user interface indoors.

---

## 🧰 Step Guard 2.0 hardware

### Outdoor unit

| Qty | Component | Link |
|---:|---|---|
| 1 | **DFRobot Beetle ESP32-C3** | [DFRobot](https://www.dfrobot.com/product-2566.html) |
| 1 | **HC-SR04 ultrasonic sensor** | [DFRobot](https://www.dfrobot.com/product-53.html?tracking=hOuIhw4fDaJRTdy4abz04npbQC78dqxBkqVt7XMFYxEXj2s0ukWgm71wbut0ewUP) |
| 1 | 5 V power supply | — |
| 1 | Weather-resistant enclosure | — |

### Indoor unit

| Qty | Component | Link |
|---:|---|---|
| 1 | **DFRobot UNIHIKER M10** | [DFRobot](https://www.dfrobot.com/product-2691.html?tracking=hOuIhw4fDaJRTdy4abz04npbQC78dqxBkqVt7XMFYxEXj2s0ukWgm71wbut0ewUP) |
| 2 | **LED push buttons** | [DFRobot](https://www.dfrobot.com/product-2351.html?tracking=hOuIhw4fDaJRTdy4abz04npbQC78dqxBkqVt7XMFYxEXj2s0ukWgm71wbut0ewUP) |
| 1 | **Relay** | [DFRobot](https://www.dfrobot.com/product-64.html?tracking=hOuIhw4fDaJRTdy4abz04npbQC78dqxBkqVt7XMFYxEXj2s0ukWgm71wbut0ewUP) |
| 1 | **Speaker** | [DFRobot](https://www.dfrobot.com/product-1506.html?tracking=hOuIhw4fDaJRTdy4abz04npbQC78dqxBkqVt7XMFYxEXj2s0ukWgm71wbut0ewUP) |
| 1 | USB sound card | — |
| 1 | PAM8403 3 W amplifier | — |
| 1 | 5 V power supply | — |
| 1 | 3D-printed enclosure | [Cults3D](https://cults3d.com/en/3d-model/gadget/step-guard) |

---

# 🔌 Step Guard 2.0 wiring

## Outdoor unit

The documented ESP32-C3 configuration is:

```cpp
const int trigPin = 9;
const int echoPin = 8;
```

Connections:

| HC-SR04 | Beetle ESP32-C3 |
|---|---:|
| `TRIG` | `D9` |
| `ECHO` | `D8` |
| `VCC` | `3.3V` |
| `GND` | `GND` |

The distance function follows the standard ultrasonic timing method:

```cpp
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

digitalWrite(trigPin, HIGH);
delayMicroseconds(10);

digitalWrite(trigPin, LOW);

duration = pulseIn(echoPin, HIGH);

distance = duration * 0.034 / 2;
```

---

## Indoor unit

The documented UNIHIKER connections are:

| Device | UNIHIKER |
|---|---|
| Relay | `P23` |
| Green button | `P21` |
| White button | `P22` |

The relay powers the audio amplifier immediately before playback.

```text
UNIHIKER
   │
   ▼
Relay
   │
   ▼
PAM8403
   │
   ▼
Speaker
```

This prevents the amplifier from remaining powered continuously.

---

# 🖥️ UNIHIKER indoor interface

The indoor application is written in:

```text
Python
```

and uses the UNIHIKER touchscreen to display system status.

The indoor controller handles:

- incoming events
- audio playback
- physical buttons
- relay switching
- system status
- event logging
- remote commands

All operations are written to a text log.

---

# 📝 Activity log

The indoor unit records activity such as:

```text
Detection
Ring
Clear request
Horn
Disable
Enable
```

A logging workflow can be represented as:

```text
event
  │
  ▼
timestamp
  │
  ▼
text log
  │
  ▼
UNIHIKER display
```

This makes it possible to review previous activations without relying exclusively on Telegram history.

---

# 📲 Telegram architecture

Earlier versions use Telegram for both:

```text
outdoor → indoor communication
```

and:

```text
family notifications
```

Two different groups can be configured:

```cpp
String chatOperativo = "";
String chatNotification = "";
```

The outdoor ESP32 sends detection events to the communications group.

The indoor Python application listens for messages using **Telethon**.

Example architecture:

```text
ESP32
   │
   ▼
Telegram operational group
   │
   ▼
UNIHIKER
   │
   ├── play audio
   ├── log action
   └── update screen
```

---

# 📲 Telegram bot setup

Create a bot using:

```text
@BotFather
```

Commands:

```text
/start
/newbot
/setprivacy
```

Disable privacy mode if the bot must read group messages.

Create separate Telegram groups for:

```text
system communication
family notifications
```

Then obtain the chat IDs using:

```text
https://api.telegram.org/bot<TOKEN>/getUpdates
```

Configure the resulting IDs in the ESP32 source.

---

# 🎛️ Remote commands

The Step Guard 2.0 Telegram interface supports manual commands.

| Command | Action |
|---|---|
| `ring` | Play initial warning |
| `despeje` | Play recorded request to clear the entrance |
| `horn` | Play loud alarm |
| `disable` | Disable automatic sounds |
| `enable` | Enable automatic sounds |

Conceptually:

```text
Phone
  │
  ▼
Telegram command
  │
  ▼
UNIHIKER
  │
  ▼
selected action
```

---

# 🔊 Progressive audio alerts

Step Guard does not immediately start with the strongest sound.

The system uses a progressive response.

Example sequence:

```text
Detection
   │
   ▼
Initial sound
   │
   ▼
Wait
   │
   ▼
Recorded request
   │
   ▼
Wait
   │
   ▼
Loud alarm
```

Version 3.0 adds:

```text
Dog Mode
```

which can replace the initial sound with recorded dog barking.

---

# 🔄 OTA updates

The outdoor ESP32 can be installed in a location that is inconvenient to reach physically.

For that reason, the project supports **Over-the-Air firmware updates**.

The documented configuration uses:

```cpp
const char* host = "ESP32C3";
```

with a small web-based OTA updater.

Workflow:

```text
Arduino IDE
     │
     ▼
Sketch
→ Export Compiled Binary
     │
     ▼
.bin file
     │
     ▼
ESP32 OTA web page
     │
     ▼
Upload firmware
```

This avoids connecting a USB cable to the outdoor module every time the firmware changes.

---

# 📷 Step Guard 3.0 — Edge AI person detection

Version 3.0 replaces distance-based sensing with **computer vision**.

```text
Raspberry Pi Camera
       │
       ▼
Grove Vision AI V2
       │
       ▼
Person detection model
       │
       ▼
XIAO ESP32S3
       │
       ▼
BLE characteristic
       │
       ▼
UNIHIKER
       │
       ▼
Alerts + logs + Telegram
```

The main improvements are:

1. **Detect people specifically** instead of any nearby object.
2. **BLE communication** replaces Telegram as the indoor/outdoor transport.
3. **Dog Mode** adds dog barking as an optional first-stage alert.

---

# 🧰 Step Guard 3.0 hardware

## Outdoor unit

| Qty | Component |
|---:|---|
| 1 | **Seeed Studio XIAO ESP32S3 Sense** |
| 1 | **Grove Vision AI Module V2** |
| 1 | **Raspberry Pi Camera Module** |

## Indoor unit

| Qty | Component |
|---:|---|
| 1 | **DFRobot UNIHIKER M10** |
| 2 | LED push buttons |
| 1 | Relay |
| 1 | USB audio interface |
| 1 | PAM8403 amplifier |
| 1 | Speaker |

---

# 🧠 Person detection model

Step Guard 3.0 uses a **pretrained person-detection model** available for the Grove Vision AI Module V2.

No custom training dataset is required for the reference implementation.

The model is loaded as a:

```text
.uf2
```

file.

General deployment flow:

```text
Download person-detection model
        │
        ▼
Put Grove Vision AI V2 in drive mode
        │
        ▼
Copy .uf2
        │
        ▼
Restart module
        │
        ▼
Run inference
```

The ESP32-side code invokes the model and checks whether bounding boxes are present.

The documented logic follows this pattern:

```cpp
if (!AI.invoke(1, false, true))
{
    Serial.println("Inference ok");

    if (AI.boxes().size() > 0)
    {
        Serial.println("Detected");
    }
}
```

---

# 🎯 Why computer vision?

Distance sensing cannot distinguish between:

```text
person
cardboard box
poster
object
plant
bag
```

If something blocks the ultrasonic sensor, the result can appear identical.

Version 3.0 instead asks:

```text
Is a person present?
```

rather than:

```text
Is something close?
```

This significantly changes the detection logic.

```text
Ultrasonic

object nearby
    │
    ▼
trigger


Computer vision

camera frame
    │
    ▼
person model
    │
 ┌──┴───┐
 ▼      ▼
yes     no
 │
 ▼
trigger
```

---

# 📡 BLE communication

Version 3.0 no longer needs Telegram for communication between the outdoor sensor and indoor controller.

The XIAO ESP32S3 operates as a:

```text
BLE server
```

and exposes a characteristic whose state changes between values such as:

```text
Clear
Detected
```

The UNIHIKER operates as the BLE client.

The Python side uses:

```text
bluepy
```

to connect to the ESP32 and read characteristic changes.

Architecture:

```text
ESP32S3
BLE server
    │
    │ characteristic
    ▼
UNIHIKER
BLE client
    │
    ▼
Python
```

Telegram remains useful for notifications from the indoor unit.

---

# 🐕 Dog Mode

Step Guard 3.0 adds an alternative first-stage alert.

Normal sequence:

```text
Detection
   │
   ▼
Ring
   │
   ▼
"Clear the entry"
   │
   ▼
Loud sound
```

Dog Mode:

```text
Detection
   │
   ▼
Dog barking
   │
   ▼
"Clear the entry"
   │
   ▼
Loud sound
```

The mode can be selected from the indoor controls.

---

# 📶 Why BLE?

The outdoor unit may be located far from the Wi-Fi router but relatively close to the indoor controller.

BLE creates a direct link:

```text
outdoor ESP32S3
      │
      ▼
    BLE
      │
      ▼
 indoor UNIHIKER
```

This removes the requirement that both devices maintain a reliable Wi-Fi connection for local communication.

---

# 🖨️ 3D-printed enclosure

The indoor unit uses a custom enclosure designed in **Fusion 360**.

**[Step Guard enclosure — Cults3D](https://cults3d.com/en/3d-model/gadget/step-guard)**

The enclosure accommodates:

- UNIHIKER
- illuminated buttons
- speaker / audio hardware
- internal electronics

---

# 📁 Repository structure

The current repository contains:

```text
StepGuard/
├── Version 1.0/
├── Version 3.0/
├── LICENSE
└── README.md
```

### `Version 1.0/`

Source and assets for the original sensor-based architecture.

### `Version 3.0/`

ESP32 and Python source for the Edge AI + BLE implementation.

### `LICENSE`

MIT License.

---

# 🚀 Building Step Guard 2.0

## 1. Clone the repository

```bash
git clone https://github.com/ronibandini/StepGuard.git
cd StepGuard
```

---

## 2. Configure the outdoor ESP32

Set:

```cpp
#define BOT_TOKEN ""

String chatOperativo = "";
String chatNotification = "";

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

String img_url =
    "https://YOURSERVER/stepguard.png";
```

The reference project also includes:

```cpp
const char* host = "ESP32C3";

const int trigPin = 9;
const int echoPin = 8;
```

---

## 3. Configure OTA authentication

The historical example contains an OTA web form with credentials such as:

```text
admin
123456
```

Change these before deploying the system.

Do not expose the update interface to an untrusted network with default credentials.

---

## 4. Configure UNIHIKER

Set the Telegram API values:

```python
api_id = ""
api_hash = ""
```

Configure the action delay:

```python
myDelay = 5
```

and GPIO:

```python
relay = Pin(Pin.P23, Pin.OUT)

buttonWhite = Pin(Pin.P21, Pin.IN)
buttonGreen = Pin(Pin.P22, Pin.IN)
```

---

## 5. Install Python dependencies

The indoor application uses packages including:

```text
Telethon
requests
```

Install as required on UNIHIKER:

```bash
pip install telethon requests
```

---

## 6. Configure audio

The project uses a USB sound adapter connected to:

```text
PAM8403 amplifier
```

and then:

```text
speaker
```

Prepare audio files for the configured actions.

Examples:

```text
ring
clear entrance
horn
dog barking
```

---

## 7. Run

Start the UNIHIKER Python application and power the outdoor ESP32.

Test manually before enabling unattended operation.

---

# 🚀 Building Step Guard 3.0

## 1. Prepare the vision hardware

Connect:

```text
Raspberry Pi Camera
        │
        ▼
Grove Vision AI Module V2
        │
        ▼
XIAO ESP32S3
```

---

## 2. Install a person-detection model

Load the compatible pretrained `.uf2` model to the Grove Vision AI Module V2.

Current model availability and deployment procedures should be checked against the latest Seeed Studio documentation.

---

## 3. Configure Arduino IDE

Install current ESP32 board support and the Seeed vision libraries required by the project.

Open the ESP32 source inside:

```text
Version 3.0/
```

---

## 4. Configure BLE

The ESP32 acts as the BLE server.

The UNIHIKER Python application acts as the client.

Make sure the UUIDs used on both sides match exactly.

---

## 5. Install BluePy

On the UNIHIKER:

```bash
pip install bluepy
```

The indoor script uses BluePy to read the BLE characteristic from the outdoor unit.

---

## 6. Configure Telegram notifications

Version 3.0 can still send Telegram notifications from the UNIHIKER.

Configure the required bot token and destination values in the Python source.

---

## 7. Test detection

Before enabling sounds, confirm that the BLE state changes correctly:

```text
No person
→ Clear

Person detected
→ Detected
```

Then enable the audio sequence.

---

# 🔬 Ideas for extending the project

1. **⏱️ Add loitering-duration logic to the vision model** — trigger only if a detected person remains in the monitored area continuously for a configurable period.

2. **📊 Local detection statistics** — store timestamps, durations and alert levels in SQLite and display daily or weekly activity charts on the UNIHIKER.

3. **🌙 Add day/night profiles** — use different detection thresholds, sound levels and alert sequences according to schedule or ambient-light conditions.

---

# 📰 External references

## 🗞️ Independent editorial coverage

### Hackster News — Stepping Up Security

Hackster News published a dedicated feature about the sensor-based Step Guard.

The article covers:

- ESP32-C3 outdoor unit
- ultrasonic sensing
- Telegram communication
- UNIHIKER indoor console
- audio amplifier
- local logs
- physical controls
- custom enclosure

**[Stepping Up Security — Hackster News](https://www.hackster.io/news/stepping-up-security-a619bc7ff4ce)**

---

## 🗞️ Hackster News — You Don't Want to Step to This

Hackster News later published a second article dedicated to **Step Guard 3.0**.

The article covers:

- computer vision
- Grove Vision AI Module V2
- person detection
- reduction of ultrasonic false positives
- XIAO ESP32S3
- camera-based monitoring

**[You Don't Want to Step to This — Hackster News](https://www.hackster.io/news/you-don-t-want-to-step-to-this-d0faba407806)**

---

# 📰 Seeed Studio

## Step Guard 3.0 — Project of the Month spotlight

Seeed Studio selected Step Guard 3.0 as one of its featured **Grove Vision AI V2** projects for May 2024.

**[Seeed Project of the Month — May 2024](https://www.seeedstudio.com/blog/2024/01/31/seeed-thematic-project-of-month-campaign/)**

---

## Seeed Vision Challenge

Step Guard 3.0 was also highlighted in Seeed Studio's **Vision Challenge** roundup.

The official article describes the project as using:

- Grove Vision AI Module V2
- AI-based detection
- BLE
- UNIHIKER
- dog barking
- progressive sound alerts

**[Seeed Vision Challenge — Winners & Cool Projects](https://www.seeedstudio.com/blog/2024/07/15/vision-challenge-winners-and-projects/)**

---

# 🛠️ Project tutorials

## Hackster.io — Step Guard 2.0

The complete indoor/outdoor system tutorial covers:

- UNIHIKER
- Beetle ESP32-C3
- HC-SR04
- Telegram
- remote commands
- OTA firmware updates
- buttons
- relay
- USB sound interface
- amplifier
- logs
- enclosure

**[Step Guard 2.0 — Hackster.io](https://www.hackster.io/roni-bandini/step-guard-2-0-821d05)**

---

## Hackster.io — Step Guard 3.0

The version 3.0 tutorial documents:

- XIAO ESP32S3 Sense
- Grove Vision AI Module V2
- Raspberry Pi Camera
- pretrained person detection
- BLE server
- BluePy client
- UNIHIKER
- Dog Mode
- Telegram notifications

**[Step Guard 3.0 — Hackster.io](https://www.hackster.io/roni-bandini/step-guard-3-0-39252b)**

---

# ✍️ Medium

## Step Guard 2.0

Spanish-language article documenting the indoor/outdoor version, hardware, Telegram architecture, sound system, circuit and enclosure.

**[Cómo mantener despejado tu escalón — Medium](https://bandini.medium.com/step-guard-2-0-por-favor-mantener-despejado-el-escal%C3%B3n-d5097cd84c47)**

---

## Step Guard 3.0

Spanish-language article documenting the migration from ultrasonic sensing to AI-based person detection.

**[Step Guard 3.0 — detección con IA](https://bandini.medium.com/step-guard-3-0-detecci%C3%B3n-con-ia-20adf48761e2)**

---

# 🎥 Demo / field testing

The project has been tested in its intended real-world environment, where normal pedestrian activity provides spontaneous test cases.

A Step Guard 3.0 demo is linked from the project documentation:

**[Step Guard demo — TikTok](https://www.tiktok.com/@ronibandini/video/7354165402626297093)**

---

# 🖨️ Cults3D

The indoor console enclosure is available as a 3D-printable model:

**[Step Guard — Cults3D](https://cults3d.com/en/3d-model/gadget/step-guard)**

---

# 📕 Contracultura Maker

Step Guard is part of a broader collection of experimental machines built around real problems, unusual interfaces, automation, embedded AI and physical computing.

More projects and context are collected in:

**[Contracultura Maker — book](https://bandini.medium.com/libro-de-contracultura-maker-94d1bb0d951c)**

---

# 📚 Useful references

- **[UNIHIKER M10](https://www.unihiker.com/products/m10)**
- **[UNIHIKER Documentation](https://www.unihiker.com/wiki/)**
- **[DFRobot Beetle ESP32-C3](https://www.dfrobot.com/product-2566.html)**
- **[Seeed Studio Grove Vision AI Module V2](https://www.seeedstudio.com/Grove-Vision-AI-Module-V2-p-5851.html)**
- **[Seeed Studio XIAO ESP32S3](https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html)**
- **[ESP32 BLE documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ble.html)**
- **[Telethon](https://docs.telethon.dev/)**
- **[Telegram Bot API](https://core.telegram.org/bots/api)**

---

# 🔗 You may also be interested in...

Other projects by **Roni Bandini** combining sensing, embedded AI, automation and unconventional physical interfaces.

## 🎧🚫 Reggaeton Be Gone

**Machine Learning system that detects reggaeton and reacts through Bluetooth experimentation.**

Another project built around a local environmental annoyance and an automated technical response.

**[github.com/ronibandini/reggaetonBeGone](https://github.com/ronibandini/reggaetonBeGone)**

---

## 🔔📷 AI Camera Doorbell

**ESP32-S3 camera doorbell combining local person detection, speech transcription, LLM logic, Telegram and physical output.**

A more advanced camera-based access and notification system.

**[github.com/ronibandini/aicamdoorbell](https://github.com/ronibandini/aicamdoorbell)**

---

## 🔔🤖 RinGPT

**AI-powered doorbell and autonomous receptionist built with UNIHIKER, speech recognition, OpenAI models, Telegram, sensors and physical tools.**

Another UNIHIKER-based project centered on events occurring at a building entrance.

**[github.com/ronibandini/rinGPT](https://github.com/ronibandini/rinGPT)**

---

# ⚠️ Deployment notes

## Camera placement

Step Guard 3.0 performs person detection.

Install the camera so that it monitors only the area required for the project and comply with applicable local privacy and surveillance rules.

The reference implementation detects the presence of a person; it does not require facial identification.

---

## Sound level

The project can operate a high-volume amplifier and speaker.

Use reasonable output levels and comply with local noise regulations.

Continuous high sound pressure levels can damage hearing.

---

## Network credentials

Do not commit configured values such as:

```text
Wi-Fi password
Telegram token
Telegram API ID
Telegram API hash
chat IDs
OTA password
```

to a public repository.

Use environment variables, local configuration files or another protected mechanism where possible.

---

# 📜 License

Step Guard is released under the **MIT License**.

See [`LICENSE`](LICENSE) for details.

---

# 👤 Author

**Roni Bandini**

Maker, AI developer, electronic artist and writer.

- 🐙 GitHub: [@ronibandini](https://github.com/ronibandini)
- 💼 LinkedIn: [Roni Bandini](https://www.linkedin.com/in/ronibandini/)
- 📸 Instagram: [@ronibandini](https://www.instagram.com/ronibandini/)
- 🐦 X: [@RoniBandini](https://x.com/RoniBandini)
- ✍️ Medium: [bandini.medium.com](https://bandini.medium.com/)
- 🛠️ Hackster: [Roni Bandini](https://www.hackster.io/roni-bandini)
- 🔧 Hackaday.io: [Roni Bandini](https://hackaday.io/ronibandini)

Buenos Aires, Argentina.
