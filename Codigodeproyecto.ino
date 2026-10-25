













// ==========================================================
// ELEVADOR 3 NIVELES (SIN PUERTAS) - ARDUINO MEGA 2560
// ==========================================================

#include <Keypad.h>

// --- DISPLAY 7 SEGMENTOS (74HC595) ---
const int clockpin = 9;      
const int data     = 7;      
const int latch    = 8;      

const int NUM[] = { 
  202,   // NUM 0 (11000000)
  249,   // NUM 1 (11111001)
  164,   // NUM 2 (10100100)
  176    // NUM 3 (10110000)
};

// --- SENSORES DE PISO (NUEVOS PINES REASIGNADOS) ---
const int s1a = 42;   // Sensor Piso 1
const int s2a = 43;   // Sensor Piso 2
const int s3a = 44;   // Sensor Piso 3

// --- MOTOR PRINCIPAL (L298N) ---
const int m_up   = 36;      
const int m_down = 37;    

// --- CONFIGURACIÓN DE TU BOTONERA PCB 4X4 VALIDADA ---
const byte FILAS = 4; 
const byte COLUMNAS = 4; 

char teclas[FILAS][COLUMNAS] = {
  {'1','2','3','4'},     // R1 -> S1 (Piso 1), S2 (Piso 2), S3 (Piso 3), S4
  {'5','6','7','8'},     // R2 -> S5 a S8
  {'9','A','B','C'},     // R3 -> S9 a S12
  {'D','E','F','G'}      // R4 -> S13 a S16
};

// Pines de la Botonera PCB (R1..R4 y C1..C4)
byte pinesFilas[FILAS]       = {28, 29, 30, 31}; // R1, R2, R3, R4
byte pinesColumnas[COLUMNAS] = {27, 26, 25, 24}; // C1, C2, C3, C4

Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// --- VARIABLES DE ESTADO Y GRAFCET ---
int contadorU;

boolean e0 = false;
boolean e1 = false, e2 = false, e3 = false, e4 = false, e5 = false, e6 = false;

boolean t01 = false, t02 = false, t03 = false, t04 = false, t05 = false, t06 = false;
boolean t10 = false, t20 = false, t30 = false, t40 = false, t50 = false, t60 = false;

// Banderas de llamada
boolean b1 = false, b2 = false, b3 = false;

void cont() {
  digitalWrite(latch, LOW);
  shiftOut(data, clockpin, MSBFIRST, NUM[contadorU]); 
  digitalWrite(latch, HIGH);
}

void setup() {
  pinMode(s1a, INPUT);
  pinMode(s2a, INPUT);
  pinMode(s3a, INPUT);

  pinMode(m_up, OUTPUT);
  pinMode(m_down, OUTPUT);

  pinMode(latch, OUTPUT);
  pinMode(clockpin, OUTPUT);
  pinMode(data, OUTPUT);

  delay(2000);

  // Calibración inicial: Buscar origen (Piso 1) si la cabina no toca ningún sensor
  while (digitalRead(s1a) == LOW && digitalRead(s2a) == LOW && digitalRead(s3a) == LOW) {
    digitalWrite(m_down, HIGH);
    digitalWrite(latch, LOW);
    shiftOut(data, clockpin, MSBFIRST, NUM[0]); 
    digitalWrite(latch, HIGH);
  }

  digitalWrite(m_up, LOW);
  digitalWrite(m_down, LOW);
}

void loop() {
  // --- LECTURA DE LA BOTONERA ---
  char tecla = teclado.getKey();

  if (tecla == '1') { b1 = true; b2 = false; b3 = false; } // Botón S1
  if (tecla == '2') { b2 = true; b1 = false; b3 = false; } // Botón S2
  if (tecla == '3') { b3 = true; b1 = false; b2 = false; } // Botón S3

  // --- LECTURA DE SENSORES DE PISO ---
  int s1 = digitalRead(s1a);
  int s2 = digitalRead(s2a);
  int s3 = digitalRead(s3a);

  // --- LÓGICA GRAFCET ---
  if (!e1 && !e2 && !e3 && !e4 && !e5 && !e6) {
    e0 = true;
  }

  // Evaluaciones de Transición
  // Piso 1 a 2
  if (e0 && s1 && b2) { t01 = true; t10 = false; }
  if (e1 && s2)       { t10 = true; t01 = false; b2 = false; }

  // Piso 1 a 3
  if (e0 && s1 && b3) { t02 = true; t20 = false; }
  if (e2 && s3)       { t20 = true; t02 = false; b3 = false; }

  // Piso 2 a 3
  if (e0 && s2 && b3) { t03 = true; t30 = false; }
  if (e3 && s3)       { t30 = true; t03 = false; b3 = false; }

  // Piso 3 a 2
  if (e0 && s3 && b2) { t04 = true; t40 = false; }
  if (e4 && s2)       { t40 = true; t04 = false; b2 = false; }

  // Piso 3 a 1
  if (e0 && s3 && b1) { t05 = true; t50 = false; }
  if (e5 && s1)       { t50 = true; t06 = false; b1 = false; }

  // Piso 2 a 1
  if (e0 && s2 && b1) { t06 = true; t60 = false; }
  if (e6 && s1)       { t60 = true; t06 = false; b1 = false; }

  // Cambios de Etapa
  if (e0 && t01) { e0 = false; e1 = true; }
  if (e1 && t10) { e1 = false; e0 = true; }

  if (e0 && t02) { e0 = false; e2 = true; }
  if (e2 && t20) { e2 = false; e0 = true; }

  if (e0 && t03) { e0 = false; e3 = true; }
  if (e3 && t30) { e3 = false; e0 = true; }

  if (e0 && t04) { e0 = false; e4 = true; }
  if (e4 && t40) { e4 = false; e0 = true; }

  if (e0 && t05) { e0 = false; e5 = true; }
  if (e5 && t50) { e5 = false; e0 = true; }

  if (e0 && t06) { e0 = false; e6 = true; }
  if (e6 && t60) { e6 = false; e0 = true; }

  // --- CONTROL DEL MOTOR PRINCIPAL ---
  if (e1 || e2 || e3) {   // Subiendo
    digitalWrite(m_up, HIGH);
    digitalWrite(m_down, LOW);
  }

  if (e0) {               // Reposo
    digitalWrite(m_up, LOW);
    digitalWrite(m_down, LOW);
  }

  if (e4 || e5 || e6) {   // Bajando
    digitalWrite(m_up, LOW);
    digitalWrite(m_down, HIGH);
  }

  // --- ACTUALIZACIÓN DEL DISPLAY ---
  if (s1 == HIGH) { contadorU = 1; cont(); }
  if (s2 == HIGH) { contadorU = 2; cont(); }
  if (s3 == HIGH) { contadorU = 3; cont(); }
}