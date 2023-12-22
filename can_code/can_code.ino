//——————————————————————————————————————————————————————————————————————————————
//  ACAN2515 Demo in loopback mode, for ESP32
//——————————————————————————————————————————————————————————————————————————————

#ifndef ARDUINO_ARCH_ESP32
  #error "Select an ESP32 board" 
#endif

//——————————————————————————————————————————————————————————————————————————————

#include <ACAN2515.h>

//——————————————————————————————————————————————————————————————————————————————
//  For using SPI on ESP32, see demo sketch SPI_Multiple_Buses
//  Two SPI busses are available in Arduino, HSPI and VSPI.
//  By default, Arduino SPI uses VSPI, leaving HSPI unused.
//  Default VSPI pins are: SCK=18, MISO=19, MOSI=23.
//  You can change the default pin with additional begin arguments
//    SPI.begin (MCP2515_SCK, MCP2515_MISO, MCP2515_MOSI)
//  CS input of MCP2515 should be connected to a digital output port
//  INT output of MCP2515 should be connected to a digital input port, with interrupt capability
//  Notes:
//    - GPIOs 34 to 39 are GPIs – input only pins. These pins don’t have internal pull-ups or
//      pull-down resistors. They can’t be used as outputs.
//    - some pins do not support INPUT_PULLUP (see https://www.esp32.com/viewtopic.php?t=439) 
//    - All GPIOs can be configured as interrupts
// See https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
//——————————————————————————————————————————————————————————————————————————————

static const byte MCP2515_SCK  = 18 ; // SCK input of MCP2515 
static const byte MCP2515_MOSI = 23 ; // SDI input of MCP2515  
static const byte MCP2515_MISO = 19 ; // SDO output of MCP2515 

static const byte MCP2515_CS  = 5 ; // CS input of MCP2515 (adapt to your design) 
static const byte MCP2515_INT = 21 ; // INT output of MCP2515 (adapt to your design)
static const byte MCP2515_RESET = 27 ; // RESET input of MCP2515 (adapt to your design)

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Driver object
//——————————————————————————————————————————————————————————————————————————————

ACAN2515 can (MCP2515_CS, SPI, MCP2515_INT) ;

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Quartz: adapt to your design
//——————————————————————————————————————————————————————————————————————————————

static const uint32_t QUARTZ_FREQUENCY = 8UL * 1000UL * 1000UL ; // 20 MHz

//——————————————————————————————————————————————————————————————————————————————
//   SETUP
//——————————————————————————————————————————————————————————————————————————————

void setup () {
//--- RESET MCP2515
  pinMode (MCP2515_RESET, OUTPUT) ;
  digitalWrite (MCP2515_RESET, LOW) ;
  delay (10) ;
  digitalWrite (MCP2515_RESET, HIGH) ;
//--- Switch on builtin led
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;
//--- Start serial
  Serial.begin (115200) ;
//--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
//--- Begin SPI
  SPI.begin (MCP2515_SCK, MCP2515_MISO, MCP2515_MOSI) ;
//--- Configure ACAN2515
  Serial.println ("Configure ACAN2515") ;
  ACAN2515Settings settings (QUARTZ_FREQUENCY, (500UL * 1000UL)) ; // CAN bit rate 125 kb/s
  settings.mRequestedMode = ACAN2515Settings::ListenOnlyMode ; // Select loopback mode
  const uint16_t errorCode = can.begin (settings, [] { can.isr () ; }) ;
   if (errorCode == 0) {
    Serial.print ("Bit Rate prescaler: ") ;
    Serial.println (settings.mBitRatePrescaler) ;
    Serial.print ("Propagation Segment: ") ;
    Serial.println (settings.mPropagationSegment) ;
    Serial.print ("Phase segment 1: ") ;
    Serial.println (settings.mPhaseSegment1) ;
    Serial.print ("Phase segment 2: ") ;
    Serial.println (settings.mPhaseSegment2) ;
    Serial.print ("SJW: ") ;
    Serial.println (settings.mSJW) ;
    Serial.print ("Triple Sampling: ") ;
    Serial.println (settings.mTripleSampling ? "yes" : "no") ;
    Serial.print ("Actual bit rate: ") ;
    Serial.print (settings.actualBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print ("Exact bit rate ? ") ;
    Serial.println (settings.exactBitRate () ? "yes" : "no") ;
    Serial.print ("Sample point: ") ;
    Serial.print (settings.samplePointFromBitStart ()) ;
    Serial.println ("%") ;
  }else{
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}

//----------------------------------------------------------------------------------------------------------------------

static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

//——————————————————————————————————————————————————————————————————————————————

void loop() {
  digitalWrite (LED_BUILTIN, LOW) ;
  //CANMessage frame ;
 /* if (gBlinkLedDate < millis ()) {
    gBlinkLedDate += 2000 ;
    
    frame.id=542;
    frame.len=1;
    frame.data[1]='12';
    frame.rtr=true;
    const bool ok = can.tryToSend (frame) ;
    if (ok) {
      gSentFrameCount += 1 ;
      Serial.println ("Sent: ") ;
      //Serial.println (gSentFrameCount) ;
      Serial.print ("id:");
    Serial.println (frame.id) ;
    Serial.print ("data:");
    Serial.println (frame.data[1]);

    }else{
      Serial.println ("Send failure") ;
    }
  }*/
  // if (can.available ()) {
  //  // can.receive (frame) ;
  //   Serial.println("msg received");
  //   can.dispatchReceivedMessage();
  //   gReceivedFrameCount ++ ;
  //   Serial.println ("Received: ") ;
  //   Serial.print ("id:");
  //   Serial.println (frame.id) ;
  //   Serial.print ("data:");
  //   Serial.print (frame.data[1]);
  //   Serial.print (frame.data[2]);
  //   Serial.println (frame.data[3]);
  CANMessage message ;
  if (can.receive (message)) {
   
    printHex(message.id);
    Serial.print(',');
    printHex(message.rtr);
    Serial.print(',');
    printHex(message.len);
    Serial.print(',');
    printdata(message.len );


// Handle received message
  }

}
  
void printHex(long num) {
  if ( num < 0x10 ){ Serial.print("0"); }
  Serial.print(num, HEX);
}//——————————————————————————————————————————————————————————————————————————————
CANMessage message ;

void printdata(int len) {
  for (int i=1 ;i<len;i++){
    printHex(message.data[i]);
  }
  printHexln(message.data[len]);  
}
void printHexln(long num) {
  if ( num < 0x10 ){ Serial.print("0"); }
  Serial.println(num, HEX);
}