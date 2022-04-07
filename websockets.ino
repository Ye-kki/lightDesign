#include <Adafruit_NeoPixel.h>
#define LED_COUNT 52
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <FS.h>

ESP8266WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

uint8_t LED_PIN=D2;
boolean stat =true;
uint16_t red, green, blue;
uint16_t redValue[2];
uint16_t greenValue[2];
uint16_t blueValue[2];
float sensorValue;
float filteredValue;
float sensitivity = 0.1;
bool LED_on = true;
bool sensorCount = false;
int serverTime = 0;
bool option[4] = {0,};
bool sensorOn = true;
bool colorOption = false;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  WiFi.begin("Yekki6264","123123123");
  SPIFFS.begin();
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/" , handleRoot);
  server.onNotFound(handleWebRequests); 
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  Serial.println("HTTP server started");
  serverTime = millis();
  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  sensorValue = analogRead(A0);
  filteredValue = sensorValue;
}

void loop() {
  if(millis() - serverTime <= 500){
    option[0]=true;
    rainbow(5);
    if(millis() - serverTime >= 500){
        option[0]=false;
        strip.setBrightness(0);
        strip.show();
    }
  }
  sensorValue = analogRead(A0);
 
  filteredValue = filteredValue * (1-sensitivity) + sensorValue*sensitivity;
  delay(50);
  
  webSocket.loop();
  if(sensorOn == true){
    if(filteredValue > 650){ //센싱되면
      sensorCount = true;
    }
  }
  if(LED_on == true && sensorCount == true && filteredValue < 650){
     sensorCount = false;
     LED_on = false;
     strip.begin();
     strip.setBrightness(0);
     strip.show();
     delay(2000);
  }
  else if(LED_on == false && sensorCount == true && filteredValue < 650){
    if(colorOption == false){
      strip.begin();
      strip.setBrightness(255);
      for(int i=0; i<LED_COUNT; i++){
         strip.setPixelColor(i, red, green, blue);
      }
      strip.show();
    }
    sensorCount = false;
    LED_on = true;
    delay(2000);
 }
 
  if(LED_on == true && option[0] == true){ //rainbow
    rainbow(10);
  }
  else if(LED_on == true && option[1] == true){ //colorwipe
    colorWipe(strip.Color(255,   0,   0), 30); // Red
    colorWipe(strip.Color(  0, 255,   0), 30); // Green
    colorWipe(strip.Color(  0,   0, 255), 30); // Blue
  }
  else if(LED_on == true && option[2] == true){ //theaterchase
    theaterChase(strip.Color(255, 255, 255), 30); // White, half brightness
    theaterChase(strip.Color(255,   0,   0), 30); // Red, half brightness
    theaterChase(strip.Color(0,   255,   0), 30);
    theaterChase(strip.Color(  0,   0, 255), 30); // Blue, half brightness
  }
  else if(LED_on == true && option[3] == true){
    colorWipe(strip.Color(255,   0,   0), 20); // Red
    colorWipe(strip.Color(  0, 255,   0), 20); // Green
    colorWipe(strip.Color(  0,   0, 255), 20); // Blue
    theaterChase(strip.Color(255, 255, 255), 20); // White, half brightness
    theaterChase(strip.Color(255,   0,   0), 20); // Red, half brightness
    theaterChase(strip.Color(0,   255,   0), 20);
    theaterChase(strip.Color(  0,   0, 255), 20); // Blue, half brightness
    rainbow(10);
  }
  server.handleClient();
  if(Serial.available()>0){
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
}

void handleRoot(){
  server.sendHeader("Location", "/index.html" , true);
  server.send(302,"text/html", "555");
}

void handleWebRequests() {
  if (loadFromSpiffs(server.uri()))return; // SPIFFS에 요청한 파일 확인
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

bool loadFromSpiffs(String path) { // SPIFFS 에서 파일 확인
  String dataType = "text/plain";

  // 요청한 파일의 확장자에 따라 데이터 타입 정의
  if (SPIFFS.exists(path)) {
    // 주소의 확장자에따라 데이터 타입 지정
    if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
    else if (path.endsWith(".html")) dataType = "text/html";
    else if (path.endsWith(".css")) dataType = "text/css";
    else if (path.endsWith(".js")) dataType = "application/javascript";
    File dataFile = SPIFFS.open(path.c_str(), "r"); // SPIFFS 에서 파일 읽기
    if (server.hasArg("download")) dataType = "application/octet-stream";
    if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    }
    dataFile.close();
    return true;
  }
  else return false;
}

void rainbow(int wait) {
  strip.begin();
  strip.setBrightness(255);
  for(long firstPixelHue = 0; firstPixelHue < 1*65536; firstPixelHue += 256) {
    if(option[0] == true || option[3] == true){
      for(int i=0; i<strip.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
    strip.show();
    delay(wait);
    }
    else break;
  }
}
void colorWipe(uint32_t color, int wait) {
  strip.begin();
  strip.setBrightness(255);
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
     if(option[1] == true || option[3] == true){
       strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
       strip.show();                          //  Update strip to match
       delay(wait);                           //  Pause for a moment
     }
     else break;
  }
}
void theaterChase(uint32_t color, int wait) {
  strip.begin();
  strip.setBrightness(255);
  for(int a=0; a<10; a++) {  // Repeat 10 times...
     if(option[2] == true || option[3] == true){
       for(int b=0; b<3; b++) {
         strip.clear();
         for(int c=b; c<strip.numPixels(); c += 3) {
         strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
       }
       strip.show(); // Update strip with new contents
       delay(wait);  // Pause for a moment
      }
     }
     else break;
  }
}
