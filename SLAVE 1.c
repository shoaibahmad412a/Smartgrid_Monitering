#include <reg51.h>
	//ACCEPTING DATA FROM ADC808 THROUGH PORT P1
#define output P1
//CHANNEL SELECTING PINS OF ADC808
sbit add_a = P2 ^ 0;
sbit add_b = P2 ^ 1;
sbit add_c = P2 ^ 2;
//CHANNEL ENABLING PIN OF ADC808
sbit al = P2 ^ 3;
//READING CONVERTING DATA FROM ANALOG TO DIGITAL OF ADC808
sbit rd = P2 ^ 4;
//START CONVERTING DATA OF ADC808
sbit wr = P2 ^ 5;
//WAITING FOR CONVERTED DATA TO FINISH PIN
sbit INTR = P2 ^ 6;
//PIN FOR STEPPER MOTOR
sbit winding1 = P3 ^ 4;
sbit winding2 = P3 ^ 5;
sbit winding3 = P3 ^ 6;
sbit winding4 = P3 ^ 7;
//PIN FOR MOTOR
sbit motor = P3 ^ 2;
//PROTOTYPING OF FUNCTIONS
void condition1(unsigned char);
void condition2(unsigned char);
void condition3(unsigned char);

unsigned char adc1();
unsigned char adc2();
unsigned char adc3();
//PROGRAM FUNCTIONS
void TimerDelay();

void rec() interrupt 4
{
	float a, b, c;

	if (RI == 1)
	{
		if (SBUF == 'V')	//IF SBUF==V THAN READ DATA FROM CHANNEL 1 AND SEND IT TO MASTER
		{
			condition1(adc1());
		}

		if (SBUF == 'P')	//IF SBUF==P THAN READ DATA FROM CHANNEL 2 AND SEND IT TO MASTER
		{
			c = adc2();
			a = (5.0 / 256) *c;	//PRESSURE FORMULA
			b = ((a / 5.0) + 0.09) / 0.009 - 1;	//PRESSURE FORMULA
			condition2(b);
		}

		if (SBUF == 'T')	//IF SBUF==T THAN READ DATA FROM CHANNEL 3 AND SEND IT TO MASTER
		{
			condition3(adc3());
		}

		RI = 0;
		TR0 = 1;
	}

	//RECIEVINF DATA FROM MASTER
}

void TimerDelay()	//Generating 65535 * 1.085us=71.1ms delay
{
	TL0 = 00;	//INITIAL VALUES ARE STARTING FROM ZERO
	TH0 = 00;
	TR0 = 1;	//STARTING TIMER
	while (TF0 == 0);	//WAIT FOR OVER FLOWING TIMER
	TR0 = 0;	//STOPING TIMER
	TF0 = 0;	//FORCING OVER FLOWING
}

unsigned char adc1()	//ANALOG TO DIGITAL CONVERTION OF VOLTAGE SENSOR FUNCTION
{
	unsigned char x;
	add_a = 0;	//SELECTING CHANNEL 0
	add_b = 0;
	add_c = 0;
	al = 1;	//ENABLING CHANNEL 0
	wr = 1;	//STARTING CONVERSION
	al = 0;
	wr = 0;	//STOPING CONVERSION
	while (INTR == 1);	//WAITING FOR CONVERSION TO STOP
	while (INTR == 0);
	rd = 1;	//READING CONVERTED DATA
	x = output;	//COPING CONVERTED DATA TO VARIABLE X
	rd = 0;	//STOP READING CONVERTED DATA
	return x;
}	//RETURNING VARIABLE

unsigned char adc2()	//ANALOG TO DIGITAL CONVERTION OF PRESSURE SENSOR FUNCTION
{
	unsigned char x;
	add_a = 1;	//SELECTING CHANNEL 1
	add_b = 0;
	add_c = 0;
	al = 1;
	wr = 1;
	al = 0;
	wr = 0;
	while (INTR == 1);
	while (INTR == 0);
	rd = 1;
	x = output;
	rd = 0;
	return x;
}

unsigned char adc3()	//ANALOG TO DIGITAL CONVERTION OF TEMPRATURE SENSOR FUNCTION
{
	unsigned char x;
	add_a = 0;	//SELECTING CHANNEL 2
	add_b = 1;
	add_c = 0;
	al = 1;
	wr = 1;
	al = 0;
	wr = 0;
	while (INTR == 1);
	while (INTR == 0);
	rd = 1;
	x = output;
	rd = 0;
	return x;
}

