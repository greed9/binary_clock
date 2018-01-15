
//
// Binary Clock
//

#include <MD_MAX72xx.h>   // LED display library
#include <SPI.h>          // LED can use SPI
#include <DS3231.h>       // clock chip

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define	MAX_DEVICES	1

//#define	CLK_PIN		13  // or SCK
//#define	DATA_PIN	11  // or MOSI
//#define	CS_PIN		10  // or SS

#define  CLK_PIN   7  // or SCK
#define DATA_PIN  6  // or MOSI
#define CS_PIN    5  // or SS

// SPI hardware interface
//MD_MAX72XX mx = MD_MAX72XX(CS_PIN, MAX_DEVICES);

// Instantiate LED driver
MD_MAX72XX mx = MD_MAX72XX(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Instantiate clock/calendar driver
// Code from the Demo Example of the DS3231 Library
DS3231  rtc(SDA, SCL);

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds

void rows()
// Demonstrates the use of setRow()
{
  mx.clear();

  for (uint8_t row = 0; row < ROW_SIZE; row++)
  {
    mx.setRow(row, 0xff);
    delay(DELAYTIME);
    mx.setRow(row, 0x00);
  }
}

void columns()
// Demonstrates the use of setColumn()
{
  mx.clear();

  for (uint8_t col = 0; col < mx.getColumnCount(); col++)
  {
    mx.setColumn(col, 0xff);
    delay(DELAYTIME / MAX_DEVICES);
    mx.setColumn(col, 0x00);
  }
}

void intensity()
// Demonstrates the control of display intensity (brightness) across
// the full range.
{
  uint8_t row;

  mx.clear();

  // Grow and get brighter
  row = 0;
  for (int8_t i = 0; i <= MAX_INTENSITY; i++)
  {
    mx.control(MD_MAX72XX::INTENSITY, i);
    if (i % 2 == 0)
      mx.setRow(row++, 0xff);
    delay(DELAYTIME * 3);
  }

  mx.control(MD_MAX72XX::INTENSITY, 8);
}

void testPoints ( )
{
  for ( int row = 0 ; row < 5 ; row ++ )
  {
    for ( int col = 0 ; col < 7  ; col ++ )
    {
      Serial.print( row ) ;
      Serial.print("," ) ;
      Serial.println ( col ) ;

      mx.setPoint(row, col, true);
      mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY);
      delay ( 1000 ) ;
      mx.setPoint(row, col, false);
    }
  }
}

// Get a c str from bits in a byte
char* bitString ( byte in )
{
  static char bits[9] = { 0 } ;
  for (int b = 7; b >= 0; b--)
  {
    int oneBit = bitRead ( in, b ) ;
    bits[b] = ( oneBit ? '1' : '0' ) ;
  }
  bits[8] = 0 ;
  return bits ;
}

// render ls 7 bits of string on led display
void display7BitString ( int row, char* bits )
{
  char thisBit = 0 ;
  for ( int i = 0 ; i < 7 ; i++ )
  {
    thisBit = bits[i] ;
    if ( bits[i] == '1' )
    {
      mx.setPoint(row, i, true);
    }
    else
    {
      mx.setPoint(row, i, false);
    }
  }
}

void chimeDisplay ( )
{
  mx.clear ( ) ;
  delay ( 100 ) ;
  for ( int row = 0 ; row < 5 ; row ++ )
  {
    for ( int col = 0 ; col < 7  ; col ++ )
    {
      mx.setPoint(row, col, true);
      delay ( 100 ) ;
      mx.setPoint(row, col, false);
    }
  }
}

void setup()
{

  // Initialize led driver
  mx.begin();

  // Initialize the rtc object
  rtc.begin() ;

  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(20, 28, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(( uint8_t ) 14, ( uint8_t ) 1, ( uint16_t ) 2018);

  Serial.begin(115200);
  delay(1000);
  Serial.println ( "Binary clock" ) ;
  mx.control(MD_MAX72XX::INTENSITY, 16);
  //Serial.println ( bitString( (byte ) 0xAA ) ) ;
}

void loop()
{
  //testPoints ( ) ;
  char* dateStr = 0 ;
  char* timeStr = 0 ;

  byte month = 0 ;
  byte day = 0 ;
  byte hours = 0 ;
  byte minutes = 0 ;

  char temp[3] = { 0 } ;

  static byte prevHour = 0 ;

  dateStr = rtc.getDateStr( ) ;
  timeStr = rtc.getTimeStr ( ) ;

  // day
  strncpy ( temp, &dateStr[0], 2 ) ;
  day = ( byte ) atoi ( temp ) ;

  // month
  strncpy ( temp, &dateStr[3], 2 ) ;
  month = ( byte ) atoi ( temp ) ;

  // hour
  strncpy ( temp, &timeStr[0], 2 ) ;
  hours = (byte ) atoi ( temp ) ;
  if ( prevHour != hours )
  {
    chimeDisplay ( ) ;
    prevHour = hours ;
  }

  // minute
  strncpy ( temp, &timeStr[3], 2 ) ;
  minutes = (byte ) atoi ( temp ) ;

  display7BitString ( 0, bitString ( month ) ) ; // month
  display7BitString ( 1, bitString ( day  ) ) ; // day
  display7BitString ( 3, bitString ( hours ) ) ; // hour
  display7BitString ( 4, bitString ( minutes) ) ;  // minute
  delay ( 10000 ) ;
  Serial.println(rtc.getDateStr());
  Serial.println ( rtc.getTimeStr ( )  ) ;
}

