void setupI2c()
{
  I2C2_Init(122222);// Initialize the I2C1 module with clock_rate of 100000. Init value wrote 122222 because arduino clock rate faster than PIC.
}

void sendMultipleDataViaI2c(char address,char length ,char *data_)
{
  char i;
  I2C2_Start();
  I2C2_Write(address<<1);//send address byte via I2C doing shift 1 bit

  for(i=0;i<length;i++)
  {
    I2C2_Write(data_[i]  );//send 1 byte data
  }
  I2C2_Stop();
}

void receiveMultipleDataViaI2c(char address,char length, char *data_)
{
  char i;
  I2C2_Start();
  I2C2_Write(address<<1 | 0x01);//0x01 necessary for arduino request event
  for(i=0;i<length;i++)
  {
   data_[i] = I2C2_Read(_I2C_ACK);//read 1 byte data
  }
  I2C2_Stop();
}