/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/index.php?route=product/product&product_id=1121>
 
 ***************************************************
 This example shows the basic function of library for DFPlayer.
 
 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/


/*
Protocolo de comunicacion

tiempo de reproduccion itxyzf  x[0,9] ; y[0,9]; z[0,9]
cmbio de volumen       ivxyf   x[0,3] ; y[0,9]
cambio de equalizador  iexf    x[0,5]
cabcion actual         icf
numero aleatorio       iaf
enviar pulsacion       ipf



*/
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <TimerOne.h>
#include <EEPROM.h>

const int pulsador=2;
const int led=13;
double lapso=1000000; //microsegundos 
volatile int contador = 0;
volatile int maximoTime = 60;
volatile bool valTime = false;
volatile int maximo = 0;
volatile bool tempo;
volatile bool estado=false;
volatile int randNumber;
volatile int volumen=20;
volatile int eq = 0;
volatile char cadena[10];   //LONGITUD MAXIMA DE LA CADENA DE CARACTERES ES 100
volatile int x = 0;

volatile int addressVol=1;
volatile int addressEq=2;
volatile int addressTime=3;


SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup()
{
	analogReference(INTERNAL);
	randomSeed(analogRead(0));

	//INICIALIZACIÓN DE PINES
	pinMode(pulsador,INPUT_PULLUP);
	attachInterrupt(0, pulso, FALLING);
	pinMode(led,OUTPUT);

	maximoTime = EEPROM.read(addressTime);


	Timer1.initialize(lapso);  //esta en microsegundos
	Timer1.attachInterrupt(timerInt); // activa interrupcion del timer1


	mySoftwareSerial.begin(9600);
	Serial.begin(19200);
  
	Serial.println();
	Serial.println(F("Reproductor de audio aleatorio. by: Jonathan Vargas"));
  
	digitalWrite(led, HIGH);

	if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
	Serial.println(F("No se ha podido iniciar:"));
	Serial.println(F("1.Revise la conexión!"));
	Serial.println(F("2.Inserte la memoria SD!"));
	while(!myDFPlayer.begin(mySoftwareSerial));
	}
	Serial.println(F("DFPlayer Conectado."));
  
	//----Set volume----
	volumen = EEPROM.read(addressVol);
	myDFPlayer.volume(volumen);  //Set volume value (0~30).
//  myDFPlayer.volumeUp(); //Volume Up
//  myDFPlayer.volumeDown(); //Volume Down

	//----Set different EQ----
	eq = EEPROM.read(addressEq);
	switch (eq)
	{
		case 0:
			myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
			break;
		case 1:
			myDFPlayer.EQ(DFPLAYER_EQ_POP);
			break;
		case 2:
			myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
			break;
		case 3:
			myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
			break;
		case 4:
			myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
			break;
		case 5:
			myDFPlayer.EQ(DFPLAYER_EQ_BASS);
			break;
		default:
			myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
			break;
	}
  
	//----Set device we use SD as default----
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
	myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);

	//----Mp3 control----
//  myDFPlayer.sleep();     //sleep
//  myDFPlayer.reset();     //Reset the module
//  myDFPlayer.enableDAC();  //Enable On-chip DAC
//  myDFPlayer.disableDAC();  //Disable On-chip DAC
//  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15


	//----Mp3 play----
	/*
	myDFPlayer.next();  //Play next mp3
	delay(1000);
	myDFPlayer.previous();  //Play previous mp3
	delay(1000);
	myDFPlayer.play(1);  //Play the first mp3
	delay(1000);
	myDFPlayer.loop(1);  //Loop the first mp3
	delay(1000);
	myDFPlayer.pause();  //pause the mp3
	delay(1000);
	myDFPlayer.start();  //start the mp3 from the pause
	delay(1000);
	myDFPlayer.playFolder(15, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
	delay(1000);
	myDFPlayer.enableLoopAll(); //loop all mp3 files.
	delay(1000);
	myDFPlayer.disableLoopAll(); //stop loop all mp3 files.
	delay(1000);
	myDFPlayer.playMp3Folder(4); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
	delay(1000);
	myDFPlayer.advertise(3); //advertise specific mp3 in SD:/ADVERT/0003.mp3; File Name(0~65535)
	delay(1000);
	myDFPlayer.stopAdvertise(); //stop advertise
	delay(1000);
	myDFPlayer.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
	delay(1000);
	myDFPlayer.loopFolder(5); //loop all mp3 files in folder SD:/05.
	delay(1000);
	myDFPlayer.randomAll(); //Random play all the mp3.
	delay(1000);
	myDFPlayer.enableLoop(); //enable loop.
	delay(1000);
	myDFPlayer.disableLoop(); //disable loop.
	delay(1000);
	*/


	//----Read imformation----
	/*   
	Serial.print("readState: ");
	Serial.println(myDFPlayer.readState()); //read mp3 state
	Serial.print("readVolume: ");
	Serial.println(myDFPlayer.readVolume()); //read current volume
	Serial.print("readEQ: ");
	Serial.println(myDFPlayer.readEQ()); //read EQ setting
	Serial.print("readFileCounts: ");
	Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
	Serial.print("readCurrentFileNumber: ");
	Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
	Serial.print("readFileCountsInFolder: ");
	Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
*/
	Serial.print("Numero de canciones: ");
	Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card

	maximo = myDFPlayer.readFileCounts();

	
	Serial.print("Tiempo de corte: ");
	Serial.println(maximoTime);
	Serial.print("Volumen: ");
	Serial.println(volumen);
	Serial.print("Ecualizador: ");
	Serial.println(eq);

}

