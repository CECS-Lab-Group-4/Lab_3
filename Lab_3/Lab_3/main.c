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
 
 const char MS1[] = "\r\nECE-412 ATMega328P Tiny OS";
 const char MS2[] = "\r\nby Eugene Rockey Copyright 2018, All Rights Reserved";
 const char MS3[] = "\r\nMenu: (L)CD, (A)CD, (E)EPROM\r\n";
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

char volts[5];					//string buffer for ADC output
int Acc;						//Accumulator for ADC use

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
	
	ASCII = '\0';
	
	while (ASCII == '\0'){
		
		LCD_Puts("               We are Team: Barely Passing               ");
	
		for (int i = 0; i < 31; i++){
			DATA = 0x1c;
			LCD_Write_Data();
			
			_delay_ms(50);
			
			UART_Get();
			
		}
		
	}
	
	/*
	Re-engineer this subroutine to have the LCD endlessly scroll a marquee sign of 
	your Team's name either vertically or horizontally. Any key press should stop
	the scrolling and return execution to the command line in Terminal. User must
	always be able to return to command line.
	*/
}

char Display_Fahrenheit(char* A2DValue){
	
	float ADValue = (float)( (int)A2DValue[0x0] + (int)A2DValue[0x1] + A2DValue[0x2] + A2DValue[0x3] + A2DValue[0x4] );
	
	const double B = 3950;
	
	double r = 10000.0 * ADValue / (1024.0 - ADValue);
	
	double T = B * 293.15 / (293.15 * log(r / 10000.0) + B );
	
	T = T - 273.15;
	
	T = T * (9.0 / 5.0) + 32.0;
	
	char value_to_return = (char)T;
	
	return value_to_return;
}

void _ADC(void)					//Lite Demo of the Analog to Digital Converter
{
	volts[0x1]='.';
	volts[0x3]=' ';
	volts[0x4]= 0;
	ADC_Get();
	Acc = ( ( (int)HADC ) * 0x100 + (int)(LADC) ) * 0xA;
	volts[0x0] = 48 + (Acc / 0x7FE);
	Acc = Acc % 0x7FE;
	volts[0x2] = ((Acc *0xA) / 0x7FE) + 48;
	Acc = (Acc * 0xA) % 0x7FE;
	if (Acc >= 0x3FF) volts[0x2]++;
	if (volts[0x2] == 58)
	{
		volts[0x2] = 48;
		volts[0x0]++;
	}
	UART_Puts(volts);
	UART_Puts(MS6);
	
	UART_PutChar(Display_Fahrenheit(volts));
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
	
	EEPROM_Location = (uint8_t )Ask_For_Input("What EEPROM Location do you want to write to?");
	
	EEPROM_Data = (uint8_t)Ask_For_Input("What data do you want to write to?");
	
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
	
	unsigned int ubrr = (int)Ask_For_Input("What BAUD Rate do you want to set? numbers only please!");
	
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


