
#include <intrins.h>

sbit DHT11=P2^3; /* Connect DHT11 output Pin to P2.1 Pin */

int I_RH,D_RH,I_Temp,D_Temp,CheckSum;


void timer_delay20ms() /* Timer0 delay function */

{
    
char i,j;
	for(i = 0; i < 50; i++){
		for(j = 0; j < 123; j++);
	}
	
  /* 
TMOD = 0x01;

TH0 = 0xB8; 

TL0 = 0x0C; 

TR0 = 1;

while(TF0 == 0); 
TR0 = 0; 

TF0 = 0; 
	*/
}


void timer_delay30us() /* Timer0 delay function */

{
 unsigned char i;
    for (i = 0; i < 8; i++) {  // S? vòng l?p có th? c?n di?u ch?nh tùy vào xung nh?p
        _nop_();  // L?nh r?ng, m?t kho?ng 1 chu k? máy
    }
/*	
 TMOD = 0x01;

TH0 = 0xFF; 

TL0 = 0xF1; 

TR0 = 1; 

while(TF0 == 0); 
	TR0 = 0; 

TF0 = 0; 
*/
}


void Request() /* Microcontroller send request */

{

DHT11 = 0; /* set to low pin */

timer_delay20ms(); /* wait for 20ms */

DHT11 = 1; /* set to high pin */

}


void Response() /* Receive response from DHT11 */

{
while(DHT11==1) ;
while(DHT11==0);
while(DHT11==1);
}


int Receive_data() /* Receive data */

{

int q,c=0;

for (q=0; q<8; q++)

{

while(DHT11==0);/* check received bit 0 or 1 */

timer_delay30us();

if(DHT11 == 1)/* If high pulse is greater than 30ms */

c = (c<<1)|(0x01);/* Then its logic HIGH */

else /* otherwise its logic LOW */

c = (c<<1);

while(DHT11==1);

}

return c;

}
