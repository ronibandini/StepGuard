// Step Guard Outside Unit 3.0
// Roni Bandini, May 2024, @RoniBandini, https://bandini.medium.com
// MIT License
// ESP32S3 with GROVE AI Vision 2.0 and Rpi Cam 1.3

#include <Seeed_Arduino_SSCMA.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
BLEService *pService;

SSCMA AI;

int afterDetectionPause=5000;
int lastDetected=0;

void setup()
{
  AI.begin();
  Serial.begin(460800);
  delay(5000);
    
  BLEDevice::init("STEPGUARD3.0");
  BLEServer *pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Started");
  pService->start();  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  pCharacteristic->setValue("Clear");
  pCharacteristic->notify();    
  BLEDevice::startAdvertising();
  
}

void loop()
{
    if (!AI.invoke(1, false, true))
    {
        Serial.println("Inference ok");       

        if(AI.boxes().size()>0)
        {
           
            Serial.println("Detected");  
            Serial.print(AI.boxes()[1].score);           
            pCharacteristic->setValue("Detected");
            pCharacteristic->notify();            
            delay(afterDetectionPause);            
            pCharacteristic->setValue("Clear");
            pCharacteristic->notify();     
            // added this line
            BLEDevice::startAdvertising();                  
        }       
    }
}