void loop()
{	

	if (valTime == true) {
		//Serial.println("paso10");
		valTime = false;
		myDFPlayer.stop();
	}
	printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.

	if (estado == true) {
		estado = false;
		contador = 0;
		myDFPlayer.play(randNumber);  //Play the first mp3											
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
	delay(10);
	tempo=digitalRead(pulsador); //ANTIREBOTES

	if(tempo==LOW)
	{
		int ranTempo = randNumber;
		do {
			randNumber = random(1, maximo);
		} while (randNumber == ranTempo);
		
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
				if (cadena[1]=='t') {
					int xx = cadena[2] - 48; //decima
					int yy = cadena[3] - 48; //unidad
					int zz = cadena[4] - 48; //unidad

					if ((xx >= 0 && xx<=3)&&(yy >= 0 || yy <= 9)&&(zz >= 0 || zz <= 9)) {
						int tempo = xx * 100 + yy * 10 + zz;
						EEPROM.write(addressTime, tempo);
						Serial.print("Cambio de tiempo");
						Serial.println(tempo);
					}
					else Serial.print("Valor incorrecto!!");
					

				}
				else if (cadena[1] == 'v'){
					int xxx = cadena[2] - 48; //decima
					int yyy = cadena[3] - 48; //unidad
					if ((xxx >= 0 && xxx <= 3) && (yyy >= 0 || yyy <= 9)) {
						int tem = xxx * 10 + yyy;
						if (tem>30) tem = 30;
						EEPROM.write(addressVol, tem);
						Serial.print("Cambio de volumen: ");
						Serial.println(tem);
					}
					else Serial.print("Valor incorrecto!!");
				}
				else if(cadena[1]=='e'){
					switch (cadena[2])
					{
					case '0':
						EEPROM.write(addressEq, 0);
						Serial.print("Cambio de valor de equalizador: ");
						Serial.println("DFPLAYER_EQ_NORMAL");
						break;
					case '1':
						EEPROM.write(addressEq, 1);
						Serial.print("Cambio de valor de equalizador: ");
						Serial.println("DFPLAYER_EQ_POP");
						break;
					case '2':
						EEPROM.write(addressEq, 2);
						Serial.print("Cambio de valor de equalizador: ");
						Serial.println("DFPLAYER_EQ_ROCK");
						break;
					case '3':
						EEPROM.write(addressEq, 3);
						Serial.print("Cambio de valor de equalizador: ");
						Serial.println("DFPLAYER_EQ_JAZZ");
						break;
					case '4':
						EEPROM.write(addressEq, 4);
						Serial.print("Cambio de valor de equalizador: ");
						Serial.println("DFPLAYER_EQ_CLASSIC");
						break;
					case '5':
						EEPROM.write(addressEq, 5);
						Serial.print("Cambio de valor de equalizador: ");
						Serial.println("DFPLAYER_EQ_BASS");
						break;
					}
				}
				else if (cadena[1] == 'c'){
					Serial.print("Canción actual: ");
					Serial.println(myDFPlayer.readCurrentFileNumber());

				}
				else if (cadena[1] == 'a'){
					Serial.print("Número aleatorio: ");
					Serial.println(randNumber);

				}
				else if (cadena[1] == 'p'){
					int ranTempo = randNumber;
					do {
						randNumber = random(1, maximo);
					} while (randNumber == ranTempo);

					Serial.println(randNumber);
					estado = true;
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


void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
