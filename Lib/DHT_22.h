#include <intrins.h>
#include "..\Lib\LCD_1602.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
sbit DHT22 = P2^2;
unsigned char dat22[5];
unsigned char txt[20];
float Hum, Temp;

void delay_ms(unsigned int t){
	TMOD &= 0xF0;
	TMOD|= 0x01;
	TH0 = (65536-1000*t)>>8;
	TL0 = (65536-1000*t) & 0x00FF;
	TR0 =1;
	while(!TF0);
	TF0 = 0;
	TR0 = 0;
}
void delay_40us(){
	TMOD &= 0xF0;
	TMOD|= 0x01;
	TH0 = (65536-20)>>8;
	TL0 = (65536-20) & 0x00FF;
	TR0 =1;
	while(!TF0);
	TF0 = 0;
	TR0 = 0;
}
void delay_1us() {
    _nop_();
}
void delay_us(unsigned int us) {
    while (us--) {
        _nop_();  // Hi?u ch?nh cho 8051
    }
}
void Request() {
    DHT22 = 0;      // Kéo chân xu?ng LOW
    delay_ms(2);    // Gi? trong 2ms
    DHT22 = 1;      // Kéo lên HIGH 
}

unsigned char Response() {
    unsigned int timeout = 0;  
    while (DHT22 == 1) {  
      
    }
    
    timeout = 0;
    while (DHT22 == 0) {  
    }
    timeout = 0;
    while (DHT22 == 1) {  
      
    }
    return 1;  
}

unsigned char Receive() {
    unsigned char value = 0, i;
    unsigned int timeout;
	LCD_String_xy(0,7,"X");
    for (i = 0; i < 8; i++) {
        timeout = 0;
        while (DHT22 == 0) { 
		
        }
	   LCD_String_xy(0,8,"X");
	   timeout = 0;
        while (DHT22 == 1) { 
		 timeout++;
           if (timeout > 100) return;
        }
	   LCD_String_xy(0,13,"X");
	   DHT22 = 0;
        value <<= 1;
        if (timeout >30) {  // N?u v?n HIGH sau 30µs thì là bit 1
            value |= 1;
        }
    }
    return value;
}

void Read_DHT22() {
    unsigned char i;
    Request();
    if (Response()) { 
        for (i = 0; i < 5; i++) {
            dat22[i] = Receive();
        }
		
        if (dat22[0] + dat22[1] + dat22[2] + dat22[3] == dat22[4]) {  // Ki?m tra checksum
            Hum = ((dat22[0] << 8) | dat22[1]) * 0.1;
            Temp = ((dat22[2] << 8) | dat22[3]) * 0.1;
        } else {  // N?u checksum sai, không c?p nh?t giá tr?
            Hum = ((dat22[0] << 8) | dat22[1]) * 0.1;
            Temp = ((dat22[2] << 8) | dat22[3]) * 0.1;
        }
    }
}
