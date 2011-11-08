#include "CPachube.h"



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





CPachube::CPachube( char * api_key, char * ip_address ) {
	sprintf_s( this->m_api_key,		api_key ); 
	sprintf_s( this->m_ip_address,	ip_address ); 
}

bool CPachube::Update( unsigned char pin, int value ) { 
	if( pin > MAX_VALUE_COUNT ) {
		return false;
	}
	this->m_values[ pin ].Update( value ); 
	return true ; 
}

int CPachube::Get( unsigned char pin, CPachubeValues::pachubetType type ) {
	if( pin > MAX_VALUE_COUNT ) {
		return 0;
	}
	return this->m_values[ pin ].Get( type ) ; 
}
