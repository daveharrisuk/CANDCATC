/* file: CANDCATC.ino
*-------------------------------------------------------------------------------
*
* Automatic Train Control of DC locos using CBUS, 
*  with OLED display and rotary encoder.
*
*
* © Dave Harris, 2021 (Andover, UK) MERG M2740
* 
* 
* 
* Flash progMemory:  5138 of 30720.  16% used.
* Global Variables:   347 of 2048.   16% used.
* 
* 
*/
#define sTITLE "CANDCATC v0a"




#include "CANDCATC.h"

#include <oled_I2C.h>



/*-----------------------------------------------------------------------------
* Notes on OLED 64x128 0.96" screen mapping    21 char x 8 line 
*
*  char# 0.........10........20
*        |         |         | 
*  line0 Info line0 1234567890 
*  line1 0123456789 info line1 
*  line2 >Cell0 100 >Cell1 101 
*  line3 >Cell2 102 >Cell3 103 
*  line4 >Cell4 104 >Cell5 105 
*  line5 >Cell6 106 >Cell7 107 
*  line6 ___track plan_lineA__ 
*  line7 ___track plan_lineB__ 
*
*
* --------------------------------------------------------------------------
*  Notes on display of track plan numbers and symbols.                      
*
*  1 to 4 are switched track sections AND stop position sensors (TOTI).
*  Either switched section 1 or 2 is live and either section 3 or 4 is live.
*   Never are both 1 and 2 live. Never are both 3 and 4 live.
*   
*  5 & 6 are slow down sensors (TOTI)
*   
*  '-'  no train is detected by sensor
*  'x'  train is detected by sensor
*  ?  sensor has not been seen
* ------------------------------------------------------------------------
*/


/* check TRACKCONFIG define value (set in CANDCATC.h) is OK.
 ---------------------------------------------------------------------*/  

#if TRACKCONFIG == CONFIG_1TO1
/*  
*   x_________-            End to end.  Runs one loco (x)      
*/
 #define sTRACKCONFIG " 1to1" 
 #define sTRACKPLANLINE1 "                     "
 #define sTRACKPLANLINE2 "4x____-5_____6-____.1"
 
  Sensor_t sensor[SENSORCOUNT]          /* chr#, lin#, nature, state    */
  {
    19, 7, NATURE_END, SENSOR_NOTSEEN,   /* sensor 1 = end stop 1       */
    00, 0, NATURE_NIU, SENSOR_NOTSEEN,   /* sensor 2 NIU                */
    00, 0, NATURE_NIU, SENSOR_NOTSEEN,   /* sensor 3 NIU                */
    01, 7, NATURE_END, SENSOR_NOTSEEN,   /* sensor 4 = end stop4        */
    06, 7, NATURE_SLOW, SENSOR_NOTSEEN,  /* sensor 5 = slow before 4    */
    14, 7, NATURE_SLOW, SENSOR_NOTSEEN   /* sensor 6 = slow before 1    */
  };
 /*---------------------------------------------------------------------*/  

#elif TRACKCONFIG == CONFIG_1TO2
/*        ____x            Headshunt to 2 sidings
*  x____/_____-            Runs 1 or 2 locos (x)           
*/
 #define sTRACKCONFIG " 1to2"
 #define sTRACKPLANLINE1 "               ____x2"
 #define sTRACKPLANLINE2 "4x__-5___6-___/____-1"

  Sensor_t sensor[SENSORCOUNT]        /* chr#, lin#, nature, state      */
  {
    19, 7, NATURE_END, SENSOR_NOTSEEN,   /* sensor 1 = end stop 1       */
    19, 6, NATURE_END, SENSOR_NOTSEEN,   /* sensor 2 = end stop 2       */
    00, 0, NATURE_NIU, SENSOR_NOTSEEN    /* sensor 3 NIU                */
    01, 7, NATURE_END, SENSOR_NOTSEEN,   /* sensor 4 = end stop 4       */
    04, 7, NATURE_SLOW, SENSOR_NOTSEEN,  /* sensor 5 = slow before 4    */
    10, 7, NATURE_SLOW, SENSOR_NOTSEEN   /* sensor 6 = slow before 1    */
  };
 /*---------------------------------------------------------------------*/ 
  
#elif TRACKCONFIG == CONFIG_2TO2
/* x___      ___-          2 sidings to line to 2 sidings
*  x___\____/___x          Runs 1, 2, or 3 locos (x)        
*/
 #define sTRACKCONFIG " 2to2"
 #define sTRACKPLANLINE1 "3-__             __x2"
 #define sTRACKPLANLINE2 "4x__\\__-5___6-__/__x1"
  /* \\ is escape \ */

  Sensor_t sensor[SENSORCOUNT]      /* chr#, lin#, nature, state        */
  {
    19, 7, NATURE_END, SENSOR_NOTSEEN,  /* sensor 1 = end stop 1        */
    19, 6, NATURE_END, SENSOR_NOTSEEN,  /* sensor 2 = end stop 2        */
    01, 6, NATURE_END, SENSOR_NOTSEEN,  /* sensor 3 = end stop 3        */
    01, 7, NATURE_END, SENSOR_NOTSEEN,  /* sensor 4 = end stop 4        */
    07, 7, NATURE_SLOW, SENSOR_NOTSEEN, /* sensor 5 = slow before 4     */
    13, 7, NATURE_SLOW, SENSOR_NOTSEEN  /* sensor 6 = slow before 1     */
  };
  
 /*---------------------------------------------------------------------*/  

