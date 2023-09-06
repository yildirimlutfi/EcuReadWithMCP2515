#include "i2c.h"
#include "timer.h"
unsigned char I2C_writeData[32];
unsigned char I2C_readData[32];
unsigned int addressArduino = 0x08;
void main()
{
  char i;
  setupTimer();
  setupI2c();

  while(1)
  {
   for(i=0;i<32;i++)
     I2C_writeData[i]=rand();

   I2C_writeData[0]=0x08;//number of bytes
   I2C_writeData[1]=0x01;//mode
   I2C_writeData[2]=0x05;//pid
   
   sendMultipleDataViaI2c(addressArduino,8,I2C_writeData);
   Delay_ms(1000);
   receiveMultipleDataViaI2c(addressArduino,8,I2C_readData);
   Delay_ms(1000);
  }
}