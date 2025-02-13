#include<P18f2580.h>
#include<delays.h>
#include<stdio.h>
#include<i2c.h>

#pragma config OSC = HS  
#pragma config MCLRE = ON  
#pragma config WDT = OFF  
#pragma config PBADEN = OFF  
#pragma config PWRT=ON   
#pragma config BOREN=OFF   
#pragma config LVP=OFF  

#define  onewire_pin  PORTAbits.RA0 
#define  onewire_pin_tris  TRISAbits.TRISA0 
#define skip_rom 0xcc
#define convert_t 0x44
#define read_scratchpad 0xbe
#define Dia_chi_LCD 0x4E
#define BT1 PORTBbits.RB0
#define BT2 PORTBbits.RB1
#define BT3 PORTBbits.RB2

char DQ,presence_pulse;
char M[40];
unsigned int temp,dig_value;
int pwm_value;
unsigned int dem=20;
int a;

void ngat_ngoai(void);
#pragma code uu_tien_cao = 0x08 
  
void ngat_cao(void)  
{ 
 ngat_ngoai();
} 

#pragma code 
#pragma interrupt ngat_ngoai 

void ngat_ngoai(void) 
{ 
  if(INTCONbits.INT0IF==1)
  {
      dem++;
      INTCONbits.INT0IF=0;
  }
  else
  {
      dem--;
      INTCON3bits.INT1IF=0;
  }  
}
void onewire_write_bit1() 
{ 
onewire_pin_tris=0;    
onewire_pin=0; 
Delay10TCYx(1);    
onewire_pin_tris=1;    
Delay10TCYx(7);  
} 
//******************************************************** 
void onewire_write_bit0() 
{ 
onewire_pin_tris=0; 
onewire_pin=0; 
Delay10TCYx(7);    
onewire_pin_tris=1;    
Delay10TCYx(1);     
} 
//******************************************************** 
void  onewire_read_bit() 
{ 
onewire_pin_tris=0; 
onewire_pin=0; 
Delay10TCYx(1); 
onewire_pin_tris=1;      
Delay10TCYx(1); 
DQ=onewire_pin;         
Delay10TCYx(5); 
}     
//******************************************************** 
void onewire_reset() 
{   
onewire_pin_tris=0;  
onewire_pin=0; 
Delay10TCYx(50); 
onewire_pin_tris=1;  
Delay10TCYx(7);   
presence_pulse=onewire_pin;  
Delay10TCYx(41); 
}
//******************************************************** 
void onewire_write_cmd_byte(unsigned char cmd_byte) 
{ 
unsigned char i=0; 
for(i=0;i<8;i++)      
{   
DQ = cmd_byte&0x01;     
if(DQ) 
onewire_write_bit1(); 
else onewire_write_bit0();      
cmd_byte>>=1;      
}       
} 
//******************************************************** 
unsigned char onewire_read_data_byte() 
{ 
unsigned char i,tam=1; 
unsigned char data_byte=0; 
for(i=0;i<8;i++)     
{  
onewire_read_bit();     
if(DQ) 
{ 
data_byte+=tam; 
} 
tam*=2;         
}
return(data_byte); 
} 
//******************************************************** 
unsigned int ds18b20_read(void) 
{ 
unsigned int temp1,temp2,digital_value; 
onewire_reset(); 
onewire_write_cmd_byte(skip_rom); 
onewire_write_cmd_byte(convert_t); 
while(!onewire_pin); 
onewire_reset(); 
onewire_write_cmd_byte(skip_rom); 
onewire_write_cmd_byte(read_scratchpad); 
temp1=onewire_read_data_byte(); 
temp2=onewire_read_data_byte(); 
digital_value= temp2*256 + temp1; 
return  digital_value; 
} 
void I2C_Ghi_Dulieu(char dia_chi,char du_lieu)
{ 
StartI2C(); 
WriteI2C(dia_chi ); 
WriteI2C(du_lieu); 
StopI2C(); 
} 
void Lcd_Ghi_Lenh (char lenh) 
{ 
char data_u, data_l; 
unsigned char data_t[4]; 
data_u = (lenh&0xf0); 
data_l = ((lenh<<4)&0xf0); 
data_t[0] = data_u|0x0C;  //en=1, rs=0 
data_t[1] = data_u|0x08;  //en=0, rs=0 
data_t[2] = data_l|0x0C;  //en=1, rs=0 
data_t[3] = data_l|0x08;  //en=0, rs=0 
I2C_Ghi_Dulieu(Dia_chi_LCD,data_t[0]); 
I2C_Ghi_Dulieu(Dia_chi_LCD,data_t[1]); 
I2C_Ghi_Dulieu(Dia_chi_LCD,data_t[2]); 
I2C_Ghi_Dulieu(Dia_chi_LCD,data_t[3]); 
}
void Lcd_Ghi_Dulieu (char data) 
{ 
char data_u, data_l; 
unsigned char data_t[4]; 
data_u = (data&0xf0); 
data_l = ((data<<4)&0xf0); 
data_t[0] = data_u|0x0D;  //en=1, rs=1 
data_t[1] = data_u|0x09;  //en=0, rs=1 
data_t[2] = data_l|0x0D;  //en=1, rs=1 
data_t[3] = data_l|0x09;  //en=0, rs=1 
I2C_Ghi_Dulieu(Dia_chi_LCD,data_t[0]); 
I2C_Ghi_Dulieu(Dia_chi_LCD,data_t[1]); 
I2C_Ghi_Dulieu(Dia_chi_LCD,data_t[2]); 
I2C_Ghi_Dulieu(Dia_chi_LCD,data_t[3]); 
}
void lcd_init (void) 
{ 
Lcd_Ghi_Lenh (0x03); Delay1KTCYx(10); 
Lcd_Ghi_Lenh (0x02); Delay1KTCYx(10); 
Lcd_Ghi_Lenh (0x28); Delay1KTCYx(10);  
Lcd_Ghi_Lenh (0x06); Delay1KTCYx(10); 
Lcd_Ghi_Lenh (0x0c); Delay1KTCYx(10); 
Lcd_Ghi_Lenh (0x01); Delay1KTCYx(10);  
}
void Lcd_Ghi_Chuoi (char *str) 
{ 
while (*str) Lcd_Ghi_Dulieu (*str++); 
} 
void setpwm(unsigned int value)
{
    CCP1CONbits.DC1B1=value>>1&0x02;
    CCP1CONbits.DC1B0=value&0x01;
    CCPR1L=value>>2&0xff;
}
void main() 
{
     ADCON1=0x0f;
     TRISC=0;
     TRISAbits.TRISA0=0;
     TRISAbits.TRISA1=0;
     OpenI2C(MASTER,SLEW_OFF);
     SSPADD=0x09;
     lcd_init();
     Lcd_Ghi_Lenh(0x80);
     sprintf(&M[0],"Connecting...!");
     Lcd_Ghi_Chuoi(&M[0]);
      do 
    { 
        onewire_reset(); 
    }  
    while(presence_pulse);
    Lcd_Ghi_Lenh(0x80);
    sprintf(&M[0],"Connected!       ");
    Lcd_Ghi_Chuoi(&M[0]);
    onewire_reset(); 
    onewire_write_cmd_byte(skip_rom); 
    onewire_write_cmd_byte(convert_t);
    Delay1KTCYx(1000);
    INTCONbits.GIE=1;
    INTCONbits.INT0IE=1;
    INTCON3bits.INT1IE=1;
    INTCON2bits.INTEDG1=0;
    INTCON2bits.INTEDG0=0;
    INTCONbits.INT0IF=0;
    INTCON3bits.INT1IF=0;
    PR2=62;
    T2CONbits.TMR2ON=1;
    //thi?t l?p h? s? chia t?n l  16
    T2CONbits.T2CKPS1=1;
    T2CONbits.T2CKPS0=1;
    setpwm(125);//T = 1ms Ton= 50%
    //thi?t l?p ch?  ? pwm
    CCP1CON=0b1100;
  while(1)
  {
      dig_value=ds18b20_read();
      temp=dig_value*6.25;
      Lcd_Ghi_Lenh(0x80);
      sprintf(&M[0],"Temp:%d.%d%cC         ",temp/100,temp%100,0xdf);
      Lcd_Ghi_Chuoi(&M[0]);
      Lcd_Ghi_Lenh(0xC0);
      a=temp/100-dem;
      if(a<2&&a>-2)
      {
          PORTAbits.RA1=0;PORTAbits.RA2=0;
          sprintf(&M[0],"Done!!:%d%cC  +-1        ",dem,0xdf);
          Lcd_Ghi_Chuoi(&M[0]);
          setpwm(0); 
      }
      else if(a>=2)
      {
          PORTAbits.RA1=1;
          sprintf(&M[0],"Expect:%d%cC  +-1        ",dem,0xdf);
          Lcd_Ghi_Chuoi(&M[0]);
      }
      else if(a<=-2)
      {
          PORTAbits.RA1=0;
          sprintf(&M[0],"Expect:%d%cC  +-1        ",dem,0xdf);
          Lcd_Ghi_Chuoi(&M[0]);
          pwm_value = -(a*30);
          if(pwm_value>250) pwm_value=250;
          setpwm(pwm_value);
      }
      }
  }
