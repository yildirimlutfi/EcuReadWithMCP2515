unsigned long millis_count=0;  //must be volatile, as is changed inside ISR
bit has_updated; //set each time millis_count is updated

void Timer1Interrupt() iv IVT_ADDR_T1INTERRUPT{
     if(IFS0bits.T1IF==1)
     {
          millis_count++;     //increment millisecond count
          has_updated = 1;    //note an update has occured
          T1IF_bit=0;
     }
}

void setupTimer()
{
//Timer1 = 100ms
     T1CON            = 0x8020;
     T1IE_bit         = 1;
     T1IF_bit         = 0;
     IPC0             = IPC0 | 0x1000;
     PR1              = 25000;
}

unsigned long millis(void)
{
    unsigned long temp;
    do {
        has_updated = 0;    //so we can detect an update
        temp = millis_count;    //fetch current count
    } while (has_updated);
    return temp;   //return if no update occurred
}