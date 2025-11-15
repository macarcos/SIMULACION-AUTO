// Sistema Inteligente de Seguridad con Control Adaptativo
// Proyecto: Control de Motor DC con Sensor PIR y Comunicación Serial

const int pirPin = 9;           // Sensor PIR
const int motorPin = 2;         // Motor DC (L293D Pin 2)
const int ledIndicator = 13;    // LED indicador (opcional)

// Variables de estado
int switchState = 0;
bool modoManual = false;        // Modo manual desde app
bool motorForzado = false;      // Estado forzado por app
unsigned long ultimoMovimiento = 0;
unsigned long tiempoEspera = 5000; // 5 segundos sin movimiento

// Variables de comunicación
String comando = "";

void setup() {
  Serial.begin(9600);
  
  pinMode(motorPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(ledIndicator, OUTPUT);
  
  // Mensaje inicial para la app
  Serial.println("SYSTEM:READY");
  Serial.println("Sistema de Seguridad Inteligente v1.0");
  Serial.println("Comandos: ON, OFF, AUTO, STATUS");
  Serial.println("-----------------------------------");
}

void loop() {
  // Leer comandos desde Serial (simulando Bluetooth)
  if (Serial.available() > 0) {
    comando = Serial.readStringUntil('\n');
    comando.trim();
    procesarComando(comando);
  }
  
  // Lógica de control adaptativa
  if (modoManual) {
    // Modo manual: controlado por app
    controlarMotorManual();
  } else {
    // Modo automático: controlado por sensor PIR
    controlarMotorAutomatico();
  }
  
  // Enviar datos periódicos a la app (cada 1 segundo)
  static unsigned long ultimoEnvio = 0;
  if (millis() - ultimoEnvio > 1000) {
    enviarDatosApp();
    ultimoEnvio = millis();
  }
  
  delay(100);
}

void controlarMotorAutomatico() {
  switchState = digitalRead(pirPin);
  
  if (switchState == HIGH) {
    digitalWrite(motorPin, HIGH);
    digitalWrite(ledIndicator, HIGH);
    ultimoMovimiento = millis();
    Serial.println("ALERT:Movimiento detectado!");
  } else {
    // Apagar motor después de tiempo de espera
    if (millis() - ultimoMovimiento > tiempoEspera) {
      digitalWrite(motorPin, LOW);
      digitalWrite(ledIndicator, LOW);
    }
  }
}

void controlarMotorManual() {
  if (motorForzado) {
    digitalWrite(motorPin, HIGH);
    digitalWrite(ledIndicator, HIGH);
  } else {
    digitalWrite(motorPin, LOW);
    digitalWrite(ledIndicator, LOW);
  }
}

void procesarComando(String cmd) {
  Serial.println("CMD:" + cmd);
  
  if (cmd == "ON") {
    modoManual = true;
    motorForzado = true;
    Serial.println("RESPONSE:Motor encendido manualmente");
    
  } else if (cmd == "OFF") {
    modoManual = true;
    motorForzado = false;
    Serial.println("RESPONSE:Motor apagado manualmente");
    
  } else if (cmd == "AUTO") {
    modoManual = false;
    Serial.println("RESPONSE:Modo automático activado");
    
  } else if (cmd == "STATUS") {
    enviarEstadoCompleto();
    
  } else {
    Serial.println("ERROR:Comando no reconocido");
  }
}

void enviarDatosApp() {
  // Formato: DATA:modo,motorState,pirState,tiempo
  String datos = "DATA:";
  datos += modoManual ? "MANUAL" : "AUTO";
  datos += ",";
  datos += digitalRead(motorPin) ? "ON" : "OFF";
  datos += ",";
  datos += digitalRead(pirPin) ? "DETECTED" : "CLEAR";
  datos += ",";
  datos += String(millis() / 1000);
  
  Serial.println(datos);
}

void enviarEstadoCompleto() {
  Serial.println("STATUS:--- Estado del Sistema ---");
  Serial.println("STATUS:Modo: " + String(modoManual ? "Manual" : "Automático"));
  Serial.println("STATUS:Motor: " + String(digitalRead(motorPin) ? "Encendido" : "Apagado"));
  Serial.println("STATUS:PIR: " + String(digitalRead(pirPin) ? "Movimiento" : "Sin movimiento"));
  Serial.println("STATUS:Tiempo activo: " + String(millis()/1000) + "s");
  Serial.println("STATUS:----------------------------");
}
