#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

//핀번호 정의
int PWMA = D1;
int PWMB = D2;
int DA = D3;
int DB = D4;
int SW1 = 10; //SD3 pin
int SW2 = D8; //8 pin
int SW3 = D7; //7 pin
int SW4 = D6; //6 pin
int SW5 = D5; //5 pin
int motorSpeed = 400;
const char* ssid     = "seongmin";
const char* password = "qwer1234";
String safedata = "고리가 연결되었습니다.";
AsyncWebServer server(80);

AsyncEventSource events("/events");

unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;
void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi seongmin..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}
String processor(const String& var){
  //Serial.println(var);
  if(var == "SAFEDATA"){
    return String(safedata);
  }
}
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="utf-8">
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>안전고리 체결감지 센서</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p> 1번 고리</p><p><span class="reading"><span id="safe">%SAFEDATA%</span></span></p>
      </div>
      <div class="card">
        <p> n번 고리</p><p><span class="reading"><span id="hum">연결안됨</span></span></p>
      </div>
      <div class="card">
        <p> n번 고리</p><p><span class="reading"><span id="pres">연결안됨</span></span></p>
      </div>
      <div class="card">
        <p> n번 고리</p><p><span class="reading"><span id="temp">연결안됨</span></span></p>
      </div>
      <div class="card">
        <p> n번 고리</p><p><span class="reading"><span id="hum">연결안됨</span></span></p>
      </div>
      <div class="card">
        <p> n번 고리</p><p><span class="reading"><span id="pres">연결안됨</span></span></p>
      </div>
      <div class="card">
        <p> n번 고리</p><p><span class="reading"><span id="temp">연결안됨</span></span></p>
      </div>
      <div class="card">
        <p> n번 고리</p><p><span class="reading"><span id="hum">연결안됨</span></span></p>
      </div>
      <div class="card">
        <p> n번 고리</p><p><span class="reading"><span id="pres">연결안됨</span></span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);

 source.addEventListener('safedata', function(e) {
  console.log("safedata", e.data);
  document.getElementById("safe").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";

void setup() {
  Serial.begin(115200);
  initWiFi();
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(PWMA, OUTPUT); 
  pinMode(PWMB, OUTPUT); 
  pinMode(DA, OUTPUT); 
  pinMode(DB, OUTPUT); 
  pinMode(SW1, INPUT);
  //motor 초기화
  digitalWrite(PWMA, 0);
  digitalWrite(PWMB, 0);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
    events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}


// 보드LED = D0, ESP LED = LED_BUILTIN
// SWITCH ON = 1, OFF = 0
// motor 멈춤 = pwm 모터스피드0일시 멈춤
// motor 작동 = DIRA,B high = 순방향, low = 역방향

void loop() { 
  if ((millis() - lastTime) > timerDelay) {
    if(digitalRead(SW1) == 1){
      digitalWrite(D0, LOW);
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(PWMA, motorSpeed);
      digitalWrite(DA, LOW);
    }
    else if(digitalRead(SW2) == 1){
      digitalWrite(D0, LOW);
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(PWMA, motorSpeed);
      digitalWrite(DA, HIGH);
    }
    else{
    digitalWrite(D0, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(PWMA, 0);
    events.send(String(safedata).c_str(),"safedata",millis());
    }
  lastTime = millis();
  }
}
