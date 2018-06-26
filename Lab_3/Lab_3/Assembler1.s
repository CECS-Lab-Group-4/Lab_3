 // Lab3P1.s
 //
 // Created: 1/30/2018 4:15:16 AM
 // Author : Eugene Rockey
 // Copyright 2018, All Rights Reserved

		;Begin here Will C.

.section ".data"					//equivalent to DSEG
.equ	DDRB,0x04					;equates DDRB to the address in SRAM that determines whether a B pin is an input or utput
.equ	DDRD,0x0A					;equates DDRD to the address in SRAM that determines whether a D pin is an input or utput
.equ	PORTB,0x05					;equates PORTB to the address in SRAM that determines what a B pin in outputting
.equ	PORTD,0x0B					;equates PORTD to the address in SRAM that determines what a D pin in outputting
.equ	U2X0,1						;doubles the transmittion speed for USART
.equ	UBRR0L,0xC4					;equates UBRR0L to the address in SRAM that sets the low byte of the prescaler for the baud rate
.equ	UBRR0H,0xC5					;equates UBRR0L to the address in SRAM that sets the high byte of the prescaler for the baud rate
.equ	UCSR0A,0xC0					;equates UCSR0A to the address in SRAM for status register A
.equ	UCSR0B,0xC1					;equates UCSR0B to the address in SRAM for status register B
.equ	UCSR0C,0xC2					;equates UCSR0C to the address in SRAM for status register C
.equ	UDR0,0xC6					//student comment here
.equ	RXC0,0x07					//student comment here
.equ	UDRE0,0x05					//student comment here
.equ	ADCSRA,0x7A					;equates ADCSRA to the address in SRAM for ADC status on register A
.equ	ADMUX,0x7C					;equates ADMUX to the address in SRAM for the ADC Multiplexer Selection Register
.equ	ADCSRB,0x7B					;equates ADCSRB to the address in SRAM for ADC status on register B
.equ	DIDR0,0x7E					;equates DIDR0 to the address in SRAM for Digital Input Disable Register 0
.equ	DIDR1,0x7F					;equates DIDR1 to the address in SRAM for Digital Input Disable Register 1
.equ	ADSC,6						//student comment here
.equ	ADIF,4						//student comment here
.equ	ADCL,0x78					//student comment here
.equ	ADCH,0x79					//student comment here
.equ	EECR,0x1F					//student comment here
.equ	EEDR,0x20					//student comment here
.equ	EEARL,0x21					//student comment here
.equ	EEARH,0x22					//student comment here
.equ	EERE,0						//student comment here
.equ	EEPE,1						//student comment here
.equ	EEMPE,2						//student comment here
.equ	EERIE,3						//student comment here

.global HADC				//student comment here
.global LADC				//student comment here
.global ASCII				//student comment here
.global DATA				//student comment here

.set	temp,0				//student comment here

		;Begin here Nathan B.

.section ".text"			//equivalent to CSEG 
.global Mega328P_Init
Mega328P_Init:
		ldi	r16,0x07		;PB0(R*W),PB1(RS),PB2(E) as fixed outputs
		out	DDRB,r16		;Sets the first 3 B pins are set to outputs
		ldi	r16,0			;Resets register 16
		out	PORTB,r16		;Sends 0 to port B pins
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
	call	UART_Off		//student comment here
	ldi		r16,0xFF		;PD0 - PD7 as outputs
	out		DDRD,r16		//student comment here
	lds		r16,DATA		//student comment here
	out		PORTD,r16		//student comment here
	ldi		r16,4			//student comment here
	out		PORTB,r16		//student comment here
	call	LCD_Delay		//student comment here
	ldi		r16,0			//student comment here
	out		PORTB,r16		//student comment here
	call	LCD_Delay		//student comment here
	call	UART_On			//student comment here
	ret						//student comment here

	;Begin here, Laith

LCD_Delay:
	ldi		r16,0xFA		//student comment here
D0:	ldi		r17,0xFF		//student comment here
D1:	dec		r17				//student comment here
	brne	D1				//student comment here
	dec		r16				//student comment here
	brne	D0				//student comment here
	ret						//student comment here

.global LCD_Write_Data
LCD_Write_Data:
	call	UART_Off		//turn off communication
	ldi		r16,0xFF		//student comment here
	out		DDRD,r16		//student comment here
	lds		r16,DATA		//student comment here
	out		PORTD,r16		//student comment here
	ldi		r16,6			//student comment here
	out		PORTB,r16		//student comment here
	call	LCD_Delay		//student comment here
	ldi		r16,0			//student comment here
	out		PORTB,r16		//student comment here
	call	LCD_Delay		//student comment here
	call	UART_On			//turn on the communication
	ret						//student comment here

.global LCD_Read_Data
LCD_Read_Data:
	call	UART_Off		//student comment here
	ldi		r16,0x00		//student comment here
	out		DDRD,r16		//student comment here
	out		PORTB,4			//student comment here
	in		r16,PORTD		//student comment here
	sts		DATA,r16		//student comment here
	out		PORTB,0			//student comment here
	call	UART_On			//student comment here
	ret						//student comment here

	;Begin here Nathan H.

.global UART_On
UART_On:
	ldi		r16,2				//student comment here
	out		DDRD,r16			//student comment here
	ldi		r16,24				//student comment here
	sts		UCSR0B,r16			//student comment here
	ret							//student comment here

.global UART_Off
UART_Off:
	ldi	r16,0					//student comment here
	sts UCSR0B,r16				//student comment here
	ret							//student comment here

.global UART_Clear
UART_Clear:
	lds		r16,UCSR0A			//student comment here
	sbrs	r16,RXC0			//student comment here
	ret							//student comment here
	lds		r16,UDR0			//student comment here
	rjmp	UART_Clear			//student comment here

.global UART_Get
UART_Get:
	lds		r16,UCSR0A			//student comment here
	sbrs	r16,RXC0			//student comment here
	rjmp	UART_Get			//student comment here
	lds		r16,UDR0			//student comment here
	sts		ASCII,r16			//student comment here
	ret							//student comment here

.global UART_Put
UART_Put:
	lds		r17,UCSR0A			//student comment here
	sbrs	r17,UDRE0			//student comment here
	rjmp	UART_Put			//student comment here
	lds		r16,ASCII			//student comment here
	sts		UDR0,r16			//student comment here
	ret							//student comment here

	;Begin here, Dhan

.global ADC_Get
ADC_Get:
		ldi		r16,0xC7			//student comment here
		sts		ADCSRA,r16			//student comment here
A2V1:	lds		r16,ADCSRA			//student comment here
		sbrc	r16,ADSC			//student comment here
		rjmp 	A2V1				//student comment here
		lds		r16,ADCL			//student comment here
		sts		LADC,r16			//student comment here
		lds		r16,ADCH			//student comment here
		sts		HADC,r16			//student comment here
		ret							//student comment here

.global EEPROM_Write
EEPROM_Write:      
		sbic    EECR,EEPE
		rjmp    EEPROM_Write		; Wait for completion of previous write
		ldi		r18,0x00			; Set up address (r18:r17) in address register
		ldi		r17,0x05 
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
		//End Dhan

		.end

