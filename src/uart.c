 /*      
 *	Carlos Enrique Apaza Chuctaya
 *	Nicolás Pardina Popp
 */
 
#include <s3c44b0x.h>
#include <uart.h>




const int32 magnitudes[] = {1,1000000000,100000000,10000000,1000000,100000,10000,1000,100,10,1};
const uint32 magnitudesHex[] = {1, 268435456, 16777216, 1048576, 65536, 4096, 256, 16, 1};
const char charDef[] = {'0','1','2','3','4','5','6','7','8','9'};

void uart0_init( void )
{
    UFCON0 = 0x1;
    UMCON0 = 0;
    ULCON0 |=  0x3 ;
    UBRDIV0 = 34;
    UCON0 = 0x5;
}

void uart0_putchar( char ch )
{
    while(  UFSTAT0 & (1 << 9) );
    UTXH0 = ch;
}        

char uart0_getchar( void )
{
    while( !(UFSTAT0 & 0xF) );
    return URXH0;
}

void uart0_puts( char *s )
{
	char * puntero = s;



    while(*puntero != 0){
    	 uart0_putchar(*puntero);
    	 puntero++;
    }
}


int32 intToString(int32 numero, char* numAlfa){ /*numAlfa es un puntero al final de un espacio de chars de 11 (reservados)
// 36.521.382 => [0|0|0|3|6|5|2|1|3|8|2] en chars
//				  0 1 2 3 4 5 6 7 8 9 10*/
	//			  		|
	//			 		 puntero

	if ( numero == 0){
		*numAlfa = '0';
		*++numAlfa = '\0';
		return 0;
	}



	int32 num = numero;
	uint8 off = 0;

	char *punt = ++numAlfa;	//puntero donde empieza la cadena definitiva a devolver
	*(punt - 1) = '0';			//poner un 0 provisional para el invariante
	uint32 count;
	uint8 negFlag = 0, nullFlag = 0;
	if(num < 0){
		negFlag = 1;	//ver si es negativo
		num *= -1;

	}
	uint8 i = 1;
	for(; i < 11; i++){		//para cada magnitud decimal:
		count = 0;

		while( (num - magnitudes[i]) >= 0x0 ) {	// cuento los 10Ex que tiene el numero en count
			nullFlag = 1;
			num -= magnitudes[i];
			count++;
		}

		*numAlfa = charDef[count];	//	chars almacenados en Chardef, si cuento 7 pongo un '7'

		if(nullFlag == 0){
			punt++;	//si no el alcanzado el primer numero =! 0, dezplazar el punta donde vaya a empezar
			off++;
		}
		numAlfa++;


	}


	*numAlfa = '\0';	//terminar la cadena

	if(negFlag == 1) {

		*(punt - 1) = '-';	//Si es neg: pongo el -, justo antes, y aculizo el puntero ahi.
		numAlfa = punt - 1;
		return off;

	}
	else {

		numAlfa = punt;	//si no era negativo no hace falta nada y solo devuelvo el puntero apuntanto al principio de la cadena valida.
		return off +1;
	}


}


void uart0_putint( int32 i )
{

   char s[12];
   char * aux = &(s[0]);
   int32 off = intToString(i, aux);

   uart0_puts(&s[off]);
}

void uart0_puthex( uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'a' + c - 10;
        i = i >> 4;
    } while( i );

    uart0_puts( p );
}

void uart0_gets( char *s )
{
	char *puntero = s - 1;
	   do {
		   puntero++;
		    *puntero = uart0_getchar();
	   } while( *puntero != '\n');
	   *puntero = 0;


}

int32 uart0_getint( void )
{
	char mem[32];

	char * s = &(mem[0]);
	char * cursor = s;

	int32 acc = 0;
	uint32 count = 10;




	uart0_gets(s);

	while(*cursor != 0) {
			cursor++;
		}	//cursor esta al final
	cursor--;

	while(cursor >= s){
		if( *cursor >= 48 && *cursor <= 57){
			acc += ((*cursor) - 48) * magnitudes[count];
			count--;
			cursor--;
		}
		else if (*cursor == '-'){
			acc *= -1;
			cursor--;
		}
		else return -1; // destruye el invariante!


	}


 return acc;
}

uint32 uart0_gethex( void )
{
	char mem[32];

	char * s = &(mem[0]);
	char * cursor = s;
	uint32 acc = 0;
	uint32 count = 8;


	uart0_gets(s);

	while(*cursor != 0) cursor++;
	cursor--;

	while(cursor >= s){
		if(*cursor >= 48 && *cursor <= 57 ){

			acc += (*cursor - 48) * magnitudesHex[count];


		}
		else if( *cursor >= 65 && *cursor <= 70){
			acc += (*cursor - 55) * magnitudesHex[count];

		}
		else if( *cursor >= 97 && *cursor <= 102){
			acc += (*cursor - 87) * magnitudesHex[count];

		}


		else return -1;
		count--;
		cursor--;
	}


 return acc;
}


