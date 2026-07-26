#include <Wire.h>

// Only needed for Arduino 1.6.5 and earlier

#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"
#include <DHT.h>
#include <SPI.h>


#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
//---------------------DEFINICIÓN BLYNK---------------#define BLYNK_PRINT Serial
//AuthTokens identifica nuestra cuenta en Blynk
char auth[] = "Yexn-NqbyEnzreBZ75nfWsizbuL9XDJ0";
//usuario:raquel.nunez.delafuente@gmail.com;
password:blynkIncubadora2021
//Credenciales Wifi
//Wifi casa 1
//char ssid[] = "MOVISTAR_C600";
//char pass[] = "**********";

//Enter your WIFI Name

//Enter your WIFI Password

//Wifi mi móvil
char ssid[] = "Movil_Raquel";

//Enter your WIFI Name

char pass[] = "*****************";

//Enter your WIFI Password

//Wifi casa 2
//char ssid[] = "MiFibra-0FD0";

//Enter your WIFI Name

//char pass[] = "***************";

//Enter your WIFI Password

//Control del tiempo de medición
//BlynkTimer timer;
SimpleTimer timer;
//LEDs
WidgetLED LEDrojo(V5);
WidgetLED LEDverde(V6);
//----------------------DEFINICIÓN DHT22-----------------------//definimos los campos del sensor DHT22
#define DHTPIN 4
#define DHTTYPE DHT22

// conectado al GPIO4 (D2)
// DHT 22

// Inicializo el sensor DHT22
DHT dht(DHTPIN, DHTTYPE);


//---------------------CONTROL PWM--------------------------const int RESISTENCIA = 14; //pin conectado la resistencia D5(GPIO 14)
// Constantes del controlador
float Kp = 0;
// variables externas del controlador
float Input = 0;
int Output = 0;
float Setpoint = 0;
// variables internas del controlador
float error = 1;
float PWM = 0;
int output = 0;
//variables para el timer incubadora lista
int timer_1 = 1; //1º vez que se alcanza un error = 0; LED ROJO
int timer_2 = 0; //2º vez que se alcanza error = 0; puede introducirse
las muestras de agua; LED VERDE
int timer_3 = 0; //se ha pulsado el botón 1; inizialización del
temporizador; se muestra el tiempo transcurrido; LED VERDE + LED ROJO
int timer_4 = 1; //cuando la incubadora está apagada timer_4 = 1e
inicializo todas las variables del cronómetro, después pasa a 0
//int timer_5 = 0; //si pulso el botón 2 FÍSICO para ON/OFF incubadora
int timer_6 = 0; //cuando pulsa el botón del cronómetro y llega a los
43 grados... se inicia el cronómetro
int timer_7 = 0; //mandar notificación sólo 1 vez cuando se enciende
la incubadora
int ON_OFF = 0;
//Variables temporizador
int h_inicio = 0;
int m_inicio = 0;
int s_inicio = 0;
int h = 0;
int m = 0;
int s = 0;


//variables LEDs y botones FÍSICOS
const int led1 = 16; //LED VERDE conectado a la entrada D0 (GIPO 16)
const int led2 = 5; //LED ROJO conectado a la entrada D1 (GIPO 5)
const int btn1 = 12; //btn1

conectado a la entrada D6 (GIPO 12)

const int btn2 = 13; //btn2

conectado a la entrada D7 (GIPO 13)

int lecturaBtn1 = 0; //estado de la variable btn1 FÍSICO - CRONÓMETRO
int lecturaBtn2 = 0; //estado de la variable btn1 FÍSICO - ON/OFF
int lecturaBtn1V = 0; //estado de la variable btn1 VIRTUAL CRONÓMETRO
int lecturaBtn2V = 0; //estado de la variable btn1 VIRTUAL - ON/OFF
//guarda estado variable del botón VIRTUAL 1
BLYNK_WRITE(V7) {
lecturaBtn1V = param.asInt();
}
//guarda estado variable del botón VIRTUAL 2
BLYNK_WRITE(V9) {
lecturaBtn2V = param.asInt();
}
void setup() {
Serial.begin(115200);
//Inicializo la app Blynk
Blynk.begin(auth, ssid, pass);
// Setup a function to be called every second
timer.setInterval(1000L, sendByBlynk);
//Inicializo el DHT22
dht.begin();
//LEDs
pinMode(led1, OUTPUT); //LED VERDE cuando la Tª de la incubación SI
está aun estabilizada - conexión pull-up (HIGH-ON; DOWN-OFF)
pinMode(led2, OUTPUT); //LED ROJO cuando la Tª de la incubación NO
está aun estabilizada - conexión pull-down (HIGH-OFF; DOWN-ON)
//Botón
pinMode(btn1, INPUT); //inizializo el temporizador
pinMode(btn2, INPUT); //inizializo el temporizador
}


