/*!
Step Guard Outdoor Unit
Roni Bandini, April 2024, https://bandini.medium.com/ @RoniBandini
MIT License
DFRobot ESP32C3 Dev Module board with USB CDC on boot enabled
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char* host = "ESP32C3";

int LED_BLINK = 10;

const int trigPin = 9;
const int echoPin = 8;
#define BOT_TOKEN ""
String chatOperativo="";
String chatNotification="";
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
String img_url = "YOURSERVERHERE/stepguard.png";

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
bool Start = false;

long duration;
int distance;

int distanceLimit     =200;
int distanceLimitLow  =40;
int secondStrikeDelay =4000;
int afterDelay        =5000;
int heightCompensation =0;
int myDistance        = 0;

WebServer server(80);

/*
 * OTA Login page, edit password
 */

const char* loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
             "<td>Username:</td>"
             "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='ota123')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

/*
 * OTA Bin Load
 */

const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";


void setup(void) {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(115200);
  delay(1000);

  digitalWrite(LED_BLINK, 1);
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  digitalWrite(LED_BLINK, 0);
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  bot.sendPhoto(chatNotification, img_url, "Started");

  if (!MDNS.begin(host)) {  
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {

      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

int getDistance(){

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.println(distance);

  if (distance==0){
    Serial.println("Wrong distance");
    bot.sendMessage(chatNotification, "Wrong distance");
    delay(50000);
    distance=5000;
    }

  return distance;
}

void loop(void) {

  server.handleClient();
  delay(1);

  myDistance=getDistance();

  if (myDistance<distanceLimit and myDistance>distanceLimitLow){

     // check again, just in case
     delay(500);

     myDistance=getDistance();

     if (myDistance<distanceLimit and myDistance>distanceLimitLow){

       Serial.println("Something was detected");
       bot.sendMessage(chatOperativo, "ring");
       myDistance=myDistance+heightCompensation;
       bot.sendMessage(chatNotification, "There is something at the door - "+String(myDistance)+ "cm");

      delay(secondStrikeDelay);

      myDistance=getDistance();

      if (myDistance<distanceLimit and myDistance>distanceLimitLow){

        Serial.println("Something is still there");

        bot.sendMessage(chatOperativo, "despeje");

        delay(secondStrikeDelay);

         myDistance=getDistance();

         if (myDistance<distanceLimit and myDistance>distanceLimitLow){
          bot.sendMessage(chatOperativo, "horn");
         } // under distance horn

      } // under distance despeje

    } // under distance verification

  } // under distance

  delay(afterDelay);
}
