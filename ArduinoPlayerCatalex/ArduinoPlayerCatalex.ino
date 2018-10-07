

/*
Protocolo de comunicacion

tiempo de reproduccion itxyzf  x[0,9] ; y[0,9]; z[0,9]
maximo aleatorio       ihxyzf  x[0,9] ; y[0,9]; z[0,9]
minimo aleatorio       ilxyzf  x[0,9] ; y[0,9]; z[0,9]
cmbio de volumen       ivxyzf   x[0,3] ; y[0,9]
cabcion actual         icf
numero aleatorio       iaf
enviar pulsacion       ipf



*/
#include "Arduino.h"
#include "SerialMP3Player.h"
#include <TimerOne.h>
#include <EEPROM.h>

const int pulsador=2;
const int led=13;
double lapso=1000000; //microsegundos 
volatile int contador = 0;
volatile int contador1 = 0;
volatile int maximoTime = 60;
volatile bool valTime = false;
volatile int minimo = 0;
volatile int maximo = 0;
volatile bool tempo;
volatile bool estado=false;
volatile bool aux = false;
volatile int randNumber[2] = {1,2};
volatile int volumen=20;
volatile int eq = 0;
volatile char cadena[10];   //LONGITUD MAXIMA DE LA CADENA DE CARACTERES ES 100
volatile int x = 0;

volatile int addressVol=1;
volatile int addressMin=5;
volatile int addressMax = 6;
volatile int addressTime=3;

#define TX 11
#define RX 10
SerialMP3Player mp3(RX, TX);


void setup()
{
	analogReference(INTERNAL);
	randomSeed(analogRead(0));

	//INICIALIZACIÓN DE PINES
	pinMode(pulsador,INPUT_PULLUP);
	

	pinMode(led,OUTPUT);

	maximoTime = EEPROM.read(addressTime);




	Serial.begin(19200);
	mp3.begin(9600);        // start mp3-communication
	delay(500);             // wait for init

	mp3.sendCommand(CMD_SEL_DEV, 0, 2);   //select sd-card

  	Serial.println(F("Reproductor de audio aleatorio. by: Jonathan Vargas"));
  
	//----Set volume----
	volumen = EEPROM.read(addressVol);
	mp3.setVol(volumen);
	mp3.qVol();
  
	//set max canciones
	minimo = EEPROM.read(addressMin);
	maximo = EEPROM.read(addressMax);
	Serial.print("Tiempo de corte: ");
	Serial.println(maximoTime);
	Serial.print("Volumen: ");
	Serial.println(volumen);
	Serial.print("Minimo: ");
	Serial.println(minimo);
	Serial.print("Maximo: ");
	Serial.println(maximo);

	//mp3.play(99, volumen);
	attachInterrupt(0, pulso, FALLING); //activa interrupcion
	Timer1.initialize(lapso);  //esta en microsegundos
	Timer1.attachInterrupt(timerInt); // activa interrupcion del timer1
	delay(1000);
	mp3.play(12, volumen);
	digitalWrite(led, HIGH);
}

void loop()
{
	if (valTime == true) {
		//Serial.println("paso10");
		valTime = false;
		mp3.stop();
	}

	if (estado == true) {
		estado = false;
		contador = 0;
		mp3.play(randNumber[0], volumen);
	}
    
}


//
void timerInt(void) //esta calibrado para 1000000us
{
	if (contador >= maximoTime) {
		valTime = true;
		contador = 0;
	}
	else contador++;
}
///////////////////////////////////////////////////////////////////////////////////////////////
/////   PULSADOR

