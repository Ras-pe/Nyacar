#include <WiFi.h>
#include <WebServer.h>

// ===== WiFi (Modo Access Point) =====
const char* ssid = "NyaTank";
const char* password = "12345678";

WebServer server(80);

// ===== Pines L298N =====
const int IN1 = 27;
const int IN2 = 26;
const int IN3 = 25;
const int IN4 = 33;


// FUNCIONES DE MOVIMIENTO


void adelante() {
  Serial.println("Movimiento: ADELANTE");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void atras() {
  Serial.println("Movimiento: ATRAS");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void izquierda() {
  Serial.println("Movimiento: IZQUIERDA");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void derecha() {
  Serial.println("Movimiento: DERECHA");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void detener() {
  Serial.println("Movimiento: DETENIDO");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// =========================
// MANEJO DE COMANDOS WEB
// =========================

void handleCmd() {

  if (server.hasArg("go")) {
    String dir = server.arg("go");

    if (dir == "forward") {
      adelante();
    }
    else if (dir == "backward") {
      atras();
    }
    else if (dir == "left") {
      izquierda();
    }
    else if (dir == "right") {
      derecha();
    }
  }

  if (server.hasArg("stop")) {
    detener();
  }

  server.send(200, "text/plain", "OK");
}


void handleSettings() {
  Serial.println("Settings recibidos");
  server.send(200, "text/plain", "Saved");
}



void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>NyaTank</title>
<style>
body{
  margin:0;
  background:#111;
  color:white;
  font-family:Arial;
  text-align:center;
}
h2{ margin-top:20px; }

.container{
  display:flex;
  justify-content:center;
  align-items:center;
  height:70vh;
}

.dpad{
  display:grid;
  grid-template-columns:80px 80px 80px;
  grid-template-rows:80px 80px 80px;
  gap:15px;
}

button{
  font-size:26px;
  border:none;
  border-radius:12px;
  background:#ff8c42;
  color:white;
}

button:active{ background:#e67a30; }

.empty{ visibility:hidden; }

.stop{
  grid-column:1/4;
  background:#e63946;
}

.stop:active{ background:#c92a35; }

.settings-panel{
  display:none;
  position:fixed;
  top:0;
  left:0;
  width:100%;
  height:100%;
  background:rgba(0,0,0,0.9);
}

.settings-content{
  background:#222;
  padding:20px;
  margin:50px auto;
  width:90%;
  max-width:300px;
  border-radius:15px;
}

input, select{
  width:100%;
  padding:8px;
  margin-top:8px;
  margin-bottom:15px;
  background:#333;
  color:white;
  border:none;
  border-radius:6px;
}
</style>
</head>
<body>

<h2>NyaTank</h2>

<button onclick="openSettings()">Settings</button>

<div class="container">
  <div class="dpad">
    <div class="empty"></div>
    <button onmousedown="move('forward')" 
            onmouseup="stop()" 
            ontouchstart="move('forward')" 
            ontouchend="stop()">w</button>
    <div class="empty"></div>

    <button onmousedown="move('left')" 
            onmouseup="stop()" 
            ontouchstart="move('left')" 
            ontouchend="stop()">a</button>

    <button onmousedown="move('backward')" 
            onmouseup="stop()" 
            ontouchstart="move('backward')" 
            ontouchend="stop()">s</button>

    <button onmousedown="move('right')" 
            onmouseup="stop()" 
            ontouchstart="move('right')" 
            ontouchend="stop()">d</button>

    <button class="stop" onclick="stop()">STOP</button>
  </div>
</div>

<div id="settingsPanel" class="settings-panel">
  <div class="settings-content">
    <h3>Settings</h3>

    <label>Motor Speed</label>
    <select id="motorSpeed">
      <option value="slow">Slow</option>
      <option value="medium" selected>Medium</option>
      <option value="fast">Fast</option>
    </select>

    <label>Frame Delay (ms)</label>
    <input type="number" id="frameDelay" value="100">

    <button onclick="saveSettings()">Save</button>
    <button onclick="closeSettings()">Close</button>
  </div>
</div>

<script>
function move(dir){
  fetch('/cmd?go=' + dir);
}

function stop(){
  fetch('/cmd?stop=1');
}

function openSettings(){
  document.getElementById("settingsPanel").style.display="block";
}

function closeSettings(){
  document.getElementById("settingsPanel").style.display="none";
}

function saveSettings(){
  let speed = document.getElementById("motorSpeed").value;
  let delay = document.getElementById("frameDelay").value;

  fetch(`/setSettings?motorSpeed=${speed}&frameDelay=${delay}`);
  closeSettings();
}
</script>

</body>
</html>
  )rawliteral";

  server.send(200, "text/html", html);
}



void setup() {

  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  detener();

  WiFi.softAP(ssid, password);

  Serial.println("ESP32 NyaTank listo");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);
  server.on("/setSettings", handleSettings);

  server.begin();
}


void loop() {
  server.handleClient();
}