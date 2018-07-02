 // Lab3P1.s
 //
 // Created: 1/30/2018 4:15:16 AM
 // Author : Eugene Rockey
 // Copyright 2018, All Rights Reserved

.section ".data"					//equivalent to DSEG
.equ	DDRB,0x04					;Defines the Data Direction Register B in AVR memory
.equ	DDRD,0x0A					;Defines the Data Direction Register D in AVR memory
.equ	PORTB,0x05					;Defines the PORT B Register in AVR memory
.equ	PORTD,0x0B					;Defines the PORT D Register in AVR memory
.equ	U2X0,1						;doubles the transmittion speed for USART
.equ	UBRR0L,0xC4					;Defines the UBRR0L in AVR Memory - will sets the low byte of the prescaler for the baud rate
.equ	UBRR0H,0xC5					;Defines the UBRR0H in AVR Memory - will  sets the high byte of the prescaler for the baud rate
.equ	UCSR0A,0xC0					;Defines status register A in AVR Memory
.equ	UCSR0B,0xC1					;Defines status register B in AVR Memory
.equ	UCSR0C,0xC2					;Defines status register C in AVR Memoryy
.equ	UDR0,0xC6					;Defines UDR0 in AVR Memory 
.equ	RXC0,0x07					;Defines RXC0 in AVR Memory - will be used as a flag that indicates if there are unread data present in the receive buffer
.equ	UDRE0,0x05					;Defines UDRE0 in AVR Memory - a flag to see if the buffer is empty to see if it is ready to recieve new data
.equ	ADCSRA,0x7A					;Defines ADC status on register A in AVR Memory
.equ	ADMUX,0x7C					;Defines the ADC Multiplexer Selection Register in AVR Memory
.equ	ADCSRB,0x7B					;Defines ADC status on register B in AVR Memory
.equ	DIDR0,0x7E					;Defines the digital Input Disable Register 0 in AVR Memory
.equ	DIDR1,0x7F					;Defines the digital Input Disable Register 1 in AVR Memory
.equ	ADSC,6						;Defines the ADC start conversion bit
.equ	ADIF,4						;Defines the ADC interrupt flag bit
.equ	ADCL,0x78					;Defines the low byte of the result of ADC in AVR Memory
.equ	ADCH,0x79					;Defines the high byte of the result of ADC in AVR Memory
.equ	EECR,0x1F					;Defines the EEPROM control register in AVR Memory
.equ	EEDR,0x20					;Defines the EEPROM data register in AVR Memory
.equ	EEARL,0x21					;Defines the low byte for the EEPROM address register in AVR Memory
.equ	EEARH,0x22					;Defines the low byte for the EEPROM address register in AVR Memory
.equ	EERE,0						;Defines EEPROM read enable bit
.equ	EEPE,1						;Defines EEPROM write enable bit
.equ	EEMPE,2						;Defines EEPROM Master Write Enable bit
.equ	EERIE,3						;Defines EEPROM ready interrupt enable bit

.global HADC				;defines HADC varaiable so it can be used with C code
.global LADC				;defines LADC varaiable so it can be used with C code
.global ASCII				;defines ASCII varaiable so it can be used with C code
.global DATA				;defines DATA varaiable so it can be used with C code

.set	temp,0				;initializes temp to 0

