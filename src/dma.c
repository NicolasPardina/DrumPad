 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <dma.h>

extern void isr_BDMA0_dummy( void ); 

void bdma0_init( void )
{
	BDCON0 = 0;
	BDISRC0 = 0;
	BDIDES0 = 0;
	BDICNT0 = 0;
}

void bdma0_open( void (*isr)(void) )
{
    pISR_BDMA0 = isr;
    I_ISPC     |= 1 << 17;
    INTMSK    &= ~((1 << 17) | (1 << 26));
}

void bdma0_close( void )
{
    INTMSK    |= (1 << 17);
    pISR_BDMA0 = (uint32) isr_BDMA0_dummy;
}
