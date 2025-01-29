#include <reg51.h>
	//PORT FOR LCD
#define lcd P1
//PIN FOR LCD            
//PIN FOR REGISTER
sbit rs = P2 ^ 0;
//PIN FOR ENABLE
sbit e = P2 ^ 1;
//PIN FOR KEYPAD
sbit col1 = P2 ^ 3;
sbit col2 = P2 ^ 7;
sbit col3 = P3 ^ 3;
sbit row1 = P2 ^ 2;
sbit row2 = P2 ^ 4;
sbit row3 = P2 ^ 5;
sbit row4 = P2 ^ 6;
//PIN FOR STREET LIGHTS
sbit LAMP = P3 ^ 2;
//PROGRAM FUNCTIONS PROTOTYPING
void delay(int);
void cmd(char);
void display(char);
void string(char*);
void init(void);
void TimerDelay();	//TIMER DELAY
void TimerDelay()	//Generating 65535 * 1.085us=71.1ms delay
{
	TL0 = 00;
	TH0 = 00;
	TR0 = 1;
	while (TF0 == 0);
	TR0 = 0;
	TF0 = 0;

}

void delay(int d)	//DELAY
{
	unsigned char i;
	for (; d > 0; d--)
	{
		for (i = 250; i > 0; i--);
		for (i = 248; i > 0; i--);
	}
}

void cmd(char c)	//COMMAND FUNCTION FOR LCD
{
	lcd = c;
	rs = 0;
	e = 1;
	delay(5);
	e = 0;
}

void display(char c)	//DISPLAY FUNCTION FOR LCD
{
	lcd = c;
	rs = 1;
	e = 1;
	delay(1);
	e = 0;
}

void string(char *p)	//STRING FUNCTION FOR LCD
{
	while (*p)
	{
		display(*p++);
	}
}

void init(void)	//LCD INITIALIZATION
{
	unsigned char x;
	cmd(0x0c);	//display on
	cmd(0x38);	//use two lines
	cmd(0x01);	// Clearing of screen
	cmd(0x80);	//BEGINNING OF THE FIRST LINE
	string("Starting...");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}

	cmd(0x01);
	string("REQUESTING");
	cmd(0xc0);
	string(".");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}

	string(".");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}	//1S delay
	string(".");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}	//1s delay
	string(".");
	for (x = 0; x < 13; x++)
	{
		TimerDelay();
	}	//1s delay
	cmd(0x01);	// beginning of first line
}

void sensor1()	//ACCEPTING DATA OF CHANNEL 1 FROM SLAVE FUNCTION
{
	unsigned char a;
	string("VOLTAGE     ");
	cmd(0xc0);

	while (RI == 0);	//RECIVING DATA FROM SLAVE AND STORING IN SBUF
	a = SBUF;
	RI = 0;
	display(a);	//DISPLAYING SBUF DATA ON LCD
	string(" V  ");
	cmd(0x80);
}

void sensor2()	//ACCEPTING DATA OF CHANNEL 2 FROM SLAVE FUNCTION
{
	unsigned char a, b;

	string("PRESSURE     ");
	cmd(0xc0);
	while (RI == 0);	//RECEIVING TENTH DATA
	a = SBUF;
	RI = 0;
	while (RI == 0);	//RECEIVING TENTH DATA
	b = SBUF;
	RI = 0;

	display(a);
	display(b);	//DISPLAYING TENTH DATA
	//DISPLAYING UNITE DATA
	string("KP");

	cmd(0x80);

}

void sensor3()	//ACCEPTING DATA OF CHANNEL 2 FROM SLAVE FUNCTION
{
	unsigned char a, b;
	string("TEMPERATURE   ");
	cmd(0xc0);
	while (RI == 0);	//RECEIVING TENTH DATA
	a = SBUF;
	RI = 0;
	while (RI == 0);	//RECEIVING TENTH DATA
	b = SBUF;
	RI = 0;
	display(a);
	display(b);	//DISPLAYING TENTH DATA
	//DISPLAYING UNITE DATA
	string("C  ");
	cmd(0x80);
}

unsigned char keypad()	//KEYPAD FUNCTION
{
	unsigned char a;
	col1 = 0, col2 = 1, col3 = 1;	//ENABLING COLOUME 1
	if (row1 == 0)	//IF KEY 1 IS PRESSED SEND 'V' TO SLAVE
	{
		SBUF = 'V';
		while (TI == 0);
		TI = 0;
		a = 1;
	}

	if (row2 == 0)	//IF KEY4 IS PRESSED SEND 'P' TO SLAVE
	{
		SBUF = 'P';
		while (TI == 0);
		TI = 0;
		a = 2;
	}

	if (row3 == 0)	//IF KEY7 IS PRESSED SEND 'T' TO SLAVE
	{
		SBUF = 'T';
		while (TI == 0);
		TI = 0;
		a = 3;
	}

	if (row4 == 0)	//IF KEY 2 IS PRESSED ON STREET LIGHTS
	{
		SBUF = '*';
		while (TI == 0);
		TI = 0;
		a = 15;
	}

	col1 = 1, col2 = 0, col3 = 1;	//ENABLING COLOUME 2
	if (row1 == 0)	//IF KEY 2 IS PRESSED OFF STREET LIGHTS
	{
		a = 16;
	}

	return a;	//RETURN a
}

void main()	//MAIN FUNCTION
{
	LAMP = 0;	//INITIALING LAMP
	TMOD = 0x21;	//TIMER1 IN MODE 2(8 BIT AUTORELOAD) AND TIMER 1 IN MODE 1(16BIT)
	TH1 = 0xFD;	//BAUD RATE OF 9600
	SCON = 0x50;	//SCON REGISTER WITH MODE 2(8 DATA BITS AND 2 STOP AND START BIT)
	TR1 = 1;	//STARTING TIMER 1

	init();	//INITIALIZING LCD

	while (1)	//FOREVER EHILE LOOP
	{
		if (keypad() == 1)	//IF 1 IS PRESSED SEND SIGNAL TO SLAVE  AND THEN RECEIVE SIGNAL OF VOLTAGE SENSOR
		{
			sensor1();
		}

		if (keypad() == 2)	//IF 4 IS PRESSED SEND SIGNAL TO SLAVE  AND THEN RECEIVE SIGNAL OF PRESSURE SENSOR
		{
			sensor2();
		}

		if (keypad() == 3)	//IF 7 IS PRESSED SEND SIGNAL TO SLAVE  AND THEN RECEIVE SIGNAL OF TEMPRATURE SENSOR
		{
			sensor3();
		}

		if (keypad() == 15)	//IF *IS PRESSED ON STREET LIGHTS
		{
			LAMP = 1;
		}

		if (keypad() == 16)	//IF 2 IS PRESSED OFF STREET LIGHTS
		{
			LAMP = 0;
		}
	}
}