.section ".text"			//equivalent to CSEG 
.global Mega328P_Init
Mega328P_Init:
		ldi	r16,0x07		;PB0(R*W),PB1(RS),PB2(E) as fixed outputs
		out	DDRB,r16		;Sets the first 3 B pins are set to outputs
		ldi	r16,0			;Resets register 16
		out	PORTB,r16		;Sends 0 to port B pins - tells the LCD to not read
		out	U2X0,r16		;initialize UART, 8bits, no parity, 1 stop, 9600
		ldi	r17,0x0			;loads 0 to Register 17
		ldi	r16,0x67		;loads 0x67 to Register 16
		sts	UBRR0H,r17		;sets the high byte of the prescaler for the baud rate to 0 to give byte 00000000
		sts	UBRR0L,r16		;sets the low byte of the prescaler for the baud rate to 103 to give byte 11001110 - 0000000011001110 (103) sets baud rate to 9600 bos
		ldi	r16,24			;loads 24 to Register 16
		sts	UCSR0B,r16		;enables reciever and transmitter and sets UCSZ02 to 0
		ldi	r16,6			;loads 6 to Register 16
		sts	UCSR0C,r16		;selects asynchronous operation mode and sets UCSZ01 and UCSZ00 to 0. This along with UCSZ02 gives '011' which sets the data frame size to 8-bits
		ldi r16,0x87		//initialize ADC
		sts	ADCSRA,r16		;enables ADC on register A; sets the division factor between the system clock fequency and the input clock to the ADC to 128
		ldi r16,0x40		;loads 0x40 to Register 16
		sts ADMUX,r16		;sets ADC0 as analog input to the ADC; selects right adjusted result; reserves the ADC voltage reference
		ldi r16,0			;loads 0 to Register 16
		sts ADCSRB,r16		;selects free running mode for tigger source for ADC
		ldi r16,0xFE		;loads 0xFE to Register 16
		sts DIDR0,r16		;disables pins 1-7 on Register 0
		ldi r16,0xFF		;loads 0xFF to Register 16
		sts DIDR1,r16		;disables pins 0 and 1 on Register 1
		ret					;returns to the line after Mega328P_Init is called
	
.global LCD_Write_Command
LCD_Write_Command:
	call	UART_Off		;calls UART_Off subroutine to disable the receiver and transmitter
	ldi		r16,0xFF		;PD0 - PD7 as outputs
	out		DDRD,r16		;sets PD0 - PD7 pins to outputs
	lds		r16,DATA		;loads DATA to register 16
	out		PORTD,r16		;Sending DATA to outputs pins on PPORTD
	ldi		r16,4			;loading 4 to register 16
	out		PORTB,r16		;sending 00000(100) to Port B - tells LCD to read from the D pins
	call	LCD_Delay		;Calls LCD_Delay to Delay program by decrementing counters
	ldi		r16,0			;resets register 16 to 0
	out		PORTB,r16		;sets all PB pins to logic level low - tell sLCD to not read
	call	LCD_Delay		;Calls LCD_Delay to Delay program by decrementing counters
	call	UART_On			;calls UART_On Subroutine to enable the receiver and transmitter
	ret						;returns to line after LCD_Write_Command is 
  
LCD_Delay:
	ldi		r16,0xFA		;load 0xFA to register 16 - sets up counter
D0:	ldi		r17,0xFF		;load 0xFF to register 17 - sets up counter
D1:	dec		r17				;decrements r17 counter
	brne	D1				;keeps decrementing r17 to delay program until it equals 0
	dec		r16				;decrements r16 counter to delay program
	brne	D0				;restarts r17 counter and keeps decrementing r16 to delay program until it equals 0
	ret						;returns to line after LCD_Delay is called

.global LCD_Write_Data
LCD_Write_Data:
	call	UART_Off		//turn off communication
	ldi		r16,0xFF		;loads 0xFF to register 16
	out		DDRD,r16		;sets all PD pins to outputs
	lds		r16,DATA		;loads DATA to register 16
	out		PORTD,r16		;sends DATA to PD pins
	ldi		r16,6			;load 6 to register 16
	out		PORTB,r16		;sends 00000(110) to PB pins - tells the LCD to read from D pins
	call	LCD_Delay		;calls LCD_Delay to felay the program by decrementing counters
	ldi		r16,0			;load 0 to register 16
	out		PORTB,r16		;loads 00000(000) to PB pins - tells LCD to not read from Dp pins
	call	LCD_Delay		;;calls LCD_Delay to felay the program by decrementing counters
	call	UART_On			//turn on the communication
	ret						;returns to line after LCD_Write_Data is called

.global LCD_Read_Data
LCD_Read_Data:
	call	UART_Off		;Calls UART_Off to disable the UART receiver and transmitter
	ldi		r16,0x00		;loads 0 to register 16
	out		DDRD,r16		;sets all PD pins to inputs
	out		PORTB,4			;sends logic 1 on PB2 pin and 0 on PB1-0 - tells LCD to Write to PortD
	in		r16,PORTD		;loads data on PD input pins into register 16
	sts		DATA,r16		;stores the data recieved from PD pins to DATA in SRAM
	out		PORTB,0			;sends logic 0 on all PB pins  - LCD from reading
	call	UART_On			;Calls UART_Off to enables the UART receiver and transmitter
	ret						;returns to the line after LCD_Read_Data is called

