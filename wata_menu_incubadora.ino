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
//-------------------------DEFINICIÓN BLYNK--------------------------#define BLYNK_PRINT Serial
//AuthTokens identifica nuestra cuenta en Blynk
char auth[] = "Yexn-NqbyEnzreBZ75nfWsizbuL9XDJ0";
//Credenciales Wifi
//Wifi casa papá
//char ssid[] = "MOVISTAR_C600";

//Enter your WIFI Name

//char pass[] = "s2RhUTXKQnWXuiw3mY8L";

//Enter your WIFI Password

//Wifi casa mamá
//char ssid[] = "vodafoneE292";

//Enter your WIFI Name

//char pass[] = "ATHYQPCMWSVUPD";

//Enter your WIFI Password

//Wifi mi móvil
char ssid[] = "Movil_Raquel";

//Enter your WIFI Name

char pass[] = "raquelomola";

//Enter your WIFI Password

//Control del tiempo de medición
BlynkTimer timer;
//--------------------------DEFINICIÓN DHT22-------------------------//definimos los campos del sensor DHT22
#define DHTPIN 4
#define DHTTYPE DHT22

// conectado al GPIO4 (D2)
// DHT 22

// Inicializo el sensor DHT22
DHT dht(DHTPIN, DHTTYPE);
//---------------------------CONTROL PWM-----------------------


const int RESISTENCIA = 14; //pin conectado la resistencia D5(GPIO 14)
// Constantes del controlador
float Kp = 0;
// variables externas del controlador
float Input = 0;
int Output = 0;
float Setpoint = 0;
// variables internas del controlador
float error = 0;
float e = 0;
float PWM_error = 0;
float PWM = 0;
int output = 0;
//-------------------DEFINICIÓN DISPLAY OLED-----------------// Incializo la pantalla OLD empleando la librería Wire
SSD1306

display(0x3c, D3, D4);

/*
SDA=D3
SCL=D4
siempre funciona con un comando de loop
*/
//---------------------DECLARACIÓN DE VARIABLES MENU------------//declaro las variables de los pusadores y los LEDs indicando en que
entrada serán conectados
const int btn1 = 12; //btn1

conectado a la entrada D6 (GIPO 12)

const int btn2 = 13; //btn2

conectado a la entrada D7 (GIPO 13)

const int led1 = 16; //LED VERDE conectado a la entrada D0 (GIPO 16)
const int led2 = 5; //LED ROJO conectado a la entrada D1 (GIPO 5)
// declaro variables donde almaceno el estado de las variables
int lecturaBtn1 = 0;
int lecturaBtn2 = 0;
int menu = 0;
int tiempo = 0;


