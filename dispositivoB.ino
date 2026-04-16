#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

// --- CONFIGURACIÓN DE DIRECCIONES MAC ---
uint8_t broadcastAddress[] = {0x94, 0x51, 0xDC, 0x4B, 0x84, 0xF4};

// --- Configuración de Pantalla ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Configuración de Teclado ---
const byte FILAS = 4;
const byte COLUMNAS = 4;
char teclas[FILAS][COLUMNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pinesFilas[FILAS] = {13, 12, 14, 27}; 
byte pinesColumnas[COLUMNAS] = {26, 25, 33, 32}; 
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// --- Diccionarios ---
const char* diccNumeros[10] = {
  "0", "1abc", "2def", "3ghi", "4jkl", "5mno", "6pqr", "7stu", "8vwx", "9yz?"
};
const char* macrosPregunta[4] = {"¿Como vas?", "¿Donde estas?", "¿Ya vienes?", "¿Que haces?"};
const char* macrosRespuesta[4] = {"Todo bien", "En la uni", "Voy en camino", "Estudiando"};

// --- Variables de Estado ---
String mensajeTx = ""; 
String mensajeRx = "Sin mensajes";
unsigned long ultimoToque = 0;
char teclaAnterior = '\0';
int indiceCiclo = 0;
bool modoEspera = false;
volatile bool actualizarOLED = false; // Bandera segura para actualizar pantalla

esp_now_peer_info_t peerInfo;

void actualizarPantalla() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(0, 0);
  display.println("RECIBIDO:");
  display.println(mensajeRx);
  
  display.drawLine(0, 26, 128, 26, WHITE);
  
  display.setCursor(0, 32);
  display.println("ENVIANDO:");
  display.setTextSize(2);
  display.println(mensajeTx);
  display.display();
}

void enviarMensaje() {
  if (mensajeTx.length() > 0) {
    esp_now_send(broadcastAddress, (uint8_t *)mensajeTx.c_str(), mensajeTx.length());
    mensajeTx = ""; 
    actualizarPantalla();
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Enviado" : "Error");
}

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
#else
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
#endif
  char buffer[len + 1];
  memcpy(buffer, incomingData, len);
  buffer[len] = '\0';
  mensajeRx = String(buffer);
  actualizarOLED = true; // Solo levantamos bandera, no tocamos la pantalla aquí
}

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;

  esp_now_register_send_cb((esp_now_send_cb_t)OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  // REFUERZO HARDWARE: Forzar pull-ups después de iniciar Wi-Fi
  for(byte i = 0; i < FILAS; i++) {
    pinMode(pinesFilas[i], INPUT_PULLUP);
  }

  actualizarPantalla();
}

void loop() {
  // Actualizar pantalla fuera del callback
  if (actualizarOLED) {
    actualizarPantalla();
    actualizarOLED = false;
  }

  char tecla = teclado.getKey();
  unsigned long tiempoAhora = millis();

  // Lógica de confirmación por tiempo
  if (modoEspera && (tiempoAhora - ultimoToque > ( (teclaAnterior >= '1' && teclaAnterior <= '9') ? 2000 : 1000))) {
    modoEspera = false;
    teclaAnterior = '\0';
  }

  if (tecla) {
    if (tecla >= '1' && tecla <= '9') {
      int idx = tecla - '0';
      if (tecla == teclaAnterior && modoEspera) {
        indiceCiclo = (indiceCiclo + 1) % 4;
        mensajeTx[mensajeTx.length() - 1] = diccNumeros[idx][indiceCiclo];
      } else {
        indiceCiclo = 0;
        if (mensajeTx.length() < 15) mensajeTx += diccNumeros[idx][indiceCiclo];
      }
      teclaAnterior = tecla;
      modoEspera = true;
    } 
    else if (tecla == '0') {
      if (tecla == teclaAnterior && modoEspera) {
        mensajeTx[mensajeTx.length() - 1] = ' '; 
      } else {
        if (mensajeTx.length() < 15) mensajeTx += '0';
      }
      teclaAnterior = tecla;
      modoEspera = true;
    }
    else if (tecla == '*') {
      if (tecla == teclaAnterior && modoEspera) {
        if (mensajeTx.length() > 0) mensajeTx.remove(mensajeTx.length() - 1); 
        teclaAnterior = '\0';
        modoEspera = false;
      } else {
        teclaAnterior = tecla;
        modoEspera = true;
      }
    }
    else if (tecla == '#') {
      if (tecla == teclaAnterior && modoEspera) {
        enviarMensaje(); 
        teclaAnterior = '\0';
        modoEspera = false;
      } else {
        teclaAnterior = tecla;
        modoEspera = true;
      }
    }
    else if (tecla >= 'A' && tecla <= 'D') {
      int idxMacro = tecla - 'A';
      if (tecla == teclaAnterior && modoEspera) {
        int tamPregunta = String(macrosPregunta[idxMacro]).length();
        for(int i=0; i<tamPregunta; i++) {
            if (mensajeTx.length() > 0) mensajeTx.remove(mensajeTx.length()-1);
        }
        mensajeTx += macrosRespuesta[idxMacro];
        if (mensajeTx.length() > 15) mensajeTx = mensajeTx.substring(0, 15);
        modoEspera = false; 
        teclaAnterior = '\0';
      } else {
        mensajeTx += macrosPregunta[idxMacro];
        if (mensajeTx.length() > 15) mensajeTx = mensajeTx.substring(0, 15);
        teclaAnterior = tecla;
        modoEspera = true;
      }
    }

    ultimoToque = tiempoAhora;
    actualizarPantalla();
  }
}