#elif TRACKCONFIG == CONFIG_LOOP
/*      __x__              Headshunt to loop to headshunt
*  x___/__-__\___x         Runs 1, 2 or 3 locos (x)         
*/
 #define sTRACKCONFIG " Loop"
 #define sTRACKPLANLINE1 "        _1-__        "
 #define sTRACKPLANLINE2 "4x__-5_/__x2_\\_6-__x3"

  Sensor_t sensor[SENSORCOUNT]        /*  chr#, lin#, nature, state     */
  {
    10, 6, NATURE_STOP, SENSOR_NOTSEEN,   /* sensor 1 = stop 1          */
    10, 7, NATURE_STOP, SENSOR_NOTSEEN,   /* sensor 2 = stop 2          */
    19, 6, NATURE_END, SENSOR_NOTSEEN,    /* sensor 3 = end stop 3      */
    01, 7, NATURE_END, SENSOR_NOTSEEN,    /* sensor 4 = end stop 4      */
    04, 7, NATURE_SLOW, SENSOR_NOTSEEN,   /* sensor 5 = slow            */
    16, 6, NATURE_SLOW, SENSOR_NOTSEEN    /* sensor 6 = slow            */
  };
  
 /*---------------------------------------------------------------------*/
#else
 #error define TRACKCONFIG value is invalid 
#endif



uint8_t var[CELLCOUNT] = /* value   */
{
  25,   /* 0-255 duty cycle of crawl speed before stop (<DCmax)   */
  50,   /* 0-255 milliseconds between DC increments/decrements    */
  220,  /* 0-255 duty cycle for cruise speed ( >DCmin )           */
  10,   /* 0-255 delay seconds between moves                      */
  0,    /* 0-255 current duty cycle                               */
  41,   /* 0-255 code: 41 means route 4 to 1, 14 is route 1 to 4  */
  11,   /* 0-255 Traction current mA                              */
  0     /* 0-3   State                                            */
};


uint8_t encPos;




/*------------------------- instantiate global objects ---------------------*/


OLED_I2C  oled;  /* instantiate oled object */





/*-------------------------------------- setup() ---------------------------*/
void setup() 
{
  Serial.begin( 115200 );
  Serial.println( F( sTITLE sTRACKCONFIG ) );
  
  oled.init( & Serial );  /* init oled and pass it the Serial object */
  
  delay( 700 ); /* delay is not actually required */
  
  oled.clearScreen(); 
  oled.contrast( 200 );
  
  oled.putPROG( PSTR( sTITLE sTRACKCONFIG ), 0, 0 );
  oled.putPROG( PSTR(" info area"), 0, 1 );
  
  for( uint8_t cell = 0; cell < CELLCOUNT; ++cell )
  {
    oledPut_Cell( cell );
  }
  oled.putPROG( PSTR( sTRACKPLANLINE1 ), 0, 6 );
  oled.putPROG( PSTR( sTRACKPLANLINE2 ), 0, 7 );
  oledPut_CellCursor( 0, '>' );
}



/*--------------------------------- loop() --------------------------------*/
void loop() 
{
  if( Serial.available() > 0 ) 
  {
    char readChr = Serial.read();
    switch( readChr )
    {
      case '+' :
        oledPut_CellCursor( encPos, ' ' );    /* clear last cursor symbol  */
        if( encPos >= CELLCOUNT -1 ) 
        {
          encPos = 0;     /* cycle back   */
        } 
        else 
        {
          ++encPos;       /* increment    */
        }
        oledPut_CellCursor( encPos, '>' );      /* put new cursor symbol      */
        break;
     
      case '1' : case '2' : case '3' : case '4' : case '5' : case '6' :
        uint8_t num = readChr - '1';            /* sensor index num = 0-5    */
        if( sensor[num].nature != NATURE_NIU )
        {
          oled.cursor( sensor[num].addr.chrNum, sensor[num].addr.linNum );
          switch( sensor[num].state )
          {
             case SENSOR_FALSE :
                sensor[num].state = SENSOR_TRUE;
                oled.putChar('x');
                break;
            case SENSOR_TRUE :
                sensor[num].state = SENSOR_FALSE;
                oled.putChar('-');
                break;
             case SENSOR_NOTSEEN :
                sensor[num].state = SENSOR_TRUE;
                oled.putChar('.');
                break;
          }
        }
      break; /* break from switch(readChr) */
    }
  }
}



/*--------------------------------- oledPut_NN() --------------------------------
* 
* 
*/
void oledPut_NN( uint16_t nn )
{
  sprintf( oled.buf, "NN %d ", nn );
  oled.putRAM( oled.buf, 0, 5 );
}



/*--------------------------------- oledPut_Cell() --------------------------------
* 
* cell is "> label val ". label is 5 char, val is unsigned 0-255
*/
void oledPut_Cell( uint8_t cellNum )
{
  sprintf( oled.buf, " %5s %3d ", cell[cellNum].labl, var[cellNum] );
  oled.putRAM( oled.buf, cell[cellNum].addr.chrNum, cell[cellNum].addr.linNum );
}



/*---------------------------- oledPut_CellCursor() ------------------------------
* 
* 
*/
void oledPut_CellCursor( uint8_t cellNum, char chr )
{
  oled.cursor( cell[cellNum].addr.chrNum, cell[cellNum].addr.linNum );
  oled.putChar( chr );
}



/*---------------------------- eof ------------------------------------------*/