int testudio = 0;
bool estadoIncubadora = true;
//-------------------------VARIABLES TIMER---------------------------unsigned long tiempo1 = 0;
unsigned long tiempoSegundos = 0;
//------------------------INICIALIZO TODO----------------------------void setup() {
// Debug console
Serial.begin(115200);
//Inicializo la app Blynk
Blynk.begin(auth, ssid, pass);
//Inicializo el DHT22
dht.begin();
// Setup a function to be called every second
timer.setInterval(1000L, sendByBlynk);
//inicializo los botones y LEDs
pinMode(btn1, INPUT); //mostrar las distantas opciones del menú
pinMode(btn2, INPUT); //seleccionar la opción que queremos del menú
pinMode(led1, OUTPUT); //se enciende un LED cuando se escoja la
opción 37ºC
pinMode(led2, OUTPUT); //se enciende un LED cuando se escoja la
opción 44ºC
// Inicializo la pantalla oled
display.init();
display.flipScreenVertically();
display.setFont(ArialMT_Plain_16); //fuente
display.setTextAlignment(TEXT_ALIGN_LEFT);
dht.begin(); // initialize dht
}
//------------------------FUNCIÓN APAGAR INCUBADORA------------------//Apago incubadora
void incubadoraApagada(){
display.clear();
display.drawString(0, 0, "Apagando incubadora...");
Serial.println("Apagando incubadora...");
Serial.print("\n");


}
//-------------------------FUNCIÓN IMPRIMO T Y H--------------------//Imprimo valores por Blynk
void sendByBlynk(){
//cuando pasen 18 h sal del bucle
/*tiempo1 = millis(); //Actualiza el tiempo actual
if (tiempo1 <= 64800000){

//Si NO han pasado 18 horas

ejecutes el

IF
tiempoSegundos = tiempo1/1000;*/

/************LEO VALORES DE T Y H DELDHT22*****************/
float h = dht.readHumidity();
float t = dht.readTemperature(); // or dht.readTemperature(true) for
Fahrenheit
if(isnan(h) || isnan(t)) {
Serial.print(F("Failed to read from DHT sensor!\r\n"));
}
else {
/**************COMPUTO ERROR PWM******************/
Input = t;

// leer una entrada del controlador

Output = computePID(Input);

// calcular el controlador

Output=output=0.4
delay(1000);
analogWrite(RESISTENCIA, Output); //0-1024 de rango de valores para
iluminar el LED (10 bits)
/*****************IMPRIMO POR SERIAL MONITOR***************/
Serial.print("Análisis a ");
Serial.print(testudio);
Serial.println("ºC");
Serial.print("Humedad: ");
Serial.println(h);
Serial.print("Temperatura: ");
Serial.println(t);


Serial.print("\n");
/*Serial.print("Tiempo transcurrido");
Serial.print(tiempoSegundos);
Serial.print("\n");*/
/*****************IMPRIMO POR BLINK**********************/
//valor PWM [0-100%]
e = testudio - t;
if(e > 10){
PWM_error = 100;
} else {
PWM_error = (e)*10;
}
Blynk.virtualWrite(V6, t); // virtual pin
Blynk.virtualWrite(V5, h); // virtual pin
Blynk.virtualWrite(V4, PWM_error); // virtual pin

/******************IMPRIMO POR OLED DISPLAY****************/
display.clear();
display.drawString(0, 16, "H: " + String(h)+ "%");
display.drawString(0, 32, "T: " + String(t) + "C");
if(testudio == 37){
display.drawString(0, 0, "E.TOTALES - 37ºC");
//display.drawString(0, 40, String(tiempoSegundos));
digitalWrite(led1, HIGH); //VERDE encendido
digitalWrite(led2, HIGH); //ROJO apagado
}
if(testudio == 44){
display.drawString(0, 0, "E.COLI - 44ºC");
//display.drawString(0, 40, String(tiempoSegundos));
digitalWrite(led1, LOW); //VERDE apagado
digitalWrite(led2, LOW); //ROJO encendido
}
}
}


//----------------------------------------------------------------FUNCIÓN CONTROL PWM--------------------------------------------------------------------------------------------int computePID(float inp){
/************TIPO DE ESTUDIO************************************/
if (testudio == 37) {
Setpoint = 37;
}
if (testudio == 44) {
Setpoint = 44;
}
/****************CÁLCULO ERROR DE TEMPERATURA*****************/
error = Setpoint - Input;

// determinar el error entre

la consigna y la medición
PWM = error*10;

//valor de la PWM

/*****************MUETRO ERROR POR PANTALLA*******************/
Serial.print("ERROR de temperatura: ");
Serial.println(error);
if(error>10){

//diferencia de más de 10º

Serial.println("100 % PWM");
output = 1023; //se enciende al 100%
delay(1000);
}
if(error<10 && error>0){

//diferencia de menos de 10º

Kp = error/10;
Serial.print("Constante Kp: ");
Serial.println(Kp);
output = (Kp*1024);
Serial.print(PWM);
Serial.println("% PWM");
delay(1000);
}
if(error<=0){

//error negativo o 0

Serial.println("0 % PWM");
output = 0; //no se encience


delay(1000);
}
Serial.print("OUTPUT: ");
Serial.println(output);
Serial.println(" -----------------------------------------------");
return output;
}

