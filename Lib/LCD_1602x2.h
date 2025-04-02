
sbit EN = P2^7;
sbit RS = P2^6;
sbit RW = P2^5;

sbit D4 = P0^4;
sbit D5 = P0^5;
sbit D6 = P0^6;
sbit D7 = P0^7;
void delay_lcd() {
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 123; j++);
    }
}
void LCD_Receive4bit( unsigned char x) {
	D4 = x & 0x01;
     D5 = (x & 0x02) >> 1;
     D6 = (x & 0x04) >> 2;
     D7 = (x & 0x08) >> 3;
    EN = 1;
    delay_lcd();
    EN = 0;
}
void LCD_Command(unsigned char x) {
    RS = 0; RW = 0;
   
    LCD_Receive4bit(x >> 4); 
    delay_lcd();
    LCD_Receive4bit(x & 0x0F); 
    delay_lcd();
}

void LCD_Init() {
	delay_lcd();
	LCD_Command(0x03); 
	delay_lcd();
	LCD_Command(0x03);
	delay_lcd();	
	LCD_Command(0x02); 
	
    LCD_Command(0x28); // Ch? d? 4-bit, 2 dòng, font 5x7
    LCD_Command(0x0e); // B?t hi?n th?, con tr? nh?p nháy
    LCD_Command(0x01); // Xóa màn hình
    LCD_Command(0x06); // T? d?ng d?ch con tr?
    LCD_Command(0x0c); // B?t hi?n th?, t?t con tr?
    LCD_Command(0x80); // Ðua con tr? v? d?u dòng
}


void LCD_Char( unsigned char x) {
    RS = 1; RW = 0;
    
    LCD_Receive4bit(x >> 4); 
    delay_lcd();
    LCD_Receive4bit(x & 0x0F); 
    delay_lcd();
}

void LCD_String(char* s) {
	int i;
     for(i = 0; s[i]!= 0 ; i++){
		LCD_Char(s[i]);
    }
}

void LCD_String_xy(char row, char colum, char* s){
	if(row == 0){
		LCD_Command((0x80&0xF0)|colum);
	}
	else {
		LCD_Command((0xC0&0xF0)|colum);
	}
	LCD_String(s);
} 