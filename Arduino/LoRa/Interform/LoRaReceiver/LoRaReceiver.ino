/* 
  Check the new incoming messages, and print via serialin 115200 baud rate.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/

#include <SPI.h>
#include <LoRa.h>

// Pin definetion of WIFI LoRa 32
//#define SCK     5    // GPIO5  -- SX1278's SCK
//#define MISO    19   // GPIO19 -- SX1278's MISO
//#define MOSI    27   // GPIO27 -- SX1278's MOSI
//#define SS      18   // GPIO18 -- SX1278's CS
//#define RST     14   // GPIO14 -- SX1278's RESET
//#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SCK       14   // GPIO14  -- SX127x's SCK
#define MISO      13   // GPIO13 -- SX127x's MISO
#define MOSI      12   // GPIO12 -- SX127x's MOSI
#define SS        15   // GPIO15 -- SX127x's CS
#define RST       32   // GPIO32 -- SX127x's RESET
#define DI0       18   // GPIO26 -- SX127x's IRQ(Interrupt Request)

#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6
#define PABOOST true

#define GPIO_LED  27

bool ledOn;

void setup() 
{
    pinMode( GPIO_LED, OUTPUT );
    ledOn = false;
    digitalWrite( GPIO_LED, ledOn );
    
    Serial.begin( 115200 );
    while ( !Serial ); //test this program,you must connect this board to a computer
    Serial.println( "LoRa Receiver" );
    SPI.begin( SCK, MISO, MOSI, SS );
    LoRa.setPins( SS, RST, DI0 );
    
    if ( !LoRa.begin( BAND,PABOOST ) ) 
    {
        Serial.println( "Starting LoRa failed!" );
        while(1);
    }
}

void loop() 
{
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) 
    {
        ledOn = !ledOn;
        digitalWrite( GPIO_LED, ledOn );
        
        // received a packet
        Serial.print("Received packet '");
        // read packet
        while (LoRa.available()) 
        {
            Serial.print((char)LoRa.read());
        }
        
        // print RSSI of packet
        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());
    }
}
