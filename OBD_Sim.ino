/* CAN OBD & UDS Simulator
 *  
 *  Currently replies to some general OBD requests 
 *  Place holders exist for what I've been able to locate on the Internet
 *  Does not currently support UDS correctly, just placeholders with mode descriptions
 *  
 *  Written By: Cory J. Fowler  December 20th, 2016
 *  
 */ 
#include <mcp_can.h>
#include <SPI.h>

#define PAD 0x00

// What CAN ID type?  Standard or Extended
#define standard 0

// 7E0/8 = Engine ECM
// 7E1/9 = Transmission ECM

#if standard == 1
  #define REPLY_ID 0x7E9
  #define LISTEN_ID 0x7E1
  #define FUNCTIONAL_ID 0x7DF  
#else
  #define REPLY_ID 0x98DAF101
  #define LISTEN_ID 0x98DA01F1
  #define FUNCTIONAL_ID 0x98DB33F1
#endif

// CAN RX Variables
unsigned long canRxId;
byte dlc;
byte rxBuf[8];

// CAN Interrupt and Chip Select
#define CAN0_INT 2                              // Set CAN0 INT to pin 2
MCP_CAN CAN0(9);                                // Set CAN0 CS to pin 9

void setup()
{
  Serial.begin(115200);
  while(!Serial);
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

#if standard == 1
  // Standard ID Filters
  CAN0.init_Mask(0,0x7F00000);                // Init first mask...
  CAN0.init_Filt(0,0x7DF0000);                // Init first filter...
  CAN0.init_Filt(1,0x7E10000);                // Init second filter...
  
  CAN0.init_Mask(1,0x7F00000);                // Init second mask... 
  CAN0.init_Filt(2,0x7DF0000);                // Init third filter...
  CAN0.init_Filt(3,0x7E10000);                // Init fourth filter...
  CAN0.init_Filt(4,0x7DF0000);                // Init fifth filter...
  CAN0.init_Filt(5,0x7E10000);                // Init sixth filter...

#else
  // Extended ID Filters
  CAN0.init_Mask(0,0x90FFFF00);                // Init first mask...
  CAN0.init_Filt(0,0x90DB3300);                // Init first filter...
  CAN0.init_Filt(1,0x90DA0100);                // Init second filter...
  
  CAN0.init_Mask(1,0x90FFFF00);                // Init second mask... 
  CAN0.init_Filt(2,0x90DB3300);                // Init third filter...
  CAN0.init_Filt(3,0x90DA0100);                // Init fourth filter...
  CAN0.init_Filt(4,0x90DB3300);                // Init fifth filter...
  CAN0.init_Filt(5,0x90DA0100);                // Init sixth filter...
#endif
  
  CAN0.setMode(MCP_NORMAL);                          // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                          // Configuring pin for /INT input
  
  Serial.println("OBD-II CAN Simulator");
}

void loop()
{
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&canRxId, &dlc, rxBuf);             // Get CAN data
    
    // First request from most adapters...
    if(canRxId == FUNCTIONAL_ID){
      obdReq(rxBuf);
    }       
  }
}

