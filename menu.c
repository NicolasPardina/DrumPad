 /*
 *      Author: Carlos Enrique Apaza Chuctaya
 *				Nicolás Pardina Popp
 */


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
#include "menu.h"
#include "trackBank.h"
#include <leds.h>
#include <segs.h>

uint8 context;
static uint8 optionPointedByArrow;
static uint8 openedFile = 15;
Options_t options[NUM_OPTIONS];

#define MEZCLAR_CTX	73
#define MENU_CTX 42
#define MENU_OPEN_CTX 144
#define MENU_VOLUMEN_CTX 55
#define MENU_BTCONF_CTX 156
#define MENU_BTCONF_CTX 156
#define MENU_BTCONFLOOP 20
#define MENU_BTCONFSLOTS 21
#define GRABAR_MIC 88
#define xLeftMargin 16
#define yTopMargin 32
#define wav1 0x0c400000
#define wav2 0x0c420000
#define wav3 0xc42ef7c
#define wav1
#define wav1
#define wav1
#define wav1
#define wav1
#define wav1
#define wav1
#define wav1
#define wav1
#define wav1
#define wav1
#define wav1





#define alto 48
#define margin 40

#define FS         (16000) // Frecuencia de muestreo de los sonidos pregrabados
#define NUM_TRACKS (16)    // Numero de pistas (y de teclas) del drumpad
#define NUM_SLOTS  (32)    // Numero de slots temporales en los que puede reproducirse un sonido
#define SPM        (480)          // Tempo de reproducción, en slots por minuto (480 SPM = 8 slots por segundo)
#define TPS        (FS/(SPM/60))  // Ticks (interrupciones) por slot 16000/480/60

void isr_timer0(void) __attribute__ ((interrupt ("IRQ"))) __attribute__ ((section (".sptext")));

void isr_timer0_myPlay(void) __attribute__ ((interrupt ("IRQ")));

void isr_PB(void)  __attribute__ ((interrupt ("IRQ")));

void isr_TS(void)  __attribute__ ((interrupt ("IRQ")));

const int32 wavs[] = {0x0c400000, 0x0c420000, 0x0c440000, NULL, 0x0c460000, 0x0c480000, 0x0c4a0000, NULL, 0x0c4c0000, 0x0c4e0000, 0x0c500000, 0x0c520000, NULL, 0x0c540000, 0x0c560000, 0x0c580000};
const int32 wavsEnd[] = {0xc40e790 , 0xc42ef7c, 0xc44f4d0 , NULL, 0xc470a78, 0xc4909c0, 0xc4b0168, NULL , 0xc4d1330, 0xc4e1338, 0xc50884c, 0xc524748, NULL ,0,0xc5427a0, 0xc564434, 0xc5d3904};
static volatile uint32 rel_tick;	//the cool boy
uint8 rec_flag;
uint8 rec_status;
uint8 track_number;
uint8 key_slots;
/* Definición del tipo pista */

typedef struct {
	fix16 *saddr; // Direccion de inicio del sonido (lo define la dirección indicada en el script de carga)
	fix16 *eaddr; // Direccion de fin del sonido (la indica gdb al cargar el sonido)
	fix16 *caddr;      // Direccion de la muestra actualmente en reproducción
	uint8 on; // Indica (ON/OFF) si la pista debe reproducirse, actua como flag de señalización entre hebras
	uint8 playing;     // Indica (TRUE/FALSE) si la pista está en reprodución
	uint8 loop;    // Indica (TRUE/FALSE) si la pista debe reproducirse en bucle
	uint8 slot[NUM_SLOTS]; // En el caso de una pista de reproducción en bucle, indica la secuencia de reproducción
	uint8 type;
	uint8 numf;
} track_t;

	/*
	 * 		Tamaño de track_t:
	 * */


/* Array de pistas, se ubicará en la scrachpad para minimizar el tiempo de acceso a los datos */

