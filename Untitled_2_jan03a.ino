/* 
  Sketch generated by the Arduino IoT Cloud Thing "Untitled 2"
  https://create.arduino.cc/cloud/things/cfbe41ae-d9f6-43f7-8521-20c8ab45ed18 

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  CloudLight no1;
  CloudLight no2;
  CloudLight no3;
  CloudLight no4;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#define aa 13
#define bb 12
#define VV 26
#define dd 27


#include "thingProperties.h"

#define ESP_DRD_USE_SPIFFS true

// Include Libraries

// WiFi Library
#include <WiFi.h>
// File System Library
#include <FS.h>
// SPI Flash Syetem Library
#include <SPIFFS.h>
// WiFiManager Library
#include <WiFiManager.h>
// Arduino JSON library
#include <ArduinoJson.h>

// JSON configuration file
#define JSON_CONFIG_FILE "/test_config.json"

// Flag for saving data
bool shouldSaveConfig = false;

// Variables to hold data from custom textboxes
char testString[50] = "test value";
int testNumber = 1234;

// Define WiFiManager Object
WiFiManager wm;

void saveConfigFile()
// Save Config in JSON format
{
  Serial.println(F("Saving configuration..."));
  
  // Create a JSON document
  StaticJsonDocument<512> json;
  json["testString"] = testString;
  json["testNumber"] = testNumber;

  // Open config file
  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if (!configFile)
  {
    // Error, file did not open
    Serial.println("failed to open config file for writing");
  }

  // Serialize JSON data to write to file
  serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0)
  {
    // Error writing file
    Serial.println(F("Failed to write to file"));
  }
  // Close file
  configFile.close();
}

bool loadConfigFile()
// Load existing configuration file
{
  // Uncomment if we need to format filesystem
  // SPIFFS.format();

  // Read configuration from FS json
  Serial.println("Mounting File System...");

  // May need to make it begin(true) first time you are using SPIFFS
  if (SPIFFS.begin(false) || SPIFFS.begin(true))
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists(JSON_CONFIG_FILE))
    {
      // The file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
      if (configFile)
      {
        Serial.println("Opened configuration file");
        StaticJsonDocument<512> json;
        DeserializationError error = deserializeJson(json, configFile);
        serializeJsonPretty(json, Serial);
        if (!error)
        {
          Serial.println("Parsing JSON");

          strcpy(testString, json["testString"]);
          testNumber = json["testNumber"].as<int>();

          return true;
        }
        else
        {
          // Error loading JSON data
          Serial.println("Failed to load json config");
        }
      }
    }
  }
  else
  {
    // Error mounting file system
    Serial.println("Failed to mount FS");
  }

  return false;
}


void saveConfigCallback()
// Callback notifying us of the need to save configuration
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback(WiFiManager *myWiFiManager)
// Called when config mode launched
{
  Serial.println("Entered Configuration Mode");

  Serial.print("Config SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());

  Serial.print("Config IP Address: ");
  Serial.println(WiFi.softAPIP());
}


void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 
  Serial.begin(115200);
  pinMode(aa,OUTPUT);
  pinMode(bb,OUTPUT);
  pinMode(VV,OUTPUT);
  pinMode(dd,OUTPUT);
  
  
  digitalWrite(aa,HIGH);
  digitalWrite(bb,HIGH);
  digitalWrite(VV,HIGH);
  digitalWrite(dd,HIGH);

    // Change to true when testing to force configuration every time we run
  bool forceConfig = false;

  bool spiffsSetup = loadConfigFile();
  if (!spiffsSetup)
  {
    Serial.println(F("Forcing config mode as there is no saved config"));
    forceConfig = true;
  }

  // Explicitly set WiFi mode
  WiFi.mode(WIFI_STA);

  // Setup Serial monitor
  Serial.begin(115200);
  delay(10);

  // Reset settings (only for development)
  wm.resetSettings();

  // Set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback);

  // Set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  // Custom elements

  // Text box (String) - 50 characters maximum
  WiFiManagerParameter custom_text_box("key_text", "Enter your string here", testString, 50);
  
  // Need to convert numerical input to string to display the default value.
  char convertedValue[6];
  sprintf(convertedValue, "%d", testNumber); 
  
  // Text box (Number) - 7 characters maximum
  WiFiManagerParameter custom_text_box_num("key_num", "Enter your number here", convertedValue, 7); 

  // Add all defined parameters
  wm.addParameter(&custom_text_box);
  wm.addParameter(&custom_text_box_num);

  if (forceConfig)
    // Run if we need a configuration
  {
    if (!wm.startConfigPortal("Mr_Padakam_Wifi", "password"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  }
  else
  {
    if (!wm.autoConnect("NEWTEST_AP", "password"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // if we still have not connected restart and try all over again
      ESP.restart();
      delay(5000);
    }
  }

  // If we get here, we are connected to the WiFi

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Lets deal with the user config values

  // Copy the string value
  strncpy(testString, custom_text_box.getValue(), sizeof(testString));
  Serial.print("testString: ");
  Serial.println(testString);

  //Convert the number value
  testNumber = atoi(custom_text_box_num.getValue());
  Serial.print("testNumber: ");
  Serial.println(testNumber);


  // Save the custom parameters to FS
  if (shouldSaveConfig)
  {
    saveConfigFile();
  }


  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  // Your code here 
  
  
}


/*
  Since No1 is READ_WRITE variable, onNo1Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onNo1Change()  {
  // Add your code here to act upon No1 change
  if(no1){
    Serial.print("on1");
    digitalWrite(aa,LOW);
  }
  else{
    Serial.print("on1n");
    digitalWrite(aa,HIGH);
  }
}

/*
  Since No2 is READ_WRITE variable, onNo2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onNo2Change()  {
  // Add your code here to act upon No2 change
  if(no2){
    Serial.print("on2h");
    digitalWrite(bb,LOW);
  }
  else{
    Serial.print("on2n");
    digitalWrite(bb,HIGH);
  }
}

/*
  Since No3 is READ_WRITE variable, onNo3Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onNo3Change()  {
  // Add your code here to act upon No3 change
  if(no3){
    Serial.print("on3h");
    digitalWrite(VV,LOW);
  }
  else{
    Serial.print("on3n");
    digitalWrite(VV,HIGH);
  }
}

/*
  Since No4 is READ_WRITE variable, onNo4Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onNo4Change()  {
  // Add your code here to act upon No4 change
  if(no4){
    Serial.print("on4h");
    digitalWrite(dd,LOW);
  }
  else{
    Serial.print("on4l");
    digitalWrite(dd,HIGH);
  }
}