//------------------------FUNCIÓN LOOP MENU--------------------------void loop() {
Blynk.run(); //When i comment this line, seems like the readHumidity
and ReadTemperature works each time
if (tiempo == 1){
timer.run();
}
lecturaBtn1 = digitalRead(btn1);
lecturaBtn2 = digitalRead(btn2);
display.display();
delay(1000);
if(lecturaBtn1 == LOW){
if(menu == 3){
menu = 0;
}
menu++;
switch(menu){
case 1: //OLED DISPLAY
display.clear();
display.drawString(0, 10, String(menu) + "
.Coliformes");
display.drawString(0, 20, " TOTALES");
//SERIAL PRINT
Serial.print(menu); Serial.println(" .Coliformes
TOALES");
Serial.print("\n");
tiempo = 0;
//inicializo los temporizadores de cada incubación


tiempo1 = 0;
//Apago los LEDs
digitalWrite(led1, LOW);
digitalWrite(led2, HIGH);
break;
case 2: //OLED DISPLAY
display.clear();
display.drawString(0, 0, String(menu) + " .Coliformes
Ecoli");
//SERIAL PRINT
Serial.print(menu); Serial.println(" .Coliformes
E.coli");
Serial.print("\n");
tiempo = 0;
//inicializo los temporizadores de cada incubación
tiempo1 = 0;
//Apago los LEDs
digitalWrite(led1, LOW);
digitalWrite(led2, HIGH);
break;
case 3: //OLED DISPLAY
display.clear();
display.drawString(0, 0, String(menu) + " Apagar
incubadora");
//SERIAL PIRNT
Serial.print(menu); Serial.println(" .Apagar
incubadora");
Serial.print("\n");
//inicializo los temporizadores de cada incubación
tiempo1 = 0;
//Apago los LEDs
digitalWrite(led1, LOW);
digitalWrite(led2, HIGH);
break;
}
while(!digitalRead(btn1));
}


if(lecturaBtn2 == LOW) {
//si se ha pulsado primero el botón 2
if(menu == 0 && estadoIncubadora == true){
//OLED DISLPLAY
display.clear();
display.drawString(0, 0, "Pulsa el botón 1");
Serial.print("\n");
//SERIAL PRINT
Serial.print("Pulsa el boton 1");
}
//si se ha pulsado el botón 2 en menu = 1
if(menu == 1 && estadoIncubadora == true){
//OLED DISPLAY
display.clear();
display.drawString(0, 20, "COLIFORMES");
display.drawString(0, 40, " TOTALES");
//SERIAL PRINT
Serial.println(" ANÁLISIS COLIFORMES TOTALES");
Serial.print("\n");
delay(2000);
tiempo = 1; //inicializo el timer de Blynk
testudio = 37;//la Temperatura de estudio en las coliformes
TOTALES en BLYNK
sendByBlynk();//imprimo valores por Blynk
}
//Enciendo LED VERDE
digitalWrite(led1, HIGH);
//si se ha pulsado el botón 2 en menu = 2
if(menu == 2 && estadoIncubadora == true){
//OLED DISPLAY
display.clear();
display.drawString(0, 0, "COLIFORMES E.COLI");
//SERIAL MONITOR
Serial.println(" ANÁLISIS COLIFORMES E.COLI");
Serial.print("\n");


delay(2000);
testudio = 44;//la Temperatura de estudio en las coliformes
E.COLI en BLYNK
tiempo = 1; //inicializo el timer de Blynk
sendByBlynk(); //imprimo valores por Blynk
}
//Enciendo LED ROJO
//digitalWrite(led2, HIGH);
//si se ha pulsado el botón 2 en menu = 3
if(menu == 3){
if(estadoIncubadora == true){
incubadoraApagada;
estadoIncubadora = false;//ya no permito qe entre en las otras
opciones del MENU
tiempo = 0;
} else{
//OLED DISPLAY
display.clear();
display.drawString(0, 0, "Incubadora apagada...");
//SERIAL MONITOR
Serial.print("Incubadora apagada..."); Serial.print("\n");
while(!digitalRead(btn2));
}
}
while(!digitalRead(btn2));
}
}

10.2.

