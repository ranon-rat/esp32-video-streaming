#include <Arduino.h>
#include <WiFi.h>
// espressif stuff

#include "esp_camera.h"
#include "esp_timer.h"
#include "ESPmDNS.h"
// idk wtf this does
#include "fb_gfx.h"
#include "soc/soc.h"          //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems


// This project was tested with the AI Thinker Model
#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM
//#define CAMERA_MODEL_WROVER_KIT

//config pins and network
#include "network_config.hpp"
#include "camera_pins.hpp"
// the files and streaming
#include "html_files.hpp"
#include "streaming.hpp"


void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(false);

  camera_config_t config = setup_camera_config(); // get the camera config

  // Camera init
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err); // in case of error
    return;
  }
  // Wi-Fi connection
  WiFi.begin(ssid, password); // connect to our own network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  if(!MDNS.begin("cameralol")){
    Serial.println("error starting mdns");
  }

  
  Serial.printf("\nWiFi connected\n\nCamera Stream Ready! Go to: http://%s", WiFi.localIP().toString().c_str());

  // Start streaming web server
  startCameraServer();
}

void loop()
{
  delay(1);
}
