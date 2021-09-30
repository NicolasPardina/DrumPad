/*-------------------------------------------------------------------
**
**  Fichero:
**    drumpad.c  19/6/2019
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**      Author: Carlos Enrique Apaza Chuctaya
**				Nicolás Pardina Popp
**  Propósito:
**    Drumpad elemental con arquitectura background/foreground
**
**  Notas de diseño:
**    - El sonido se genera por interrupción periódica por lo que 
**      la RTI esta optimizada y se ubicada en scratchpad
** 
**-----------------------------------------------------------------*/

#include <s3c44b0x.h>
#include <s3cev40.h>
#include <common_types.h>
#include <fix_types.h>
#include <system.h>
#include <timers.h>
#include <keypad.h>
#include <uda1341ts.h>
#include <iis.h>
#include <lcd.h>
#include <ts.h>
#include <leds.h>
#include <segs.h>
#include "menu.h"

/*********************************************************************/


#define MEZCLAR_CTX	73
#define MENU_CTX 42
#define MENU_OPEN_CTX 144
#define MENU_VOLUMEN_CTX 55
#define MENU_BTCONF_CTX 156
#define GRABAR_MIC_CTX 88
/*********************************************************************/
extern uint8 context;



#define margen 5





/*********************************************************************/

/*
 * 	ICONOS, 60 X 60 PIXELS, CODIFICADOS EN BINARIO, MONOCROMO. SOLO OCUPA 450 BYTES
 * */
const uint8 ico[450] = {

};


void pantallaDeCarga(){
	lcd_puts_x2(70, 60, BLACK, "PSyD DRUMPAD");
	lcd_draw_box(40,120,280,180, BLACK, 3);
}

void carga(int32 num){

	lcd_draw_hline( 40 + margen + num*(60),  100 + margen + num*60, 125, BLACK, 50 );
}

void main( void )
{  
    

	sys_init();
	timers_init();
	ts_init();
	lcd_init();
	lcd_clear();
	lcd_on();
	pantallaDeCarga();
	pbs_init();
    carga(0);
    uda1341ts_init();
    uart0_init();
    carga(1);
    iis_init( IIS_POLLING );
    leds_init();
    led_off(RIGHT_LED);
    led_off(LEFT_LED);
    segs_init();
    segs_off();
    carga(2);
    trackBank_init();
    keypad_init();




    menuInit();
    context = MENU_CTX;

    while(1)
    {
    	if(context == MENU_CTX)
    		menuIdle();
    	else if(context == MENU_MIX_CTX)
    		menuMixIdle();
    	else if(context == MENU_OPEN_CTX)
    		menuOpenIdle();
    	else if(context == MENU_VOLUMEN_CTX)
    		menuVolumeIdle();
    	else if(context == MENU_BTCONF_CTX)
    		menuBtConfIdle();
    	else if (context == GRABAR_MIC_CTX)
    		menuGrabarMicIdle();
    }


}


uint8 color_pos(uint8 ico, uint8 x, uint8 y){
	return 0;
}

void draw_icon(uint8 ico, uint8 x, uint8 y){		

	register i,j;
	for(i = 0; i < 105; i++){
		for(j = 0; j < 120; j++){
			lcd_putpixel(i,j, color_pos(ico,x,y));
		}
	}

}





