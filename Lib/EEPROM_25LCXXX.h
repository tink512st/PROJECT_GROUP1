sbit SPI_SCK = P2^0;
sbit SPI_MOSI = P2^1;
sbit SPI_MISO = P2^2;
sbit SPI_CS = P2^3;
void SPI_Init(){
	SPI_CS = 1;
	SPI_SCK = 0;
	SPI_MISO = 1;
}
void SPI_Write(unsigned char c){
	int i;
	for(i = 0; i < 8;i++){
		if(c&0x80){
			SPI_MOSI = 1;
		}
		else SPI_MOSI = 0;
	SPI_SCK = 1;
	SPI_SCK = 0;
	c<<=1;
	}
}
unsigned char SPI_Read(){
	unsigned char i,b;
	for(i = 0; i < 8;i++){
		SPI_SCK = 1;
		b<<=1;
		if(SPI_MISO) b|=0x01;
		SPI_SCK = 0;
	}
}
void EEPROM_Write_25LCXXX(unsigned int add, unsigned char c){
	SPI_CS = 0;
	SPI_Write(0x06);// ghi dia chi WREN cho phep ghi
	SPI_CS = 1;
	
	SPI_CS = 0;
	SPI_Write(0x02);//gui lenh ghi
	SPI_Write(add>>8);// gui 8 bit cua add
	SPI_Write(add&0x00FF);//gui 8 bit thap cua add
	SPI_Write(c);
	SPI_CS=1;
}
unsigned char EEPROM_Read_25LCXXX(unsigned int add){
	unsigned char b;
	SPI_CS = 0;
	SPI_Write(0x03);//gui  lenh doc cho eeprom
	SPI_Write(add>>8);//gui 8 bit cao add
	SPI_Write(add&0x00FF);//gui 8 bit thap add
	b = SPI_Read();
	SPI_CS = 1;
	return b;
}
unsigned char EEPROM_25LCXXX_WIP(){
	unsigned char b;
	SPI_CS = 0;
	SPI_Write(0x05);
	b = SPI_Read();
	SPI_CS = 1;
	b &=0x01;
	return b;
}