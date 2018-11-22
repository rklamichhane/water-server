#include<avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>
#define data PORTD
#define ctrl PORTB
#define en PB0
#define rs PB1
#define bithigh() (bit_is_set(PINC,PC0))
#define bithigh1() (bit_is_set(PINC,PC1))
#define F_CPU 16000000
#define level_low 7
#define level_high 4
#define temp_high 35
#define temp_low 30
#define hot_water_time 5000
#define cold_water_time 5000
unsigned int key,d1,d2,d3;
unsigned char value[]={'0','1','2','3','4','5','6','7','8','9'};
static volatile long int pulse=0;
static volatile int i=0;
void lcd_cmd(unsigned char cmd)
{
   PORTB&=~(1<<rs);
   data=cmd&0xf0;
   ctrl|=(1<<en);
   _delay_us(1000);
   ctrl&=~(1<<en);
   _delay_us(10000);
   data=cmd<<4;
   ctrl|=(1<<en);
   _delay_us(1000);
   ctrl&=~(1<<en);
   _delay_us(1000);
}
void lcd_data(unsigned char dta)
{
   ctrl|=(1<<rs);
   data=dta&0xf0;
   ctrl|=(1<<en);
   _delay_us(1000);
   ctrl&=~(1<<en);
   _delay_us(1000);
   data=dta<<4;
   ctrl|=(1<<en);
   _delay_us(1000);
   ctrl&=~(1<<en);
   _delay_us(1000);
}
void lcd_init()
{
   DDRD=0xff;
   DDRB=0xff;
   ctrl&=~(1<<en);
   _delay_us(1000);
   lcd_cmd(0x33);
   lcd_cmd(0x32);
   lcd_cmd(0x28);
   lcd_cmd(0x0e);
   _delay_us(2000);
   lcd_cmd(0x06);
   _delay_us(1000);
}
void lcd_gotoxy(unsigned char x, unsigned char y)
{
   unsigned char add[]={0x80,0xC0};
   lcd_cmd(add[x-1]+y-1);
   _delay_us(1000);
}
void lcd_strng( char *str)
{
   unsigned char i=0;
   while(str[i]!=0)
   {
      lcd_data(str[i]);
      i++;
   }
}
void lcd_intg(int a,unsigned int b)
{
 
   int d1[5],i=0;
   for(i=0;i<b;i++)
   {
   d1[i]=a%10;
   a/=10;
   
   }
   for(i=b;i>=0;i--)
   lcd_data(value[d1[i]]);
}


void get_range()
{
  //static volatile unsigned int value[10]={'0','1','2','3','4','5','6','7','8','9'};
    //  _delay_ms(500);
     // COUNTA=COUNTA/16;
     // _delay_ms(200);
   //int16_t COUNTA;
      PORTD|=(1<<PD1);
      _delay_us(15);
      PORTD&=~(1<<PD1);
      _delay_ms(100);
     // COUNTA=pulse/(2*58)+1;
     /* lcd_cmd(0xc0);
      d1=COUNTA%10;
       COUNTA=COUNTA/10;
      d2=COUNTA%10;
      COUNTA=COUNTA/10;
      d3=COUNTA%10;
      COUNTA=COUNTA/10;
      d4=COUNTA%10;
      COUNTA=COUNTA/10;
      d5=COUNTA%10;
      lcd_data(value[d5]);
      lcd_data(value[d4]);
      lcd_data(value[d3]);
      lcd_data(value[d2]);
      lcd_data(value[d1]);
      lcd_strng(" cm");
      */
   //return COUNTA;
}


void adc_init()
{
    ADMUX|=(1<<REFS0)|(1<<REFS1)|(1<<ADLAR)|(1<<MUX0);
   //ADMUX=0X87;
    ADCSRA|=(1<<ADEN)|(1<<ADPS0)|(ADPS1)|(1<<ADPS2);
 //ADCSRA=0XE0;
}
 uint8_t get_adc(uint8_t ch)
{
 char temp;
   ch=(ch&0b00000111);
    ADMUX=(ADMUX&0XE0)|ch;
      ADCSRA|=(1<<ADSC);
    while((ADCSRA&(1<<ADIF))==0){};
    ADCSRA|=(1<<ADIF);
       temp=ADCH;
    //lcd_cmd1();
    //lcd_intg(temp,2);
    //_delay_ms(100);
    return temp;
 }
 void cold_water(unsigned char a)
 {
    if(a==1)
    {
       PORTC|=(1<<PC2);
    }
    else
    {
       PORTC&=~(1<<PC2);
    }
 }
 void hot_water(unsigned char a)
 {
    if(a==1)
    {
       PORTC|=(1<<PC3);
    }
    else
    {
       PORTC&=~(1<<PC3);
    }
 }