track_t tracks[NUM_TRACKS] __attribute__ ((section (".spdata"))) = { {
		(fix16 *) 0x0c400000, (fix16 *) 0xc40e790, NULL, OFF, FALSE, TRUE, { 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { (fix16 *) 0x0c420000,
		(fix16 *) 0xc42ef7c, NULL, OFF, FALSE, TRUE, { 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
				0, 0, 0 } }, { (fix16 *) 0x0c440000, (fix16 *) 0xc44f4d0, NULL,
		OFF, FALSE, TRUE, { 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { NULL, NULL,
		NULL, OFF, FALSE, FALSE, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, {
		(fix16 *) 0x0c460000, (fix16 *) 0xc470a78, NULL, OFF, FALSE, TRUE, { 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { (fix16 *) 0x0c480000,
		(fix16 *) 0xc4909c0, NULL, OFF, FALSE, TRUE, { 0, 0, 0, 1, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0 } }, { (fix16 *) 0x0c4a0000, (fix16 *) 0xc4b0168, NULL,
		OFF, FALSE, TRUE, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { NULL, NULL,
		NULL, OFF, FALSE, FALSE, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, {
		(fix16 *) 0x0c4c0000, (fix16 *) 0xc4d1330, NULL, OFF, FALSE, FALSE, { 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { (fix16 *) 0x0c4e0000,
		(fix16 *) 0xc4e1338, NULL, OFF, FALSE, TRUE, { 0, 0, 1, 0, 0, 0, 1, 0,
				0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
				0, 1, 0 } }, { (fix16 *) 0x0c500000, (fix16 *) 0xc50884c, NULL,
		OFF, FALSE, TRUE, { 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
				0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 } }, { NULL, NULL,
		NULL, OFF, FALSE, FALSE, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, {
		(fix16 *) 0x0c520000, (fix16 *) 0xc524748, NULL, OFF, FALSE, TRUE, { 1,
				0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0,
				0, 0, 1, 0, 0, 0, 1, 0, 0, 0 } }, { (fix16 *) 0x0c540000,
		(fix16 *) 0xc5427a0, NULL, OFF, FALSE, TRUE, { 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0 } }, { (fix16 *) 0x0c560000, (fix16 *) 0xc564434, NULL,
		OFF, FALSE, TRUE, { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
				0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 } }, {
		(fix16 *) 0x0c580000, (fix16 *) 0xc5d3904, NULL, OFF, FALSE, FALSE, { 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } };

/* Rutina de tratamiento de interrupción, se ubicará en la scrachpad para minimizar el tiempo de acceso al código */
void isr_timer0(void) {
	uint8 track_number;
	track_t *track;
	fix32 sample;
	fix32 auxSample;
	static uint8 slot_number = NUM_SLOTS - 1; // Por un error del compilador las variables static locales no pueden inicializarse a 0
	static uint16 ticks = TPS - 1;

	if (!ticks--) // Lleva la cuenta del slot en que se encuentra la reproducción
	{
		ticks = TPS - 1;
		if (!slot_number--)
			slot_number = NUM_SLOTS - 1;
		led_toggle(RIGHT_LED);
		led_toggle(LEFT_LED);
	}

	sample = 0;
	for (track_number = 0; track_number < NUM_TRACKS; track_number++) // Recorre el array de pistas para ir componiendo la muestra a enviar al audio codec
	{



		track = &(tracks[track_number]); // Para optimizar el acceso a los elementos de la pista, usa un puntero
		if (track->on)                             // Si la pista está activa...
		{


			if(tracks[track_number].type == 1)
			{
				if (track->loop && track->slot[slot_number] && ticks == TPS - 1) // ... y es un bucle, debe reproducirse en el slot actual, y estamos al inicio del slot
				{
					track->playing = TRUE;                // la pone en reproducción
					resetCount();
					closeFile(NULL);
				}

				if(track->playing)
				{
					openFile(track->numf, O_RDONLY);
					if(readWord(&auxSample) == EOF)
					{
						resetCount();
						closeFile(NULL);
						track->playing = FALSE;
						if (!track->loop) // y si además no es un bucle, la desactiva
							track->on = FALSE;

					}
					else
					{
						int16 ch = auxSample >> 16;
						sample = (sample + (fix32) ch);

						//no resetcount!
						closeFile(NULL);
					}
				}
			}

			else if(tracks[track_number].type == 0){

				if (track->loop && track->slot[slot_number] && ticks == TPS - 1) // ... y es un bucle, debe reproducirse en el slot actual, y estamos al inicio del slot
						{
							track->playing = TRUE;                // la pone en reproducción
							track->caddr = track->saddr;
						}
						if (track->playing)                    // ... y esta reproduciendose
						{

							sample = (sample + (fix32) *(track->caddr)); // acumula la muestra de la pista, al resto de muestras de las pistas activas
							track->caddr += 2; // ignora el otro canal (el sonido tipicamente esta grabado en estéreo)
							if (track->caddr >= track->eaddr) // Si se ha llegado al final de la pista...
							{
								track->playing = FALSE;      // ... la quita de reproducción
								if (!track->loop) // y si además no es un bucle, la desactiva
									track->on = FALSE;
							}
						}

			}

		}
	}
	// envia la muestra compuesta por ambos canales del audio codec
	iis_putSample((fix16) (sample >> 2), (fix16) (sample >> 2));

	if (rec_flag == 1)	//
		rel_tick++;


	I_ISPC |= BIT_TIMER0;	//limpia interrupcion pendiente

}

/* Marca sobre la cuadricula del LCD las pistas que deben reproducirse en bucle */

void lcd_refresh(void) {
	uint8 track_number;

	const uint16 x[NUM_TRACKS] = { 32, 112, 192, 272, 32, 112, 192, 272, 32,
			112, 192, 272, 32, 112, 192, 272 };
	const uint16 y[NUM_TRACKS] = { 16, 16, 16, 16, 76, 76, 76, 76, 136, 136,
			136, 136, 196, 196, 196, 196 };

	for (track_number = 0; track_number < NUM_TRACKS; track_number++)
		if (tracks[track_number].on && tracks[track_number].loop)
			lcd_putchar_x2(x[track_number], y[track_number], BLACK, 'X');
		else
			lcd_putchar_x2(x[track_number], y[track_number], WHITE, 'X');
}

/* Hebra en background que, en respuesta a pulsaciones del keypad, cambia el estado de las pistas y lo refresca en el LCD  */

void menuInit() {

	options[0] = Mezclar;
	options[1] = GrabarMic;
	options[2] = ConfButtons;
	options[3] = Volume;
	options[4] = Efects;
	optionPointedByArrow = 0;

	lcd_puts_x2(xLeftMargin + 32, yTopMargin, BLACK, "Mezclar");
	lcd_puts_x2(xLeftMargin + 32, yTopMargin + 32, BLACK, "GrabarMic");
	lcd_puts_x2(xLeftMargin + 32, yTopMargin + 64, BLACK, "ConfBotones");
	lcd_puts_x2(xLeftMargin + 32, yTopMargin + 96, BLACK, "Volumen");
	lcd_puts_x2(xLeftMargin + 32, yTopMargin + 128, BLACK, "Abrir");
	lcd_puts_x2(xLeftMargin, yTopMargin, BLACK, "=>");
}

void menuIdle() {
	lcd_clear();
	menuInit();
	while (context == MENU_CTX) {
		uint8 bt = pb_getchar();
		if (bt == PB_LEFT)
			nextOption();
		else if (bt == PB_RIGHT)
			selectOption();
	}
}

void menuMixIdle() {

	uint8 scancode;
	rec_flag = 0;
	lcd_clear();
	lcd_draw_grid();
	timer0_open_tick(isr_timer0, FS);
	openedFile = 0;
	while (context == MENU_MIX_CTX) {

		led_play();
		track_number = keypad_scan();
		if (track_number != KEYPAD_FAILURE) {

			if (rec_flag == 1) {
				if (rec_status == OPENED_FILE) {
					writeWord(rel_tick);
					writeWord(track_number);
				}

			}

			if (tracks[track_number].loop)
				tracks[track_number].on = !tracks[track_number].on; // Si la pista es un bucle, conmuta su estado ON/OFF
			else {
				tracks[track_number].on = ON; // Si la pista NO es un bucle, la pone en reproducción
				tracks[track_number].playing = TRUE;
				tracks[track_number].caddr = tracks[track_number].saddr;
			}
		}
		if ((scancode = pb_scan()) != PB_FAILURE) {	//PULSADOR PULSADO
			if (scancode == PB_LEFT) {	//salir
				rec_flag = 0;
				context = MENU_CTX;
				timer0_close();
				lcd_clear();
				closeFile(NULL);
				openedFile = 0;
			} else if (scancode == PB_RIGHT) {		//Toggle gragar/no grabar
				if (rec_flag == 0) {//NO estaba grabando > Abrir fichero y grabar

					segs_putchar(8);
					rec_flag = 1;
					openFile(MIDI_FILE, O_WRONLY);
					openedFile = 1;
					rec_status = OPENED_FILE;
					rel_tick = 0;
					sw_delay_ms(250);

				} else {//rec_flag == 1	//Ya estaba grabando > cerrar fichero y guardar
					segs_off();
					rec_flag = 0;
					rec_status = CLOSED_FILE;
					closeFile(NULL );
					openedFile = 0;
				}
			}
		}

		lcd_refresh();
	}
	led_stop();

}


void selCuadro(int8 num, uint16 color){

	lcd_draw_box(0,num *alto,320,alto *(num+1), color, 2);				//60 = alto

}

void ponerCuadro(int8 num, char* name, uint8 tipo, uint32 size){


	char int2str[12];


	lcd_puts_x2(10, num*alto , BLACK, name);	//NOMBRE

	typeToString(tipo, &int2str);
	lcd_puts(20, 32 + num*alto, BLACK, &int2str);	//tipo

	int32 off = intToString(size, &int2str);

	lcd_puts(100, 32 + (num * alto), BLACK, "Tamaño:");
	lcd_puts(160, 32 + (num * alto), BLACK, &int2str[off]);	//tamaño
	lcd_puts(260, 32 + (num * alto), BLACK, "Bytes\0");

}

int8 display_files() {



	uint8 type[MAX_FILES];
	uint32 size[MAX_FILES];
	char* names[MAX_FILES];
	uint8 valido[MAX_FILES];


	int8 numf = dir(NULL, NULL, NULL );
	int8 i = 0;

	for(; i< MAX_FILES; i++){
		valido[i] = 0;
	}
	for(i = 0; i < numf; i++) valido[i] = 1;


	uint8 numPag = 0;

	uint8 eleccion = 0;
	int8 pointed = 0, numPantalla = 0;
	lcd_clear();
	for (i = numf - 1; i >= 0; i--) {
		dir(&names[i], &type[i], &size[i]);
	}

	for(i = numPag*4; i < numPag*4 + 4; i++){
		if(valido[i] == 1) ponerCuadro(i, names[i], type[i], size[i]);
	}
	while(eleccion != 11){

		selCuadro(pointed,BLACK);

		lcd_puts_x2( 60, 200, BLACK, "Pag: ");
		lcd_putint_x2(124, 200, BLACK, numPag + 1);
		lcd_puts_x2(140, 200, BLACK, "/4");



		if(( eleccion = keypad_scan()) != KEYPAD_FAILURE){

			if(eleccion == KEYPAD_KEYF){
				numPag = (numPag + 1) % 4;
				lcd_clear();

				for(i = numPag*4; i < numPag*4 + 4; i++){
					if(valido[i] == 1) ponerCuadro(i % 4, names[i], type[i], size[i]);
				}
			}
			else if(eleccion == KEYPAD_KEYC){
				removeFile(pointed + (numPag*4));
				pointed = -1;
				eleccion = 11;
			}
			else{
				selCuadro(eleccion % 5, BLACK);
				selCuadro(pointed, WHITE);
				pointed = eleccion % 5;
			}
		}


		if((eleccion = pb_scan()) != PB_FAILURE){
			if(eleccion == PB_LEFT) {
						eleccion = 11;
						pointed = -1;
			}
			else if(eleccion == PB_RIGHT) return pointed + (numPag*4);
		}


	}

	return pointed;

}

int32 play_wav_file(int32 numf){

	uint8 playing = 1, scancode;
	uint8 play_flag = 0;

	uint32 ticks = 32000;


	char * name;
	getFileName(numf, &name);

	uint8 type = getFileType(numf);
	uint32 size =getFileSize(numf);

	openFile(numf, O_RDONLY);
	resetCount();
	openedFile = 1;

	lcd_clear();

	ponerCuadro(0, name, type, size);
	selCuadro(0, BLACK);


	lcd_puts_x2(140, 120, BLACK, "STOP...");

	while(playing){
		if ((scancode = pb_scan()) != PB_FAILURE) {	//PULSADORPULSADO

			if (scancode == PB_RIGHT) {	//	play			DERECHO			-play

				if (play_flag == 0) {	//parar > tocar
					if (openedFile == 0) {
						openFile(numf, O_RDONLY);
						resetCount();
					}
						play_flag = 1;
						led_play();
						timer0_open_tick(isr_timer0_myPlay, FS);
						lcd_puts_x2(140, 120, BLACK, "PLAY...");

					} else {	//tocar > parar
						play_flag = 0;
						led_stop();
						timer0_close();
						lcd_puts_x2(140, 120, BLACK, "STOP...");
					}

					sw_delay_ms(250);


				}

				else if (scancode == PB_LEFT) {			//IZQUIERDO	 		-salir

					timer0_close();
					resetCount();
					closeFile(NULL );
					playing = 0;
					play_flag = 0;
				}

			}

		//



	}

	return 0;


}


void menuOpenIdle()
{

	uint8 scancode = PB_FAILURE;
	uint8 play_flag = 0;
	openedFile = 0;
	int8 option = 0;


	while (context == MENU_OPEN_CTX) {

		option = display_files();


		if(option == -1){
			timer0_close();
			context = MENU_CTX;
			closeFile(NULL );
		}
		else {
			uint8 t = getFileType(option);
			if(t == MIDI_FILE){
				play_midi_file(option);
			}
			else if( t == WAV_FILE){
				play_wav_file(option);
			}
			else{

			}

		}



	}
}

void isr_timer0_myPlay() {		//incluye led_play

	int32 ret;
	int32 buf;
	uint16 ch1, ch2;
	static int32 ticks = 32000;
	if ((ret = readWord(&buf)) == EOF) {
		timer0_close();
		resetCount();
		closeFile(NULL );
		lcd_puts_x2(140, 120, BLACK, "FIN... ");
		openedFile = 0;
		led_stop();

	} else if (ret < 0) {

	}
	else {
		int32 aux = (int32) (buf & 0xFFFF0000);
		aux = aux >> 16;
		ch1 = aux;

		ch2 = buf & 0xFFFF;

		iis_putSample(ch1, ch2);

		if (--ticks == 0) {
			led_toggle(RIGHT_LED);
			led_toggle(LEFT_LED);
			ticks = 32000;
		}

	}
	I_ISPC |= BIT_TIMER0;

}

void menuVolumeIdle() {
	uint8 volumen;
		uint16 x, y;
		volumen = uda1341ts_getvol();
		lcd_clear();
		dibujarBarrasDeVolumenVacias();
		rellenarBarrasDeVolumen(volumen / 8);
		ts_on();
		pbs_open(isr_PB);
		while(context == MENU_VOLUMEN_CTX)
		{
			ts_getpos(& x, & y);
			if(x <= 52)
				volumen = VOL_MIN;
			else if (x > 265)
				volumen = VOL_MAX;
			else
				volumen = ((x - 17) /35) * 8;
			rellenarBarrasDeVolumen(volumen / 8);
			uda1341ts_setvol(volumen);
			iis_playwawFile(0x0c400000);
			sw_delay_ms(100);
		}
		ts_off();
}

void dibujarBarrasDeVolumenVacias() {
	uint8 i;
	for (i = 0; i < 8; i++)
		lcd_draw_box(28 + i * 35, 195 - 25 * i, 47 + i * 35, 220, BLACK, 3);
}

void rellenarBarrasDeVolumen(int volumen) {
	uint8 i;
	for (i = 0; i < 8; i++) {
		if (i <= volumen)
			lcd_draw_box(31 + i * 35, 198 - 25 * i, 44 + i * 35, 217, BLACK, 7);
		else
			lcd_draw_box(31 + i * 35, 198 - 25 * i, 44 + i * 35, 217, WHITE, 7);
	}
}

void isr_PB(void)
{
	if(context == MENU_VOLUMEN_CTX)
		context = MENU_CTX;
	else if (context == MENU_BTCONFLOOP)
		context = MENU_BTCONF_CTX;
	else if (context == MENU_BTCONFSLOTS)
			context = MENU_BTCONF_CTX;
    INTMSK  |= (1 << 21);
	I_ISPC |= BIT_PB;	//limpia interrupcion pendiente
	pbs_close();
}


void menuBtConfIdle()
{
	lcd_clear();
	context = MENU_BTCONF_CTX;
	ts_on();
	uint16 x, y;
	while (context == MENU_BTCONF_CTX)
	{
		lcd_clear();
		lcd_puts_x2(xLeftMargin + 32, yTopMargin, BLACK, "Loop");
		lcd_puts_x2(xLeftMargin + 32, yTopMargin + 32, BLACK, "Slots");
		lcd_puts_x2(xLeftMargin + 32, yTopMargin + 64, BLACK, "Tracks");
		lcd_puts_x2(xLeftMargin + 32, yTopMargin + 96, BLACK, "Salir");
		ts_getpos(& x, & y);
		if(y <= yTopMargin + 32)
			btConfLoop();
		else if (y < yTopMargin +64)
			btConfSlots();
		else if (y < yTopMargin + 96)
			btConfTracks();
		else
			context = MENU_CTX;
	}
	ts_off();
}

void btConfLoop()
{
	context = MENU_BTCONFLOOP;
	pbs_open(isr_PB);
	uint8 key;
	lcd_clear();
	lcd_draw_grid();
	lcd_refreshLoop();
	while (context == MENU_BTCONFLOOP)
	{
		key = keypad_scan();
		if(key != KEYPAD_FAILURE)
		{
			tracks[key].loop = !tracks[key].loop;
			lcd_refreshLoop();
		}
	}
}

void lcd_refreshLoop(void) {
	uint8 track_number;

	const uint16 x[NUM_TRACKS] = { 32, 112, 192, 272, 32, 112, 192, 272, 32,
			112, 192, 272, 32, 112, 192, 272 };
	const uint16 y[NUM_TRACKS] = { 16, 16, 16, 16, 76, 76, 76, 76, 136, 136,
			136, 136, 196, 196, 196, 196 };

	for (track_number = 0; track_number < NUM_TRACKS; track_number++)
		if (tracks[track_number].loop)
			lcd_putchar_x2(x[track_number], y[track_number], BLACK, 'X');
		else
			lcd_putchar_x2(x[track_number], y[track_number], WHITE, 'X');
}

void btConfSlots()
{
	context = MENU_BTCONFSLOTS;
	pbs_open(isr_PB);
	ts_open(isr_TS);
	uint8 key;
	lcd_clear();
	lcd_draw_grid();
	key_slots = keypad_getchar();
	lcd_clear();
	drawGrid32();
	refreshSlots(key_slots);
	while (context == MENU_BTCONFSLOTS);
	ts_close();
}

void refreshSlots(uint8 key)
{
	uint16 i;
	for(i = 0; i < NUM_SLOTS; i++)
	{
		if (tracks[key].slot[i] == 1)
			lcd_putchar_x2((12 + i * 40) % 320 , 16 + 60 * (i / 8), BLACK, 'X');
		else
			lcd_putchar_x2((12 + i * 40) % 320 , 16 + 60 * (i / 8), WHITE, 'X');
	}
}

void drawGrid32()
{
	uint8 i;
	for(i = 0; i < 9; i++)
		lcd_draw_vline(0, 238, i * 40, BLACK, 1);
	for(i = 0; i < 5; i++)
		lcd_draw_hline(0, 318, i * 60, BLACK, 1);
}

void isr_TS()
{
	uint16 x;
	uint16 y;
	uint8 pos;
	if(context == MENU_BTCONFSLOTS)
	{
		ts_getpos(&x, &y);
		pos = (x / 40) + 8 * (y / 60 );
		if(tracks[key_slots].slot[pos] == 1)
			tracks[key_slots].slot[pos] = 0;
		else
			tracks[key_slots].slot[pos] = 1;
		refreshSlots(key_slots);
	}
	I_ISPC |= BIT_TS;
}

void btConfTracks()
{
	lcd_clear();
	lcd_draw_grid();
	uint8 scancode, track_change;
	while((scancode = keypad_scan()) == KEYPAD_FAILURE);

	track_change = display_files();
	if(track_change == -1)
	{
		tracks[scancode].saddr = wavs[scancode];
			tracks[scancode].eaddr = wavsEnd[scancode];
			tracks[scancode].caddr = wavs[scancode];
			tracks[scancode].on = OFF;
			tracks[scancode].playing = FALSE;
			tracks[scancode].type = 1;
			tracks[scancode].numf = track_change;
	}
	tracks[scancode].saddr = 0;
	tracks[scancode].eaddr = getFileSize(track_change);
	tracks[scancode].caddr = 0;
	tracks[scancode].on = OFF;
	tracks[scancode].playing = FALSE;
	tracks[scancode].type = 1;
	tracks[scancode].numf = track_change;
	uint8 i = 0;
	for(i = 0; i < NUM_SLOTS; i++)
		tracks[scancode].slot[i] = 0;


}

void refreshTracks(uint8 key)
{
	uint8 track_number;
	const uint16 x[NUM_TRACKS] = { 32, 112, 192, 272, 32, 112, 192, 272, 32,
			112, 192, 272, 32, 112, 192, 272 };
	const uint16 y[NUM_TRACKS] = { 16, 16, 16, 16, 76, 76, 76, 76, 136, 136,
			136, 136, 196, 196, 196, 196 };

	for (track_number = 0; track_number < NUM_TRACKS; track_number++)
		if (tracks[track_number].on && tracks[track_number].loop)
			lcd_putchar_x2(x[track_number], y[track_number], BLACK, 'X');
		else
			lcd_putchar_x2(x[track_number], y[track_number], WHITE, 'X');
}

void menuGrabarMicIdle() {

	uint8 rec_flag = 0;
	uint8 scancode = PB_FAILURE;
	uint16 ch1, ch2;

	lcd_clear();
	lcd_puts_x2(40, 40, BLACK, "Grabar Micro");
	lcd_puts_x2(100, 130, BLACK, "[OFF]");

	openedFile = 0;


	while (context == GRABAR_MIC_CTX) {

		if ((scancode = pb_scan()) != PB_FAILURE) {

			if (scancode == PB_RIGHT) {	//	toggle recording
				if (rec_flag == 1) {	//estaba grabando > parar

					lcd_puts_x2(100, 130, BLACK, "[OFF]");
					segs_off();
					closeFile(NULL);
					sw_delay_ms(250);
					rec_flag = 0;
					openedFile = 0;
				} else if (rec_flag == 0) {	//no estaba grabando > empezar

					if(openedFile == 0){
						openedFile = 1;
						if(openFile(WAV_FILE, O_WRONLY) != 0){

							openedFile = 0;
							context = MENU_CTX;
							closeFile(NULL);
						}
					}

					rec_flag = 1;
					sw_delay_ms(250);
					lcd_puts_x2(100, 130, BLACK, "[ON] ");
					segs_putchar((uint8) 8);
				}
			}

			else {	//scancode == PB_LEFT	-salir al menu
				if (rec_flag == 1) {	//estaba grabando > parar

					rec_flag = 0;
				}
				closeFile(NULL );
				context = MENU_CTX;
				openedFile = 0;
			}

		}

		if (rec_flag == 1) {


			if ((IISFCON & 0xf) < 2) {

			} else {
				ch1 = IISFIF;
				ch2 = IISFIF;
				uint32 aux = (ch1 << 16) | ch2;
				if (writeWord(aux) == ENOBLOCK) {
					rec_flag = 0;
					openedFile = 0;
					segs_putchar(8);
					lcd_puts_x2(100, 130, BLACK, "[OFF]");
					closeFile("wav\0");
				}
			}
		}
	}

}
void nextOption() {
	lcd_puts_x2(xLeftMargin, yTopMargin + optionPointedByArrow * 32, BLACK,
			"  ");
	optionPointedByArrow = (optionPointedByArrow + 1) % NUM_OPTIONS;//back end
	lcd_puts_x2(xLeftMargin, yTopMargin + optionPointedByArrow * 32, BLACK,
			"=>");

}

void selectOption() {
	if (optionPointedByArrow == 0)
		context = MENU_MIX_CTX;
	else if (optionPointedByArrow == 1)
		context = GRABAR_MIC;
	else if (optionPointedByArrow == 2)
		context = MENU_BTCONF_CTX;
	else if (optionPointedByArrow == 3)
		context = MENU_VOLUMEN_CTX;
	else if (optionPointedByArrow == 4) {
		context = MENU_OPEN_CTX;
	}
}

/* Visualiza una cuadrícula en el LCD de 4x4 */

void lcd_draw_grid(void) {
	lcd_draw_box(0, 0, 79, 59, BLACK, 1);
	lcd_draw_box(79, 0, 159, 59, BLACK, 1);
	lcd_draw_box(159, 0, 239, 59, BLACK, 1);
	lcd_draw_box(239, 0, 319, 59, BLACK, 1);
	lcd_draw_box(0, 59, 79, 119, BLACK, 1);
	lcd_draw_box(79, 59, 159, 119, BLACK, 1);
	lcd_draw_box(159, 59, 239, 119, BLACK, 1);
	lcd_draw_box(239, 59, 319, 119, BLACK, 1);
	lcd_draw_box(0, 119, 79, 179, BLACK, 1);
	lcd_draw_box(79, 119, 159, 179, BLACK, 1);
	lcd_draw_box(159, 119, 239, 179, BLACK, 1);
	lcd_draw_box(239, 119, 319, 179, BLACK, 1);
	lcd_draw_box(0, 179, 79, 239, BLACK, 1);
	lcd_draw_box(79, 179, 159, 239, BLACK, 1);
	lcd_draw_box(159, 179, 239, 239, BLACK, 1);
	lcd_draw_box(239, 179, 319, 239, BLACK, 1);
}

void led_play() {
	led_off(RIGHT_LED);
	led_off(LEFT_LED);
	led_on(RIGHT_LED);
}
void led_stop() {
	led_off(RIGHT_LED);
	led_off(LEFT_LED);
}

void play_midi_file(uint8 num){


	uint8 playing = 0;

	uint32 scancode, tck = 0;
	rel_tick = 0;
	rec_flag = 0;
	uint8 pb;
	uint32 ticks = 32000;

	openFile(num,O_RDONLY);
	resetCount();
	openedFile = 1;

	lcd_clear();
	lcd_draw_grid();
	if(readWord(&tck) == EOF) goto error;


	while(1){			//usar goto, demasiado tratamiento de errores;



		if((pb =pb_scan()) != PB_FAILURE){


			if(pb == PB_LEFT){
				goto error;
			}
			else if(pb == PB_RIGHT){

				if(playing == 0){
					rec_flag = 1;
					led_play();
					playing = 1;
					if (openedFile == 0){
						openFile(num,O_RDONLY);
						resetCount();
						openedFile = 1;
					}
					timer0_open_tick(isr_timer0, FS);
				}
				else if(playing == 1){
					timer0_close();
					playing = 0;
					led_stop();
				}

			}

		}

		track_number = keypad_scan();
		if (track_number != KEYPAD_FAILURE) {


					if (tracks[track_number].loop)
						tracks[track_number].on = !tracks[track_number].on; // Si la pista es un bucle, conmuta su estado ON/OFF
					else {
						tracks[track_number].on = ON; // Si la pista NO es un bucle, la pone en reproducción
						tracks[track_number].playing = TRUE;
						tracks[track_number].caddr = tracks[track_number].saddr;
					}
		}





		 while(rel_tick >= tck){

			 if(readWord(&scancode) == EOF) goto error;


			 if (tracks[scancode].loop)
			 	tracks[scancode].on = !tracks[scancode].on; // Si la pista es un bucle, conmuta su estado ON/OFF
			 else {
			 	tracks[scancode].on = ON; // Si la pista NO es un bucle, la pone en reproducción
			 	tracks[scancode].playing = TRUE;
			 	tracks[scancode].caddr = tracks[scancode].saddr;
			}
			 if(readWord(&tck) == EOF) {
			 	goto error;
			 }


		 }

		lcd_refresh();


		if (--ticks == 0) {
			led_toggle(RIGHT_LED);
			led_toggle(LEFT_LED);
			ticks = 64000;
		}

	}



	error:
		resetCount();
		closeFile(NULL);
		playing = 0;
		rel_tick = 0;
		timer0_close();
		return;

}


