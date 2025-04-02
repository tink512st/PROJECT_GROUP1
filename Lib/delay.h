#include "main.h"

#if(FREQ_OSC == 12000000)
	void delay_ms(unsigned int t){
		unsigned int i,j;
		for(i = 0;i<t;i++)
			for(j=0;j<123;j++);
	}

#elif(FREQ_OSC==11059200)
	void delay_ms(unsigned int t){
		unsigned int i,j;
		for(i = 0;i<t;i++)
			for(j=0;j<113;j++);
	}
#elif(FREQ_OSC==4000000)
	void delay_ms(unsigned int t){
		unsigned int i,j;
		for(i = 0;i<t;i++)
			for(j=0;j<106;j++);
	}

#elif(FREQ_OSC==8000000)
	void delay_ms(unsigned int t){
		unsigned int i,j;
		for(i = 0;i<t;i++)
			for(j=0;j<218;j++);
	}


#endif
