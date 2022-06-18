// IMPORTACIONES
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <EEPROM.h>
#include "countdown.h"

// INSTANCIAS
LiquidCrystal_I2C lcd(0x27,16,2);
Countdown cuentaApertura("Apertura");
Countdown cuentaCierre("Cierre");

// VARIABLES GLOBALES
byte DT = 2, CLK = 4, SW = 3, rele = A0;
byte valor[7] = {0,0,0,0,0,0,0};
byte indice = 0;
bool cambio = false;
unsigned int debounceTime = 10;
bool programmed = false;
unsigned long triggeredEncoderTime = 0;
unsigned long triggeredPulsanteTime = 0;

// FUNCIONES
void(* resetFunc) (void) = 0;

void pulsante()
{
  if((millis() - triggeredPulsanteTime) > (debounceTime*25))
  {
    triggeredPulsanteTime = millis();
    if(!programmed)
    {
      indice++;
      if(indice > 6)
      {
        indice = 0;
      }
    }
    else
    {
      digitalWrite(rele,1);
      resetFunc();
    }
  }
}

void encoder()
{
  if((millis() - triggeredEncoderTime) > debounceTime)
  {
		cambio = true;
    triggeredEncoderTime = millis();
    boolean clkState = digitalRead(CLK);
    byte limite = (indice == 3) | (indice == 0) ? 23 : (indice == 6 ? 1 : 59);
    if(clkState)
    {
      valor[indice]++;
      if(valor[indice]>limite)
      {
        valor[indice]=0;
      }
    }
    else
    {
      valor[indice]--;
      if(valor[indice]>limite)
      {
        valor[indice]=limite;
      }
    }
  }
}

void pantallaPrincipal()
{
  lcd.clear();     
  lcd.setCursor(0, 0); 
  lcd.print("T1>00:00:00");
  lcd.setCursor(3,0);
  lcd.print(valor[0]<=9 ? "0"+String(valor[0]) : valor[0]);
  lcd.setCursor(6,0);
  lcd.print(valor[1]<=9 ? "0"+String(valor[1]) : valor[1]);
  lcd.setCursor(9,0);
  lcd.print(valor[2]<=9 ? "0"+String(valor[2]) : valor[2]);
  lcd.setCursor(0, 1);
  lcd.print("T2>00:00:00");
  lcd.setCursor(3,1);
  lcd.print(valor[3]<=9 ? "0"+String(valor[3]) : valor[3]);
  lcd.setCursor(6,1);
  lcd.print(valor[4]<=9 ? "0"+String(valor[4]) : valor[4]);
  lcd.setCursor(9,1);
  lcd.print(valor[5]<=9 ? "0"+String(valor[5]) : valor[5]);
  lcd.setCursor(13,0);
  lcd.print("M>");
  lcd.print("C");
}

void actualizarTiempo()
{
  lcd.setCursor(3,0);
  lcd.print(cuentaApertura.hours<=9 ? "0"+String(cuentaApertura.hours) : cuentaApertura.hours);
  lcd.setCursor(6,0);
  lcd.print(cuentaApertura.minutes<=9 ? "0"+String(cuentaApertura.minutes) : cuentaApertura.minutes);
  lcd.setCursor(9,0);
  lcd.print(cuentaApertura.seconds<=9 ? "0"+String(cuentaApertura.seconds) : cuentaApertura.seconds);
  lcd.setCursor(3,1);
  lcd.print(cuentaCierre.hours<=9 ? "0"+String(cuentaCierre.hours) : cuentaCierre.hours);
  lcd.setCursor(6,1);
  lcd.print(cuentaCierre.minutes<=9 ? "0"+String(cuentaCierre.minutes) : cuentaCierre.minutes);
  lcd.setCursor(9,1);
  lcd.print(cuentaCierre.seconds<=9 ? "0"+String(cuentaCierre.seconds) : cuentaCierre.seconds);
  lcd.setCursor(13,1);
  lcd.print("V>");
  lcd.print((digitalRead(rele))?0:1);
}

void actualizarMenu()
{
  lcd.setCursor(3,0);
  lcd.print(valor[0]<=9 ? "0"+String(valor[0]) : valor[0]);
  lcd.setCursor(6,0);
  lcd.print(valor[1]<=9 ? "0"+String(valor[1]) : valor[1]);
  lcd.setCursor(9,0);
  lcd.print(valor[2]<=9 ? "0"+String(valor[2]) : valor[2]);
  lcd.setCursor(3,1);
  lcd.print(valor[3]<=9 ? "0"+String(valor[3]) : valor[3]);
  lcd.setCursor(6,1);
  lcd.print(valor[4]<=9 ? "0"+String(valor[4]) : valor[4]);
  lcd.setCursor(9,1);
  lcd.print(valor[5]<=9 ? "0"+String(valor[5]) : valor[5]);
	lcd.setCursor(15,0);
  lcd.print(valor[6] == 0 ? "C": "I");
}

bool menu()
{
  while((indice != 0) | (valor[6] != 1))
  {
		lcd.cursor();
		lcd.blink();
		switch(indice)
		{
			case 0: lcd.setCursor(4,0); break;
			case 1: lcd.setCursor(7,0); break;
			case 2: lcd.setCursor(10,0); break;
			case 3: lcd.setCursor(4,1); break;
			case 4: lcd.setCursor(7,1); break;
			case 5: lcd.setCursor(10,1); break;
			case 6: lcd.setCursor(15,0); break;
		}
		if(cambio)
		{
			cambio = false;
			actualizarMenu();
		}
  }
  for(int i=0; i < 6; i++)
  {
    EEPROM.put(i,valor[i]);
  }
  EEPROM.put(6,6);
  return true;
}

void setup() 
{
  Serial.begin(9600);

  pinMode(rele, OUTPUT);
  digitalWrite(rele,1);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(DT), encoder, LOW);
  attachInterrupt(digitalPinToInterrupt(SW), pulsante,FALLING);

  for(int i=0; i<6; i++)
  {
    EEPROM.get(i, valor[i]);
		valor[i] = valor[i]>59 ? 0 : valor[i];
  }
  EEPROM.get(6, indice);
	indice = indice>6 ? 0 : indice;

  lcd.init();
  lcd.backlight();

  pantallaPrincipal();

  programmed = menu();
  
  lcd.setCursor(15,0);
  lcd.println("F");

  cuentaApertura.setCountdown(valor[0],valor[1],valor[2]);
  cuentaCierre.setCountdown(valor[3],valor[4],valor[5]);
}

void loop() 
{
  if(cuentaApertura.startCount())
  {
    digitalWrite(rele,0);
    if(cuentaCierre.startCount())
    {
      digitalWrite(rele,1);
      cuentaApertura.restart();
      cuentaCierre.restart();    
    }
  }
  actualizarTiempo();
}