#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include <MFRC522.h>

                       
//                                  ---------TECHMEUP PROJECT 1--------
//                                      ---The Simple Sexy---
//
//This is Chimpo Mcoodle's "Simple Sexy Code" for "The Simple Sexy" module. It uses a 12v - 5v Buck converters,
//an Arduino Nano,a Y5300 MP3 Player,a Mini RC522 4 channel relay module, and a 4 channel logic converter. When the correct
//tag or card or implant is applied to the reader. this code alows the user to set a series of lights, buzzers,
//mp3 or relay commands, alowing you RFID access control to your existing systems.


//This current example is set to alows access to a motorcycle using the RC522 ,logic converter, YX5200 and a 4 
//Channel Relay . it lights up the dash, waits 4 second and then activates the ignition for 4 seconds. with the 
//corisponding voice commands. please change the "void correctuid" and "void wronguid'' functions respectivley 
//to meet your specific requirements. 
//
//                                   --------- Library Required --------

//    RC522 Library -    https://github.com/miguelbalboa/rfid/archive/master.zip
//    Dfmini -           https://github.com/DFRobot/DFRobotDFPlayerMini/archive/master.zip

//                                   ------------- Pinouts ------------- 
//
// 
//     Arduino   |       MODULE     |  
//      Nano     |       PINOUT     |   
//-----------------------------------
//       GND     |        GND       |  
//       D2      |      LED/BUZ     |            
//       D3      |      RELAY 1     |       
//       D4      |      RELAY 2     |       
//       D5      |      RELAY 3     |       
//       D6      |      RELAY 4     |       
//       D7      |      MP3-RX      |        
//       D8      |      MP3-TX      |        
//       D9      |      RC522-RST   |                  
//       D10     |      RC522-SS    |                
//       D11     |      RC522-MOSI  |                 
//       D12     |      RC522- MISO |                       
//       D13     |      RC522-SCK   |                   
//       3.3V    |      RC522- 3.3v |             
//       GND     |                  |       
//       VIN     |                  |    
//-----------------------------------



//                             -----------Additional/Unused Pins------------

//A quick note about any modules you do not decide to use. these pins in conjunction with the break out pins
//allow the use of a wider range of modules, we encorage you to tinkey and combine new modules to this pcb, 
//please follow the pinout for the Arduino Nano. We would Love to see anything you make with this system.


//               -------Definitions- Delete the "//" of the modules you are using----------

//#define usingledbuz// unmark this if your using the pin 6 LED or Buzzer
//#define usingmp3player// unmark this if using the df mp3player
#define usingrc522 // unmark this if you are using the RC522 13.56 NFC-HF RFID reader
#define toggleMode //unmark this if you want the NFC chip to toggle 


//                                -------Add your Details here-------
String hfuid1 = "DE AD BE EF 00 13 37";// if using RC522 Put your UID here!
String hfuid2 ="Put spare UID here";// if using more than one uid for RC522, edit this
//                         ----------------------------------------------
//                                          Code Start
//                         ----------------------------------------------



SoftwareSerial mySoftwareSerial(8, 7); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#define SS_PIN 10 // The SS pin is connected to Pin 10
#define RST_PIN 9 // the RST pin is conected tp pin 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int relay1 = 6; // relay 1 dash 
int relay2 = 5; // relay 2 ignition 
int relay3 = 4; //relay 3 for mp3 player pops
int relay4 = 3; //relay 4 NC
int ledbuz = 2;// buzzer or led

bool toggleState = false;


void setup() 
{

  pinMode(relay1, OUTPUT);   // Declaring this Relay as output
  digitalWrite(relay1, HIGH); // setting it to OFF
  pinMode(relay2, OUTPUT);   //Declaring this Relay as output
  digitalWrite(relay2, HIGH); // setting it to OFF
 
 
  #if defined(usingledbuz)
  pinMode(ledbuz, OUTPUT);   //Declaring the buzzer or LED as output
  digitalWrite(ledbuz, LOW); // setting it to OFF
  #endif
  
  #if defined(usingmp3player)

  mySoftwareSerial.begin(9600);
  #endif
  
  Serial.begin(115200);   // Initiate a serial communication
  
   #if defined(usingmp3player)
        Serial.println();
        Serial.println(F(""));
          
        if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
            Serial.println(F("Unable to begin:"));
            Serial.println(F("1.Please recheck the connection!"));
            Serial.println(F("2.Please insert the SD card!"));
            while(true){
            delay(0); // Code to compatible with ESP8266 watch dog.
            }
  }
  Serial.println(F("mp3Player ready."));
 //myDFPlayer.volume(28);  //Set volume value. From 0 to 30
  myDFPlayer.play(2);  //Play the first mp3
   #endif

  #if defined(usingrc522)
    SPI.begin();      // Initiate  SPI bus
    mfrc522.PCD_Init();   // Initiate MFRC522
    //If you set Antenna Gain to Max it will increase reading distance
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    Serial.println("Approximate your UID to the reader...");
    Serial.println();
    #endif

    #if defined(usinghz1051)
 wg.begin();
 #endif
 
  #if defined(usingledbuz)
    digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
    delay(200);//
    digitalWrite(ledbuz, LOW); 
    #endif
  }
void loop() 
{
  #if defined(usingrc522)
    // Awaiting an UID to be Presented
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    }
    //Display UID on serial monitor
    Serial.print("UID tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
     Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    if (content.substring(1)==(hfuid1) || content.substring(1)==(hfuid2))  
    //change here the UID of the cards to give access, add more with  || content.substring(1)== "another UID"
    {correctuid();}
   else  
    {wronguid();}
  } 
 #endif

void correctuid(){ // If UID is correct do this ....

Serial.println("Authorized access Motorcycle Engaged");
   
      digitalWrite(relay1, LOW); //activates bike, lights up Dash
      Serial.println("Motorcycle Engaged");
  
     #if defined(usingledbuz) 
      digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
      delay(200);//
      digitalWrite(ledbuz, LOW); //
      #endif
    
      #if defined (usingmp3player)
         myDFPlayer.play(3);  //Play the first mp3
         #endif

    #if !defined(toggleMode) //if not using toggleState the ignition is turned on momentarily
      delay(2500);//allows bike time to start up
      digitalWrite(relay2, LOW);//activates ignition
      Serial.println("Motorcycle Starting");
      delay(2500);//waits 4 seconds for bike to tick over
      digitalWrite(relay2, HIGH); //deactivates ignition.
      Serial.println("Deactivate Ignition");
      #endif

    #if defined(toggleMode)
      toggleState = !toggleState;
      Serial.print("Toggling relay state to: ");
      if(toggleState) Serial.println("ON!");
      else Serial.println("OFF!");
      digitalWrite(relay2, toggleState);
    #endif
}
void wronguid (){ // if uid incorrect do this....
  Serial.println(" Access denied");
  
     #if defined (usingmp3player)
     myDFPlayer.play(1);  //Play the first mp3
     #endif
  
    #if defined(usingledbuz)
      digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
      delay(100);
      digitalWrite(ledbuz, LOW); //deactivates ignition.
      delay(100);
      digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
      delay(200);//
      digitalWrite(ledbuz, LOW); 
      #endif
      delay(3000);
}

//                -------------------------------------------------
//                                     Code End
//                -------------------------------------------------
