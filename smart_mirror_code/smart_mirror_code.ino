#include <ArduinoJson.h>

#include <doxygen.h>
#include <ESP8266.h>

#include <memorysaver.h>
#include <UTFT.h>

#define DEBUG true

#define SSID        "SSID"
#define PASSWORD    "PASSWORD"
#define HOST_NAME   "api.openweathermap.org"
#define HOST_PORT   (80)

UTFT myGLCD(ITDB43,38,39,40,41);

// 폰트 목록
extern uint8_t SmallFont[];
extern uint8_t DotMatrix_M_Slash[];
extern uint8_t SixteenSegment16x24[];
extern uint8_t SixteenSegment40x60[];
extern uint8_t SevenSeg_XXXL_Num[];
extern uint8_t SevenSegment96x144Num[];
extern uint8_t UbuntuBold[];

ESP8266 wifi(Serial3);

// JSON 파싱을 통한 날씨 정보를 저장할 변수들
const char* temp;
const char* temp_min;
const char* temp_max;
const char* humidity;
const char* weather;
const char* windSpeed;
double windDirection;
const char* rain;

// millis를 이용하여 delay주기
unsigned long buttonMillis;
unsigned long currentMillis;
unsigned long interval = 3600000;

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(0));
  Serial.begin(9600);
  Serial3.begin(9600);

  // Setup Wi-Fi
  if (wifi.setOprToStationSoftAP()) {
      Serial.print("to station + softap ok\r\n");
  } else {
      Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
      Serial.print("Join AP success\r\n");

      Serial.print("IP:");
      Serial.println( wifi.getLocalIP().c_str());       
  } else {
      Serial.print("Join AP failure\r\n");
  }
  
  if (wifi.disableMUX()) {
      Serial.print("single ok\r\n");
  } else {
      Serial.print("single err\r\n");
  }
  
  Serial.print("setup end\r\n");
  
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();

  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMillis = millis();


  
    
    uint8_t buffer[1024] = {0};

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }
  
    char *hello = "GET /data/2.5/weather?q=Seoul&appid=[APPID]&units=metric HTTP/1.1\r\nHost: api.openweathermap.org\r\nConnection: close\r\n\r\n";
    wifi.send((const uint8_t*)hello, strlen(hello));
  
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  
    char *token;
    char *jsonStr[2];
  
    token = strtok_r(buffer, "{", &jsonStr[0]);
  
    String jsonString = String((char *)jsonStr[0]);
    jsonString = "{" + jsonString;
    
    Serial.println(jsonString);

    parseJSONString(jsonString);

  if (wifi.releaseTCP()) {
      Serial.print("release tcp ok\r\n");
  } else {
      Serial.print("release tcp err\r\n");
  }

  while(1) {
    buttonMillis = millis();

//    Serial.print("currentMillis = ");
//  Serial.print(currentMillis);
//  Serial.print(" : buttonMillis = ");
//  Serial.print(buttonMillis);
//  Serial.print(" : buttonMillis - currentMillis = ");
//  Serial.print(buttonMillis - currentMillis);
//  Serial.print(" : interval = ");
//  Serial.println(interval);
  
    if((buttonMillis - currentMillis) > interval) {
//      Serial.print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> break : ");
//      Serial.println(buttonMillis - currentMillis);
       break;
    }
  }
}

void parseJSONString (String jsonString) {
  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonString);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    while(1);
  }

  temp = root["main"]["temp"];
  temp_min = root["main"]["temp_min"];
  temp_max = root["main"]["temp_max"];
  humidity = root["main"]["humidity"];
  weather = root["weather"][0]["description"];
  windSpeed = root["wind"]["speed"];
  windDirection = root["wind"]["deg"];
  rain = root["rain"]["3h"];

  if (rain == NULL) {
    Serial.println("No Rain");
  } else {
    Serial.println("It's Rainy");
  }

  Serial.print("temp = ");
  Serial.println(temp);
  Serial.print("temp_max = ");
  Serial.println(temp_max);
  Serial.print("temp_min = ");
  Serial.println(temp_min);
  Serial.print("humidity = ");
  Serial.println(humidity);
  Serial.print("weather = ");
  Serial.println(weather);
  Serial.print("windSpeed = ");
  Serial.println(windSpeed);
  Serial.print("windDirection = ");
  Serial.println(windDirection);
  
  writeTemp(temp);
  writeTempHL(temp_max, temp_min);
  writeWeatherETC(humidity, weather, windSpeed, windDirection);
}

void writeTemp (char* temp) {
  String tempStr = String(temp);
 
  myGLCD.setFont(SixteenSegment40x60);
  myGLCD.print(tempStr, 30, 60);
  myGLCD.setFont(UbuntuBold);
  myGLCD.print("`C", 30 + tempStr.length() * 40, 60);
}

void writeTempHL (char* tempH, char* tempL) {
  String tempHStr = "H : " + String(tempH);
  String tempLStr = "L : " + String(tempL);

  int hlen = tempHStr.length();
  int llen = tempLStr.length();
  int xpoint = 0;

  if (hlen > llen) {
    xpoint = 480 - (hlen * 24) - 10;
  } else {
    xpoint = 480 - (llen * 24) - 10;
  }
  
  myGLCD.setFont(SixteenSegment16x24);
  myGLCD.print(tempHStr, xpoint, 60);
  myGLCD.print(tempLStr, xpoint, 96);
}

void writeWeatherETC(char* humidity, char* weather, char* windSpeed, double windDirection) {
  String humidityStr = String(humidity) + "%";
  String weatherStr = String(weather);
  String windSpeedStr = String(windSpeed) + " M/S";
  String windDirectionStr = "";

  weatherStr.toUpperCase();
  
  if (windDirection >= 22.5 && windDirection < 67.5) {
    windDirectionStr = "NE";
  } else if (windDirection >= 67.5 && windDirection < 112.5) {
    windDirectionStr = "E";
  } else if (windDirection >= 112.5 && windDirection < 157.5) {
    windDirectionStr = "SE";
  } else if (windDirection >= 157.5 && windDirection < 202.5) {
    windDirectionStr = "S";
  } else if (windDirection >= 202.5 && windDirection < 247.5) {
    windDirectionStr = "SW";
  } else if (windDirection >= 247.5 && windDirection < 292.5) {
    windDirectionStr = "W";
  } else if (windDirection >= 292.5 && windDirection < 337.5) {
    windDirectionStr = "NW";
  } else if (windDirection >= 337.5 || windDirection < 22.5) {
    windDirectionStr = "N";
  }

  String windStr = windSpeedStr + " " + windDirectionStr;

  myGLCD.setFont(SixteenSegment16x24);
  myGLCD.print(humidityStr, 30, 15);
  myGLCD.print(weatherStr, 190, 15);
  myGLCD.print(windStr, 480 - (windStr.length() * 16) - 30, 15);
  myGLCD.setFont(SmallFont);
  myGLCD.print("-----================================================-----", CENTER, 44);
}