//----------------------FUNCIÓN IMPRIMO T Y H----------------void sendByBlynk() {
/***********************LEO VALORES DE T Y H
DELDHT22***********************/
float h = dht.readHumidity();
float t = dht.readTemperature(); // or dht.readTemperature(true) for
Fahrenheit
if (isnan(h) || isnan(t)) {
Serial.print(F("Failed to read from DHT sensor!\r\n"));
}
else {
delay(2000);
/**************IMPRIMO POR SERIAL MONITOR***************/
Serial.println("-----------------------------------------");
Serial.println("Análisis a 44ºC");
Serial.print("Humedad: ");
Serial.println(h);
Serial.print("Temperatura: ");
Serial.println(t);
Serial.print("\n");
/*************IMPRIMO POR TERMINAL BLINK*************/
Blynk.virtualWrite(V1, "--------------------------------------");
Blynk.virtualWrite(V1, "

ANÁLISIS COLIFORMES E.COLI (44ºC)");

Blynk.virtualWrite(V1, "--------------------------------------");
/*Blynk.virtualWrite(V1, "TEMPERATURA: ");
Blynk.virtualWrite(V1, t);*/
//Gauge Widget
Blynk.virtualWrite(V0, t);
Blynk.virtualWrite(V3, h);
/*********************COMPUTO ERROR PWM*******************/
Input = t;

// leer una entrada del controlador

Output = computePID(Input);
delay(2000);


analogWrite(RESISTENCIA, Output); //0-1024 de rango de valores
para iluminar el LED (10 bits)
}
}
//----------------------FUNCIÓN CONTROL PWM--------------int computePID(float inp) {
/************TEMPERATURA OBJETIVO DEL ESTUDIO************/
Setpoint = 44;
/************CÁLCULO ERROR DE TEMPERATURA***************/
float error = Setpoint - Input;

// determinar el error

entre la consigna y la medición
/** ESTADOS DE LA INCUBADORA: 1.Calentando; 2.TEstabilizada ;
3.Temporizador **/
//1.Calentando - timer_1 = 1; timer_2 = 0; timer_3 = 0; timer_4 = 1
if (timer_1 == 1 && timer_2 == 0 && timer_3 == 0) {
//Incubadora llegando a Tª incubación... NO COLOCAR LAS MUESTRAS
AUN
//LEDs físicos
digitalWrite(led1, LOW); //VERDE apagado
digitalWrite(led2, LOW); //ROJO encendido
//LEDs virtuales
LEDrojo.on(); //ROJO encendido
LEDverde.off(); //VERDE apagado
delay(2000);
}
//2. TEstabilizada - timer_1 = 1; timer_2 = 1; timer_3 = 0; timer_4
= 1
if (error == 0 && timer_1 == 1 && timer_2 == 1 && timer_3 == 0) {
//segunda estabilización de la temperatura
//realidad
digitalWrite(led1, HIGH); //VERDE encendido
digitalWrite(led2, HIGH); //ROJO apagado
//virtual
LEDrojo.off(); //ROJO apagado


LEDverde.on(); //VERDE encendido
//no vuuelvas a meterte a este bucle
timer_2 = 2;
delay(2000);
}
//timer_1 = 1; timer_2 = 0; timer_3 = 0; timer_4 = 1
if (error == 0 && timer_1 == 1 && timer_2 == 0 && timer_3 == 0) { //
primera estabilización de la temperatura
timer_2 = 1;
delay(2000);
}
//3. Temporizador - timer_1 = 1; timer_2 = 1; timer_3 = 1; timer_4 =
