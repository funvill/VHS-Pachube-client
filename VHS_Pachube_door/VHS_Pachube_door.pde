#include <SPI.h>

/* ==============================
 * This code, which assumes you're using the official Arduino Ethernet shield,
 * updates a Pachube feed with your analog-in values and grabs values from a Pachube
 * feed - basically it enables you to have both "local" and "remote" sensors.
 * 
 * Tested with Arduino 14
 *
 * Pachube is www.pachube.com - connect, tag and share real time sensor data
 * code by usman (www.haque.co.uk), may 2009
 * copy, distribute, whatever, as you like.
 *
 * v1.1 - added User-Agent & fixed HTTP parser for new Pachube headers
 * and check millis() for when it wraps around
 *
 * =============================== */


#include <Ethernet.h>
#include <string.h>

#undef int() // needed by arduino 0011 to allow use of stdio
#include <stdio.h> // for function sprintf

#include <CPachube.h>

CPachube dataLogger; 
unsigned long lastSentToPachube ;

void setup()
{
  Serial.begin(9600); 
  dataLogger.setupEthernet(); 
  
  dataLogger.Config( "", "173.203.98.29" ); 
  lastSentToPachube = 0 ; 
}



void loop()
{
  dataLogger.UpdateValue( 1, analogRead( 1 ) ); 
  dataLogger.UpdateValue( 2, analogRead( 2 ) );   

  if (millis() > lastSentToPachube) { 
      lastSentToPachube = millis() + 10*1000 ;
      
      dataLogger.Update(); 
      Serial.println( "\n\nReset\n\n" );       
  }
  
  
  Serial.print( "Pin(1): " ); 
  Serial.print( dataLogger.GetValue( 1, CPachubeValues::MAX ) ); 

  Serial.print( " Pin(2): " ); 
  Serial.print( dataLogger.GetValue( 2, CPachubeValues::MAX ) ); 
  Serial.println(); 

  // call 'pachube_in_out' at the beginning of the loop, handles timing, requesting
  // and reading. use serial monitor to view debug messages
  // pachube_in_out();
}