void obdReq(byte *data){
  byte numofBytes = data[0];
  byte mode = data[1] & 0x0F;
  byte pid = data[2];
  bool tx = false;
  //byte txData[] = {0x00,(0x40 | mode),pid,PAD,PAD,PAD,PAD,PAD};
  byte txData[] = {0x00,(0x40 | mode),pid,PAD,PAD,PAD,PAD,PAD};
  
  //=============================================================================
  // MODE $01 - Show current data
  //=============================================================================
  if(mode == 0x01){
    if(pid == 0x00){        // Supported PIDs 01-20
      txData[0] = 0x06;
      
      txData[3] = 0x80;
      txData[4] = 0x38;
      txData[5] = 0x00;
      txData[6] = 0x01;
      tx = true;
    }
    else if(pid == 0x01){    // Monitor status since DTs cleared.
      bool MIL = true;
      byte DTC = 5;
      txData[0] = 0x06;
      
      txData[3] = (MIL << 7) | (DTC & 0x7F);
      txData[4] = 0x07;
      txData[5] = 0xFF;
      txData[6] = 0x00;
      tx = true;
    }
    else if(pid == 0x03){    // Fuel system status
      txData[0] = 0x03;
      
      txData[3] = 0xFA;
      tx = true;
    }
    else if(pid == 0x05){    // Engine coolant temperature
      txData[0] = 0x03;
      
      txData[3] = 0xFA;
      txData[4] = 0xFB;
      txData[5] = 0xFC;
      txData[6] = 0xFD;
      txData[7] = random(0xFF);
      tx = true;
    }
    else if(pid == 0x0B){    // Intake manifold absolute pressure
      txData[0] = 0x03;
      
      txData[3] = 0x64;
      tx = true;
    }
    else if(pid == 0x0C){    // Engine RPM
      txData[0] = 0x04;
      
      txData[3] = 0x9C;
      txData[4] = 0x40;
      tx = true;
    }
    else if(pid == 0x0D){    // Vehicle speed
      txData[0] = 0x03;
      
      txData[3] = 0xFA;
      tx = true;
    }
    else if(pid == 0x0F){    // Intake air temperature
      txData[0] = 0x03;
      
      txData[3] = 0xFA;
      tx = true;
    }
    else if(pid == 0x11){    // Throttle position
      txData[0] = 0x03;
      
      txData[3] = 0xFA;
      tx = true;
    }
    else if(pid == 0x20){    // Supported PIDs 21-40
      txData[0] = 0x06;
      
      txData[3] = 0x80;
      txData[4] = 0x00;
      txData[5] = 0x00;
      txData[6] = 0x01;
      tx = true;
    }
    else if(pid == 0x21){    // Distance traveled with MIL on
      txData[0] = 0x04;
      
      txData[3] = 0x00;
      txData[4] = 0x23;
      tx = true;
    }
    else if(pid == 0x40){    // Supported PIDs 41-60
      txData[0] = 0x06;
      
      txData[3] = 0x00;
      txData[4] = 0x08;
      txData[5] = 0x00;
      txData[6] = 0x0D;
      tx = true;
    }
    else if(pid == 0x4D){    // Time run with MIL on
      txData[0] = 0x04;
      
      txData[3] = 0x00;
      txData[4] = 0x3C;
      tx = true;
    }
    else if(pid == 0x5C){    // Engine oil Temperature
      txData[0] = 0x03;
      
      txData[3] = 0x1E;
      tx = true;
    }
    else if(pid == 0x5D){    // Fuel injection timing
      txData[0] = 0x04;
      
      txData[3] = 0x61;
      txData[4] = 0x80;
      tx = true;
    }
    else if(pid == 0x5E){    // Engine fuel rate
      txData[0] = 0x04;
      
      txData[3] = 0x07;
      txData[4] = 0xD0;
      tx = true;
    }
    else if(pid == 0x60){    // Supported PIDs 61-80
      txData[0] = 0x06;
      
      txData[3] = 0x00;
      txData[4] = 0x00;
      txData[5] = 0x00;
      txData[6] = 0x01;
      tx = true;
    }
    else if(pid == 0x80){    // Supported PIDs 81-A0
      txData[0] = 0x06;
      
      txData[3] = 0x00;
      txData[4] = 0x00;
      txData[5] = 0x00;
      txData[6] = 0x01;
      tx = true;
    }
    else if(pid == 0xA0){    // Supported PIDs A1-C0
      txData[0] = 0x06;
      
      txData[3] = 0x00;
      txData[4] = 0x00;
      txData[5] = 0x00;
      txData[6] = 0x01;
      tx = true;
    }
    else if(pid == 0xC0){    // Supported PIDs C1-E0
      txData[0] = 0x06;
      
      txData[3] = 0x00;
      txData[4] = 0x00;
      txData[5] = 0x00;
      txData[6] = 0x01;
      tx = true;
    }
    else if(pid == 0xE0){    // Supported PIDs E1-FF?
      txData[0] = 0x06;
      
      txData[3] = 0x00;
      txData[4] = 0x00;
      txData[5] = 0x00;
      txData[6] = 0x00;
      tx = true;
    }
    else{
      unsupported(mode, pid);
    }
  }
  
  //=============================================================================
  // MODE $02 - Show freeze frame data
  //=============================================================================
  else if(mode == 0x02){
      unsupported(mode, pid);
  }
  
  //=============================================================================
  // MODE $03 - Show stored DTCs
  //=============================================================================
  else if(mode == 0x03){
      byte DTCs[] = {(0x40 | mode), 0x05, 0xC0, 0xBA, 0x00, 0x11, 0x80, 0x13, 0x90, 0x45, 0xA0, 0x31};
      iso_tp(mode, pid, 12, DTCs);
  }
  
  //=============================================================================
  // MODE $04 - Clear DTCs and stored values
  //=============================================================================
  else if(mode == 0x04){
      // Need to cleat DTCs.  We just acknowledge the command for now.
      txData[0] = 0x01;
      tx = true;
  }
  
  //=============================================================================
  // MODE $05 - Test Results, oxygen sensor monitoring (non CAN only)
  //=============================================================================
  else if(mode == 0x05){
      unsupported(mode, pid);
  }
  
  //=============================================================================
  // MODE $06 - Test Results, On-Board Monitoring (Oxygen sensor monitoring for CAN only)
  //=============================================================================
  else if(mode == 0x06){
    if(pid == 0x00){        // Supported PIDs 01-20
      txData[0] = 0x06;
      
      txData[3] = 0x00;
      txData[4] = 0x00;
      txData[5] = 0x00;
      txData[6] = 0x00;
      tx = true;
    }
    else{
      unsupported(mode, pid);
    }
  }
  
  //=============================================================================
  // MODE $07 - Show pending DTCs (Detected during current or last driving cycle)
  //=============================================================================
  else if(mode == 0x07){
      byte DTCs[] = {(0x40 | mode), 0x05, 0xC0, 0xBA, 0x00, 0x11, 0x80, 0x13, 0x90, 0x45, 0xA0, 0x31};
      iso_tp(mode, pid, 12, DTCs);
  }
  
  //=============================================================================
  // MODE $08 - Control operation of on-board component/system
  //=============================================================================
  else if(mode == 0x08){
      unsupported(mode, pid);
  }
  
  //=============================================================================
  // MODE $09 - Request vehcile information
  //=============================================================================
  else if(mode == 0x09){
    if(pid == 0x00){        // Supported PIDs 01-20
      txData[0] = 0x06;
      
      txData[3] = 0x54;
      txData[4] = 0x40;
      txData[5] = 0x00;
      txData[6] = 0x00;
      tx = true;
    }
    else if(pid == 0x02){    // VIN (17 to 20 Bytes) Uses ISO-TP
      byte VIN[] = {(0x40 | mode), pid, 0x01, 0x31, 0x5a, 0x56, 0x42, 0x50, 0x38, 0x41, 0x4d, 0x37, 0x44, 0x35, 0x32, 0x32, 0x30, 0x31, 0x38, 0x31};
      iso_tp(mode, pid, 20, VIN);
    }
    else if(pid == 0x04){    // Calibration ID
      byte CID[] = {(0x40 | mode), pid, 0x01, 0x41, 0x72, 0x64, 0x75, 0x69, 0x6E, 0x6F, 0x20, 0x4F, 0x42, 0x44, 0x49, 0x49, 0x73, 0x69, 0x6D, 0x51, 0x52, 0x53, 0x54};
      iso_tp(mode, pid, 23, CID);
    }
    else if(pid == 0x06){    // CVN
      byte CVN[] = {(0x40 | mode), pid, 0x02, 0x11, 0x42, 0x42, 0x42, 0x22, 0x43, 0x43, 0x43};
      iso_tp(mode, pid, 11, CVN);
    }
    else if(pid == 0x0A){    // ECM Name
      byte ECMname[] = {(0x40 | mode), pid, 0x01, 0x45, 0x43, 0x4D, 0x00, 0x2D, 0x41, 0x72, 0x64, 0x75, 0x69, 0x6E, 0x6F, 0x4F, 0x42, 0x44, 0x49, 0x49, 0x73, 0x69, 0x6D};
      iso_tp(mode, pid, 23, ECMname);
    }
    else if(pid == 0x0D){    // ESN
      byte ESN[] = {(0x40 | mode), pid, 0x01, 0x41, 0x72, 0x64, 0x75, 0x69, 0x6E, 0x6F, 0x2D, 0x4F, 0x42, 0x44, 0x49, 0x49, 0x73, 0x69, 0x6D, 0x00};
      iso_tp(mode, pid, 20, ESN);
    }
    else{
      unsupported(mode, pid); 
    }
  }
  
  //=============================================================================
  // MODE $0A - Show permanent DTCs 
  //=============================================================================
  else if(mode == 0x0A){
      byte DTCs[] = {(0x40 | mode), 0x05, 0xC0, 0xBA, 0x00, 0x11, 0x80, 0x13, 0x90, 0x45, 0xA0, 0x31};
      iso_tp(mode, pid, 12, DTCs);
  }
  else { 
    unsupported(mode, pid);
  }
  
  if(tx)
    CAN0.sendMsgBuf(REPLY_ID, 8, txData);
}


