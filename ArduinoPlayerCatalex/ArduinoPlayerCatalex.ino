

/*
Protocolo de comunicacion

tiempo de reproduccion itxyzf  x[0,9] ; y[0,9]; z[0,9]
maximo aleatorio       ihxyzf  x[0,9] ; y[0,9]; z[0,9]
minimo aleatorio       ilxyzf  x[0,9] ; y[0,9]; z[0,9]
initial song		   iixyzf   x[0,9] ; y[0,9]; z[0,9]
cambio de volumen      ivxyf   x[0,3] ; y[0,9]
play song		       irxyzf   x[0,9] ; y[0,9]; z[0,9]
stop song			   isf
numero aleatorio       iaf
enviar pulsacion       ipf



*/
#include "Arduino.h"
#include "SerialMP3Player.h"
#include <TimerOne.h>
#include <EEPROM.h>

const int pulsador=2;
const int led=13;
double lapso=10000; //microsegundos = 10ms
volatile int contador = 0;
volatile int contador1 = 0;
volatile int contador2 = 0;
volatile int maximoTime = 60;
volatile bool valTime = false;
volatile int minimo = 0;
volatile int maximo = 0;
volatile bool tempo;
volatile bool estado=false;
volatile bool aux = false;
volatile int randNumber[40];
volatile int elementos;
volatile int volumen=20;
volatile int eq = 0;
volatile char cadena[10];   //LONGITUD MAXIMA DE LA CADENA DE CARACTERES ES 100
volatile int x = 0;

volatile int addressVol=1;
volatile int addressMin=5;
volatile int addressMax = 6;
volatile int addressTime=3;
volatile int addressInitial = 8;

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

  	Serial.println("Reproductor de audio aleatorio.");
	Serial.println("Desarrollado por: Jonathan Vargas");
  
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

	elementos = (int) round((maximo-minimo+1) / 5);

	Serial.print("Filtro: ");
	Serial.println(elementos);
	
	contador2 = minimo;
	attachInterrupt(0, pulso, FALLING); //activa interrupcion
	Timer1.initialize(lapso);  //esta en microsegundos
	Timer1.attachInterrupt(timerInt); // activa interrupcion del timer1
	delay(1000);
	volatile int num = 2;

	int init = EEPROM.read(addressInitial);
	Serial.print("Cancion inicial: ");
	Serial.println(init);
	mp3.play(init, volumen);
	digitalWrite(led, HIGH);
}

void loop()
{
	if (valTime == true) {
		valTime = false;
		mp3.stop();
	}

	if (estado == true) {
		estado = false;
		contador = 0;
		mp3.play(randNumber[0], volumen);
		delay(3000);
		attachInterrupt(0, pulso, FALLING); //activa interrupcion
	}
    
}


//
void timerInt(void) //esta calibrado para 1000000us
{
	if (contador2 >= maximo) {
		contador2 = minimo;
	}
	else contador2++;
	/*
	if (aux == false) {
		if (contador2 >= maximo) {
			contador2 = minimo;
			int x=random(1,10+1);
			if (x <= 5) aux == false;
			else aux == true;
		} 
		else contador2++;
	}
	else {
		if (contador2 <= minimo) {
			contador2 = maximo;
			int x = random(1, 10 + 1);
			if (x <= 5) aux == true;
			else aux == false;
		}
		else contador2--;
	}
	*/
	if (contador1 >= 100) { //ya 1 segundo
		contador1 = 0;
		if (contador >= maximoTime) {
			valTime = true;
			contador = 0;
		}
		else contador++;

	}
	else contador1++;

}
///////////////////////////////////////////////////////////////////////////////////////////////
/////   PULSADOR

void pulso()
{
	delay(20);
	tempo=digitalRead(pulsador); //ANTIREBOTES

	if(tempo==LOW)
	{
		int ranTempo = contador2;
		bool coincidencia = false;
		do {
			coincidencia = true;
			for (int i = 0; i < elementos; i++) {
				if (randNumber[i] == ranTempo) {
					coincidencia = false;
				}
			}
			if (coincidencia == false) ranTempo = random(minimo, maximo + 1);
		} while (coincidencia == false);

		for (int i = elementos; i >= 0; --i) {
			randNumber[i] = randNumber[i - 1];

		}
		randNumber[0] = ranTempo;
		Serial.print("ALEATORIO: ");
		Serial.println(randNumber[0]);
		estado = true;
		detachInterrupt(0);
	
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
				else if (cadena[1] == 'l') {
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
				else if (cadena[1]=='i') {
					int xx = cadena[2] - 48; //decima
					int yy = cadena[3] - 48; //unidad
					int zz = cadena[4] - 48; //unidad

					if ((xx >= 0 && xx<=3)&&(yy >= 0 || yy <= 9)&&(zz >= 0 || zz <= 9)) {
						int tempo = xx * 100 + yy * 10 + zz;
						EEPROM.write(addressInitial, tempo);
						Serial.print("Cancion inicial: ");
						Serial.println(tempo);
					}
					else Serial.print("Valor incorrecto!!");
				}
				else if (cadena[1] == 't') {
					int xx = cadena[2] - 48; //decima
					int yy = cadena[3] - 48; //unidad
					int zz = cadena[4] - 48; //unidad

					if ((xx >= 0 && xx <= 3) && (yy >= 0 || yy <= 9) && (zz >= 0 || zz <= 9)) {
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
				else if (cadena[1] == 's'){
					Serial.println("Canción detenida!! ");
					mp3.stop();

				}
				else if (cadena[1] == 'a'){
					Serial.print("Número aleatorio: ");
					Serial.println(randNumber[0]);

				}
				else if (cadena[1] == 'p'){
					int ranTempo = contador2;
					bool coincidencia=false;
					while (coincidencia == false) {
						coincidencia = true;
						for (int i = 0; i < elementos; i++) {
							if (randNumber[i] == ranTempo) {
								coincidencia = false;
							}
						}
						if (coincidencia == false) ranTempo = random(minimo, maximo + 1);
					}

					for (int i = elementos; i >=0; --i) {
						randNumber[i] = randNumber[i-1];
						
					}
					randNumber[0] = ranTempo;

					for (int i = 0; i < elementos; i++) {
						Serial.print(randNumber[i]);
						Serial.print(',');
					}

					Serial.print("contador2: ");
					Serial.println(contador2);
					Serial.print("aleatorio: ");
					Serial.println(randNumber[0]);
					estado = true;
				}
				else if (cadena[1] == 'r') {
					int xx = cadena[2] - 48; //decima
					int yy = cadena[3] - 48; //unidad
					int zz = cadena[4] - 48; //unidad

					if ((xx >= 0 && xx <= 9) && (yy >= 0 || yy <= 9) && (zz >= 0 || zz <= 9)) {
						int tempo = xx * 100 + yy * 10 + zz;
						mp3.play(tempo, volumen);
					}
					else Serial.print("Valor incorrecto!!");
					
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
