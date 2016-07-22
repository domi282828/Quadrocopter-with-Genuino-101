// Datum:     07.06.2016
// Projekt:   Project D
// Funktion:  Funkübertragung
//            Motoren steuern
//            Stabilisieren mit Gyro Sensor
//
//
//	Übersicht Befehle von Fernsteuerung:
//  ===================================
//	(0)		Wird immer gesendet wenn nichts auf der Fernsteuerung gedrückt wird
//	(1)		Links drehen
//	(2) 	Rechts drehen
//	(3)		Hoch
//	(4)		Runter
//	(5)		Links
//	(6)		Rechts
//	(7)		Vor
//	(8)		Zurück
//	(9)		Motoren wieder an 	
//	(11)	Motoren aus (Not aus)


#include <Servo.h>          // Motoren
#include <SPI.h>            // Funkmodul
#include <nRF24L01.h>       // Funkmodul
#include <RF24.h>           // Funkmodul
#include <CurieIMU.h>       // Gyro

//Gyro
int lastOrientation = - 1;

//Motoren
#define MOTOR_PIN_1 4       //Motor 1 ist an Pin 4 angeschlossen
#define MOTOR_PIN_2 5       //Motor 2 ist an Pin 5 angeschlossen
#define MOTOR_PIN_3 6       //Motor 3 ist an Pin 6 angeschlossen
#define MOTOR_PIN_4 7       //Motor 4 ist an Pin 7 angeschlossen

Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

//Funkmodul
#define CE_PIN   9
#define CSN_PIN 10
const uint64_t pipe = 0xE8E8F0F0E1LL;
RF24 radio(CE_PIN, CSN_PIN);

// Müssen vordefiniert werden
int vmotor1 = 1200;
int vmotor2 = 1240;
int vmotor3 = 1200;
int vmotor4 = 1200;
int vstandart= 1350;        // maximale Geschwindigkeit für Anfang; Darf noch nicht abheben
int vgyro = 5;              // Geschwindigkeitsänderung für Korrekturen
int vhoch = 50;             // Geschwindigkeitsänderung für steigen
int vrunter = 50;           // Geschwindigkeitsänderung für fallen
int vdrehen = 50;
int vrichtung = 50;
// Bis hier


void Motoreninitialisieren() {    //Motoren werden initialisiert
  motor1.attach(MOTOR_PIN_1);     //Motor 1 Pin 4 zuweisen
  motor1.writeMicroseconds(1000); //Geschwindigkeit auf 1000
  motor2.attach(MOTOR_PIN_2);
  motor2.writeMicroseconds(1000);
  motor3.attach(MOTOR_PIN_3);
  motor3.writeMicroseconds(1000);
  motor4.attach(MOTOR_PIN_4);
  motor4.writeMicroseconds(1000);
}

void Gyroinitialisieren() {       //Gyro wird initialisiert
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();
  CurieIMU.setAccelerometerRange(2);
  delay(500);
}

void Funkmodulinitialisieren() {  //Funkmodul wird initialisert
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}

int Befehlswerteinlesen() {       //Befehl der Fernsteuerung wird eingelesen und zurückgegeben
  int Befehl[1];
   radio.read( Befehl, sizeof(Befehl) );            //Befehlwert einlesen
  return Befehl[0];                                 //Rückgabe des Befehls
}

void Gyroeinlesen() {
  int orientation = - 1;
  String orientationString;
  int x = CurieIMU.readAccelerometer(X_AXIS);
  int y = CurieIMU.readAccelerometer(Y_AXIS);
  
   if(vmotor4 < 2000 && vmotor2 > 1200){			// Drehzahlen von Motoren zwischen 1200 und 2000 
     if (y <= -3000 || x <= -3000) {
      vmotor4 = vmotor4 + vgyro;
      vmotor2 = vmotor2 - vgyro;
     } 
   }
   if(vmotor1 < 2000 && vmotor3 > 1200){			// Drehzahlen von Motoren zwischen 1200 und 2000 
     if (y >= 2000 || x <= -3000) {
      vmotor1 = vmotor1 + vgyro;
      vmotor3 = vmotor3 - vgyro;
     } 
   }
   if(vmotor2 < 2000 && vmotor4 > 1200){			// Drehzahlen von Motoren zwischen 1200 und 2000 
     if (y >= 2000 || x >= 2000) {
      vmotor2 = vmotor2 + vgyro;
      vmotor4 = vmotor4 - vgyro;
     }
   }
   if(vmotor3 < 2000 && vmotor1 > 1200){			// Drehzahlen von Motoren zwischen 1200 und 2000 

     if (y <= -3000 || x >= 2000) {
      vmotor3 = vmotor3 + vgyro;
      vmotor1 = vmotor1 - vgyro;
     }
   }
}

