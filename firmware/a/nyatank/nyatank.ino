#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// ===== WiFi (Modo Access Point) =====
const char* ssid = "NyaTank";
const char* password = "12345678";

// ===== Pines L298N =====
const int IN1 = 27;
const int IN2 = 26;
const int IN3 = 25;
const int IN4 = 33;

// ===== Timeout de seguridad =====
const unsigned long SAFETY_TIMEOUT = 60000; // 60 segundos sin comandos = detener
unsigned long lastCmdTime = 0;
String currentDirection = "stop";

// ===== Servidor y WebSocket =====
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ===== FUNCIONES DE MOVIMIENTO =====

void adelante() {
  currentDirection = "forward";
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void atras() {
  currentDirection = "backward";
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void izquierda() {
  currentDirection = "left";
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void derecha() {
  currentDirection = "right";
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void detener() {
  currentDirection = "stop";
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ===== MANEJO DE WEBSOCKET =====

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String msg = String((char*)data);
    lastCmdTime = millis();

    if (msg == "forward") {
      adelante();
    } else if (msg == "backward") {
      atras();
    } else if (msg == "left") {
      izquierda();
    } else if (msg == "right") {
      derecha();
    } else if (msg == "stop") {
      detener();
    }

    ws.textAll(currentDirection);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket #%u conectado desde %s\n", client->id(), client->remoteIP().toString().c_str());
      client->text(currentDirection);
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket #%u desconectado\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// ===== PAGINA WEB =====

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>NyaTank</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent}
body{
  background:#0a0a0f;
  background-image:
    radial-gradient(ellipse at 20% 50%, rgba(255,107,53,.06) 0%, transparent 50%),
    radial-gradient(ellipse at 80% 20%, rgba(0,200,255,.04) 0%, transparent 50%);
  color:#e8e8e8;
  font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;
  text-align:center;
  min-height:100vh;
  overflow-x:hidden;
  user-select:none;
}

/* ---- HEADER ---- */
.header{
  padding:18px 0 8px;
}
.header h1{
  font-size:1.6em;
  font-weight:800;
  letter-spacing:2px;
  background:linear-gradient(135deg,#ff6b35,#ffaa00);
  -webkit-background-clip:text;
  -webkit-text-fill-color:transparent;
  background-clip:text;
}
.header .sub{
  font-size:.7em;
  color:#555;
  letter-spacing:4px;
  text-transform:uppercase;
  margin-top:2px;
}

/* ---- STATUS BAR ---- */
.status-bar{
  display:inline-flex;
  align-items:center;
  gap:8px;
  padding:6px 16px;
  margin:6px 0 12px;
  background:rgba(255,255,255,.04);
  border:1px solid rgba(255,255,255,.06);
  border-radius:50px;
  font-size:.78em;
  color:#777;
}
.status-dot{
  width:8px;height:8px;
  border-radius:50%;
  background:#e63946;
  box-shadow:0 0 6px #e63946;
  transition:all .3s;
}
.status-dot.connected{
  background:#2ecc71;
  box-shadow:0 0 8px #2ecc71;
}
.last-cmd{
  background:rgba(255,107,53,.12);
  padding:2px 10px;
  border-radius:6px;
  font-family:'SF Mono',Consolas,monospace;
  font-size:.92em;
  color:#ff8c42;
  border:1px solid rgba(255,107,53,.2);
}

/* ---- D-PAD ---- */
.dpad-wrap{
  display:flex;
  justify-content:center;
  padding:8px 0 4px;
}
.dpad{
  display:grid;
  grid-template-columns:90px 90px 90px;
  grid-template-rows:90px 90px 90px;
  gap:8px;
}
.dpad-btn{
  display:flex;
  align-items:center;
  justify-content:center;
  font-size:22px;
  font-weight:700;
  border:none;
  border-radius:18px;
  background:linear-gradient(145deg,#2a2a35,#1e1e28);
  color:#ccc;
  cursor:pointer;
  box-shadow:
    4px 4px 10px rgba(0,0,0,.5),
    -2px -2px 8px rgba(255,255,255,.03);
  transition:all .1s;
  position:relative;
  overflow:hidden;
}
.dpad-btn::after{
  content:'';
  position:absolute;
  inset:0;
  border-radius:18px;
  border:1px solid rgba(255,255,255,.06);
  pointer-events:none;
}
.dpad-btn:active,
.dpad-btn.active{
  background:linear-gradient(145deg,#ff6b35,#e65a20);
  color:#fff;
  box-shadow:
    0 0 20px rgba(255,107,53,.35),
    inset 0 0 10px rgba(255,255,255,.1);
  transform:scale(.93);
}
.dpad-btn .arrow{font-size:26px}
.dpad-btn .label{
  font-size:9px;
  color:#666;
  margin-top:1px;
  letter-spacing:1px;
}
.dpad-btn:active .label,
.dpad-btn.active .label{color:rgba(255,255,255,.6)}
.dpad-empty{visibility:hidden}
.stop-btn{
  grid-column:1/4;
  display:flex;
  align-items:center;
  justify-content:center;
  font-size:13px;
  font-weight:700;
  letter-spacing:3px;
  text-transform:uppercase;
  border:none;
  border-radius:14px;
  background:linear-gradient(145deg,#c0392b,#a93226);
  color:#fff;
  cursor:pointer;
  box-shadow:
    4px 4px 10px rgba(0,0,0,.5),
    0 0 0 0 rgba(231,76,60,0);
  transition:all .15s;
}
.stop-btn:active{
  background:linear-gradient(145deg,#e74c3c,#c0392b);
  box-shadow:0 0 25px rgba(231,76,60,.4);
  transform:scale(.96);
}

/* ---- COMMANDS PANEL ---- */
.panel{
  background:rgba(255,255,255,.025);
  border:1px solid rgba(255,255,255,.05);
  border-radius:16px;
  padding:16px 18px;
  margin:12px auto;
  max-width:340px;
  text-align:left;
  backdrop-filter:blur(10px);
}
.panel-title{
  font-size:.72em;
  color:#555;
  text-transform:uppercase;
  letter-spacing:2px;
  margin-bottom:12px;
  text-align:center;
}
.cmd-section{margin-bottom:14px}
.cmd-section:last-child{margin-bottom:0}
.cmd-section-head{
  font-size:.65em;
  color:#ff8c42;
  text-transform:uppercase;
  letter-spacing:2px;
  margin-bottom:8px;
  padding-bottom:4px;
  border-bottom:1px solid rgba(255,140,66,.15);
}
.cmd-line{
  display:flex;
  align-items:center;
  justify-content:space-between;
  padding:5px 0;
}
.cmd-line span{font-size:.88em}
.kbd-group{display:flex;gap:4px;align-items:center}
.kbd{
  display:inline-flex;
  align-items:center;
  justify-content:center;
  min-width:26px;
  height:24px;
  padding:0 6px;
  background:rgba(255,255,255,.06);
  border:1px solid rgba(255,255,255,.1);
  border-bottom-width:2px;
  border-radius:5px;
  font-family:'SF Mono',Consolas,monospace;
  font-size:.78em;
  color:#bbb;
}
.kbd-plus{
  font-size:.7em;
  color:#444;
}
.cmd-desc{color:#666}
.cmd-note{
  color:#444;
  font-size:.82em;
  font-style:italic;
}

/* ---- SETTINGS TOGGLE ---- */
.settings-toggle{
  display:inline-block;
  margin:8px 0 16px;
  padding:7px 20px;
  font-size:.75em;
  letter-spacing:2px;
  text-transform:uppercase;
  color:#666;
  background:rgba(255,255,255,.03);
  border:1px solid rgba(255,255,255,.06);
  border-radius:50px;
  cursor:pointer;
  transition:all .2s;
}
.settings-toggle:active{
  background:rgba(255,255,255,.08);
  color:#aaa;
}

/* ---- SETTINGS MODAL ---- */
.overlay{
  display:none;
  position:fixed;
  inset:0;
  background:rgba(0,0,0,.85);
  backdrop-filter:blur(6px);
  z-index:100;
  justify-content:center;
  align-items:center;
}
.overlay.open{display:flex}
.modal{
  background:#16161e;
  border:1px solid rgba(255,255,255,.08);
  border-radius:20px;
  padding:24px;
  width:90%;
  max-width:320px;
}
.modal h3{
  font-size:1em;
  font-weight:700;
  margin-bottom:16px;
  background:linear-gradient(135deg,#ff6b35,#ffaa00);
  -webkit-background-clip:text;
  -webkit-text-fill-color:transparent;
  background-clip:text;
}
.modal label{
  display:block;
  text-align:left;
  font-size:.78em;
  color:#888;
  margin:10px 0 4px;
  text-transform:uppercase;
  letter-spacing:1px;
}
.modal select,
.modal input{
  width:100%;
  padding:10px 12px;
  background:rgba(255,255,255,.05);
  border:1px solid rgba(255,255,255,.08);
  border-radius:10px;
  color:#ddd;
  font-size:.9em;
  outline:none;
  transition:border-color .2s;
}
.modal select:focus,
.modal input:focus{
  border-color:rgba(255,107,53,.4);
}
.modal-btns{
  display:flex;
  gap:8px;
  margin-top:18px;
}
.modal-btn{
  flex:1;
  padding:10px;
  border:none;
  border-radius:10px;
  font-size:.85em;
  font-weight:600;
  cursor:pointer;
  transition:all .15s;
}
.btn-save{
  background:linear-gradient(135deg,#ff6b35,#e65a20);
  color:#fff;
}
.btn-save:active{transform:scale(.96)}
.btn-close{
  background:rgba(255,255,255,.06);
  color:#888;
}

/* ---- FOOTER ---- */
.footer{
  padding:12px 0 20px;
  font-size:.65em;
  color:#333;
  letter-spacing:1px;
}

@keyframes pulse{
  0%,100%{opacity:1}
  50%{opacity:.4}
}
.connecting{animation:pulse 1.5s infinite}
</style>
</head>
<body>

<div class="header">
  <h1>NYATANK</h1>
  <div class="sub">Control Remoto</div>
</div>

<div class="status-bar">
  <div id="statusDot" class="status-dot"></div>
  <span id="statusText">Desconectado</span>
  <span class="last-cmd" id="lastCmd">---</span>
</div>

<div class="dpad-wrap">
  <div class="dpad">
    <div class="dpad-empty"></div>
    <button class="dpad-btn" data-dir="forward" onmousedown="sendCmd('forward')" ontouchstart="sendCmd('forward')">
      <div><div class="arrow">&#9650;</div><div class="label">W</div></div>
    </button>
    <div class="dpad-empty"></div>

    <button class="dpad-btn" data-dir="left" onmousedown="sendCmd('left')" ontouchstart="sendCmd('left')">
      <div><div class="arrow">&#9664;</div><div class="label">A</div></div>
    </button>
    <button class="dpad-btn" data-dir="backward" onmousedown="sendCmd('backward')" ontouchstart="sendCmd('backward')">
      <div><div class="arrow">&#9660;</div><div class="label">S</div></div>
    </button>
    <button class="dpad-btn" data-dir="right" onmousedown="sendCmd('right')" ontouchstart="sendCmd('right')">
      <div><div class="arrow">&#9654;</div><div class="label">D</div></div>
    </button>

    <button class="stop-btn" onclick="sendCmd('stop')">&#9632; STOP</button>
  </div>
</div>

<div class="panel">
  <div class="panel-title">Controles Disponibles</div>
  <div class="cmd-section">
    <div class="cmd-section-head">Teclado</div>
    <div class="cmd-line">
      <span class="kbd-group"><span class="kbd">W</span><span class="kbd">&#8593;</span></span>
      <span class="cmd-desc">Adelante</span>
    </div>
    <div class="cmd-line">
      <span class="kbd-group"><span class="kbd">S</span><span class="kbd">&#8595;</span></span>
      <span class="cmd-desc">Atr&aacute;s</span>
    </div>
    <div class="cmd-line">
      <span class="kbd-group"><span class="kbd">A</span><span class="kbd">&#8592;</span></span>
      <span class="cmd-desc">Izquierda</span>
    </div>
    <div class="cmd-line">
      <span class="kbd-group"><span class="kbd">D</span><span class="kbd">&#8594;</span></span>
      <span class="cmd-desc">Derecha</span>
    </div>
    <div class="cmd-line">
      <span class="kbd-group"><span class="kbd">Space</span></span>
      <span class="cmd-desc">Detener</span>
    </div>
  </div>
  <div class="cmd-section">
    <div class="cmd-section-head">Tactil</div>
    <div class="cmd-line">
      <span>Mantener bot&oacute;n</span>
      <span class="cmd-desc">Mover</span>
    </div>
    <div class="cmd-line">
      <span>Soltar</span>
      <span class="cmd-desc">Detener</span>
    </div>
  </div>
  <div class="cmd-section">
    <div class="cmd-section-head">Seguridad</div>
    <div class="cmd-note">Sin comandos por 60s &rarr; auto-stop</div>
  </div>
</div>

<button class="settings-toggle" onclick="openSettings()">&#9881; Configuraci&oacute;n</button>

<div class="overlay" id="settingsPanel">
  <div class="modal">
    <h3>Configuraci&oacute;n</h3>
    <label>Velocidad del Motor</label>
    <select id="motorSpeed">
      <option value="slow">Lento</option>
      <option value="medium" selected>Medio</option>
      <option value="fast">R&aacute;pido</option>
    </select>
    <label>Timeout (ms)</label>
    <input type="number" id="timeoutMs" value="1000" min="200" max="5000">
    <div class="modal-btns">
      <button class="modal-btn btn-save" onclick="saveSettings()">Guardar</button>
      <button class="modal-btn btn-close" onclick="closeSettings()">Cerrar</button>
    </div>
  </div>
</div>

<div class="footer">NYATANK &middot; ESP32</div>

<script>
var gateway = 'ws://' + location.hostname + '/ws';
var websocket;
var pressedKeys = new Set();

function initWebSocket() {
  websocket = new WebSocket(gateway);
  websocket.onopen = function() {
    document.getElementById('statusDot').classList.add('connected');
    document.getElementById('statusText').textContent = 'Conectado';
    document.getElementById('statusText').classList.remove('connecting');
  };
  websocket.onclose = function() {
    document.getElementById('statusDot').classList.remove('connected');
    document.getElementById('statusText').textContent = 'Reconectando...';
    document.getElementById('statusText').classList.add('connecting');
    setTimeout(initWebSocket, 2000);
  };
  websocket.onmessage = function(e) {
    document.getElementById('lastCmd').textContent = e.data;
  };
}

function sendCmd(dir) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(dir);
  }
  document.getElementById('lastCmd').textContent = dir;
}

document.querySelectorAll('.dpad-btn[data-dir]').forEach(function(btn) {
  var dir = btn.getAttribute('data-dir');
  btn.addEventListener('mouseup', function() { sendCmd('stop'); });
  btn.addEventListener('touchend', function() { sendCmd('stop'); });
  btn.addEventListener('mouseleave', function() { sendCmd('stop'); });
  btn.addEventListener('touchcancel', function() { sendCmd('stop'); });
});

var keyMap = {
  'w':'forward','ArrowUp':'forward',
  's':'backward','ArrowDown':'backward',
  'a':'left','ArrowLeft':'left',
  'd':'right','ArrowRight':'right',
  ' ':'stop'
};

document.addEventListener('keydown', function(e) {
  var dir = keyMap[e.key];
  if (dir && !pressedKeys.has(e.key)) {
    pressedKeys.add(e.key);
    sendCmd(dir);
    e.preventDefault();
    highlightBtn(dir);
  }
});

document.addEventListener('keyup', function(e) {
  if (keyMap[e.key]) {
    pressedKeys.delete(e.key);
    if (pressedKeys.size === 0) {
      sendCmd('stop');
    } else {
      var lastKey = Array.from(pressedKeys).pop();
      sendCmd(keyMap[lastKey]);
    }
    e.preventDefault();
    clearHighlight();
  }
});

function highlightBtn(dir) {
  clearHighlight();
  var btn = document.querySelector('.dpad-btn[data-dir="' + dir + '"]');
  if (btn) btn.classList.add('active');
}

function clearHighlight() {
  document.querySelectorAll('.dpad-btn').forEach(function(b) {
    b.classList.remove('active');
  });
}

function openSettings() {
  document.getElementById('settingsPanel').classList.add('open');
}
function closeSettings() {
  document.getElementById('settingsPanel').classList.remove('open');
}
function saveSettings() {
  closeSettings();
}

window.addEventListener('load', initWebSocket);
</script>

</body>
</html>
)rawliteral";

// ===== SETUP =====

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

  initWebSocket();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

// ===== LOOP =====

void loop() {
  ws.cleanupClients();

  if (currentDirection != "stop" && (millis() - lastCmdTime > SAFETY_TIMEOUT)) {
    detener();
    ws.textAll("stop");
  }
}
