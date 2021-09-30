 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <s3c44b0x.h>
#include <l3.h>
#include <leds.h>

#define SHORT_DELAY    { int8 j; for( j=0; j<4; j++ ); }

void L3_init( void )
{
	PCONA &= ~(1 << 9);
	PCONB &= ~(3 << 4);
}

void L3_putByte( uint8 byte, uint8 mode )
{
    uint8 i;
    uint8 rled, lled;
    
    rled = !led_status( RIGHT_LED );
    lled = !led_status( LEFT_LED );    




    PDATB = (1 << 5) | (mode << 4);
    /*if (mode)
        	PDATB |= 1 << 4;
        else
        	PDATB &= ~(1 << 4);*/

    SHORT_DELAY;

    for( i=0; i<8; i++ )
    {
        PDATB &= ~(1 << 5);

      PDATA = ((byte & 1) << 9);
      byte = byte >> 1;
        SHORT_DELAY;    
        PDATB |= 1 << 5;
        SHORT_DELAY;
    }
    PDATB = (rled << 10) | (lled << 9) | (1 << 5) | (1 << 4);   
}


