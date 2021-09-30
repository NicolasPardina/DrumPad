 /*
 *      Author: Carlos Enrique Apaza Chuctaya
 *				Nicolás Pardina Popp
 */

#include "trackBank.h"

static myFile *fi;
static bankFS bank;	//ponerlo en la zona de memoria adecuada, con "__ATRIBUTTE__ "
static uint8 autoNameFile = 15;	//para implementar nombre auto si necesario.
static uint8 autoNameFileMix = 15;
static int8 openMode;

const int32 magnitudes[] = {1,1000000000,100000000,10000000,1000000,100000,10000,1000,100,10,1};
const uint32 magnitudesHex[] = {1, 268435456, 16777216, 1048576, 65536, 4096, 256, 16, 1};
const char charDef[] = {'0','1','2','3','4','5','6','7','8','9'};

void trackBank_init(){

	register i = 0;
	for(i = 0; i < MAX_FILES; i++){
		bank.files[i].flag = 0;
	}

	for(i = 0; i < MAX_BLOCKS - 1; i++){
		bank.bank[i].next_block = &bank.bank[i+1];
	}

	bank.freeBlocks = &bank.bank[0];

	fi = NULL;


	openMode = -1;
}

int32 openFile(uint32 type, uint8 mode){

	uint32 i = 0;

	if(fi != NULL) return EFAULT; //si no hay otro abierto ret error!

	if(mode == O_WRONLY){
		fi = &bank.files;
		while(i < MAX_FILES && fi[i].flag == 1) i++ ;	//buscar sitio i = sitio
		if (i == MAX_FILES) return NOFILESPC;	//no hay sitio? error!
		fi = &fi[i];	//fi es myFile *

		if(bank.freeBlocks == NULL) return ENOBLOCK;
		fi->flag = 1;	//poner ocupado
		fi->type = type;	//poner tipo


		fi->firstBlock = bank.freeBlocks;
		fi->lastBlock = bank.freeBlocks;
		bank.freeBlocks = bank.freeBlocks->next_block;
		fi->lastBlock->next_block = NULL;
		fi->rel_count = 0;
		fi->size = 0;
		fi->offset = 0;
		fi->cursor = fi->firstBlock;

		openMode = O_WRONLY;
	}
	else if(mode == O_RDONLY){

		fi = &bank.files[type];

		openMode = O_RDONLY;
	}



	return 0;

}

void resetCount()
{
	if(fi == NULL) return;
	else
	{
		fi->cursor = fi->firstBlock;
		fi->offset = 0;
		fi->rel_count = 0;
	}

}

int32 getNewBlock(){

	if(bank.freeBlocks == NULL) return ENOBLOCK;

	fi->lastBlock->next_block = bank.freeBlocks;	//2
	bank.freeBlocks = bank.freeBlocks->next_block;	//3
	fi->lastBlock = fi->lastBlock->next_block;	//4
	fi->lastBlock->next_block = NULL;	//5

	fi->cursor = fi->lastBlock;
	fi->rel_count = 0;

	return 0;
}
void closeFile(char *name){

	const char defName[] = "Grabacion\0";
	const char defNameMix[] = "Mezcla\0";
	if(fi != NULL){

		if(openMode == O_WRONLY){

			if(name != NULL){
						register i = 0;
						while(i < 20 && *name != 0){
							fi->name[i] = *name;
							i++;
							name++;
						}
						i++;
						fi->name[i] = 0;

					}
					else {
						register i = 0;
						if(fi->type == WAV_FILE){
							while(i < 20 && defName[i] != 0){
								fi->name[i] = defName[i];
								i++;
							}
							fi->name[i] = (16-autoNameFile) + 48;
							i++;
							fi->name[i] = 0;
							autoNameFile--;
						}
						else if(fi->type == MIDI_FILE){
							while(i < 20 && defNameMix[i] != 0){
								fi->name[i] = defNameMix[i];
								i++;
							}
							fi->name[i] = (16-autoNameFileMix) + 48;
							i++;
							fi->name[i] = 0;
							autoNameFileMix--;
						}


					}
		}

		fi = NULL;


	}


}


int32 writeWord(uint32 buf)
{

	if(fi->rel_count == 1023) {
		if(getNewBlock() != 0)
			return ENOBLOCK;
	}

	*(fi->cursor) = buf;
	fi->size += 1;
	fi->cursor++;
	fi->offset++;
	fi->rel_count++;
	return 0;

}

int32 readWord(int32 *buf){

	if(fi == NULL) return EFAULT;

	if( fi->offset == fi->size ) return EOF;

	if(fi->type != DEFAULT_FILE && fi->rel_count == 1023){
		fi->cursor = *(fi->cursor);
		fi->rel_count = 0;
	}

	fi->offset += 1;
	*buf = *(fi->cursor);
	fi->cursor++;
	fi->rel_count++;

	return 0;
}

int32 dir(char **buf, uint8 *type, uint32 *size){

	static int8 dir_status = -1;
	int8 ret = -1;
	uint8 numf;
	if(buf == NULL){
		dir_status = 0;
		uint8 i = 0;
		numf = 0;
		for (i = 0; i < 16; i++){
			if(bank.files[i].flag == 1) numf++;
		}
		ret = numf;
		dir_status = numf;
	}
	else{
		if(dir_status > 0 && dir_status < 20){
			ret = --dir_status;
			*type = (uint8) bank.files[dir_status].type;
			*size = (uint32) bank.files[dir_status].size;
			*buf = (char * ) &(bank.files[dir_status].name);

		}
		else {
			ret = -1;
		}
	}

	return ret;
}

int32 removeFile(numf){


	bank.files[numf].lastBlock->next_block = bank.freeBlocks;
	bank.freeBlocks = bank.files[numf].firstBlock;
	bank.files[numf].flag = 0;

	return 0;
}

int32 getFileSize(int32 num){

	return bank.files[num].size;
}
uint8 getFileType(int32 num){
	return bank.files[num].type;
}
void getFileName(uint8 numf, char ** n){
	*n = (char *) &(bank.files[numf].name);
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

void typeToString( uint8 type, char *buf){

	uint8 i;
	const char midi[4] = "MIDI";
	const char wav[3] = "WAV";
	if(type == MIDI_FILE){

		for(i = 0; i < 4; i++){
			buf[i] = midi[i];
		}
		buf[i] = 0;
	}
	else if(type == WAV_FILE){
		for(i = 0; i < 3; i++){
			buf[i] = wav[i];
		}
		buf[i] = 0;
	}

}











