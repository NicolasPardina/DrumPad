 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <keypad.h>

extern void isr_KEYPAD_dummy( void );

void keypad_init( void )
{
	EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);
	//keypad_open( keypad_down_isr );
};

uint8 keypad_scan( void )
{
    uint8 aux;

    aux = *( KEYPAD_ADDR + 0x1c );
    if( (aux & 0x0f) != 0x0f ) // distinto de 0xf significa que alguna está pulsada en esa fila
    {
        if( (aux & 0x8) == 0 )
            return KEYPAD_KEY0;
        else if( (aux & 0x4) == 0 )
            return KEYPAD_KEY1;
        else if( (aux & 0x2) == 0 )
            return KEYPAD_KEY2;
        else if( (aux & 0x1) == 0 )
            return KEYPAD_KEY3;
    }
    aux = * (KEYPAD_ADDR + 0x1a );
    if( (aux & 0x0f) != 0x0f )
        {
            if( (aux & 0x8) == 0 )
                return KEYPAD_KEY4;
            else if( (aux & 0x4) == 0 )
                return KEYPAD_KEY5;
            else if( (aux & 0x2) == 0 )
                return KEYPAD_KEY6;
            else if( (aux & 0x1) == 0 )
                return KEYPAD_KEY7;
        }
    aux = * (KEYPAD_ADDR + 0x16 );
    if( (aux & 0x0f) != 0x0f )
            {
                if( (aux & 0x8) == 0 )
                    return KEYPAD_KEY8;
                else if( (aux & 0x4) == 0 )
                    return KEYPAD_KEY9;
                else if( (aux & 0x2) == 0 )
                    return KEYPAD_KEYA;
                else if( (aux & 0x1) == 0 )
                    return KEYPAD_KEYB;
            }
    aux = * (KEYPAD_ADDR + 0x0E );
        if( (aux & 0x0f) != 0x0f )
        {
           if( (aux & 0x8) == 0 )
               return KEYPAD_KEYC;
           else if( (aux & 0x4) == 0 )
        	   return KEYPAD_KEYD;
           else if( (aux & 0x2) == 0 )
        	   	return KEYPAD_KEYE;
           else if( (aux & 0x1) == 0 )
        	    return KEYPAD_KEYF;
          }

    return KEYPAD_FAILURE;
}

uint8 keypad_status( uint8 scancode )
{
	uint8 aux;
	switch (scancode){
	case KEYPAD_KEY0:
		aux = *( KEYPAD_ADDR + 0x1c );
		if( (aux & 0x8) == 0 )
		    return KEY_DOWN;
		else
			return KEY_UP;
		break;
	case KEYPAD_KEY1:
			aux = *( KEYPAD_ADDR + 0x1c );
			if( (aux & 0x4) == 0 )
			    return KEY_DOWN;
			else
				return KEY_UP;
			break;
	case KEYPAD_KEY2:
			aux = *( KEYPAD_ADDR + 0x1c );
			if( (aux & 0x2) == 0 )
			    return KEY_DOWN;
			else
				return KEY_UP;
			break;
	case KEYPAD_KEY3:
			aux = *( KEYPAD_ADDR + 0x1c );
			if( (aux & 0x1) == 0 )
			    return KEY_DOWN;
			else
				return KEY_UP;
			break;
	case KEYPAD_KEY4:
			aux = *( KEYPAD_ADDR + 0x1a );
			if( (aux & 0x8) == 0 )
			    return KEY_DOWN;
			else
				return KEY_UP;
			break;
		case KEYPAD_KEY5:
				aux = *( KEYPAD_ADDR + 0x1a );
				if( (aux & 0x4) == 0 )
				    return KEY_DOWN;
				else
					return KEY_UP;
				break;
		case KEYPAD_KEY6:
				aux = *( KEYPAD_ADDR + 0x1a );
				if( (aux & 0x2) == 0 )
				    return KEY_DOWN;
				else
					return KEY_UP;
				break;
		case KEYPAD_KEY7:
				aux = *( KEYPAD_ADDR + 0x1a );
				if( (aux & 0x1) == 0 )
				    return KEY_DOWN;
				else
					return KEY_UP;
				break;
		case KEYPAD_KEY8:
			aux = *( KEYPAD_ADDR + 0x16 );
			if( (aux & 0x8) == 0 )
			    return KEY_DOWN;
			else
				return KEY_UP;
			break;
		case KEYPAD_KEY9:
				aux = *( KEYPAD_ADDR + 0x16 );
				if( (aux & 0x4) == 0 )
				    return KEY_DOWN;
				else
					return KEY_UP;
				break;
		case KEYPAD_KEYA:
				aux = *( KEYPAD_ADDR + 0x16 );
				if( (aux & 0x2) == 0 )
				    return KEY_DOWN;
				else
					return KEY_UP;
				break;
		case KEYPAD_KEYB:
				aux = *( KEYPAD_ADDR + 0x16);
				if( (aux & 0x1) == 0 )
				    return KEY_DOWN;
				else
					return KEY_UP;
				break;
		case KEYPAD_KEYC:
				aux = *( KEYPAD_ADDR + 0x0E );
				if( (aux & 0x8) == 0 )
				    return KEY_DOWN;
				else
					return KEY_UP;
				break;
			case KEYPAD_KEYD:
					aux = *( KEYPAD_ADDR + 0x0E );
					if( (aux & 0x4) == 0 )
					    return KEY_DOWN;
					else
						return KEY_UP;
					break;
			case KEYPAD_KEYE:
					aux = *( KEYPAD_ADDR + 0x0E );
					if( (aux & 0x2) == 0 )
					    return KEY_DOWN;
					else
						return KEY_UP;
					break;
			case KEYPAD_KEYF:
					aux = *( KEYPAD_ADDR + 0x0E );
					if( (aux & 0x1) == 0 )
					    return KEY_DOWN;
					else
						return KEY_UP;
					break;
	}
  return KEYPAD_FAILURE;
}

