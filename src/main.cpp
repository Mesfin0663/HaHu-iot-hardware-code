// Libraries to create the functionality of connecting to the Available WIFI
#include <WiFiManager.h>
WiFiManager wm;
String wifiName = "";
#include "time.h"
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

// Libraries to connect the hardware with the server
#include <Firebase_ESP_Client.h> //Main library
#include <addons/TokenHelper.h>  //Provide the token generation process info.
#include <addons/RTDBHelper.h>   //Provide the RTDB payload printing info and other helper functions.

// Cloud server credential to establish successful comunication between the hardware and the software
#define API_KEY "AIzaSyAofcWUVCQ_NeRQzrpsemiQtrLmtQ7bpi8" // are created when the project is created
#define FIREBASE_PROJECT_ID "fir-iot-aac7e"
#define FIREBASE_CLIENT_EMAIL "firebase-adminsdk-dywuq@fir-iot-aac7e.iam.gserviceaccount.com"
#define DATABASE_URL "https://fir-iot-aac7e-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCq7HJpRMqvsL2t\nGh/mdoBTmxdBxgp/tBx+HjUcvq5Y0MQJUNoqgHXjI8R3qNiekKQcmRBay4ulChYn\nkzUmjXvqFScZK15/JkL9nuVcKLvZi4BRVYXPjWVnrVXlUnNOCWhPYWaO8LNTSb5Z\nT4KS7VicqCj0Hah3O+AZlrL8R8a/9jJsnn9Noa/cUv26gMwnIduzW0SNStSayQg4\n4/cBxaN8ghZRCs3Vs7haamKUZEJlKVZ1PBpmGX9iKNa3iB7boB9hsN8L1w8nBNg+\nj1RF/L20mXepwCaD7bCXM651NA7gECa3UBQQaJaPecLI5PX5139ZVa2hhNhL2nyM\n2vwqtM9PAgMBAAECggEAJchkth5ty45/Lz+iI5i4X0LeTjzP31DaJ4rX5MXF1n43\nh3Om9lQ1OwOSD310fQCuNmAjfF6YPOBQ62igDCikZqdMYET0+5hioJu6Sbf7Zmhu\n8gu8q5eu/c8UOMAUGwD7XOjD+f79NQENIcLwszfiKacGfof70Hec3ysv35KiPZZ6\n/9dxUO1AygkRnKTu+SxM7E9XmyOLQwjwCKcVRB6GtHALR/FTwl8w5f3kpGAs2MmB\nYtKTaj7oPkWpZixt9Qbh3/uSYveAE3e2vQmOwgaKHObZabw5JnxLfpYmDveqxjOY\niV+r/MEF+883jilT5o3ogINNmxMX9fiMXcR1TSAA8QKBgQDeqgEcftwVe07txfgP\nauQotLTgky/AMxS0YFpL+rmcgFSzpz83oLeiuIJorQcxCsBaddErHAiMNdDJM+VL\nm7n9sAnh1eFtQFqaEII1QQuAXJeKkqU/paT8SlOzhCBIsjnskBTPv+C1cYw5galL\nmODK/FvCzz4OIThF68dHT1ixmQKBgQDEg2XTVIgK0IXBklgmEgLja5HFGyXenzkk\nQ9oOEesXdIDwXgvqHUDffDeyVNa2DDWqKE8iTwaspSCEwtkpSB5qYJHdn4c0mzst\nSnF/BAAiQ5SbkEOVIJhvr5Ln0dJy7udfZ39PwgOLt0F3lTeirSEPm97ueP19Xf6J\n2qb5umppJwKBgQClPS5D8lTrUC3bwG19K4sUNj6A5lmqXX4A+fwopHE4FfhFtljw\ntEFy7VBB1B4hI+RpfUNzpYMmYg2GMw/3kyJshEZhJPL8u6RA7v1h/dpGMPB48qjV\nWdzdKV+COgwnRrQ7vRTks3VT0WcJihKTumkf2jVSla9slvSG1xl07wpYCQKBgQCn\nLo5TUugGI06X1vPtN4eDiQFTqfv2wIjxhFQfgjj6FGCZnbkPLGIDf3821RxxdkO1\nhmOEcazg9f6LU+VCTJurroqJ/G0fYSzgJCQYhgPDIrFsAzirxzZIVhtp40TiCMOI\nk9lotksj8vzcSnMxj46/48CuKvqlTHD/QKirbl+XIwKBgB6C+E1OwqRrR9VGCnct\nhs0S5N6dUjCK9njvKrYLr5iGOejkxwuyT5rvvU0YMJuwbo+u7B7SVqXy12wek/kt\npS9VJ4EamAIzJW4LULjJH5BTVKo0S+dWuz3GABvsIscyOXhooakYdEneYQYLStvY\nOSRoWnMkJHcjV4GioixvRdmi\n-----END PRIVATE KEY-----\n";
// Individual user ID that is the owner of the hardware
// It is used to authenticate the user to the cloud server
String uid = "fbvBBcTbMqgKH547iokZqJKjIFw2";

