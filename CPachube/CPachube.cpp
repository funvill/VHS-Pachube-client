#include "CPachube.h"


byte mac[] = { 0x86, 0x72, 0x83, 0x01, 0x01, 0x01 }; // make sure this is unique on your network
byte ip[] = { 10, 10, 2, 119 };                  // no DHCP so we set our own IP address
byte remoteServer[] = { 173,203,98,29 };            // pachube.com

float remoteSensor[REMOTE_FEED_DATASTREAMS];        // we know that feed 256 has floats - this might need changing for feeds without floats
Client localClient(remoteServer, 80);




unsigned int interval;

char buff[64];
int pointer = 0;

char pachube_data[70];

boolean found_status_200 = false;
boolean found_session_id = false;
boolean found_CSV = false;
char *found;
unsigned int successes = 0;
unsigned int failures = 0;
boolean ready_to_update = true;
boolean reading_pachube = false;

boolean request_pause = false;
boolean found_content = false;

unsigned long last_connect;

int content_length;


CPachubeValues::CPachubeValues() {
	this->Reset(); 
}

void CPachubeValues::Reset() {
	this->m_count = 0 ; 
	this->m_total = 0 ; 
	this->m_value = 0 ; 

	this->m_max	  = 0 ;
	this->m_min	  = 0 ;
}


