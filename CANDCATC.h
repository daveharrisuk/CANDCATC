/* file: CANDCATC.h
*-------------------------------------------------------------------------------
*
* Automatic Train Control of DC locos using CBUS, 
*  with OLED display and rotary encoder.
*
*
* © Dave Harris, 2021 (Andover, UK) MERG M2740
* 
*/ 
#ifndef _CANDCATC_h_
#define _CANDCATC_h_



/*-------------------------------------------------------------------
*  Possible values for TRACKCONFIG define            
*/
#define CONFIG_1TO1  0
/*  
*   x_________-            End to end.  Runs one loco (x)      
*/
#define CONFIG_1TO2  1
/*        ____x            Headshunt to 2 sidings
*  x____/_____-            Runs 1 or 2 locos (x)           
*/
#define CONFIG_2TO2  2
/* x___      ___-          2 sidings to line to 2 sidings
*  x___\____/___x          Runs 1, 2, or 3 locos (x)        
*/
#define CONFIG_LOOP  3
/*      __x__              Headshunt to loop to headshunt
*  x___/__-__\___x         Runs 1, 2 or 3 locos (x)         
*/


/* This define selects one of the possible track config values  */

#define TRACKCONFIG  CONFIG_LOOP




const uint8_t CELLCOUNT { 8 };    /* the number of cells on the OLED screen */

const uint8_t SENSORCOUNT { 6 };  /* the total number of sensors            */




enum State_t: uint8_t
{
  STATE_STOPDELAY = 0,    /* Stopped and delay count down   */
  STATE_ACCEL = 1,        /* Accelerating duty cycle        */
  STATE_STEADY = 2,       /* Steady duty cycle              */
  STATE_DECEL = 3         /* Decelerating duty cycle        */
};



struct DisplayAddr_t
{
  uint8_t chrNum;
  uint8_t linNum;
};


struct Cell_t
{
  const DisplayAddr_t  addr;          /* chr#, lin#     */
  const char           labl[6];       /* 5 char +null   */
};



enum CellIndex_t: uint8_t   /* index to cells    */
{
  CELL_DCMIN  = 0,
  CELL_INCDEC = 1,
  CELL_DCMAX  = 2,
  CELL_DLYSEC = 3,
  CELL_DCCUR  = 4,
  CELL_ROUTE  = 5
};


/* data for put cells on screen   char#, line#, label                  */

const Cell_t  cell[CELLCOUNT]
{
  0,  2, "DCmin",    /* cell 0 char#, line#, label  */
  11, 2, "+- ms",    /* cell 1 char#, line#, label  */
  0,  3, "DCmax",    /* cell 2 char#, line#, label  */
  11, 3, "Dly s",    /* cell 3 char#, line#, label  */
  0,  4, "DCcur",    /* cell 4 char#, line#, label  */
  11, 4, "Route",     /* cell 5 char#, line#, label  */
  0,  5, "   mA",    /* cell 6 char#, line#, label  */
  11, 5, "state"     /* cell 7 char#, line#, label  */
};



enum SensorNature_t : uint8_t
{
  NATURE_STOP,          /* stop/pause without changing direction    */
  NATURE_END,           /* terminate before changing direction      */
  NATURE_SLOW,          /* slowdown before stopping                 */
  NATURE_NIU            /* sensor is Not In Use (not displayed)     */
};


enum SensorState_t : uint8_t
{
  SENSOR_FALSE = 0,     /* displays '-'                             */
  SENSOR_TRUE = 1,      /* displays 'x'                             */
  SENSOR_NOTSEEN        /* displays '.'   CBUS SOD resolves this    */
};


struct Sensor_t
{
  const DisplayAddr_t  addr;      /* char#, line# on screen         */
  const SensorNature_t nature;    /* the nature of the sensor       */
        SensorState_t  state;     /* the current state of sensor    */
};





/*------------------------ function declarations ----------------------------*/

void setup();

void loop();

void oledPut_NN( uint16_t );

void oledPut_Cell( uint8_t );

void oledPut_CellCursor( uint8_t, char );



#endif /* _CANDCATC_h_
 ---------------------------------- eof CANDCATC.h -----------------------------
*/
