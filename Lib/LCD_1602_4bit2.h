#include "..\Lib\LCD_1602_4bit.h"

sbit D41 = P0^0;
sbit D51 = P0^1;
sbit D61 = P0^2;
sbit D71 = P0^3;

void LCD_Receive4bit2( unsigned char x) {
	D41 = x & 0x01;
     D51 = (x & 0x02) >> 1;
     D61 = (x & 0x04) >> 2;
     D71 = (x & 0x08) >> 3;
    EN = 1;
    delay_lcd();
    EN = 0;
}
void LCD_Command2(unsigned char x) {
    RS = 0; RW = 0;
   
    LCD_Receive4bit2(x >> 4); 
    delay_lcd();
    LCD_Receive4bit2(x & 0x0F); 
    delay_lcd();
}

void LCD_Init2() {
	delay_lcd();
	LCD_Command2(0x03); 
	delay_lcd();
	LCD_Command2(0x03);
	delay_lcd();	
	LCD_Command2(0x02); 
	
    LCD_Command2(0x28); // Ch? d? 4-bit, 2 dòng, font 5x7
    LCD_Command2(0x0e); // B?t hi?n th?, con tr? nh?p nháy
    LCD_Command2(0x01); // Xóa màn hình
    LCD_Command2(0x06); // T? d?ng d?ch con tr?
    LCD_Command2(0x0c); // B?t hi?n th?, t?t con tr?
    LCD_Command2(0x80); // Ðua con tr? v? d?u dòng
}


void LCD_Char2( unsigned char x) {
    RS = 1; RW = 0;
    
    LCD_Receive4bit2(x >> 4); 
    delay_lcd();
    LCD_Receive4bit2(x & 0x0F); 
    delay_lcd();
}

void LCD_String2(char* s) {
	int i;
     for(i = 0; s[i]!= 0 ; i++){
		LCD_Char2(s[i]);
    }
}

void LCD_String_xy2(char row, char colum, char* s){
	if(row == 0){
		LCD_Command2((0x80&0xF0)|colum);
	}
	else {
		LCD_Command2((0xC0&0xF0)|colum);
	}
	LCD_String2(s);
} 