void condition1(unsigned char
	var)	//CONDITIONS FOR VOLTAGE SENSOR
{
	var = 5.02 *
		var / 256;	//FORMULA FOR VOLTAGE SENSOR
	SBUF =
		var +0x30;
	while (TI == 0);	//TRANSMITTING DATA TO MASTER
	TI = 0;
}

void condition2(unsigned char
	var)	//CONDITIONS FOR PRESSURE SENSOR
{
	unsigned char unite, tenth;
	tenth =
		var / 10;	//RECEIVING DATA AND EXTRACTING TENTH DIGIT
	tenth = tenth + 0x30;	//CONVERTING TO ASCII
	SBUF = tenth;	//STORING INTO SBUF
	while (TI == 0);
	TI = 0;
	TimerDelay();
	TimerDelay();
	TimerDelay();

	unite =
		var % 10;	//RECEIVING DATA AND EXTRACTING UNITE DIGIT
	unite = unite + 0x30;	//CONVERTING TO ASCII
	SBUF = unite;	//STORING INTO SBUF
	while (TI == 0);
	TI = 0;

}

void condition3(unsigned char
	var)	//CONDITIONS FOR TEMPRATURE SENSOR
{
	unsigned char unite, tenth;
	var =
	var *1.966;	//FORMULA FOR TEMPRATURE SENSOR
	tenth =
		var / 10;
	tenth = tenth + 0x30;
	SBUF = tenth;
	while (TI == 0);
	TI = 0;
	TimerDelay();
	TimerDelay();
	TimerDelay();
	unite =
		var % 10;
	unite = unite + 0x30;
	SBUF = unite;
	while (TI == 0);
	TI = 0;
}

void condition3a(unsigned char
	var)	//CONDITIONS FOR TEMPRATURE SENSOR
{
	unsigned char tenth;
	var =
	var * 2;	//FORMULA FOR TEMPRATURE SENSOR
	tenth =
		var / 10;
	if (tenth > 3)
	{
		motor = 1;
	}
	else
	{
		motor = 0;
	}
}

void condition2a(unsigned char
	var)	//CONDITIONS FOR TEMPRATURE SENSOR
{
	float g, h, tenth;
	g = (5.0 / 256) *
		var;	//PRESSURE FORMULA
	h = ((g / 5.0) + 0.09) / 0.009 - 1;	//FORMULA FOR TEMPRATURE SENSOR
	tenth = h / 10;
	if (tenth >= 4 && tenth < 5)	//IF PRESSURE IS below 50 PASCAL ROTATE STEPPER MOTOR 0 DEGREE CONDITION
	{
		winding1 = 1;
		winding2 = 0;
		winding3 = 0;
		winding4 = 1;
	}

	if (tenth >= 5 && tenth < 6)	//IF PRESSURE IS ABOVE 50 PASCAL ROTATE STEPPER MOTOR 45 DEGREE CONDITION
	{
		winding1 = 0;
		winding2 = 0;
		winding3 = 0;
		winding4 = 1;
	}

	if (tenth >= 6 && tenth < 7)	//IF PRESSURE IS ABOVE 60 PASCAL ROTATE STEPPER MOTOR 45 DEGREE CONDITION
	{
		winding1 = 0;
		winding2 = 0;
		winding3 = 1;
		winding4 = 1;
	}

	if (tenth >= 7 && tenth < 8)	//IF PRESSURE IS ABOVE 70 PASCAL ROTATE STEPPER MOTOR 135 DEGREE CONDITION
	{
		winding1 = 0;
		winding2 = 0;
		winding3 = 1;
		winding4 = 0;
	}

	if (tenth >= 8 && tenth < 9)	//IF PRESSURE IS ABOVE 80 PASCAL ROTATE STEPPER MOTOR 180 DEGREE CONDITION
	{
		winding1 = 0;
		winding2 = 1;
		winding3 = 1;
		winding4 = 0;
	}
}

void main()	//MAIN FUNCTION
{
	//DECLARING VARIABLE
	TimerDelay();	//TIMER DELAY
	INTR = 1;	//INITIALIING ALL VALUES
	rd = 0;
	wr = 0;
	al = 0;
	TMOD = 0x21;	//TIMER1 IN MODE 2(8 BIT AUTORELOAD) AND TIMER 1 IN MODE 1(16BIT)
	TH1 = 0xFD;	//BAUD RATE OF 9600  
	SCON = 0x50;	//SCON REGISTER WITH MODE 2(8 DATA BITS AND 2 STOP AND START BIT)
	IE = 0x90;
	TR1 = 1;	//STARTING TIMER 1

	condition2a(adc2());
	condition3a(adc3());
}
