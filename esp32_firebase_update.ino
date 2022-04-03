#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <M5Core2.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "ASUS_X00TD"
#define WIFI_PASSWORD "biscuitppp"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDCE8PFIpoTwsz8WZ4ZJEoo6IOSjXDUV68"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-firebase-e009f-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
int floatValue;
String stringvalue;
bool signupOK = false;

int motor1Pin1 = 27; 
int motor1Pin2 = 26; 

int motor2Pin1 = 33; 
int motor2Pin2 = 25; 

int lightPin = 15;

#include <ESP32Servo.h>

#define SERVO_PIN 12

Servo myServo;

void setup() {
  Serial.begin(115200);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  pinMode(lightPin, OUTPUT);

  myServo.attach(SERVO_PIN);

  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW); 
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW); 

  digitalWrite(lightPin, LOW); 

//  M5.begin();
//  M5.Lcd.begin(); 
//
//  M5.Lcd.fillScreen(YELLOW);
//
//  M5.Lcd.setTextColor(BLACK); 
//  M5.Lcd.setTextSize(5); 
//
//  M5.Lcd.setCursor(50, 40); 
//  M5.Lcd.print("ESP32");
//
//  M5.Lcd.setCursor(140, 95); 
//  M5.Lcd.print(" +");
//
//  M5.Lcd.setCursor(30, 150); 
//  M5.Lcd.setTextColor(BLACK); 
//  M5.Lcd.setTextSize(5);  
//  M5.Lcd.print("FIREBASE");

  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

//  Serial.println(stringvalue.datatype());
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    if (Firebase.RTDB.getInt(&fbdo, "/test/light")) {
      if (fbdo.dataType() == "int") {
        intValue = fbdo.intData();

        if(intValue == 1){
          Serial.println("LIGHT ON");
          digitalWrite(lightPin, HIGH);
        }
        else if(intValue == 0)
        {
          Serial.println("LIGHT OFF");
          digitalWrite(lightPin, LOW);
        }
        Serial.println("LIGHT : " + String(intValue));
//        Serial.println(intValue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    
    if (Firebase.RTDB.getInt(&fbdo, "/test/arm")) {
      if (fbdo.dataType() ) {
        floatValue = fbdo.intData();

        if(floatValue == 0){
          myServo.write(0);
        }
        else if(floatValue == 1){
          myServo.write(45);
        }
        else if(floatValue == 2){
          myServo.write(90);
        }
        
        Serial.println("ARM : " + String(floatValue));
//        Serial.println(floatValue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getString(&fbdo, "/test/command")) {
      if (fbdo.dataType() ) {
        stringvalue = fbdo.stringData();
        Serial.println("COMMAND : " + stringvalue);
        
        if(stringvalue == "FR")
        {
          Serial.println("GO FRONT");
          digitalWrite(motor1Pin1, HIGH);
          digitalWrite(motor1Pin2, LOW); 
          digitalWrite(motor2Pin1, HIGH);
          digitalWrite(motor2Pin2, LOW); 
        }
        else if (stringvalue == "BK")
        {
          Serial.println("GO BACK");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, HIGH); 
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, HIGH); 
        }
        else if (stringvalue == "LR")
        {
          Serial.println("LEFT ROTATE");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, HIGH); 
          digitalWrite(motor2Pin1, HIGH);
          digitalWrite(motor2Pin2, LOW); 
        }
        else if (stringvalue == "RR")
        {
          Serial.println("RIGHT ROTATE");
          digitalWrite(motor1Pin1, HIGH);
          digitalWrite(motor1Pin2, LOW); 
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, HIGH); 
        }
        else if (stringvalue == "LT")
        {
          Serial.println("LEFT TURN");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, LOW); 
          digitalWrite(motor2Pin1, HIGH);
          digitalWrite(motor2Pin2, LOW); 
        }
        else if (stringvalue == "RT")
        {
          Serial.println("RIGHT TURN");
          digitalWrite(motor1Pin1, HIGH);
          digitalWrite(motor1Pin2, LOW); 
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, LOW); 
        }
        else if (stringvalue == "ST")
        {
          Serial.println("GO BACK");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, LOW); 
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, LOW); 
        }
        else
        {
          Serial.println("STOP");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, LOW); 
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, LOW); 
        }
//        Serial.println(stringvalue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
  }
}
