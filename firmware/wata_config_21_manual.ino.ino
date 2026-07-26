#include <Wire.h>

// Only needed for Arduino 1.6.5 and earlier

#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"
#include <DHT.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include <TimeLib.h>
//--------------------------DEFINICIÓN DHT22-------------------//definimos los campos del sensor DHT22
#define DHTPIN 4
#define DHTTYPE DHT22

// conectado al GPIO4 (D2)
// DHT 22

// Inicializo el sensor DHT22
DHT dht(DHTPIN, DHTTYPE);
//------------------ OLED DISPLAY - I2C - 128X64---------------------SSD1306Wire display(0x3c, D3, D4);
//SDA = D3 (datos)
//SCL = D4/D5 (clock)
//--------------------------CONTROL PWM----------------------const int RESISTENCIA = 14; //pin conectado la resistencia D5(GPIO 14)
// Constantes del controlador


float Kp = 0;
// variables externas del controlador
float Input = 0;
int Output = 0;
float Setpoint = 0;
// variables internas del controlador
float error = 1;
float PWM = 0;
int PWM_int = 0;
int output = 0;
//variables para el timer incubadora lista
int timer_1 = 1; //1º vez que se alcanza un error = 0; LED ROJO
int timer_2 = 0; //2º vez que se alcanza error = 0; puede introducirse
las muestras de agua; LED VERDE
int timer_3 = 0; //se ha pulsado el botón 1; inizialización del
temporizador; se muestra el tiempo transcurrido; LED VERDE + LED ROJO
int timer_4 = 1; //cuando la incubadora está apagada timer_4 = 1e
inicializo todas las variables del cronómetro, después pasa a 0
int timer_5 = 0; //si pulso el botón 2 FÍSICO para ON/OFF incubadora
int timer_6 = 0; //cuando pulsa el botón del cronómetro y llega a los
43 grados... se inicia el cronómetro
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

conectado a la entrada D6 (GIPO 12) -

AMARILLO


const int btn2 = 13; //btn2

conectado a la entrada D7 (GIPO 13) -

AZUL
int lecturaBtn1 = 0; //estado de la variable btn1 FÍSICO - CRONÓMETRO
int lecturaBtn2 = 0; //estado de la variable btn1 FÍSICO - ON/OFF

void setup() {
Serial.begin(115200);
//oled display i2c - 128x64
display.init();
display.flipScreenVertically();
display.setFont(ArialMT_Plain_24);
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
//-------------------------FUNCIÓN IMPRIMO T Y H-------------------void analisisIncubacion() {
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
/***********************IMPRIMO POR SERIAL
MONITOR****************************/
Serial.println("-----------------------------------------");
Serial.println("Análisis a 44ºC");
Serial.print("Humedad: ");
Serial.println(h);
Serial.print("Temperatura: ");
Serial.println(t);
Serial.print("\n");
/***********************COMPUTO ERROR
PWM************************************/
Input = t;

// leer una entrada del controlador

Output = computePID(Input);
delay(2000);
analogWrite(RESISTENCIA, Output); //0-1024 de rango de valores
para iluminar el LED (10 bits - 2^10)
}
}
//-------------------------FUNCIÓN CONTROL PWM-----------------------int computePID(float inp) {
/***********************TEMPERATURA OBJETIVO DEL
ESTUDIO************************/
Setpoint = 44;
/***********************CÁLCULO ERROR DE
TEMPERATURA************************/
float error = Setpoint - Input;

// determinar el error