void Befehlauswerten(int befehl) {                  // Befehlswert wird ausgewertet
  switch(befehl){
    case(1): Serial.println("Links drehen");        // Wird die Zahl 1 empfangen soll sich der Quadrocpoter gegen den Uhrzeigersinn drehen
       motor1.writeMicroseconds(vmotor1-vdrehen);   // Drehzahlen für Motor 1 um vdrehen verringern
       motor2.writeMicroseconds(vmotor2+vdrehen);	// Drehzahlen für Motor 2 um vdrehen erhöhen
       motor3.writeMicroseconds(vmotor3-vdrehen);	// Drehzahlen für Motor 3 um vdrehen verringern
       motor4.writeMicroseconds(vmotor4+vdrehen);	// Drehzahlen für Motor 4 um vdrehen erhöhen
       delay(200);                                  // 200ms warten und dann die Drehzahlen wieder zurücksetzten
       motor1.writeMicroseconds(vmotor1+vdrehen);	// Drehzahlen für Motor 1 um vdrehen erhöhen
       motor2.writeMicroseconds(vmotor2-vdrehen);	// Drehzahlen für Motor 2 um vdrehen verringern
       motor3.writeMicroseconds(vmotor3+vdrehen);	// Drehzahlen für Motor 3 um vdrehen erhöhen
       motor4.writeMicroseconds(vmotor4-vdrehen);	// Drehzahlen für Motor 4 um vdrehen verringern
       break;
    case(2): Serial.println("Rechts drehen");        // Wird die Zahl 2 empfangen soll sich der Quadrocpoter im Uhrzeigersinn drehen
       motor1.writeMicroseconds(vmotor1+vdrehen);
       motor2.writeMicroseconds(vmotor2-vdrehen);
       motor3.writeMicroseconds(vmotor3+vdrehen);
       motor4.writeMicroseconds(vmotor4-vdrehen);
       delay(200);
       motor1.writeMicroseconds(vmotor1-vdrehen);
       motor2.writeMicroseconds(vmotor2+vdrehen);
       motor3.writeMicroseconds(vmotor3-vdrehen);
       motor4.writeMicroseconds(vmotor4+vdrehen);
       break;
    case(3): Serial.println("Hoch");				  // Wird die Zahl 3 empfangen soll sich der Quadrocpoter nach oben fliegen
       motor1.writeMicroseconds(vmotor1+vhoch);
       motor2.writeMicroseconds(vmotor2+vhoch);
       motor3.writeMicroseconds(vmotor3+vhoch);
       motor4.writeMicroseconds(vmotor4+vhoch);
       delay(200);
       motor1.writeMicroseconds(vmotor1-vrunter);
       motor2.writeMicroseconds(vmotor2-vrunter);
       motor3.writeMicroseconds(vmotor3-vrunter);
       motor4.writeMicroseconds(vmotor4-vrunter);
       break;
    case(4): Serial.println("Runter");				  // Wird die Zahl 4 empfangen soll sich der Quadrocpoter nach unten fliegen
       motor1.writeMicroseconds(vmotor1-vrunter);
       motor2.writeMicroseconds(vmotor2-vrunter);
       motor3.writeMicroseconds(vmotor3-vrunter);
       motor4.writeMicroseconds(vmotor4-vrunter);
       delay(200);
       motor1.writeMicroseconds(vmotor1+vrunter);
       motor2.writeMicroseconds(vmotor2+vrunter);
       motor3.writeMicroseconds(vmotor3+vrunter);
       motor4.writeMicroseconds(vmotor4+vrunter);
       break;
    case(5): Serial.println("Links");				  // Wird die Zahl 5 empfangen soll sich der Quadrocpoter nach links fliegen
       motor1.writeMicroseconds(vmotor1+vrichtung);
       motor2.writeMicroseconds(vmotor2-vrichtung);
       motor3.writeMicroseconds(vmotor3-vrichtung);
       motor4.writeMicroseconds(vmotor4+vrichtung);
       delay(200);
       motor1.writeMicroseconds(vmotor1-vrichtung);
       motor2.writeMicroseconds(vmotor2+vrichtung);
       motor3.writeMicroseconds(vmotor3+vrichtung);
       motor4.writeMicroseconds(vmotor4-vrichtung);
       break;
    case(6): Serial.println("Rechts");				  // Wird die Zahl 6 empfangen soll sich der Quadrocpoter nach rechts fliegen
       motor1.writeMicroseconds(vmotor1-vrichtung);
       motor2.writeMicroseconds(vmotor2+vrichtung);
       motor3.writeMicroseconds(vmotor3+vrichtung);
       motor4.writeMicroseconds(vmotor4-vrichtung);
       delay(200);
       motor1.writeMicroseconds(vmotor1+vrichtung);
       motor2.writeMicroseconds(vmotor2-vrichtung);
       motor3.writeMicroseconds(vmotor3-vrichtung);
       motor4.writeMicroseconds(vmotor4+vrichtung);
       break;
    case(7): Serial.println("Vor");				  // Wird die Zahl 7 empfangen soll sich der Quadrocpoter nach vorne fliegen
       motor1.writeMicroseconds(vmotor1-vrichtung);
       motor2.writeMicroseconds(vmotor2-vrichtung);
       motor3.writeMicroseconds(vmotor3+vrichtung);
       motor4.writeMicroseconds(vmotor4+vrichtung);
       delay(200);
       motor1.writeMicroseconds(vmotor1+vrichtung);
       motor2.writeMicroseconds(vmotor2+vrichtung);
       motor3.writeMicroseconds(vmotor3-vrichtung);
       motor4.writeMicroseconds(vmotor4-vrichtung);
       break;
    case(8): Serial.println("Zurueck");				  // Wird die Zahl 8 empfangen soll sich der Quadrocpoter nach hinten fliegen
       motor1.writeMicroseconds(vmotor1+vrichtung);
       motor2.writeMicroseconds(vmotor2+vrichtung);
       motor3.writeMicroseconds(vmotor3-vrichtung);
       motor4.writeMicroseconds(vmotor4-vrichtung);
       delay(200);
       motor1.writeMicroseconds(vmotor1-vrichtung);
       motor2.writeMicroseconds(vmotor2-vrichtung);
       motor3.writeMicroseconds(vmotor3+vrichtung);
       motor4.writeMicroseconds(vmotor4+vrichtung);
       break;      
  }
}