// Variables used in the library to send the data to the server
/* 5. Define the Firebase Data object */
FirebaseData fbdo;

/* 6. Define the FirebaseAuth data for authenti1cation data */
FirebaseAuth auth;

/* 7. Define the FirebaseConfig data for config data */
FirebaseConfig config;

// DHT library for temperature and humidity sensor of DHT11
#include <DHT.h>
#define DHTPIN 18     // Choosing Pin number
#define DHTTYPE DHT11 // specifying the type of sensor
DHT dht(DHTPIN, DHTTYPE);
float temperature;
float humidity;
float moisture;

// Liquid crystal library for programming the hard ware
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 4);
// Time interval variables used to print connection status on LCD
unsigned long previousTimeConn = millis();
int conInterval = 6000;

// variables used for time counting
unsigned long second = 1000; // one second
unsigned long minute = 60000; // one minute
unsigned long hour = 60 * minute; // one hour

// Time Intervals to send the Data to the server every 3 seconds
unsigned long dataMillis = 0;
unsigned long previousDataSendTime = millis(); //the current time in the future willl be previous time
int sendDataTimeInterval = 3000; // can be changed to any value 

unsigned long prevDataTime = millis();
int dataInterval = second;

unsigned long dataPerQuarter = millis();
int dataIntervalPerQuarter = second;


unsigned long previousSensorTime = millis();
int sensorInterval = 4000;
// Variables and constants used to establish correct Network or server time
struct tm timeinfo;
int count = 0;
const char *NTPServer = "pool.ntp.org";
const long GMTOffset_sec = 3600;     
const int DayLightOffset_sec = 3600;

// variables for controlling a pin with remote and local mode
bool manualMode = false;
bool localMode = false;
bool remotBotton = false;
bool localButton = false;
bool waterPump = false;
// Time interval for restarting the Microcontroller 
unsigned long prevRestart = 0;
unsigned long restartInterval = 60 * minute;
/* A method used to establish Server time requird to establish successfull
 server communication */
