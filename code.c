#include <lpc214x.h> 
#include "TIMER.H" 
#include "ULTRASONIC.H" 
#include "LCD.H" 
#define EN (1<<16)|(1<<17) // Motor enable pins on P1.16 and P1.17 int cent = 100; unsigned int range, i, buffer = 0; unsigned int get_bufreg(unsigned int trange) {     if (trange <= 100 && trange >= 50) return 1;     else if (trange < 50) return 0;     else return 2; 
} 
int main() { 
    VPBDIV = 0x01;                 // PCLK = 60MHz 
    IO0DIR |= 0x00FF3400;          // LCD and Ultrasonic pins (P0.8 - P0.11, P0.12 - P0.23)     IO1DIR = EN;                   // Motor control pins     ultrasonic_init();     lcd_init(); 
    show("Distance :");     IOSET1 = EN;     while (1) { 
        cmd(0x8A); // Move cursor to second line         range = get_range(); 
        // Display the measured distance         dat((range / 100) % 10 + '0');         dat((range / 10) % 10 + '0');         dat((range % 10) + '0'); 
        show(" cm ");         if (range < 50) {             IOCLR1 = EN;             cmd(0xC0);             show("CRASH WARNING  "); 
        } 
        else if (range <= 100 && range >= 50) {             if ((IO1PIN & EN) == EN) {                 cmd(0xC0);                 show("CRASH WARNING  "); 
                cent = range;                 for (i = 0; i < 50; i++) { 
                    if (get_bufreg(buffer) == get_bufreg(range)) continue; 
                    else {                         while (cent != 0) {                             range = get_range();                             if (range <= 100 && range >= 50) {                                 IOSET1 = EN;                                 timer1delay(cent * 200);                                 IOCLR1 = EN;                                 timer1delay((100 - cent) * 200); 
                                cent--; 
                            } else break; 
                        } 
                    } 
                } 
                IOCLR1 = EN; 
            } else {                 IOCLR1 = EN;                 cmd(0xC0);                 show("CRASH WARNING  ");             } 
            buffer = range;             range = get_range(); 
        } 
        if (range > 100) {             IOSET1 = EN;             cmd(0xC0);             show("You are safe :) "); 
        } 
    } 
    return 0; 
} 
//Time.h void timer0delay(unsigned int a); void timer1delay(unsigned int b); void timer0delay(unsigned int a)    //1ms 
{ 
    T0TCR=0X02; 
    T0CTCR=0X0000; 
    T0PR=59999; 
    T0MR0=a; 
    T0MCR=0x00000004;     T0TCR=0X01;     while(T0TC!=T0MR0); 
    T0TC=0; 
} 
void timer1delay(unsigned int b)   //1us 
{ 
    T1TCR=0X02; 
T1CTCR=0X0000; 
T1PR=59; 
T1MR0=b; 
T1MCR=0x00000004; T1TCR=0X01; while(T1TC!=T1MR0); 
    T1TC=0; 
} 
 
//Ultrasonic.h 
#define trig (1<<11)             //P0.8 
#define echo (IO0PIN&(1<<9))         //P0.9 as EINT3 void ultrasonic_init(); void send_pulse(); unsigned int get_range(); void ultrasonic_init() 
{ 
    IO0DIR|=trig; 
    T0CTCR=0; 
    T0PR=59; 
} 
void send_pulse() 
{ 
    T0TC=T0PC=0; 
    IO0SET=trig;                            //trig=1     timer1delay(10);                        //10us delay 
    IO0CLR=trig;                            //trig=0 
} 
unsigned int get_range() 
{ 
    unsigned int get=0; send_pulse(); while(!echo); T0TCR=0x01; 
while(echo); T0TCR=0; get=T0TC;     //if(get<38000)         get=get/59;     /*else         get=0;*/     return get; 
}  
//LCD.h #define bit(x) (1<<x) void lcd_init(); unsigned char a; void cmd(unsigned char a); void dat(unsigned char b); void show(unsigned char *s); void lcd_delay(); void lcd_init() 
{ 
    cmd(0x38);     cmd(0x0e);     cmd(0x06);     cmd(0x0c);     cmd(0x80); 
} 
void cmd(unsigned char a) 
{ 
IO0PIN &= 0x00; 
IO0PIN|=(a<<16); 
IO0CLR|=bit(10);                   //rs=0 
IO0CLR|=bit(12);                   //rw=0 IO0SET|=bit(13);                    //en=1 
lcd_delay(); 
    IO0CLR|=bit(13);                    //en=0 
} 
void dat(unsigned char b) 
{ 
    IO0PIN &= 0x00; 
    IO0PIN|=(b<<16); 
    IO0SET|=bit(10);                   //rs=1 
    IO0CLR|=bit(12);                   //rw=0     IO0SET|=bit(13);                    //en=1     lcd_delay(); 
    IO0CLR|=bit(13);                    //en=0 
} 
void show(unsigned char *s) 
{     while(*s) {         dat(*s++); 
    } 
} 
void lcd_delay(){     unsigned int i;     for(i=0;i<=1000;i++); 
} 
