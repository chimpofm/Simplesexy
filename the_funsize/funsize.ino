
#include "Arduino.h"
#include <avr/sleep.h> //this AVR library contains the methods that controls the sleep modes
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include <MFRC522.h>
//               -------Definitions- Delete the "//" of the modules you are using----------

#define usingledbuz// unmark this if your using the pin 6 LED or Buzzer
//#define usingmp3player// unmark this if using the df mp3player
#define usingrc522 // unmark this if you are using the RC522 13.56 NFC-HF RFID reader


//                                -------Add your Details here-------
String hfuid1 = "DE AD BE EF 13 37";// if using RC522 Put your UID here!
String hfuid2 = "[UID]";// if using more than one uid for RC522, edit this
//                         ----------------------------------------------
//                                          Code Start
//                         ----------------------------------------------



SoftwareSerial mySoftwareSerial(8, 7); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

unsigned long currTime,
prevTime=0;
#define timeToWaitForNFCBeforeSleeping 10000ul

#define interruptPin 3 //Pin we are going to use to wake up the Arduino
#define SS_PIN 10 // The SS pin is connected to Pin 10
#define RST_PIN 9 // the RST pin is conected tp pin 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int relay1 = 6; // relay 1 dash 
int relay2 = 5; // relay 2 ignition
bool checklock;
//int checklock = digitalRead(4);//  checks if door is locked or open.(limit switch)
int ledbuz = 2;// buzzer or led





void setup() 
{
  pinMode(4, INPUT_PULLUP);

  pinMode(interruptPin,INPUT);//Set pin d3 to input
  pinMode(relay1, OUTPUT);   // Declaring this Relay as output
  digitalWrite(relay1, HIGH); // setting it to OFF
  pinMode(relay2, OUTPUT);   //Declaring this Relay as output
  digitalWrite(relay2, HIGH); // setting it to OFF
  checklock = digitalRead(4);
 
  #if defined(usingledbuz)
  pinMode(ledbuz, OUTPUT);   //Declaring the buzzer or LED as output
  digitalWrite(ledbuz, LOW); // setting it to OFF
  #endif
  
  #if defined(usingmp3player)

  mySoftwareSerial.begin(9600);
  #endif
  
  Serial.begin(115200);   // Initiate a serial communication
  Serial.println("im awake");
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
 
  #if defined(usingledbuz)
    digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
    delay(200);//
    digitalWrite(ledbuz, LOW); 
    #endif
  }
void loop() 
{

 unsigned long millisAtStartOfLoop = millis();

  #if defined(usingrc522)
    // Awaiting an UID to be Presented
    while ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      if(millis() > millisAtStartOfLoop + timeToWaitForNFCBeforeSleeping ) {
        Serial.println("Timed Out");
        Going_To_Sleep();
      }
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      //this if probably rarely triggered, but if a read fails, the device should just go to sleep.
      //maybe even play the access denied message? .. meaning: fire the wronguid() function, and then sleep.
      Going_To_Sleep();
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
    if (content.substring(1) == (hfuid1)|| content.substring(1)== (hfuid2))  
    //change here the UID of the cards to give access, add more with  || content.substring(1)== "another UID"
    {correctuid();}
   else  
    {wronguid();}
  } 
 #endif

void correctuid(){ // If UID is correct do this ....
  Serial.println("Authorized access Door Engaged");
     
       #if defined(usingledbuz) 
        digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
        delay(200);//
        digitalWrite(ledbuz, LOW); //
        #endif
      
        #if defined (usingmp3player)
           myDFPlayer.play(3);  //Play the first mp3
           #endif
      //---------------------------------------------------------------------------------------------------------------------------
       Serial.println(checklock);
       checklock = digitalRead(4);
       if (checklock == 1) { // checks if door is open or locked.
  
        digitalWrite(relay1, LOW);
          #if defined(usingledbuz)
              digitalWrite(ledbuz, HIGH);
              digitalWrite(ledbuz, LOW); 
          #endif
        delay (3000);
        digitalWrite(relay1, HIGH); // one beep for locked
        Serial.println("door Locked");
        Going_To_Sleep();
      
    } else {
  
     digitalWrite(relay2, LOW);
         #if defined(usingledbuz)
             digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
             delay(200);//
             digitalWrite(ledbuz, LOW); 
             delay (200);//
             digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
             delay(200);//
             digitalWrite(ledbuz, LOW);  // 2 beeps for locked
         #endif
      delay (3000);
      digitalWrite(relay2, HIGH);
      Serial.println("Door Unlocked");
      delay (100);
      Going_To_Sleep();
  
    }
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
      delay(100);
      digitalWrite(ledbuz, LOW); //deactivates ignition.
      delay(100);
      digitalWrite(ledbuz, HIGH);//activates led or buzzer , one beep
      delay(200);//
      digitalWrite(ledbuz, LOW);  // three beeps wrong UID
      #endif
      delay(3000);
  Going_To_Sleep();    
}


void Going_To_Sleep(){
    Serial.println("going to sleep");
    delay (100);
    sleep_enable();//Enabling sleep mode
    attachInterrupt(1, wakeUp, RISING);//attaching a interrupt to pin d2
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    sleep_cpu();//activating sleep mode
    Serial.println("Scan your Uid!");//next line of code executed after the interrupt 
  }
void wakeUp(){
  Serial.println("System Awake");//Print message to serial monitor
   sleep_disable();//Disable sleep mode
  detachInterrupt(1); //Removes the interrupt from pin 3;
}

void timeout(long interval){
  while (1){
    currTime = millis();
    if (currTime-prevTime>interval){
    prevTime=currTime;
  break;}
}}

//                -------------------------------------------------
//                                     Code End
//                -------------------------------------------------
