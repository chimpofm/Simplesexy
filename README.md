# Simplesexy
Simple Sexy Talking RFID access controller 
This is Chimpo Mcoodle's "Simple Sexy Code" for "The Simple Sexy" module. It uses a 12v - 5v Buck converters,
an Arduino Nano,a Y5300 MP3 Player,a Mini RC522 4 channel relay module, and a 4 channel logic converter. 
When the correct tag or card or implant is applied to the reader. this code alows the user to set a series of 
lights, buzzers,mp3 or relay commands, alowing you RFID access control to your existing systems.


This current example is set to alows access to a motorcycle using the RC522 ,logic converter, YX5200 and a 4 
Channel Relay . it lights up the dash, waits 4 second and then activates the ignition for 4 seconds. with the 
corisponding voice commands. please change the "void correctuid" and "void wronguid'' functions respectivley 
to meet your specific requirements. 
