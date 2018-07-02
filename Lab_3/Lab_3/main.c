
 // Lab3P1.c
 //
 // Created: 1/30/2018 4:04:52 AM
 // Author : Eugene Rockey
 // Copyright 2018, All Rights Reserved
 
 //no includes, no ASF, no libraries
 
 #include <util/delay.h>
 #include <avr/eeprom.h>
 #include <avr/interrupt.h>
 #include <avr/io.h>
 #include <math.h>
 #include <stdlib.h>
 
 const char MS1[] = "\r\nECE-412 ATMega328P Tiny OS";
 const char MS2[] = "\r\nby Eugene Rockey Copyright 2018, All Rights Reserved";
 const char MS3[] = "\r\nMenu: (L)CD, (A)CD, (E)EPROM\r\n";
 const char MS4[] = "\r\nReady: ";
 const char MS5[] = "\r\nInvalid Command Try Again...";
 const char MS6[] = "Volts\r";
 const char MS7[] = " F\r";

void LCD_Init(void);			//external Assembly functions
void UART_Init(void);
void UART_Clear(void);
void UART_Get(void);
void UART_Put(void);
void UART_On(void);
void UART_Off(void);
void LCD_Write_Data(void);
void LCD_Write_Command(void);
void LCD_Read_Data(void);
void Mega328P_Init(void);
void ADC_Get(void);
void EEPROM_Read(void);
void EEPROM_Write(void);

unsigned char ASCII;			//shared I/O variable with Assembly
unsigned char DATA;				//shared internal variable with Assembly
char HADC;						//shared ADC variable with Assembly
char LADC;						//shared ADC variable with Assembly
char temperature[5];			//string buffer for ADC output
int Acc;						//Accumulator for ADC use
int isValInUARTBuff;

void UART_Puts(const char *str)	//Display a string in the PC Terminal Program
{
	while (*str)
	{
		ASCII = *str++;
		UART_Put();
	}
}

void UART_PutChar(const char str){
	
	ASCII = str;
	
	UART_Put();
	
}

void UART_Poll(void){
	
	if((UCSR0A & (1<<RXC0))){
		
		ASCII = UDR0;
		
	}
	
}

void LCD_Puts(const char* str)	//Display a string on the LCD Module
{
	
	while(*str){
		DATA = *str++;
		LCD_Write_Data();
	}
	
}

void LCD_PutChar(const char str){
	DATA = str;
	
	LCD_Write_Data();
}


void Banner(void)				//Display Tiny OS Banner on Terminal
{
	UART_Puts(MS1);
	UART_Puts(MS2);
	UART_Puts(MS4);
}

void HELP(void)						//Display available Tiny OS Commands on Terminal
{
	UART_Puts(MS3);
}

//void initialize_button_pin(void){
	//DDRB = 0x00;
	//PORTB = 0x00;
//}

int isButtonPushed(){
	if(!(PINB & (1<<PINB7))) return 1;
	else return 0;
}

void LCD(void)						//Lite LCD demo
{
	DATA = 0x34;					//stores a command into data that allows the LCD to be edited by future input
	LCD_Write_Command();
	DATA = 0x08;					//stores a command into data that turns the display and cursor of the LCD off
	LCD_Write_Command();
	DATA = 0x02;					//stores a command into data that tells the LCD to return the cursor to the starting position
	LCD_Write_Command();
	DATA = 0x06;					//stores a command into data that powers the enables entry mode on the LCD
	LCD_Write_Command();
	DATA = 0x0f;					//stores a command into data that powers the display and blinks it
	LCD_Write_Command();
	

	LCD_Puts("               We are Team: Barely Passing                "); 
	
	//initialize_button_pin();
	
	while(isButtonPushed() == 0){
		
		int i = 0;
		
		while (isButtonPushed() == 0 && i < 27){
			
			DATA = 0x18;		//shift string by 1 to the right
			
			LCD_Write_Command();
			
			i++;
			
		}
		
		DATA = 0x02;			//reset the string
		
		LCD_Write_Command();
	}		
	
	//looping until a button is pressed
	
	/*
	Re-engineer this subroutine to have the LCD endlessly scroll a marquee sign of 
	your Team's name either vertically or horizontally. Any key press should stop
	the scrolling and return execution to the command line in Terminal. User must
	always be able to return to command line.
	*/
	//
	//UART_Off();
	//UART_On();
}

void Diplay_Fahrenheit(void)					//Lite Demo of the Analog to Digital Converter
{
	double B = 3900.0;	//3950
	
	ADC_Get();
	
	//initialize_button_pin();
	
	while(isButtonPushed() == 0) {
		
		ADC_Get();
		
		Acc = ( ( (int)HADC ) * 0x100 + (int)(LADC) );
		
		double r = ( 10000.0 * (double)Acc ) / (1024.0 - (double)Acc); 
		
		double T = ( B * 298.15 ) / (298.15 * log(r / 10000.0 ) + B );
		
		T = T - 273.15;
		
		T = T * ( 9 / 5 ) + 32;
		
		dtostrf(T, 3, 1, &temperature);		
	
		UART_Puts(temperature);
		UART_Puts(MS7);
		
		UART_Clear(); //resetting the UART terminal
		
		_delay_ms(100); //delaying so the cursor is not jumping
		
	}
	/*
		Re-engineer this subroutine to display temperature in degrees Fahrenheit on the Terminal.
		The potentiometer simulates a thermistor, its varying resistance simulates the
		varying resistance of a thermistor as it is heated and cooled. See the thermistor
		equations in the lab 3 folder. User must always be able to return to command line.
	*/
}

