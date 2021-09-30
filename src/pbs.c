 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <pbs.h>
#include <timers.h>

extern void isr_PB_dummy( void );

void pbs_init( void )
{
    timers_init();
}

uint8 pb_scan( void )
{
    if( (PDATG & PB_LEFT) != PB_LEFT  )// que esta a 0 el pin left
        return PB_LEFT;
    else if( (PDATG & PB_RIGHT) != PB_RIGHT)	//ESta a 0 el pin right
        return PB_RIGHT;
    else
        return PB_FAILURE;
}

uint8 pb_status( uint8 scancode )
{

	if(scancode == PB_LEFT)
	{
		 if( (PDATG & PB_LEFT) == PB_LEFT ) // no esta pulsado,
		        return PB_UP;
		 else
			 return PB_DOWN;
	}
	else if(scancode == PB_RIGHT)
	{
		if( (PDATG & PB_RIGHT) == PB_RIGHT )	//no esta pulsado, xq es 1
			return PB_UP;
		 else
			return PB_DOWN;
	}
	return PB_FAILURE;
}

void pb_wait_keydown( uint8 scancode )
{
	while( pb_status(scancode) == PB_UP );
	sw_delay_ms( PB_KEYDOWN_DELAY );
}

void pb_wait_keyup( uint8 scancode )
{

	while( pb_status(scancode) == PB_UP);	//mientras este arriba se queda aqui
	sw_delay_ms( PB_KEYDOWN_DELAY );
	while( pb_status(scancode) == PB_DOWN);
	sw_delay_ms( PB_KEYUP_DELAY );
}

void pb_wait_any_keydown( void )
{
	while( pb_scan() != PB_FAILURE  );
	sw_delay_ms( PB_KEYDOWN_DELAY );
}

void pb_wait_any_keyup( void )

{
	uint8 scancode;	//PB_RIGHT o PB_LEFT
	while( (scancode = pb_scan()) == PB_FAILURE );
	sw_delay_ms( PB_KEYDOWN_DELAY );
	while( pb_status(scancode) == PB_DOWN);
	sw_delay_ms( PB_KEYUP_DELAY );

}

uint8 pb_getchar( void )
{
	uint8 scancode;	//PB_RIGHT o PB_LEFT

	 while((scancode = pb_scan()) == PB_FAILURE );//(PDATG & (PB_LEFT | PB_RIGHT)) == (PB_LEFT | PB_RIGHT) );
	 sw_delay_ms( PB_KEYDOWN_DELAY );
	 while(pb_status(scancode) == PB_DOWN);
	 sw_delay_ms( PB_KEYUP_DELAY );
	 return scancode;


}

uint8 pb_getchartime( uint16 *ms )
{
    uint8 scancode;	//PB_RIGHT o PB_LEFT
    
    while( (scancode = pb_scan()) == PB_FAILURE );
    sw_delay_ms( PB_KEYDOWN_DELAY );
    timer3_start();
    
    


    while(pb_status(scancode) == PB_DOWN);
    sw_delay_ms( PB_KEYUP_DELAY );
    *ms = timer3_stop() / 10;

    return scancode;
}

uint8 pb_timeout_getchar( uint16 ms )
{
	 uint8 scancode;	//PB_RIGHT o PB_LEFT
	    timer3_start_timeout(PB_TIMEOUT);
	    while( ( scancode = pb_scan() == PB_FAILURE ) && (timer3_timeout() != 0) );
	    sw_delay_ms( PB_KEYDOWN_DELAY );
	    timer3_stop();

	    timer3_start();




	    while( pb_status(scancode) == PB_DOWN);
	    ms = timer3_stop() / 10;
	    sw_delay_ms( PB_KEYUP_DELAY );

	    return scancode;
}

void pbs_open( void (*isr)(void) )
{
    pISR_PB   = isr;
    EXTINTPND = 0xf;
    I_ISPC    |= 1 << 21;
    INTMSK   &= ~((1 << 21) | (1 << 26)) ;
}

void pbs_close( void )
{
    INTMSK  |= (1 << 21);
    pISR_PB      = (uint32)isr_PB_dummy;

}
