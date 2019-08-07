
/*************************************************************
 *
 * $Workfile: DFPlayer_Keypad.ino $
 *
 * $Creator: Jonathan Vargas $
 *
 * $Description:  $
 *************************************************************/

//! Libraries included
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include "Keypad.h"


//! Keypad keys definition
const byte ROWS = 4; // four rows
const byte COLS = 3; // three columns
char keys[ROWS][COLS] =
{
{'1','2','3' },
{'4','5','6' },
{'7','8','9' },
{'*','0','#' }
};

byte colPins[COLS] = {6, 12, 3}; //{col0,col1,col2,} -> {pin6, pin12, pin3}
byte rowPins[ROWS] = {7, 2, 4, 5}; //{row0,row1,row2,row3} -> {pin7, pin2, pin4, pin5}
/* //Membarne keypad
  byte colPins[COLS] = {4, 3, 2};
  byte rowPins[ROWS] = {12, 7, 6, 5};
*/
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//! DFPlayer definitions
SoftwareSerial serialPortOnce(8, 9); // RX, TX
SoftwareSerial serialPortLoop(10, 11); // RX, TX
DFRobotDFPlayerMini playerOnce;
DFRobotDFPlayerMini playerLoop;

//! variables
volatile int volumen=28; //Set volume value (0~30).
byte memory = 0;
 
/***************************************************************************
 *
 * setup Function
 *
 ***************************************************************************/
void setup() {

  Serial.begin(19200);
  serialPortOnce.begin(9600);
  serialPortLoop.begin(9600);

  delay(1000);

  if (!playerOnce.begin(serialPortOnce)) {  //Use softwareSerial to communicate with mp3.
  //  Serial.println(F("No conection with playerOnce:"));
  //  while(!playerOnce.begin(serialPortOnce));
	}

  if (!playerLoop.begin(serialPortLoop)) {  //Use softwareSerial to communicate with mp3.
  //  Serial.println(F("No conection with  playerLoop:"));
	//  while(!playerLoop.begin(serialPortLoop));
	}

  playerOnce.volume(volumen);  //Set volume value (0~30).
  playerLoop.volume(volumen);  //Set volume value (0~30).

  //! Set equalizer
  /*
    EQ(DFPLAYER_EQ_NORMAL);
    EQ(DFPLAYER_EQ_POP);
    EQ(DFPLAYER_EQ_ROCK);
    EQ(DFPLAYER_EQ_JAZZ);
    EQ(DFPLAYER_EQ_CLASSIC);
    EQ(DFPLAYER_EQ_BASS);
    EQ(DFPLAYER_EQ_NORMAL);
  */
  playerOnce.EQ(DFPLAYER_EQ_NORMAL);
  playerLoop.EQ(DFPLAYER_EQ_NORMAL);

	//! Set device we use SD as default
	playerOnce.outputDevice(DFPLAYER_DEVICE_SD);
  playerLoop.outputDevice(DFPLAYER_DEVICE_SD);



	Serial.println(F("DFPlayer Conectado."));

}
/***************************************************************************
 *
 * Loop Function
 *
 ***************************************************************************/
void loop() {
  char key = keypad.getKey();
  if (key) playKey(key);
  delay(100);
}

/***************************************************************************
 *
 * playKey Function
 *
 ***************************************************************************/
//! Maps key with play song
void playKey(char key) {
  switch (key)
  {
  case '1':
    playerOnce.play(1); 
    break;
  case '2':
    playerOnce.play(2); 
    break;
  case '3':  
    playerOnce.play(3); 
    break;
  case '4':   
    playerOnce.play(4); 
    break;
  case '5':    
    playerOnce.play(5); 
    break;
  case '6':    
    playerOnce.play(6);     
    break;
  case '7':    
    playerOnce.play(7); 
    break;
  case '8':
    playerOnce.play(8); 
    break;
  case '9':
    playerOnce.play(9); 
    break;
  case '*':
    if(memory==1){
      playerLoop.stop(); 
      memory = 0;
    }
    else {
      playerLoop.loop(1);
      memory = 1;
    }
    break;
  case '0':
    if(memory==2){
      playerLoop.stop();
      memory = 0;
    }
    else {
      playerLoop.loop(2);
      memory = 2;
    }
    break;
  case '#':
    if(memory==3){
      playerLoop.stop();
      memory = 0;
    }
    else {
      playerLoop.loop(3);
      memory = 3;
    }

    break;
  default:
    break;
  }

  Serial.println(key);
}
