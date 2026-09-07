#include "board.h"
#include "c2000ware_libraries.h"
#include "device.h"
#include "driverlib.h"
#include "hw_i2c.h"
#include "meter_lcd.h"
 
void G(){
    MeterLcd::SetGlassSegment(MeterLcd::K42, ON);
}
void M(){
    MeterLcd::SetGlassSegment(MeterLcd::K43, ON);
}
void k(){
    MeterLcd::SetGlassSegment(MeterLcd::K44, ON);
}
void W(){
    MeterLcd::SetGlassSegment(MeterLcd::K45, ON);
    MeterLcd::SetGlassSegment(MeterLcd::K50, ON);
    MeterLcd::SetGlassSegment(MeterLcd::K46, ON);
}
void V(){
    MeterLcd::SetGlassSegment(MeterLcd::K45, ON);
}
void A(){
    MeterLcd::SetGlassSegment(MeterLcd::K52, ON);
    MeterLcd::SetGlassSegment(MeterLcd::K51, ON);
    MeterLcd::SetGlassSegment(MeterLcd::K47, ON);
}

void h(){
    MeterLcd::SetGlassSegment(MeterLcd::K49, ON);
    MeterLcd::SetGlassSegment(MeterLcd::K53, ON);
}

void Warning(){
    MeterLcd::SetGlassSegment(MeterLcd::K35, ON);
}

void L1(){
    MeterLcd::SetGlassSegment(MeterLcd::K32, ON);
}

void L2(){
    MeterLcd::SetGlassSegment(MeterLcd::K33, ON);
}

void L3(){
    MeterLcd::SetGlassSegment(MeterLcd::K34, ON);
}

void BATT(){
    MeterLcd::SetGlassSegment(MeterLcd::K17, ON);
}

void CONNECTED(){
    MeterLcd::SetGlassSegment(MeterLcd::K14, ON);
    MeterLcd::SetGlassSegment(MeterLcd::K16, ON);
}
void DISCONNECTED(){
    MeterLcd::SetGlassSegment(MeterLcd::K14, ON);
    MeterLcd::SetGlassSegment(MeterLcd::K15, ON);
}
uint8_t i = 0;

void main(void){
    
    Device_init();
    Device_initGPIO();

    Interrupt_initModule();
    Interrupt_initVectorTable();

    Board_init();
    C2000Ware_libraries_init();

    HwI2C::Init();
    MeterLcd::Init();

    MeterLcd::PrintErrorCode(5U);

    k(); V();

    
    // MeterLcd::SetGlassSegment(MeterLcd::K5, ON);

   
    LCD_SetSegment(COM2, 26U, ON);
    

    EINT;
    ERTM;

    while (1)
    {

        // for (i = 0;i<87;i++){

        //     DEVICE_DELAY_US(2000000);


        // }
     
    }
}