.global UART_On
UART_On:
	ldi		r16,2				;loads 2 into register 16  
	out		DDRD,r16			;sets the PD2 pin as an output and the other D pins as inputs
	ldi		r16,24				;loads 24 into register 16
	sts		UCSR0B,r16			;enables the receiver and transmitter
	ret							;returns to the line after UART_On is called

.global UART_Off
UART_Off:
	ldi	r16,0					;loads 0 to register 16
	sts UCSR0B,r16				;disables receiver and transmitter
	ret							;returns to the line after UART_Off is called

.global UART_Clear
UART_Clear:
	lds		r16,UCSR0A			;loads data from UCSR0A to register 16
	sbrs	r16,RXC0			;skips ret if there is unread data present in the receive buffer
	ret							;returns if there is not unread data present in the receive buffer
	lds		r16,UDR0			;reads data from UDR0
	rjmp	UART_Clear			;returns to UART_Clear to see if there is any more unread data

.global UART_Get
UART_Get:
	lds		r16,UCSR0A			;loads data from UCSR0A to register 16
	sbrs	r16,RXC0			;skips the jump to UART_PUT if there is unread data present in the receive buffer
	rjmp	UART_Get			;jumps to UART_Get if there is not unread data present in the receive buffer
	lds		r16,UDR0			;read USART data register 0 data into register 16
	sts		ASCII,r16			;store USART data register 0 data that is in register 16 to ASCII for later use
	ret							;returns to the line after UART_Get was called

.global UART_Put
UART_Put:
	lds		r17,UCSR0A			;loads data from UCSR0A to register 17
	sbrs	r17,UDRE0			;skips the jump to UART_PUT if the buffer is empty and the ready to recieve data
	rjmp	UART_Put			;jumps to UART_PUT if the buffer is not empty and the ready to recieve data
	lds		r16,ASCII			;loads ASCII data into register 16
	sts		UDR0,r16			;loads ASCII value into USART DATA Register 0 for later
	ret							;returns to the line after UART_Put was called
  
.global ADC_Get
ADC_Get:
		ldi		r16,0xC7			;loads 11000111 into register 16
		sts		ADCSRA,r16			;enables the ADC, starts the conversion, sets the division factor between the system clock frequency nd the input clock frequency to the ADC to 128 ADPS
A2V1:	lds		r16,ADCSRA			;loads ADCSRA register into register 16
		sbrc	r16,ADSC			;if the coversion is started, skip the jump to A2V1
		rjmp 	A2V1				;if the conversion is not started, jump to A2V1
		lds		r16,ADCL			;the low byte of result from the conversion is read from ADCL and written into r16
		sts		LADC,r16			;loads low byte result to the LADC variable in SRAM
		lds		r16,ADCH			;the high byte of result from the conversion is read from ADCH and written into r16
		sts		HADC,r16			;loads high byte result to the HADC variable in SRAM
		ret							;returns to the line after ADC_Get is called

.global EEPROM_Write
EEPROM_Write:      
		sbic    EECR,EEPE
		rjmp    EEPROM_Write		; Wait for completion of previous write
		ldi		r18,0x00			; Set up address (r18:r17) in address register
		ldi		r17,
		 
		ldi		r16,'F'				; Set up data in r16    
		out     EEARH, r18      
		out     EEARL, r17			      
		out     EEDR,r16			; Write data (r16) to Data Register  
		sbi     EECR,EEMPE			; Write logical one to EEMPE
		sbi     EECR,EEPE			; Start eeprom write by setting EEPE
		ret 

.global EEPROM_Read
EEPROM_Read:					    
		sbic    EECR,EEPE    
		rjmp    EEPROM_Read		; Wait for completion of previous write
		ldi		r18,0x00		; Set up address (r18:r17) in EEPROM address register
		ldi		r17,0x05
		ldi		r16,0x00   
		out     EEARH, r18   
		out     EEARL, r17		   
		sbi     EECR,EERE		; Start eeprom read by writing EERE
		in      r16,EEDR		; Read data from Data Register
		sts		ASCII,r16  
		ret

		.end