void heat_water(unsigned char a)
 {
    if(a==1)
    {
       PORTC|=(1<<PC4);
    }
    else
    {
       PORTC&=~(1<<PC4);
    }
 }
 void supply_water(unsigned char a)
 {
    if(a==1)
    {
       PORTC|=(1<<PC5);
    }
    else
    {
       PORTC&=~(1<<PC5);
    }
 }



int main()
{
   int temp,range;
   ///////////////////////////////////////////////
  DDRD|=(1<<PD1);
  DDRD&=~(1<<PD2);
  DDRC=0X3F;
  // lcd_init();
   GICR|=(1<<INT0);
   MCUCR|=(1<<ISC00);
    sei();
  int32_t COUNTA=0;
   sei();
   lcd_init();
   adc_init();
   while(1)
   {
   ///////////////////////////////////
   lcd_strng("   WATER SERVER"   );
   lcd_cmd(0xc0);
   lcd_strng("PLZ INSERT COIN");
   if(bithigh())
   {
      supply_water(0);
      temp=get_adc(2);
      while(temp>=100)
      { 
	 lcd_cmd(0x01);
	 _delay_ms(10);
	 lcd_strng("PLEASE");
	 lcd_cmd(0xc0);
	 _delay_ms(10);
	 lcd_strng("INSERT GLASS");
          temp=get_adc(2);
      }
      lcd_cmd(0x01);
      _delay_ms(10);
      lcd_strng("serving cold water");
      cold_water(1);
      _delay_ms(cold_water_time);
      cold_water(0);
       lcd_cmd(0x01);
      _delay_ms(10);
      lcd_strng("THANX FOR COMING");
      lcd_cmd(0xc0);
      lcd_strng("HAVE A GREAT DAY");
   }
  else if(bithigh1())
   {
      supply_water(0);
      temp=get_adc(2);
      while(temp>=100)
      { 
	 lcd_cmd(0x01);
	 _delay_ms(10);
	 lcd_strng("PLEASE");
	 lcd_cmd(0xc0);
	 _delay_ms(10);
	 lcd_strng("INSERT GLASS");
          temp=get_adc(2);
      }
      lcd_cmd(0x01);
      _delay_ms(10);
      lcd_strng("serving hot water");
      hot_water(1);
      _delay_ms(hot_water_time);
      cold_water(0);
      lcd_cmd(0x01);
      _delay_ms(10);
      lcd_strng("THANX FOR COMING");
      lcd_cmd(0xc0);
      lcd_strng("HAVE A GREAT DAY");
   }
 temp=get_adc(1);
  get_range();
  range=pulse/(2*58)+1;
  if(temp<=temp_low)
     heat_water(1);
  else if(temp>=temp_high)
     heat_water(0);
  if(range<=level_low)
     supply_water(1);
  if(range>=level_high)
     supply_water(0);
 
   
   
   }
   
   lcd_strng("range      temp");
   while(1)
   {
      lcd_gotoxy(2,1);
     get_range();
     range=pulse/(2*58)+1;
      lcd_intg(range,2);
      temp=get_adc(1);
      lcd_gotoxy(2,13);
      lcd_intg(temp,2);
   }
    
    return 0;
   
}
      
ISR(INT0_vect)
{
  // lcd_cmd(0x01);
  // _delay_ms(100);
  // lcd_strng("hh");
   if(i==1)
   {
       TCCR1B=0;
      
   pulse=TCNT1;
      
      TCNT1=0;
   i=0;
   }
   if(i==0)
   {
      TCCR1B|=(1<<CS11);
      i=1;     
   }
      
}

   
   