void printLocalTime()
{
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
//The method that is used 
void connectToWifi(){
   // Start of Wifi Connection Set Up
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // automatically connect using saved credentials if they exist
  WiFi.setAutoReconnect(true);
  // If connection fails it starts an access point with the specified name
  wm.setEnableConfigPortal(false);
  wm.setConfigPortalBlocking(false);
  if (wm.autoConnect("changeMe", "12345678"))
  {
    Serial.println("connected...yeey :)");
    wifiName = WiFi.SSID();
  }
  else
  {
    Serial.println("Configportal running");
  }
// End of wifi connection setup
}

//A mthod used to connect the hardware with the Firebase server
void connectToFirebase(){


  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the sevice account credentials and private key (required) */
  config.service_account.data.client_email = FIREBASE_CLIENT_EMAIL;
  config.service_account.data.project_id = FIREBASE_PROJECT_ID;
  config.service_account.data.private_key = PRIVATE_KEY;

  auth.token.uid = uid;

  /* Assign the RTDB URL */
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(false);

  /* path for user data is now "/UsersData/Node1" */
  String base_path = "/users/";

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  /** Assign the maximum retry of token generation */
  config.max_token_generation_retry = 5;

}
// the setup function that is required to perform the general setup
// It only run once
void setup()
{
  Serial.begin(115200); // Setting up the baudrate for apploading the code on the micrcontroller
  // Declaring Input output pins for later user


  pinMode(26, OUTPUT); // for controlling the relay
  pinMode(17, INPUT_PULLDOWN); // for erasing wifi and reseting the wifi name and passowrd
  pinMode(19, INPUT_PULLDOWN); // for choosing Automatic or Manual control
  pinMode(15, INPUT_PULLDOWN); // For choosing manual rmote or manual local
  pinMode(16, INPUT_PULLDOWN); // For controlling the  relay locally
  
  connectToWifi();

// start Demonstrating Our Brand to Our Users until the setup is completed
  lcd.init(); // initialize the lcd
  lcd.backlight();

  lcd.setCursor(0, 0); // move cursor to   (0, 0)
  lcd.print("  AASTU IOT");
  delay(1000);                
  lcd.setCursor(0, 1);        
  lcd.print("  Starting..."); 
  delay(1000);
  dht.begin();
// End of Demonstrating Our Brand to Our Users this message will be cleared from the lcd at the end of the setup

  printLocalTime();

connectToFirebase();
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!getLocalTime(&timeinfo))
    {
      for (int i = 0; i < 3; i++)
      {
        Serial.print("Trying to get Time..");
        Serial.println(i);
        configTime(GMTOffset_sec, DayLightOffset_sec, NTPServer);
        if (getLocalTime(&timeinfo))
        {
          Firebase.begin(&config, &auth);
          return;
        }
      }
    }
  }

  lcd.clear();
}
// Method used to send a status update on the server
void sendState(double val)
{

  if ((WiFi.RSSI() < -20) && (WiFi.RSSI() > -150))
  {
    String place = "/users/";
    place += uid; //<- user uid of current user that sign in with Emal/Password
    place += "/Devices/Device_1/Live/State";

    FirebaseJson json;

    json.set("Value", val);
    // now we will set the timestamp value at Ts
    json.set("Ts/.sv", "timestamp"); // .sv is the required place holder for sever value which currently supports only string "timestamp" as a value

    // Set data with timestamp
    Serial.printf("", Firebase.RTDB.setJSON(&fbdo, place, &json) ? fbdo.to<FirebaseJson>().raw() : "");
    // Serial.printf("Set data with timestamp... %s\n", Firebase.RTDB.setJSON(&fbdo, place, &json) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());
  }
  else
  {
    Serial.println("Low Signal");
  }
}
// A method used to print connection status on  LCD
void printConection()
{
  // Serial.println("Print CONN to LCD");

  if (WiFi.status() != WL_CONNECTED)
  {
    // Serial.println("Print Conn to LCD");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WIFI NAME:");
    lcd.print(wifiName);
    // Serial.print("WIFI NAME:");
    //  Serial.print(WiFi.SSID());
    //  Serial.println("No");
    long rssi = WiFi.RSSI();
    // Serial.print("WIFI STRENGTH IS :::");
    // Serial.println(rssi);
    lcd.setCursor(0, 1); // move cursor to   (2, 1)
    lcd.print("CON: ");
    lcd.print("NO");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WIFI NAME:");
    lcd.print(wifiName);

    lcd.setCursor(0, 1); // move cursor to   (2, 1)
    lcd.print("CON: ");
    lcd.print("YES");
    //  Serial.print(wifiName);
    //   Serial.print(WiFi.SSID());
    Serial.println("YES");
    long rssi = WiFi.RSSI();
    // Serial.print("WIFI STRENGTH IS :::");
    // Serial.println(rssi);
    lcd.setCursor(0, 2);
    lcd.print("WIFI STRENGTH: ");
    lcd.print(rssi);
  }
}
//Method used to print sensor value on LCD
void printSensor(double val1, double val2, double val3, double val4, double val5, double val6)
{
  // Serial.println("Print SENSOR to LCD");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(val1);
if(manualMode == false){
  lcd.setCursor(12, 0);
    lcd.print("AUTO");
}else{
   if (localMode == false)
  {

    lcd.setCursor(14, 0);
    lcd.print("RC");
  }
  else
  {

    lcd.setCursor(14, 0);
    lcd.print("LC");
  }
}
  lcd.setCursor(0, 1);
  lcd.print("H: ");
  lcd.print(val2);
  lcd.setCursor(8, 1);
  lcd.print(" M: ");
  lcd.print(val3);
}
// a method used to update all sensor  data to the server at the same time
void setMultipleSensorData(double val1, double val2, double val3, double val4, double val5, double val6)
{
  if ((WiFi.RSSI() < -20) && (WiFi.RSSI() > -150))
  {

    sendState(0);
    String place = "/users/";
    place += uid; //<- user uid of current user that sign in with Emal/Password
    place += "/Devices/Device_1/Live/Sensors/";
    place += "MultipleData";

    FirebaseJson json;

    json.set("Sensor_1", val1);
    json.set("Sensor_2", val2);
    json.set("Sensor_3", val3);
    json.set("Sensor_4", val4);
    json.set("Sensor_5", val5);
    json.set("Sensor_6", val6);
    // now we will set the timestamp value at Ts
    json.set("Ts/.sv", "timestamp"); // .sv is the required place holder for sever value which currently supports only string "timestamp" as a value

    // Set data with timestamp
    //   Serial.printf("", Firebase.RTDB.setJSON(&fbdo, place, &json) ? fbdo.to<FirebaseJson>().raw() : "");
    Serial.printf("Set data with timestamp... %s\n", Firebase.RTDB.setJSON(&fbdo, place, &json) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());

    sendState(1);
  }
  else
  {
    Serial.println("Low Signal");
  }
}
// a method used to send sensor data separatelly 
void sendSensor(double val, String location)
{
  if ((WiFi.RSSI() < -20) && (WiFi.RSSI() > -150))
  {

    sendState(0);
    String place = "/users/";
    place += uid; //<- user uid of current user that sign in with Emal/Password
    place += "/Devices/Device_1/Live/Sensors/";
    place += location;

    FirebaseJson json;

    json.set("Value", val);
    // now we will set the timestamp value at Ts
    json.set("Ts/.sv", "timestamp"); // .sv is the required place holder for sever value which currently supports only string "timestamp" as a value

    // Set data with timestamp
    Serial.printf("", Firebase.RTDB.setJSON(&fbdo, place, &json) ? fbdo.to<FirebaseJson>().raw() : "");
    //   Serial.printf("Set data with timestamp... %s\n", Firebase.RTDB.setJSON(&fbdo, place, &json) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());

    sendState(1);
  }
  else
  {
    Serial.println("Low Signal");
  }
}
// a method required to read a boolean command from the server
bool readButtonState(String location)
{
  if ((WiFi.RSSI() < -20) && (WiFi.RSSI() > -150))
  {
    String path = "/users/";
    path += uid; //<- user uid of current user that sign in with Emal/Password
    path += "/Devices/Device_1/Live/Buttons/";
    path += location;
    bool bVal;
    Serial.printf("", Firebase.RTDB.getBool(&fbdo, path, &bVal) ? bVal ? "true" : "false" : "");
    // Serial.printf("Get bool ref... %s\n", Firebase.RTDB.getBool(&fbdo, path, &bVal) ? bVal ? "true" : "false" : fbdo.errorReason().c_str());

    return bVal;
  }
  else
  {

    Serial.println("Low Signal");
  }
}
bool setButtonState(String location, boolean condition)
{
  if ((WiFi.RSSI() < -20) && (WiFi.RSSI() > -150))
  {
    String path = "/users/";
    path += uid; //<- user uid of current user that sign in with Emal/Password
    path += "/Devices/Device_1/Live/Buttons/";
    path += location;
    bool bVal;
    
    Serial.printf("Set Boolean", Firebase.RTDB.setBool(&fbdo, path, condition) ?"Ok" : fbdo.errorReason().c_str());
    // Serial.printf("Get bool ref... %s\n", Firebase.RTDB.getBool(&fbdo, path, &bVal) ? bVal ? "true" : "false" : fbdo.errorReason().c_str());

    return bVal;
  }
  else
  {

    Serial.println("Low Signal");
  }
}
// a method used to push or append Multiple sensor data with the current timestamp
void pushMultipleSensorData(double val1, double val2, double val3, double val4, double val5, double val6)
{
  if ((WiFi.RSSI() < -20) && (WiFi.RSSI() > -150))
  {

    sendState(0);
    String path = "/users/";
    path += uid; //<- user uid of current user that sign in with Emal/Password
    path += "/Devices/Device_1/Storage/MultipleData";

    FirebaseJson json;

    json.set("Sensor_1", val1);
    json.set("Sensor_2", val2);
    json.set("Sensor_3", val3);
    json.set("Sensor_4", val4);
    json.set("Sensor_5", val5);
    json.set("Sensor_6", val6);
    // now we will set the timestamp value at Ts
    json.set("Ts/.sv", "timestamp"); // .sv is the required place holder for sever value which currently supports only string "timestamp" as a value

    // Set data with timestamp
    Serial.printf("Set data with timestamp... %s\n", Firebase.RTDB.pushJSON(&fbdo, path, &json) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());
    // Serial.printf("", Firebase.RTDB.pushJSON(&fbdo, path, &json) ? fbdo.to<FirebaseJson>().raw() : "");

    sendState(1);
  }
  else
  {
    Serial.println("Low Signal");
  }
}
// a method used to push or append a single sensor data
void sendDataWithTime(double val, String location)
{
  if ((WiFi.RSSI() < -20) && (WiFi.RSSI() > -150))
  {

    sendState(0);
    String path = "/users/";
    path += uid; //<- user uid of current user that sign in with Emal/Password
    path += "/Devices/Device_1/Storage/";
    path += location;
    FirebaseJson json;

    json.set("Value", val);
    // now we will set the timestamp value at Ts
    json.set("Ts/.sv", "timestamp"); // .sv is the required place holder for sever value which currently supports only string "timestamp" as a value

    // Set data with timestamp
    //  Serial.printf("Set data with timestamp... %s\n", Firebase.RTDB.pushJSON(&fbdo, path, &json) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());
    Serial.printf("", Firebase.RTDB.pushJSON(&fbdo, path, &json) ? fbdo.to<FirebaseJson>().raw() : "");

    sendState(1);
  }
  else
  {
    Serial.println("Low Signal");
  }
}
void eraseWifi()
{

  WiFi.disconnect();
  wm.resetSettings();
}
// the loop function that would run forever
void loop()
{

  wm.process();  //used to check the wifi connection status related to wifi manager
 
  // Cange Wifi if pin 17 is high
  int WifiErase = digitalRead(17);  // wifi erase pin
  if (WifiErase == HIGH)
  {
    eraseWifi();
  }
  
   
  // set Control mode if pin 19 is high
  if (digitalRead(19) == HIGH)
  {
    // setControlMode();
    manualMode = true;
     Serial.println("Manual Control");
       // set Control mode if pin 19 is high
  if (digitalRead(16) == HIGH)
  {
    // setControlMode();
    localMode = true;
            Serial.println("Manual Local Control");

  }
  else
  {
        localMode = false;

            Serial.println("Manual Remote Control");
  }
  }
  else
  {
    manualMode = false;

        Serial.println("Automatic Control");

  }

 if (digitalRead(16) == HIGH)
  {
    // setControlMode();
    localMode = true;

  }
  else
  {
        localMode = false;

  }

  if ((WiFi.status() == WL_CONNECTED))
  {
    prevRestart = millis();
  }
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (millis() - prevRestart >= restartInterval))
  {
    prevRestart = millis();
    Serial.print(millis());
    Serial.println("Restarting...");
    ESP.restart();
  }