void Write_To_Default(void){
	
	EEPROM_Write();
	UART_Puts("\r\n");
	EEPROM_Read();
	UART_Put();
	UART_Puts("\r\n");
	
}

char Ask_For_Input(char *msg){
	
	UART_Puts(msg);
	
	ASCII = '\0';
	
	while (ASCII == '\0'){
		UART_Get();
	}
	
	return ASCII;
}

void Write_To_New(void){
	
	uint8_t EEPROM_Location, EEPROM_Data;
	
	EEPROM_Location = ((int)Ask_For_Input("What EEPROM Location do you want to write to?\r\n") - 48);
	
	//EEPROM_Data = (Ask_For_Input("What data do you want to write to?\r\n"));
	//
	//eeprom_write_byte(&EEPROM_Location, EEPROM_Data);
	
	EEPROM_Data = eeprom_read_byte(&EEPROM_Location);
	
	ASCII = EEPROM_Data;
	
	UART_Put();
}

void EEPROM(void)
{
	UART_Puts("\r\nEEPROM Write and Read.");
	/*
	Re-engineer this subroutine so that a byte of data can be written to any address in EEPROM
	during run-time via the command line and the same byte of data can be read back and verified after the power to
	the Xplained Mini board has been cycled. Ask the user to enter a valid EEPROM address and an
	8-bit data value. Utilize the following two given Assembly based drivers to communicate with the EEPROM. You
	may modify the EEPROM drivers as needed. User must be able to always return to command line.
	*/
	
	UART_Puts("Would you like to write to the default location? y/n\r\n");
	
	ASCII = '\0';
	while (ASCII == '\0')
	{
		UART_Get();
	}
	switch (ASCII){
		case 'y' | 'Y': Write_To_Default();
		break;
		case 'n' | 'N': Write_To_New();
		break;
	}
}

void Parity(void){
	
	ASCII = Ask_For_Input("Parity: (a)even? (b)odd?\r\n");
	
	switch(ASCII){
		case 'a' | 'A':
		
		UCSR0C |= (0 << UPM00) | (1 << UPM01);
		
		break;
		
		case 'b' | 'B':
		
		UCSR0C |= (1 << UPM00) | (1 << UPM01);
		
		break;
	}
}

void StopBit(void){
	
	ASCII = Ask_For_Input("StopBit: (a)1bit? (b)2bit?\r\n");
		
	switch(ASCII){
		case 'a' | 'A':
			
		UCSR0C |= (0 << USBS0);
			
		break;
			
		case 'b' | 'B':
			
		UCSR0C |= (1 << USBS0);
			
		break;
	}
}

void DataSize(void){
	
	ASCII = Ask_For_Input("DataSize: (a)5-bit? (b)6-bit (c)7-bit? (d)8-bit (e)9-bit\r\n");
		
	switch(ASCII){
		case 'a' | 'A':
			
		UCSR0C |= (0<<UCSZ00)|(0<<UCSZ01)|(0<<UCSZ02);
			
		break;
			
		case 'b' | 'B':
			
		UCSR0C |= (1<<UCSZ00)|(0<<UCSZ01)|(0<<UCSZ02);
		
		break;
		
		case 'c'|'C':
		
		UCSR0C |= (0<<UCSZ00)|(1<<UCSZ01)|(0<<UCSZ02);
			
		break;
		
		case 'd' | 'D':
		
		UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01)|(0<<UCSZ02);
		
		break;
		
		case 'e' | 'E':
		
		UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01)|(1<<UCSZ02);
		
		break;
	}

}

void USART(void) {
	
	unsigned int ubrr;
	
	ASCII = Ask_For_Input("Baud Rate: (a)9600 (b)4800 (c)2400\r\n");
	
	switch(ASCII){
		
		case 'a' | 'A':
		
		ubrr = 9600;
		
		break;
		
		case 'b' | 'B':
		
		ubrr = 4800;
		
		break;
		
		case 'c' | 'C':
		
		ubrr = 2400;
		
		break;
		
		default:
		
		ubrr = 9600;
		
	}
	
	ubrr = 16000000/16/ubrr-1;
	
	//Set BAUD Rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable Receiver and Transmitter
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	
	Parity();
	StopBit();
	DataSize();
	
}

void Command(void)					//command interpreter
{
	UART_Puts(MS3);
	ASCII = '\0';						
	while (ASCII == '\0')
	{
		UART_Get();
	}
	switch (ASCII)
	{
		case 'L' | 'l': LCD();
		break;
		case 'A' | 'a': Diplay_Fahrenheit();
		break;
		case 'E' | 'e': EEPROM();
		break;
		default:
		UART_Puts(MS5);
		HELP();
		break;  			//Add a 'USART' command and subroutine to allow the user to reconfigure the 						
							//serial port parameters during runtime. Modify baud rate, # of data bits, parity, 							
							//# of stop bits.
	}
}

int main(void)
{
	Mega328P_Init();
	Banner();
	while (1)
	{
		Command();				//infinite command loop
	}
}

