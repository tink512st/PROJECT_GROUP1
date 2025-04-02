
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define lcd P0
sbit en = P2^7;
sbit rs =P2^6;
sbit rw =P2^5;


void delay_lcd(){
	int i;
	for(i=0;i<6000;i++);
}
void cmd(unsigned char x){
	lcd = x;
	rs = 0;
	rw = 0;
	en = 1;
	delay_lcd();
	en = 0;
}
void dat(unsigned char x){
	lcd = x;
	rs = 1;
	rw = 0;
	en = 1;
	delay_lcd();
	en = 0;
}
void show(unsigned char *s){
	while(*s){
		dat(*s++);
	}
}
void show_xy(char hang,char pos, char *s){
	if(hang==0){
		cmd((0x0F & pos) | 0x80);
		show(s);
	}
	else {
		cmd((0x0F & pos) | 0xC0);
		show(s);
	}
}
void init(){
	cmd(0x38);
	cmd(0x0c);
	cmd(0x06);
	cmd(0x01);
	cmd(0x80);
}