// Reading the sensor Data
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  moisture = analogRead(A0);
  moisture = (moisture / 4095) * 100;


  long random1 = random(100);
  long random2 = random(100);
  long random3 = random(100);
  if (millis() - previousTimeConn > conInterval)
  {
    printConection();
    previousTimeConn = millis();
  }

  if (millis() - previousSensorTime > sensorInterval)
  {
    printSensor(temperature, humidity, moisture, 0.0, 0.0, 0.0);
    previousSensorTime = millis();
  }

  if (WiFi.status() != WL_CONNECTED)
  {

    // move cursor to   (2, 1)
    if (WiFi.SSID() == "" && WiFi.psk() == "")
    {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Wifi Deleted");
      lcd.setCursor(0, 2);
      lcd.print("Pleas Configur Wifi");
      WiFi.mode(WIFI_STA);
      WiFi.setAutoReconnect(true);

      wm.setEnableConfigPortal(true);

      wm.setConfigPortalBlocking(true);
      wm.autoConnect("changeMe", "12345678");
      if (wm.autoConnect("changeMe", "12345678"))
      {
        Serial.println("connected...yeey :)");
        wifiName = WiFi.SSID();
      }
      else
      {
        Serial.println("Configportal running");
      }
      Firebase.begin(&config, &auth);
      wm.setEnableConfigPortal(false);
      wm.setConfigPortalBlocking(false);
    }

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

    WiFi.setAutoReconnect(true);
    wm.setEnableConfigPortal(false);

    wm.setConfigPortalBlocking(false);
    wm.autoConnect("changeMe", "12345678");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    if (!Firebase.ready())
    {
      if (!getLocalTime(&timeinfo))
      {
        for (int i = 0; i < 3; i++)
        {
          configTime(GMTOffset_sec, DayLightOffset_sec, NTPServer);
          Serial.print("Trying to get Time..");
          Serial.println(i);
          if (getLocalTime(&timeinfo))
          {
            Firebase.begin(&config, &auth);
            return;
          }
        }
      }
      else
      {
        Firebase.begin(&config, &auth);
      }

      printLocalTime();
    }
  }
  if (millis() - previousDataSendTime > sendDataTimeInterval && WiFi.status() == WL_CONNECTED && Firebase.ready())
  {

    previousDataSendTime = millis();
    wifiName = WiFi.SSID();
    sendState(0);
    sendState(1);

    setMultipleSensorData(temperature, humidity, moisture, 0.0, 0.0, 0.0);
    printConection();
    if (millis() - prevDataTime > dataInterval)
    {
      prevDataTime = millis();
      pushMultipleSensorData(temperature, humidity, moisture, 0.0, 0.0, 0.0);
    }

    //  if (millis() - dataPerQuarter > dataIntervalPerQuarter ){
    //        dataPerQuarter = millis();
    //        randNumber = random(100);
    //        sendDataWithTime(t,"Sensor_4");
    //        sendDataWithTime(h,"Sensor_5");
    //        sendDataWithTime(moisture,"Sensor_6");
    //  }
    remotBotton = readButtonState("Button_1");
    if(waterPump == true){
      setButtonState("Button_2", true);
    }else{
      setButtonState("Button_2", false);

    }
  }

  if(manualMode == false){
      if(moisture >= 70){
                   waterPump = true;

      }else{
                  waterPump = false;

      }
    //auto
  }else{
      if (localMode == false)
  {
    if (remotBotton == true)
    {
            waterPump = true;

    }
    else
    {
            waterPump = false;

    }
  }
  else
  {

    if (digitalRead(15) == HIGH)
    {
      
       waterPump = true;
    }
    else
    {
      waterPump = false;
    }
  }
  }
 if(waterPump == true){
      Serial.println("Watter PUmp 0NNNNNNNNNNN");
      digitalWrite(26, HIGH);
 }else{
   Serial.println("Water Pump Offffffff");
      digitalWrite(26, LOW);
 }
}