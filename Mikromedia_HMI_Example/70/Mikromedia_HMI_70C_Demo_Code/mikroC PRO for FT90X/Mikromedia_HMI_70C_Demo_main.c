/*
 * Project name:
     Mikromedia_HMI_70C_Demo
 * Generated by:
     Visual TFT
 * Date of creation
     23.5.2016
 * Time of creation
     15:13:43
 * Test configuration:
     MCU:             FT900
     Dev.Board:       Mikromedia_HMI_7_CTP
     Oscillator:      100000000 Hz
     SW:              mikroC PRO for FT90X
                      http://www.mikroe.com/mikroc/ft90x/
     Note:            Download FT812 library at
                      http://www.mikroe.com/visualtft/#FT812
                      Download (if using external memory storage) FAT32 library at
                      http://www.libstock.com/projects/view/108/fat32-library
 */

#include "Mikromedia_HMI_70C_Demo_objects.h"
#define _DATA_ARRAY_SIZE 16
#include <built_in.h>
#include "SF_driver.h"
#include "resources.h"

sbit CS_Serial_Flash_bit at GPIO_PIN28_bit;

char cSF_test_status=0;

extern unsigned char a,vol,arm[6];
extern void Start_mcu();
extern void Enable_Home_Change();
extern void Home_Change();
extern void Green_power_bars();


void InitMCU(){
  GPIO_Digital_Output(&GPIO_PORTD, _GPIO_PINMASK_4);   //MM-HMI
  CS_Serial_Flash_bit = 1;
}

void SF_Start() {
  SerialFlash_init();
  SerialFlash_WriteEnable();
}


void SF_Test(char *test)
{
  unsigned char temp1,temp2, SerialFlashID;
  unsigned char i, success;

  // Reset error flag
  *test = 0;

  SerialFlash_ChipErase();

  // Read Flash ID
  SerialFlashID = SerialFlash_ReadID();

  Delay_ms(500);

  // Write and read back a single byte
  temp1 = 222;
  SerialFlash_WriteByte(temp1, 0x123456);
  temp2 = 111;

  Delay_ms(100);
  temp2 = SerialFlash_ReadByte(0x123456);

  Delay_ms(500);
  *test=temp2;
}

void SF_mem()
{
  unsigned char i,l,cnt[4];
  long adr=0x111111;

  for(i=0;i<6;i++) {
    SerialFlash_WriteByte(arm[i],adr);
    adr+=0x000020;
  }

  strcpy(Label24.Caption,"  ******");
  DrawScreen(&System_Log_Screen);
  for(l=0;l<10;l++) {
    ByteToStr(10-l,cnt);
    strcpy(Label25.Caption,"      ");
    strcat(Label25.Caption,cnt);
    DrawScreen(&System_Log_Screen);
    FT812_Sound_SetAndPlay(_FT812_SOUND_CLICK,_FT812_SOUND_PITCH_B3,vol);
    Delay_ms(1000);
  }
  Box30.Color=0x00FF0000;
  Box36.Color=0x00FF0000;
  Image65.Visible=0;
  Image66.Visible=1;
  strcpy(Label25.Caption,"   ARMED");
  strcpy(Label8.Caption,"   ARMED");
}

void SF_rem()
{
  unsigned char i,temp[6];
  char txte[4],txtex[4];
  int val=0;
  long adr=0x111111;

  for(i=0;i<6;i++) {
    temp[i] = SerialFlash_ReadByte(adr);
    ByteToStr(temp[i],txte);
    ByteToStr(arm[i],txtex);
    if(temp[i]!=arm[i]) val=1;
    adr+=0x000020;
  }

  Delay_ms(3000);
  for(i=0;i<6;i++) arm[i]=0;
  strcpy(Label24.Caption,"");
  DrawScreen(&System_Log_Screen);
  if(val==0) {
    a=0;
    Box30.Color=0x00008040;
    Box36.Color=0x00008040;
    Image65.Visible=1;
    Image66.Visible=0;
    strcpy(Label25.Caption,"DISARMED");
    strcpy(Label8.Caption,"DISARMED");
  }
}


void InitTimerA() {
  TIMER_CONTROL_0 = 2;
  TIMER_SELECT = 0;
  TIMER_PRESC_LS = 36;
  TIMER_PRESC_MS = 244;
  TIMER_WRITE_LS = 159;
  TIMER_WRITE_MS = 15;
  TIMER_CONTROL_3 = 0;
  TIMER_CONTROL_4 |= 17;
  TIMER_CONTROL_2 |= 16;
  TIMER_INT |= 2;
  IRQ_CTRL.B31 = 0;
  TIMER_CONTROL_1 |= 1;
}

void TimerInterrupt() iv IVT_TIMERS_IRQ{
  Enable_Home_Change();
  if (TIMER_INT_A_bit){
    TIMER_INT = (TIMER_INT & 0xAA) | (1 << 0);
  }
}


void main() {

  InitVTFTStack();
  Start_mcu();
  InitTimerA();

  while (1) {

    ProcessVTFTStack();
    Home_Change();
    Green_power_bars();

  }

}