void CPachubeValues::Update( int value ){

	this->m_value = value ; 
	if( this->m_count == 0 ) {
		this->m_max = value; 
		this->m_min = value; 
	}

	if( value > this->m_max ) {
		this->m_max = value; 
	}
	if( value < this->m_min ) {
		this->m_min = value; 
	}

	// Update the count 
	this->m_count++;
	this->m_total += value ; 
}
int	CPachubeValues::Get( pachubetType type ){

	switch( type ) {
		case CPachubeValues::AVERAGE: {
			return this->m_total/this->m_count ; 
		}
		case CPachubeValues::MAX: {
			return this->m_max ; 
		}
		case CPachubeValues::MIN: {
			return this->m_min ; 
		}
		case CPachubeValues::TOTAL: {
			return this->m_total ; 
		}
		case CPachubeValues::LAST_VALUE: 
		default : {
			return this->m_value ; 
		}
	}
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------


CPachube::CPachube( ) {
	
}

void CPachube::Config( char * api_key, char * ip_address )
{
	sprintf( this->m_api_key,		api_key ); 
	sprintf( this->m_ip_address,	ip_address ); 
	this->setupEthernet(); 
}

void CPachube::Reset( unsigned char pin ) {
	if( pin > MAX_VALUE_COUNT ) {
		return;
	}
	this->m_values[ pin ].Reset() ; 	
} 
void CPachube::ResetAll(  ) {
	for( int offset = 0 ; offset < MAX_VALUE_COUNT ; offset++ ) {
		this->Reset( offset ); 
	}
}

bool CPachube::UpdateValue( unsigned char pin, int value ) { 
	if( pin > MAX_VALUE_COUNT ) {
		return false;
	}
	this->m_values[ pin ].Update( value ); 
	return true ; 
}

int CPachube::GetValue( unsigned char pin, CPachubeValues::pachubetType type ) {
	if( pin > MAX_VALUE_COUNT ) {
		return 0;
	}
	return this->m_values[ pin ].Get( type ) ; 
}

void CPachube::Update() {

	if (true)
	{
		Serial.println("Connecting...");
		if (localClient.connect()) 
		{
			// here we assign comma-separated values to 'data', which will update Pachube datastreams
			// we use all the analog-in values, but could of course use anything else millis(), digital
			// inputs, etc. . i also like to keep track of successful and failed connection
			// attempts, sometimes useful for determining whether there are major problems.

			sprintf(pachube_data,"%d,%d,", 
						this->GetValue( 1, CPachubeValues::MAX ), 
						this->GetValue( 2, CPachubeValues::MAX ) );
			content_length = strlen(pachube_data);

			Serial.println("GET request to retrieve");

			localClient.print("GET /api/");
			localClient.print(REMOTE_FEED_ID);
			localClient.print(".csv HTTP/1.1\nHost: pachube.com\nX-PachubeApiKey: ");
			localClient.print(PACHUBE_API_KEY);
			localClient.print("\nUser-Agent: Arduino (Pachube In Out v1.1)");
			localClient.println("\n");

			//Serial.println("finished GET now PUT, to update");

			localClient.print("PUT /api/");
			localClient.print(SHARE_FEED_ID);
			localClient.print(".csv HTTP/1.1\nHost: pachube.com\nX-PachubeApiKey: ");
			localClient.print(PACHUBE_API_KEY);

			localClient.print("\nUser-Agent: Arduino (Pachube In Out v1.1)");
			localClient.print("\nContent-Type: text/csv\nContent-Length: ");
			localClient.print(content_length);
			localClient.print("\nConnection: close\n\n");
			localClient.print(pachube_data);



			localClient.print("\n");

			ready_to_update = false;
			reading_pachube = true;
			request_pause = false;
			interval = UPDATE_INTERVAL;

			// Serial.print("finished PUT: ");
			// Serial.println(millis());

		} else {
			Serial.print("connection failed!");
			Serial.print(++failures);
			found_status_200 = false;
			found_session_id = false;
			found_CSV = false;
			ready_to_update = false;
			reading_pachube = false;
			request_pause = true;
			last_connect = millis();
			interval = RESET_INTERVAL;
			setupEthernet();
		}
	}

	while (reading_pachube){
		while (localClient.available()) {
			checkForResponse();
		} 

		if (!localClient.connected()) {
			disconnect_pachube();
		}
	} 

	
	
	// ToDo: 
	this->ResetAll(); 
}



void CPachube::disconnect_pachube(){
  Serial.println("disconnecting.\n=====\n\n");
  localClient.stop();
  ready_to_update = false;
  reading_pachube = false;
  request_pause = true;
  last_connect = millis();
  found_content = false;
  resetEthernetShield();
}


void CPachube::checkForResponse(){  
  char c = localClient.read();
  //Serial.print(c);
  buff[pointer] = c;
  if (pointer < 64) pointer++;
  if (c == '\n') {
    found = strstr(buff, "200 OK");
    if (found != 0){
      found_status_200 = true; 
      //Serial.println("Status 200");
    }
    buff[pointer]=0;
    found_content = true;
    clean_buffer();    
  }

  if ((found_session_id) && (!found_CSV)){
    found = strstr(buff, "HTTP/1.1");
    if (found != 0){
      char csvLine[strlen(buff)-9];
      strncpy (csvLine,buff,strlen(buff)-9);

      //Serial.println("This is the retrieved CSV:");     
      //Serial.println("---");     
      //Serial.println(csvLine);
      //Serial.println("---");   
      Serial.println("\n--- updated: ");
      Serial.println(pachube_data);
      Serial.println("\n--- retrieved: ");
      char delims[] = ",";
      char *result = NULL;
      char * ptr;
      result = strtok_r( buff, delims, &ptr );
      int counter = 0;
      while( result != NULL ) {
        remoteSensor[counter++] = atof(result); 
        result = strtok_r( NULL, delims, &ptr );
      }  
      for (int i = 0; i < REMOTE_FEED_DATASTREAMS; i++){
        Serial.print( (int)remoteSensor[i]); // because we can't print floats
        Serial.print("\t");
      }

      found_CSV = true;

      Serial.print("\nsuccessful updates=");
      Serial.println(++successes);

    }
  }

  if (found_status_200){
    found = strstr(buff, "_id=");
    if (found != 0){
      clean_buffer();
      found_session_id = true; 
    }
  }
}





void CPachube::setupEthernet(){
  this->resetEthernetShield();
  Client remoteClient(255);
  delay(500);
  interval = UPDATE_INTERVAL;
  Serial.println("setup complete");
}

void CPachube::clean_buffer() {
  pointer = 0;
  memset(buff,0,sizeof(buff)); 
}

void CPachube::resetEthernetShield(){
  Serial.println("reset ethernet");
  Ethernet.begin(mac, ip);
}
