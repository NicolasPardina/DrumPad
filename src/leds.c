 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <s3c44b0x.h>
#include <leds.h>



void leds_init( void )
{
	PCONB &= ~(3 << 9);
}

void led_on( uint8 led )
{
    if(led == LEFT_LED){
    	PDATB &= ~(1 << 9);
    }
    else{
    	PDATB &= ~(1 << 10);
    }
}

void led_off( uint8 led )
{
	if(led == LEFT_LED){
	    	PDATB |= (1 << 9);
	    }
	    else{
	    	PDATB |= 1 << 10;
	    }
}

void led_toggle( uint8 led )
{
    if(led_status(led) != 0){
    	led_off(led);
    }
    else{
    	led_on(led);
    }
}

uint8 led_status( uint8 led )
{
	if(PDATB & (1 << (led + 8)))
		return LED_OFF;
	else
		return LED_ON;
}