void Motorenaktuell() {                   //Motoren drehen mit der aktuellen Geschwindigkeit
  motor1.writeMicroseconds(vmotor1);
  motor2.writeMicroseconds(vmotor2);
  motor3.writeMicroseconds(vmotor3);
  motor4.writeMicroseconds(vmotor4);
}

void setup(){
  int vaktuell = 1000;					// Variable für aktuelle Geschwindigkeit
  int wert[1];							// Varibale für Befehl
  pinMode(8, OUTPUT);					// LED an Pin 8
  delay(2000);							// 2 Sekunden warten bis GEnuino betreit ist
  digitalWrite(8, HIGH);				// LED an
  Funkmodulinitialisieren();			// Funkmodul initialisieren
  Motoreninitialisieren();				// Motoren initialisieren
  Gyroinitialisieren();					// Gyro initialisieren
  Serial.println("Motoren starten");	// Ausgabe für Serial Monitor
  while(vaktuell < vstandart) {         // Motor bis vstandart hochdrehen lassen
    vmotor1 = vaktuell;					
    vmotor2 = vaktuell;
    vmotor3 = vaktuell;
    vmotor4 = vaktuell;
    Motorenaktuell();
    vaktuell=vaktuell+1;				// Geschwindigkeiten immer um 1 erhöhen
    delay(20);							// 20ms warten
  }
   digitalWrite(8, LOW);				// LED aus
   Serial.println("Start Loop");		// Ausgabe
}

void loop(){
  int wert[1];							// Varibale für Befehl
  int x=0;								// Variable für Notaus-Schleife
  Motorenaktuell();						// Motoren mit aktuelle Geschwindkeit drehen
  if ( radio.available()) {				// Wird Signal empfangen 
    wert[0] = Befehlswerteinlesen();	// Befehlswert einlesen
    Serial.println(wert[0]);			// Ausgabe
    if (wert[0] == 11){					// Befehl gleich 11 -> Notaus
    motor1.writeMicroseconds(1000);		// Geschwinidkeitd auf 0 
    motor2.writeMicroseconds(1000);
    motor3.writeMicroseconds(1000);
    motor4.writeMicroseconds(1000);
    while(x == 0){						// Befehl gleich 9 -> Notaus wieder beenden
      wert[0] = Befehlswerteinlesen();	// Befehlswert einlesen
      delay(20);						// 20ms warten
      if (wert[0] == 9) {				// Varibale x auf 1 wenn eine 9 ankommt
        x = 1;							// Variable x auf 1
      }
    }
  } else{								 
    if (wert[0] == 0) {					// Wenn nichta auf der Fernsteuerung gedrückt wird aber ein Signal ankommt
      Gyroeinlesen();					// Stabilisieren mit Gyro 
      Serial.println("Gyro");			// Ausgabe
    } else {							// Wenn Siganl ankommt das nicht 0, 9 oder 11 ist
      Befehlauswerten(wert[0]);			// Befehl auswerten
    }
  }
  } else {								// wenn kein Signal empfangen wird
  Gyroeinlesen();						// Stabilisieren mit Gyro
  Serial.println("Gyro");				// Ausgabe 
  }
}