entre la consigna y la medición
/*************************** ESTADOS DE LA INCUBADORA: 1.Calentando;
2.TEstabilizada ; 3.Temporizador ***************************/
//1.Calentando - timer_1 = 1; timer_2 = 0; timer_3 = 0; timer_4 = 1
if (timer_1 == 1 && timer_2 == 0 && timer_3 == 0) {
//Incubadora llegando a Tª incubación... NO COLOCAR LAS MUESTRAS
AUN


//LEDs físicos
digitalWrite(led1, LOW); //VERDE apagado
digitalWrite(led2, LOW); //ROJO encendido
delay(2000);
}
//2. TEstabilizada - timer_1 = 1; timer_2 = 1; timer_3 = 0; timer_4
= 1
if (error == 0 && timer_1 == 1 && timer_2 == 1 && timer_3 == 0) {
//segunda estabilización de la temperatura
//realidad
digitalWrite(led1, HIGH); //VERDE encendido
digitalWrite(led2, HIGH); //ROJO apagado
//no vuelvas a meterte a este bucle
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
if(timer_1 == 1 && timer_2 == 2 && timer_3 == 1){
//LEDs físicos
digitalWrite(led1, HIGH); //VERDE encendido
digitalWrite(led2, LOW); //ROJO encendido
delay(2000);
}
/******************CÓDIGO PWM PARA TEMPERATURA************/
if (error > 5) { //cuando T<<<<SetPoint - PWM [100%]
PWM = 100;
output = 1023; //se enciende al 100%
delay(500);


}
if (error < 5 && error > 1) { //cuando T<<<SetPoint - PWM [100%-50%]
Kp = ((error - 1) / (5 - 1));
PWM = ((Kp * (100 - 50)) + 50);
output = ((PWM * 1024) / 100);
delay(500);
}
if (error < 1 && error > 0.5) { // cuando T<<SetPoint - PWM [50%25%]
Kp = ((error - 0.5) / (1 - 0.5));
PWM = ((Kp * (50 - 20)) + 20);
output = ((PWM * 1024) / 100);
delay(500);
}
if (error < 0.5 && error > (-0.5)) { // cuando T<SetPoint - PWM
[25%-10%]
Kp = ((error + 0.5)/ (0.5 + 0.5));
PWM = ((Kp * (15 -5)) + 5);
output = ((PWM * 1024) / 100);
delay(500);
}
/*if (error < 0 && error > (-0.5)) { // cuando T>SetPoint - PWM [5%3%]
vv

Kp = ((error + 0.5) / (0.5));
PWM = ((Kp * (5 - 3)) + 3);
output = ((PWM * 1024) / 100);
delay(500);

}*/
/*if (error < (-0.5) && error > (-1)) { // cuando está cerca de
alcanzar la Tª setpoint - PWM [5%-2.5%]
Kp = ((error + 1) / (-0.5 + 1));
PWM = ((Kp * (5 - 2.5)) + 2.5);
output = ((PWM * 1024) / 100);
delay(500);
}*/


if (error < (-0.5)) { //cuando T>>SetPoint - PWM [4%]
PWM = 4;
output = 30;
delay(500);
}
/**********************SERIAL MONITOR*********/
Serial.print("Constante Kp: ");
Serial.println(Kp);
Serial.print("ERROR de temperatura: ");
Serial.println(error);
Serial.print("PWM: ");
Serial.println(PWM);
Serial.print("OUTPUT: ");
Serial.println(output);
Serial.println(" -----------------------------------------------");
/**********************OLED DISPLAY I2C - 128X64**********/
PWM_int = PWM;
display.clear();
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_16);
display.drawString(40, 0, " e.Coli");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(5, 20, "T: " + String(Input));
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(95, 20, "ºC");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_16);
display.drawString(20, 48, "PWM: " + String (PWM_int) + " %");
display.display();
delay (5000);
/*************** CASOS EN EL ANÁLSIS DE COLIFORMES ************/
if(timer_2 != 2){ // temperatura estabilizándose


delay(1000);
//mensaje aviso
Serial.println("TEMPERATURA ESTABILIZÁNDOSE");
Serial.println("NO COLOQUE LAS MUESTRAS AUN");
}
if(timer_2 == 2 && timer_3 == 0){ // coloque las muestras de agua
Serial.println("PUEDE COLOCAR LAS MUESTRAS DE AGUA");
Serial.println("EN LA ZONA DE INCUBACIÓN");
}
if(timer_2 == 2 && timer_3 == 1){ //inicio de la incubación por un
periodo de 18h
Serial.println("SE HA INICIADO LA INCUBACIÓN");
if(Input > 43 && timer_6 == 0){
timer_6 = 1;
}
if(timer_6 == 1){
MostrarTiempo();
delay(2000);
/*************ALARMA SI EL ERROR ES MAYOR A 1ºC*********/
if (error > 2 || error < (-2)) {
Serial.println("¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡ALERTA!!!!!!!!!!!!!!!!!!
!!!!!!!");
Serial.print("EL ERROR DE TEMPERATURA ES MAYOR A 1ºC: ");
Serial.println(error);
Serial.print("
");
delay(500);
}
}
}
return output;
}
//--------------FUNCIÓN MOSTRAR TIEMPO TRASNCURRIDO TEPORIZADOR---void MostrarTiempo()

{

if(timer_4 == 1){ //si se ha apagado a incubadora
//inicializo todas las variables del cronómetro
h_inicio = hour();


m_inicio = minute();
s_inicio = second();
timer_4 = 0;
}
//Monitor Serial
h = hour() - h_inicio;
m = minute() - m_inicio;
s = second() - s_inicio;
//Monitor serie
Serial.print("Tiempo transcurrido: ");
Serial.print(h);
Serial.print(":");
Serial.print(m);
Serial.print(":");
Serial.println(s);
//Oled Dsiplay i2c
display.clear();
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_16);
display.drawString(40, 0, " e.Coli");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(5, 20, String(m));
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(65, 20, "min");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_16);
display.drawString(20, 48, "PWM: " + String (PWM_int) + " %");
display.display();
delay(4000);
}
void loop() {
/*******ON/ OFF INCUBADORA*************/
//BOTÓN FÍSICO
lecturaBtn2 = digitalRead(btn2);
if(lecturaBtn2 == LOW){
if(ON_OFF == 2){


ON_OFF = 0;
}
ON_OFF++;
switch(ON_OFF){
case 1:
Serial.println("INCUBADORA ENCENDIDA POR BOTÓN FÍSICO");
display.clear();
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 10, "Incubadora");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 40, "encendida");
display.display();
timer_5 = 1;
delay(500);
break;
case 2:
Serial.println("INCUBADORA APAGADA POR BOTÓN FÍSICO");
display.clear();
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 10, "Incubadora");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 40, "apagada");
display.display();
timer_5 = 0;
analogWrite(RESISTENCIA, 0);
delay(500);
break;
}
while(!digitalRead(btn2));
}
if(timer_5 == 1){ //cuando la incubadora enté encendida...
if(m <= 1080){ // si no hemos sobrepasado el tiempo de incubación
- 18h = 1.080 minutos


analisisIncubacion();
}
if(m >= 1080){ // ha pasado el tiempo de incubación pero la
incubadora sigue encendida
Serial.print("FIN INCUBACIÓN");
Serial.println("APAGUE LA INCUBADORA POR FAVOR");
display.clear();
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 0, "Fin");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 20, "incubacion");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_16);
display.drawString(0, 47, "

Pulse boton 1");

display.display();
//apago resistencia
analogWrite(RESISTENCIA, 0);
}
}
if(timer_5 == 0){ //cuando apago incubadora
Serial.print("INCUBADORA APAGADA");
display.clear();
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 10, "Incubadora");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 40, "apagada");
display.display();
//apago todos los LEDs
//LEDs físicos
digitalWrite(led1, LOW); //VERDE apagado
digitalWrite(led2, HIGH); //ROJO apagado
//inicializo timers
timer_1 = 1;
timer_2 = 0;
timer_3 = 0;


timer_4 = 1;
timer_5 = 0;
timer_6 = 0;
h = 0;
m = 0;
s = 0;
//apago resistencia
analogWrite(RESISTENCIA, 0);
}
/***********************CRONÓMETRO************************/
//Cualdo pulso botón FÍSICO
lecturaBtn1 = digitalRead(btn1);
if(lecturaBtn1 == LOW){
if(timer_1 == 1 && timer_2 == 2 && timer_3 == 0){ //si lo he
pulsado cuando acaba el estado 2
timer_3 = 1;
} if(timer_2 != 2) {
display.clear();
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 0, "Espere a");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 20, "temperatura");
display.setTextAlignment(TEXT_ALIGN_LEFT);
display.setFont(ArialMT_Plain_24);
display.drawString(0, 40, "estabilizada");
display.display();
}
while(!digitalRead(btn1));
}
}