void pulso()
{
	delay(20);
	tempo=digitalRead(pulsador); //ANTIREBOTES

	if(tempo==LOW)
	{
		int ranTempo;
		do {
			ranTempo = random(minimo, maximo+1);
		} while (randNumber[0] == ranTempo || randNumber[1] == ranTempo);

		randNumber[0] = randNumber[1];
		randNumber[1] = ranTempo;

		
		//Serial.println(randNumber);
		estado = true;
	
	} 
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////SERIAL serialEvent
void serialEvent() {

  //se debe enviar la cada ejemplo 005 020  100
	if (Serial.available()) {
		// get the new byte: 
		// get the new byte: 
		char inChar = (char)Serial.read();
		if (inChar == 'f')   //SI EL DATO DE ENTRADA ES ENTER
		{
			cadena[x++] = '\0';  //indica fin de cadena
			x = 0;                 //se resetea el contador

			if (cadena[0] == 'i')    //inicia la comunicacion
			{
				if (cadena[1] == 'h') {
					int xx = cadena[2] - 48; //decima
					int yy = cadena[3] - 48; //unidad
					int zz = cadena[4] - 48; //unidad

					if ((xx >= 0 && xx <= 9) && (yy >= 0 || yy <= 9) && (zz >= 0 || zz <= 9)) {
						int tempo = xx * 100 + yy * 10 + zz;
						EEPROM.write(addressMax, tempo);
						Serial.print("Maximo aleatorio: ");
						Serial.println(tempo);
						maximo = tempo;
					}
					else Serial.print("Valor incorrecto!!");
				}
				if (cadena[1] == 'l') {
					int xx = cadena[2] - 48; //decima
					int yy = cadena[3] - 48; //unidad
					int zz = cadena[4] - 48; //unidad

					if ((xx >= 0 && xx <= 9) && (yy >= 0 || yy <= 9) && (zz >= 0 || zz <= 9)) {
						int tempo = xx * 100 + yy * 10 + zz;
						EEPROM.write(addressMin, tempo);
						Serial.print("Minimo aleatorio: ");
						Serial.println(tempo);
						minimo = tempo;
					}
					else Serial.print("Valor incorrecto!!");
				}
				if (cadena[1]=='t') {
					int xx = cadena[2] - 48; //decima
					int yy = cadena[3] - 48; //unidad
					int zz = cadena[4] - 48; //unidad

					if ((xx >= 0 && xx<=3)&&(yy >= 0 || yy <= 9)&&(zz >= 0 || zz <= 9)) {
						int tempo = xx * 100 + yy * 10 + zz;
						EEPROM.write(addressTime, tempo);
						Serial.print("Cambio de tiempo: ");
						Serial.print(tempo);
						Serial.println("segundos");
						maximoTime = tempo;
					}
					else Serial.print("Valor incorrecto!!");
				}
				else if (cadena[1] == 'v'){
					int xxx = cadena[2] - '0'; //decima
					int yyy = cadena[3] - '0'; //unidad
					if ((xxx >= 0 && xxx <= 3) && (yyy >= 0 || yyy <= 9)) {
						int tem = xxx * 10 + yyy;
						if (tem>30) tem = 30;
						EEPROM.write(addressVol, tem);
						Serial.print("Cambio de volumen: ");
						Serial.println(tem);
						mp3.setVol(tem);
						mp3.qVol();
						volumen = tem;
					}
					else Serial.print("Valor incorrecto!!");
				}
				else if (cadena[1] == 'c'){
					Serial.println("Canción actual: ");
					mp3.qPlaying();

				}
				else if (cadena[1] == 'a'){
					Serial.print("Número aleatorio: ");
					Serial.println(randNumber[0]);

				}
				else if (cadena[1] == 'p'){
					int ranTempo;
					do {
						ranTempo = random(minimo, maximo + 1);
					} while (randNumber[0] == ranTempo || randNumber[1] == ranTempo);

					randNumber[0] = randNumber[1];
					randNumber[1] = ranTempo;

					Serial.println(randNumber[0]);
					estado = true;
				}
				else if (cadena[1] == 'n') {
					mp3.qTTracks();
					Serial.println(mp3.numcanciones);
				}
			}
			else Serial.println("protocolo equivocado!!");
		}
		else
		{
			cadena[x++] = inChar;   //SE GUARDA EL DATO EN UNA POSICION DEL VECTOR
		}
	   
	}
}
