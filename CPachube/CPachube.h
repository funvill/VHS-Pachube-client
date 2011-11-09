#include <stdio.h>
#include <Ethernet.h>

#ifndef __CPACHUBE_H__
#define __CPACHUBE_H__


#define SHARE_FEED_ID              38684     // this is your Pachube feed ID that you want to share to
#define REMOTE_FEED_ID             256      // this is the ID of the remote Pachube feed that you want to connect to
#define REMOTE_FEED_DATASTREAMS    4        // make sure that remoteSensor array is big enough to fit all the remote data streams
#define UPDATE_INTERVAL            10000    // if the connection is good wait 10 seconds before updating again - should not be less than 5
#define RESET_INTERVAL             10000    // if connection fails/resets wait 10 seconds before trying again - should not be less than 5

#define PACHUBE_API_KEY            "px03VzsRIJsBy0caHUCuIGDwlkQVdZs7LkLPs2VfqDs" // fill in your API key 



#define MAX_VALUE_COUNT	20 

class CPachubeValues {
	public:

		enum pachubetType {
			AVERAGE, LAST_VALUE, TOTAL,
			MAX, MIN
		};

		CPachubeValues(); 
 
		void Update( int value ); 
		int	Get( pachubetType type ); 

		void Reset(); 

	private: 
		int m_value;
		int m_max;
		int m_min; 

		int m_total; 
		unsigned int m_count; 
		

} ; 


// ----------------------------------------------------------------------------

class CPachube
{
	private:
		char m_api_key[255] ; 
		char m_ip_address[20] ; 

		CPachubeValues m_values[MAX_VALUE_COUNT]; 
		
		unsigned long last_connect;
		
	public: 
		CPachube(  ); 
		void Config( char * api_key, char * ip_address );

		bool UpdateValue( unsigned char pin, int value ); 
		int GetValue( unsigned char pin, CPachubeValues::pachubetType type ) ; 
		void ResetAll(  ); 
		void Reset( unsigned char pin ); 
		
		void Update() ; 
		
		
		void checkForResponse() ; 
		void disconnect_pachube() ; 
		

		void setupEthernet(); 
		void clean_buffer() ;
		void resetEthernetShield();		
};

// ----------------------------------------------------------------------------


#endif // __CPACHUBE_H__