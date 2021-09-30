 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <iis.h>
#include <dma.h>

static void isr_bdma0( void ) __attribute__ ((interrupt ("IRQ")));

static uint8 flag;
static uint8 iomode;

void iis_init( uint8 mode )
{
    iomode = mode;

    if( mode == IIS_POLLING )
    {
        IISPSR   |= 0x77;
        IISMOD = 0x0c9;
        IISFCON |= (1 << 8) | (1 << 9);
        IISFCON &= ~((1 << 11) | (1 << 10));
       IISCON |= 3;
       IISCON &= ~(0xf << 2);
    }
    if( mode == IIS_DMA )
    {

        IISPSR  = 0x77;
        IISMOD = 0xc9;
        //IISFCON = 0xf00;
        IISFCON = 0;
       // IISCON = 0x33;
        IISCON = 0;
        bdma0_init();
        bdma0_open( isr_bdma0 );
        flag = OFF;
    }
}

static void isr_bdma0( void )
{

	if(!(BDICNT0 & (1 << 21)))
    IISCON &= 0xFFFFFFFE; // poner IISCON[0] a 0
    flag = OFF;
    I_ISPC = BIT_BDMA0;
}

inline void iis_putSample( int16 ch0, int16 ch1 )
{
	 while((IISFCON & 0xf0) >= 0x60);	//espera que haya dos huecos
    IISFIF = (uint32)ch0;
    IISFIF = (uint32)ch1;
}

inline void iis_getSample( int16 *ch0, int16 *ch1 )
{
	 while((IISFCON & 0xf) < 2);
	    *ch0 = IISFIF;
	    *ch1 = IISFIF;
}

void iis_play( int16 *buffer, uint32 length, uint8 loop )
{
    uint32 i;
    int16 ch1, ch2;

    if( iomode == IIS_POLLING )
        for( i=0; i<length/2; )
        {
            ch1 = buffer[i++];
            ch2 = buffer[i++];
            iis_putSample( ch1, ch2 );
        }
    if( iomode == IIS_DMA ){

    	IISMOD |= (2 << 6);
    	IISMOD &= ~(1 << 8);

    	IISCON &= ~(1 << 3);
    	IISCON |= (1 | (1 << 1) | (1 << 5) | (1 << 2));
    	IISCON &= ~((1 << 4) | (1 << 3));


    	IISFCON |= ((1 << 9) | (1 << 11));
    	IISFCON &= ~(1 << 8 | 1 << 10);

    	BDIDES0 |= (3 << 28) | (1 << 30);
    	BDIDES0 &= ~(1 << 31);
    	BDIDES0 &= ~0xFFFFFFF; // 7 F
    	BDIDES0 |= 0x1D18010;



    	BDISRC0 |= (1 << 30) | ((1 << 28) | (uint32)buffer) ;
    	BDISRC0 &= ~((1 << 31) | (1 << 29));
    	BDISRC0 &= ((uint32)buffer | 0xf0000000);

    	if( loop == 0)
        	{
        	   BDICNT0 &= ~(1 << 21);
        	}
        	else
        	{
        		BDICNT0 |= (1 <<22);
        		BDICNT0 &=  ~(1 << 23);
        	   BDICNT0 |= (1 << 21);
        	}
    	BDICNT0 &=  ~((1 << 31 ) | (0xFFFFF));
    	BDICNT0 |= (1 << 30) | (3 << 22) | length;
    	BDICNT0 |= (1 << 20);

    }
}

void iis_rec( int16 *buffer, uint32 length )
{
    uint32 i;
    int16 ch1, ch2;

    if( iomode == IIS_POLLING )
    {
    	 for( i=0; i<length/2; )
    	        {
    	            iis_getSample(&ch1, &ch2 );
    	            buffer[i++] = ch1;
    	            buffer[i++] =  ch2;
    	        }
    }
    if( iomode == IIS_DMA )
    {
        while( flag != OFF );
        BDISRC0  = (1 << 30) | (3 << 28) | (uint32) &IISFIF;	//configura la bdma0
        BDIDES0  = (2 << 30) | (1 << 28) | (uint32) buffer;      
        BDCON0   = 0;
        BDICNT0  = (1 << 30) | (1 << 26) | (3 << 22) | (0xfffff & length); 
        BDICNT0 |= (1 << 20);


        IISMOD |= (1 << 6);
        IISMOD &= ~((1 << 8) | (1 << 7));

        IISCON |= (1 | (1 << 1) | (1 << 4)|  (1 << 3));
        IISCON &= ~((1 << 5) | (1 << 2));

        IISFCON |= ((1 << 8) | (1 << 10));
        IISFCON &= ~(1 << 9 | 1 << 11);

        flag = ON; //hecho

    }
}

void iis_pause( void )
{
	IISCON &= ~1;
}

void iis_continue( void )
{
	IISCON |= 1;
}

uint8 iis_status( void )
{
	return IISCON & 1;
}

void iis_playwawFile( uint8 *fileAddr )
{
    uint32 size;

    while ( !(fileAddr[0] == 'd' && fileAddr[1] == 'a' && fileAddr[2] == 't' && fileAddr[3] == 'a') )
        fileAddr++;
    fileAddr += 4;

    size = (uint32) fileAddr[0];
    size += (uint32) fileAddr[1] << 8;
    size += (uint32) fileAddr[2] << 16;
    size += (uint32) fileAddr[3] << 24;
    fileAddr += 4;

    iis_play( (int16 *)fileAddr, size, OFF );
}

