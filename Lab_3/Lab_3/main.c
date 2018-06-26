 // Lab3P1.c
 //
 // Created: 1/30/2018 4:04:52 AM
 // Author : Eugene Rockey
 // Copyright 2018, All Rights Reserved
 
 #define F_CPU 16000000
 
 //no includes, no ASF, no libraries
 #include <math.h>
 #include <avr/eeprom.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>
 #include <avr/io.h>
 
 const char MS1[] = "\r\nECE-412 ATMega328P Tiny OS";
 const char MS2[] = "\r\nby Eugene Rockey Copyright 2018, All Rights Reserved";
 const char MS3[] = "\r\nMenu: (L)CD, (A)CD, (E)EPROM (C)hange BAUD\r\n";
 const char MS4[] = "\r\nReady: ";
 const char MS5[] = "\r\nInvalid Command Try Again...";
 const char MS6[] = "Volts\r";
 const char MS7[] = "F\r";

void LCD_Init(void);			//external Assembly functions
void UART_Init(void);
void UART_Clear(void);
void UART_Get(void);
void UART_Put(void);
void LCD_Write_Data(void);
void LCD_Write_Command(void);
void LCD_Read_Data(void);
void LCD_Delay(void);
void Mega328P_Init(void);
void ADC_Get(void);
void EEPROM_Read(void);
void EEPROM_Write(void);

unsigned char ASCII;			//shared I/O variable with Assembly
unsigned char DATA;				//shared internal variable with Assembly
char HADC;						//shared ADC variable with Assembly
char LADC;						//shared ADC variable with Assembly
unsigned char DATA_FOR_EEPROM;
unsigned char REGISTER_FOR_EEPROM;

char temperature[5];					//string buffer for ADC output
int Acc;						//Accumulator for ADC use

volatile int USART_INPUT = 1;

ISR(USART1_RX_vect){
	
	USART_INPUT = 0;
	
}

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

void LCD_Puts(const char* str)	//Display a string on the LCD Module
{
	while(*str){
		
		DATA = *str++;
		
		LCD_Write_Data();
	}
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

void LCD(void)						//Lite LCD demo
{
	DATA = 0x34;					//Student Comment Here
	LCD_Write_Command();
	DATA = 0x08;					//Student Comment Here
	LCD_Write_Command();
	DATA = 0x02;					//Student Comment Here
	LCD_Write_Command();
	DATA = 0x06;					//Student Comment Here
	LCD_Write_Command();
	DATA = 0x0f;					//Student Comment Here
	LCD_Write_Command();
	
	LCD_Puts("         We are Team: Barely Passing           ");
	
	while( USART_INPUT == 1 ){
		
		for (int i = 0; i < 11; i++){
			
			DATA = 0x18;
			
			LCD_Write_Command();
			
		}
		
		//UART_Get();
		
		DATA = 0x02;
		
		LCD_Write_Command();
		
	}
	
	USART_INPUT = 1;
	
	cli();
	
	/*
	Re-engineer this subroutine to have the LCD endlessly scroll a marquee sign of 
	your Team's name either vertically or horizontally. Any key press should stop
	the scrolling and return execution to the command line in Terminal. User must
	always be able to return to command line.
	*/
}

double Display_Fahrenheit(float A2DValue){
	
	const double B = 3950.0;
	
	double r = 10000.0 * A2DValue / (1024.0 - A2DValue);
	
	double T = B * 298.15 / (298.15 * log(r / 10000.0) + B );
	
	T = T - 273.15;
	
	T = T * 9.0 / 5.0 + 32.0;
	
	return T;
}

void _ADC(void)					//Lite Demo of the Analog to Digital Converter
{
	
	temperature[0x2]='.';
	temperature[0x4]= 0;
	ADC_Get();
	Acc = ( ( (int)HADC ) * 0x100 + (int)(LADC) );
	
	double converted_temperature = Display_Fahrenheit(Acc);
	
	int ACC_No_decimal_place = (converted_temperature * 10);
	int ACC_Regular_int = converted_temperature;
	
	temperature[0x0] = ACC_No_decimal_place / 100 + 48;
	
	temperature[0x1] = ACC_Regular_int % 10 + 48;
	
	temperature[0x3] = ACC_No_decimal_place % 10 + 48;
	
	//UART_Puts(volts);
	//UART_Puts(MS6);
	
	UART_Puts(temperature);
	UART_Puts(MS7);
	
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
	
	uint8_t EEPROM_Location;
	uint8_t EEPROM_Data;
	
	EEPROM_Location = (uint8_t)Ask_For_Input("What EEPROM Location do you want to write to?\r\n");
	
	EEPROM_Data = (uint8_t)Ask_For_Input("What data do you want to write to?\r\n");
	
	eeprom_write_byte(&EEPROM_Location, EEPROM_Data);
	
	EEPROM_Data = eeprom_read_byte(&EEPROM_Location);
	
	ASCII = (unsigned char)EEPROM_Data;
	
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
	
	UART_Puts("Would you like to write to the default location? y/n");
	
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

void USART(void) {
	
	unsigned int ubrr = 1000 * ( (int)Ask_For_Input("What BAUD Rate do you want to set? 1000 place") );
	ubrr += 100 * ( (int)Ask_For_Input("What BAUD Rate do you want to set? 100 place") );
	ubrr += 10 * ( (int)Ask_For_Input("What BAUD Rate do you want to set? 10") );
	ubrr += 1 * ( (int)Ask_For_Input("What BAUD Rate do you want to set? 1") );
	
	ubrr = ((16000000/16/ubrr) - 1);
	
	//Set BAUD Rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable Receiver and Transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	//Set frame format: 8data, 2stop bit
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	
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
		case 'A' | 'a': _ADC();
		break;
		case 'E' | 'e': EEPROM();
		break;
		case 'C' | 'c' : USART();
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
	
	sei();
		
	Banner();
	while (1)
	{
		Command();				//infinite command loop
	}
}