// Generic debug serial output
void unsupported(byte mode, byte pid){
  negAck(mode, 0x12);
  unsupportedPrint(mode, pid);  
}


// Generic debug serial output
void negAck(byte mode, byte reason){
  byte txData[] = {0x03,0x7F,mode,reason,PAD,PAD,PAD,PAD};
  CAN0.sendMsgBuf(REPLY_ID, 8, txData);
}


// Generic debug serial output
void unsupportedPrint(byte mode, byte pid){
  char msgstring[64];
  sprintf(msgstring, "Mode $%02X: Unsupported PID $%02X requested!", mode, pid);
  Serial.println(msgstring);
}

// Blocking example of ISO transport
void iso_tp(byte mode, byte pid, int len, byte *data){
  byte tpData[8];
  int offset = 0;
  byte index = 0;

  // First frame
  tpData[0] = 0x10 | ((len >> 8) & 0x0F);
  tpData[1] = 0x00FF & len;
  for(byte i=2; i<8; i++){
    tpData[i] = data[offset++];
  }
  CAN0.sendMsgBuf(REPLY_ID, 8, tpData);
  index++; // We sent a packet so increase our index.
  
  bool not_done = true;
  unsigned long sepPrev = millis();
  byte sepInvl = 0;
  byte frames = 0;
  bool lockout = false;
  while(not_done){
    // Need to wait for flow frame
    if(!digitalRead(CAN0_INT)){
      CAN0.readMsgBuf(&canRxId, &dlc, rxBuf);
    
      if((canRxId == LISTEN_ID) && ((rxBuf[0] & 0xF0) == 0x30)){
        if((rxBuf[0] & 0x0F) == 0x00){
          // Continue
          frames = rxBuf[1];
          sepInvl = rxBuf[2];
          lockout = true;
        } else if((rxBuf[0] & 0x0F) == 0x01){
          // Wait
          lockout = false;
          delay(rxBuf[2]);
        } else if((rxBuf[0] & 0x0F) == 0x03){
          // Abort
          not_done = false;
          return;
        }
      }
    }

    if(((millis() - sepPrev) >= sepInvl) && lockout){
      sepPrev = millis();

      tpData[0] = 0x20 | index++;
      for(byte i=1; i<8; i++){
        if(offset != len)
          tpData[i] = data[offset++];
        else
          tpData[i] = 0x00;
      }
      
      // Do consecutive frames as instructed via flow frame
      CAN0.sendMsgBuf(REPLY_ID, 8, tpData);
      
      if(frames-- == 1)
        lockout = false;
        
    }

    if(offset == len)
      not_done = false;
    else{
      char msgstring[32];
      sprintf(msgstring,"Offset: 0x%04X\tLen: 0x%04X", offset, len);
      Serial.println(msgstring);
    }


    // Timeout
    if((millis() - sepPrev) >= 1000)
      not_done = false;
  }
  
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
