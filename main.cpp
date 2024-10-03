/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h" 
#include <cstring>

#define tiempo_muestreo   1s
// Dirección del TMP102
#define TMP102_ADDRESS 0x90

//Pines y puertos 
BufferedSerial serial(USBTX, USBRX);
I2C i2c (D14,D15);
Adafruit_SSD1306_I2c oled (i2c, D0);
AnalogIn ain(A0);

// Variables globales 
float Vin=0.0;
int ent=0;
int dec=0;
char men[40];

char comando[3]= {0x01, 0x60,0xA0};
char data[2];
const char *mensaje_inicio = "Arranque del programa\n\r";
// Funciones 

void message(void);
void screen(void);
void start(void);
void formula(void);
void clean(void);
void hello(void);
void voltage(void);
void temperatura(void);
void form(void);
void time_temp(void);
void registro(void);



void time(void){
    ThisThread::sleep_for(3000ms);
}

void time_temp(void)
{
    ThisThread::sleep_for(2000ms);
}

int main()
{ 
    hello(); //Saludo OLED
    start(); // Edita la pantalla
    time(); // Espera por 3 segundos
    screen(); // Inicia Pantalla
    

    //arranque del sensor de temperatura
    i2c.write(TMP102_ADDRESS, comando, 3);

    while (true) {
        // Lectura del sensor analogo (Potenciometro)
        formula(); //Ecuacion para calcular el voltage y mostrarlo en la OLED

        clean(); // Limpia la Oled

        voltage(); //Volage Mostrado en la pocision (0,2) OLED

        // impresion puerto serie
        serial.write(men, strlen(men));

        //Lectura sensor I2C

        registro(); //Envia, guarda y Lee el sensor De Temperatura
        
        form(); //Desplaza los bits mas significativos y calcula la temperatura en grados

        time_temp();//Tiempo de espera

        clean();//Limpia la pantalla oled

        temperatura();//Imprime las variables ent y dec en la oled
        
        // impresion puerto serie
        serial.write(men, strlen(men));        

        ThisThread::sleep_for(tiempo_muestreo);
    }
}

void hello(void)
{
    // saludo por el puerto serial
    serial.write(mensaje_inicio, strlen(mensaje_inicio));
}

void start(void)
{
    oled.begin();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.display();
}

void clean(void)
{
    oled.clearDisplay();
    oled.display();
}

void formula(void)
{
    Vin = ain*3.3;
    ent = int(Vin); 
    dec = int((Vin-ent)*10000); 
}

void voltage(void)
{
    sprintf(men, "El voltaje es:\n\r %01u.%04u volts \n\r", ent, dec);
    oled.setTextCursor(0,2);
    oled.printf(men);
    oled.display();
}

void registro(void)
{
    comando[0] = 0; // Accede al Registro 0x00
    i2c.write(TMP102_ADDRESS, comando, 1); // Enviar al registro 0x90 
    //(Sensor Temp)

    i2c.read(TMP102_ADDRESS, data, 2); // Leer el sensor y lo guarda variable data
}

void form(void)
{
    int16_t temp = (data[0] << 4) | (data[1] >> 4); //Obtiene los bits mas significativos

    float Temperatura = temp *0.0625;  //Formula para calcular la temperatura
    ent = int(Temperatura); 
    dec = int((Temperatura-ent)*10000); 
}

void temperatura()
{
    sprintf(men, "La Temperatura es:\n\r %01u.%04u Celsius\n\r", ent, dec);
    oled.setTextCursor(0,2);
    oled.printf(men);
    oled.display();
}