void keypad_wait_keydown( uint8 scancode )
{
     while( 1 )
     {
    	while((PDATG & 2));	//no pulsado ALGO
    	 sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
        if ( scancode == keypad_scan() )
            return;
        while(!(PDATG & 2));	// pulsado -> esperar
        sw_delay_ms( KEYPAD_KEYUP_DELAY );
    }
}

void keypad_wait_keyup( uint8 scancode )
{
	while(1){
		while(PDATG & 2);
		sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
		if(scancode == keypad_scan()){

			sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
			return;
		}


		while(!(PDATG & 2));
		sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	}
}

void keypad_wait_any_keydown( void )
{

    while((PDATG & 2));
    sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
}

void keypad_wait_any_keyup( void )
{
	uint8 scancode;
	while((PDATG & 2));
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	scancode = keypad_scan();

    while(!(PDATG & 2) != 0);
    sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
}

uint8 keypad_getchar( void )
{
	uint8 scancode;
	while((PDATG & 2));
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	scancode = keypad_scan();
	while(!(PDATG & 2));
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	return scancode;
}

uint8 keypad_getchartime( uint16 *ms )
{
	   uint8 scancode;

	    while( (PDATG & 2) );
	    sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	    timer3_start();
	    scancode = keypad_scan();
	    while(!(PDATG & 2));
	    sw_delay_ms( KEYPAD_KEYUP_DELAY );
	    *ms = timer3_stop() / 10;

	    return scancode;
}

uint8 keypad_timeout_getchar( uint16 ms )
{
	 uint8 scancode;
	 timer3_start_timeout(KEYPAD_TIMEOUT);
	 while( (PDATG & 2) && (timer3_timeout() != 0) );
	 sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	 timer3_stop();
	 timer3_start();
	 scancode = keypad_scan();
	 while(!(PDATG & 2));
	 ms = timer3_stop() / 10;
	 sw_delay_ms( KEYPAD_KEYUP_DELAY );


	 return scancode;
}

void keypad_open( void (*isr)(void) )
{
	pISR_KEYPAD   = isr;
	EXTINTPND = 0;
	I_ISPC    |= 1 << 24;
	INTMSK   &= ~((1 << 24) | (1 << 26)) ;
}

void keypad_close( void )
{
	INTMSK  |= (1 << 24);
	pISR_KEYPAD      = (uint32) isr_KEYPAD_dummy;
}
