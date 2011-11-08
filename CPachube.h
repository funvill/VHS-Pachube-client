
#define MAX_VALUE_COUNT	20 

class CPachubeValues {
	public:

		enum pachubetType {
			AVERAGE,
			LAST_VALUE,
			TOTAL,
			MAX,
			MIN
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

	public: 
		CPachube( char * api_key, char * ip_address ); 

		bool	Update	( unsigned char pin, int value ); 
		int		Get		( unsigned char pin, CPachubeValues::pachubetType type ) ; 
};

// ----------------------------------------------------------------------------

