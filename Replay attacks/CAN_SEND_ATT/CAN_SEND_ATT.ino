// demo: CAN-BUS Shield, send data
// loovee@seeed.cc


#include <SPI.h>

#define CAN_2515
// #define CAN_2518FD

// Set SPI CS Pin according to your hardware

#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
// For Wio Terminal w/ MCP2518FD RPi Hat：
// Channel 0 SPI_CS Pin: BCM 8
// Channel 1 SPI_CS Pin: BCM 7
// Interupt Pin: BCM25
const int SPI_CS_PIN  = BCM8;
const int CAN_INT_PIN = BCM25;
#else

// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
const int CAN_INT_PIN = 2;
#endif


#ifdef CAN_2518FD
#include "mcp2518fd_can.h"
mcp2518fd CAN(SPI_CS_PIN); // Set CS pin
#endif

#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#endif

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
long randID;

void MCP2515_ISR() {
   flagRecv = 1;
}


void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    randomSeed(analogRead(0));
    while(!Serial){};
    attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), MCP2515_ISR, FALLING);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

unsigned char stmp[8] = {0,0,0,0,0,0,0,0};
unsigned long canId;
void loop() {
    if (flagRecv) {
        flagRecv = 0;
        while (CAN_MSGAVAIL == CAN.checkReceive()){
            CAN.readMsgBuf(&len, buf);
            canId = CAN.getCanId();
            Serial.println(buf[len-1]);
            if (buf[len-1] == 50){
                Serial.println("Ser Trig");
                for (int i=0; i<5; i++){
                    CAN.MCP_CAN::sendMsgBuf(canId, 0, len, buf);
                    delayMicroseconds(50);
                }
            }
        }
        SERIAL_PORT_MONITOR.println("CAN BUS Replay Complete!");
    }   
}

// END FILE
