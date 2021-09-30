 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <rtc.h>


const int32 magDec[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
const int32 magnitudes4bcd[] = {10000000,1000000,100000,10000,1000,100,10,1};
						//		0			1		2		  3		 4		5    6   7   8  9
extern void isr_TICK_dummy( void );




int32 int2bcd(int32 numero, int32 nBits){
	if( numero >= 100000000) return -1;


	int32 num = numero;
	int32 result = 0;
	uint32 count;
	int32 baseMask = 0xf;
	int32 digits[8];

	uint32 i = 0;
	for(; i < 8; i++){		//para cada magnitud decimal:
		count = 0;
		while( (num - magnitudes4bcd[i]) >= 0x0 ) {	// cuento los 10Ex que tiene el numero en count

			num -= magnitudes4bcd[i];
			count++;
		}

		digits[i] = count ;

	}
	int32 z = 7;
	for (; z >= 0 && nBits > 0; z--){

		if (nBits < 4) baseMask = baseMask >> (4-nBits);

		register aux = digits[z] << (4*(7 - z));
		result |= aux & (baseMask << 4*(7-z));
		nBits -= 4;
	}


	return result;
}

void rtc_init( void )
{
    TICNT   = 0x0;
    RTCALM  = 0x0;
    RTCRST  = 0x0;
        
    RTCCON |= 0x1;
    
    BCDYEAR = 0xd;
    /*BCDMON  &= ~(0x1 << 4);	//0-4	01
    BCDMON
    BCDMON  |= 1;*/
    BCDMON |= int2bcd(1,5);
    BCDMON &= (int2bcd(1,5) | ~(31));
    /*BCDDAY  &= ~(3 << 4);
    BCDDAY  |= (1);
    BCDDAY  &= ~(7 << 1);*/
    BCDDAY |= int2bcd(1,6);
    BCDDAY &= (int2bcd(1,6) | ~(63));
  /*  BCDDATE |= (1 << 1);
    BCDDATE &= ~(0x5);*/
    BCDDATE |= int2bcd(2,3);
    BCDDATE &= (int2bcd(2,3) | ~(7));
  /*  BCDHOUR &= (1 << 5);*/
    BCDHOUR |= int2bcd(0,6);
    BCDHOUR &= (int2bcd(0,6) | ~(63));
  /*  BCDMIN  &= (1 << 6);*/
    BCDMIN |= int2bcd(0,7);
    BCDMIN &= (int2bcd(0,7) | ~(127));
    /*BCDSEC  &= (1 << 6);*/
    BCDSEC |= int2bcd(0,7);
    BCDSEC &= (int2bcd(0,7) | ~(127));

    ALMYEAR = 0x0;
    ALMMON  = 0x0;
    ALMDAY  = 0x0;
    ALMHOUR = 0x0;
    ALMMIN  = 0x0;
    ALMSEC  = 0x0;

    RTCCON &= 0x1;
}

void rtc_puttime( rtc_time_t *rtc_time )// nos pasan enteros
{
    RTCCON |= 0x1;
    
    BCDYEAR = rtc_time->year;
    if( rtc_time->mon < 13 || rtc_time->mon > 0)
    	BCDMON  |= int2bcd(rtc_time->mon,5);
    BCDMON  &= (int2bcd(rtc_time->mon,5) | ~(31));
    BCDDAY  |= (int2bcd(rtc_time->mday,6));
    BCDDAY  &= (int2bcd(rtc_time->mday,6) | ~(63));
    BCDDATE |= (int2bcd(rtc_time->wday,3));
    BCDDATE &= (int2bcd(rtc_time->wday,3) | ~(7));
    BCDHOUR |= (int2bcd(rtc_time->hour,6));
    BCDHOUR &= (int2bcd(rtc_time->hour,6)| ~(63));
    BCDMIN  |= (int2bcd(rtc_time->min,7));
    BCDMIN  &= (int2bcd(rtc_time->min,7)| ~(127));
    BCDSEC  |= (int2bcd(rtc_time->sec,7));
    BCDSEC  &= (int2bcd(rtc_time->sec,7)| ~(127));
        
    RTCCON &= ~(0x1);
}





int32 bcd2int(int32 bcd, int32 nBits){
/*    
    I: result = sum(digitos cod en bcd * su magnitud correspondiente, desde 0 hasta i(grupos de 4 bits));

    0001 0010 0101 0111 = 1257 en decimal
  & 
                   1111    = conseguir esta mascara es dezplazar i*4 veces a la izq un 0xf
    0000 0000 0000 0111  = 7 en decimal; ademas dezplazar todo el bloque de 4 bits a la derecha. 
                                        para poder mult por su mag de 10

                    7 * mag[i] (uds) = 7
*/

uint32 i = 0;
uint32 result = 0;
int32 baseMask = 0xf, maski, digit, mult;

    while ( nBits > 0){
        
      if (nBits < 4) {
            baseMask = baseMask >> (4 -nBits);
        }

       maski = baseMask << (i*4);
       digit = maski & bcd;
       mult = (digit >> (i*4)) * magDec[i];
       result += mult;
       i++;
       nBits -= 4;

    } 

    return result;
}

void rtc_gettime( rtc_time_t *rtc_time )
{
    RTCCON |= 0x1;

    
    rtc_time->year = BCDYEAR;
    rtc_time->mon  = bcd2int(BCDMON,5);
    rtc_time->mday = bcd2int(BCDDAY,6);
    rtc_time->wday = bcd2int(BCDDATE,3);
    rtc_time->hour = bcd2int(BCDHOUR,6);
    rtc_time->min  = bcd2int(BCDMIN,7);
    rtc_time->sec  = bcd2int(BCDSEC,7);
    if( ! rtc_time->sec ){
    	 rtc_time->year = BCDYEAR;
    	 rtc_time->mon  = bcd2int(BCDMON,5);
    	 rtc_time->mday = bcd2int(BCDDAY,6);
    	 rtc_time->wday = bcd2int(BCDDATE,3);
    	 rtc_time->hour = bcd2int(BCDHOUR,6);
    	 rtc_time->min  = bcd2int(BCDMIN,7);
    	 rtc_time->sec  = bcd2int(BCDSEC,7);
    };

    RTCCON &= ~(0x1);
}


void rtc_open( void (*isr)(void), uint8 tick_count )
{
    pISR_TICK = (uint32)isr;
    I_ISPC    &= ~(1 << 7);
    INTMSK   &= ~((1 << 20) | (1 << 26));

    TICNT   |= ((1 << 7) | tick_count);
}

void rtc_close( void )
{
    TICNT     &= ~(1 << 7);
    INTMSK   |= (1 << 20) | (1 << 26);
    pISR_TICK = (uint32) isr_TICK_dummy;
}
