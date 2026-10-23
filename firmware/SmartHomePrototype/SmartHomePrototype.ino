#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Firebase_ESP_Client.h>
#include <ESP8266HTTPClient.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <Servo.h>
#include <DHT11.h>
#include <ESP8266WiFi.h>
#include "config.h" // Local configuration; see config.example.h.


//------------------------------------Firebase-----------------------------//

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

//------------------------------sensors------------------------------------//

DHT11 dht11(2); // For ESP8266: Connect the sensor to GPIO2 or D4.
Servo myservo;  // create servo object to control a servo
LiquidCrystal_I2C lcd(0x27, 16, 2);

//-----------------------------output-------------------------------------//

int ledPin = 13; // the LED pin GPIO4 D7 
int FanPin = 12; // the LED pin GPIO5 D6
const int sensorPin = 16; // Digital pin GPIO16 D0  
int servoPin = 0; // GPIO0 D3 

//------------------------------------------------------------------------//
unsigned long pt = millis();
//-----------------------------------------------------------------------------------------------------//
void setup() {

  pinMode(sensorPin, INPUT); // declare sensor as input
  pinMode(ledPin, OUTPUT);
  pinMode(FanPin, OUTPUT);
  Serial.begin(9600);

//----------------------------------------------------------------------------------------------------//

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
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
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

//------------------------------------------------------------------------------------------------------//
  // Uncomment the line below to set a custom delay between sensor readings (in milliseconds).
  dht11.setDelay(100); // Set this to the desired delay. Default is 500ms.

  lcd.init();
  lcd.backlight();
  
	myservo.attach(servoPin); // attaches the servo on pin 18 to the servo object
//-------------------------------------------------------------------------------------------------------//
}

void loop() {

  int temperature = 0;
  int humidity = 0;
  int result = dht11.readTemperatureHumidity(temperature, humidity);
  char yes[] = "Someone was detected";
  char no[] = "Nobody is around";
//----------------------------------sending data to the firebase-------------------------------------------//
if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write the temperature on the database path sensor/temperature
    if (Firebase.RTDB.setInt(&fbdo, "sensor/temperature", temperature)){
      Serial.println(); 
      Serial.println(temperature);
      Serial.println(" (" + fbdo.dataType() +") ");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setInt(&fbdo, "sensor/humidity", humidity)){
      Serial.println(); 
      Serial.println(humidity);
      Serial.println(" (" + fbdo.dataType() +") ");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }}
//----------------------------------stsrt controling the system---------------------------------------------//
  // Check for motion first
  int motion = digitalRead(sensorPin);
  unsigned long ct = millis();

  if (motion == LOW) {

    myservo.write(0);

  if (ct - pt > 1500){
    lcd.setCursor(0, 0);
    lcd.print("Motion detected!");
    myservo.write(165);
    pt = ct;
  }

    Serial.println("Motion Detected!");
    
    //lcd.setCursor(0, 0);
   // lcd.print("Motion detected!");

    if (result == 0) {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" °C\tHumidity: ");
      Serial.print(humidity);
      Serial.println(" %");
      Serial.println(DHT11::getErrorString(result));
    }

    // Turn on LED and Fan only if temperature is high
    if (temperature > 25) {
      digitalWrite(ledPin, HIGH);
      digitalWrite(FanPin, HIGH);
      Serial.println("Fan and LED turned ON");
     
      // Wait for temperature to cool down
      while (temperature > 24) {
        delay(1000); // Check temperature more frequently
        result = dht11.readTemperatureHumidity(temperature, humidity);
        if (result != 0) {
          Serial.println(DHT11::getErrorString(result));
          break; // Exit loop if error occurs during reading
        }
      }

      digitalWrite(ledPin, LOW);
      digitalWrite(FanPin, LOW);
      Serial.println("Fan and LED turned OFF (Temperature reached below 24°C)");
      delay(15000);
    }
  } 
   

  if(motion == HIGH) {
   
    Serial.println("No Motion Detected");
     lcd.setCursor(0, 0);
    lcd.print("No Motion Detected");
    
  }

  lcd.setCursor(0, 1);
  lcd.print("H=");
  lcd.print(humidity);
  lcd.setCursor(8, 1);
  lcd.print("T=");
  lcd.print(temperature);

  delay(500); 
}
