 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <l3.h>
#include <uda1341ts.h>

#define ADDRESS (0x05)

#define DATA0  (0x0)
#define DATA1  (0x1)
#define STATUS (0x2)

#define EA (0x18 << 3)
#define ED (0x7 << 5)

static uint8 volume;
static uint8 state;



void uda1341ts_init( void )
{
    L3_init();     
    
    L3_putByte( (ADDRESS << 2) | STATUS, L3_ADDR_MODE );
    L3_putByte( (1 << 6) | (2 << 4), L3_DATA_MODE );
    L3_putByte( (2 << 4), L3_DATA_MODE );

    L3_putByte( (ADDRESS << 2) | DATA0, L3_ADDR_MODE  ); 
    L3_putByte( EA | (2), L3_DATA_MODE ); 
    L3_putByte( ED | 1, L3_DATA_MODE );
  
    uda1341ts_setvol( VOL_MAX );
    uda1341ts_mute(MUTE_OFF);
    state = 0;
    uda1341ts_on( UDA_DAC );
    uda1341ts_on( UDA_ADC );

}

void uda1341ts_mute( uint8 on )
{

	L3_putByte( (ADDRESS << 2) | DATA0, L3_ADDR_MODE );
	L3_putByte( (1 << 7) | (on << 2), L3_DATA_MODE );

};

void uda1341ts_on( uint8 converter )
{
	uint8 auxState = state;
	auxState |= converter;
	L3_putByte( (ADDRESS << 2) | STATUS, L3_ADDR_MODE );
	L3_putByte( (1 << 7 | 1 << 5) | auxState, L3_DATA_MODE );
	state = auxState;
}

void uda1341ts_off( uint8 converter )
{
	uint8 aux = state & ~converter;
	L3_putByte( (ADDRESS << 2) | STATUS, L3_ADDR_MODE );
	L3_putByte( (1 << 7 | 1 << 5) | aux , L3_DATA_MODE );
	state = aux;
}

uint8 uda1341ts_status( uint8 converter )
{
  return state;
}

void uda1341ts_setvol( uint8 vol )
{
	L3_putByte( (ADDRESS << 2) | DATA0, L3_ADDR_MODE );
	uint8 auxVol = 0x3F - vol;
	L3_putByte( auxVol, L3_DATA_MODE );
	volume = vol;
};

uint8 uda1341ts_getvol( void )
{
   return volume;
};
