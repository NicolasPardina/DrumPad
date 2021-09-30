/*
 * trackBank.h
 *
 *  Created on: 10/12/2019
 *      Author: Carlos Enrique Apaza Chuctaya
 *				Nicolás Pardina Popp
 */

#ifndef TRACKBANK_H_
#define TRACKBANK_H_


#include <common_types.h>

#define ENOBLOCK -17
#define NAMEFILE_SIZE 20
#define NOFILESPC -13
#define EFAULT -8
#define MAX_FILES 16
#define MAX_BLOCKS 488
#define O_RDONLY 2
#define O_WRONLY 3
#define MIDI_FILE 111
#define WAV_FILE 77
#define DEFAULT_FILE 12
#define OPENED_FILE 5
#define CLOSED_FILE 8
#define EOF -41

typedef struct{
	uint32 ch[1023];
	struct block * next_block;
}block ;

typedef struct{
	uint8 flag;
	uint8 type;
	uint32 size;
	block * firstBlock;
	block * lastBlock;
	char name[NAMEFILE_SIZE];
	uint32 * cursor;
	uint32 offset;
	uint16 rel_count;
} myFile;

typedef struct{
	myFile files[MAX_FILES];// files[MAX_FILES];
	block * freeBlocks;
	block bank[MAX_BLOCKS];
}bankFS;

//Haz dir para ver los archivos. VALOR DEVUELTO: numero de ficheros activos,
//SI buf == NULL, prepara la func para empezar a enviarte todos los ficheros;
//a modo de optarg, las llamadas sucesivas con buf == "puntero a un buf de 20 chars*,
//pondra en buf el nombre del primer fichero, asi como sus propiedades. Y devolvera el num de fich restantes.
/*PUBLIC*/	int32 dir(char **buf, uint8 *type, uint32 *size);


/*Inicializa el track bank, fi, cursor, bloques enlazados,...*/
/*PUBLIC*/	void trackBank_init();

/*VALOR DEVUELTO: 0 si exito, y negativo si error
 * SI mode == O_RDONLY
 * 		type	: 	es el numero de fichero en el trakBank
 * SI mode == O_WRONLY
 * 		type	: 	es el tipo de fichero a crear, WAV_FILE para grab de mic, MIDI_FILE, para puls de keypad*/
/*PUBLIC*/	int32 openFile(uint32 type, uint8 mode );//

/*PRIVATE*/	int32 getNewBlock();

/*PUBLIC*/	void closeFile(char *name);

/*PUBLIC*/	int32 writeWord(uint32 buf);

/*PUBLIC*/	int32 readWord(int32 *buf);//return 0 si exito, < 0 en otro caso, con codigo de error;

/*PUBLIC*/	int32 getFileSize(int32 num);
/*PUBLIC*/	uint8 getFileType(int32 num);
/*PUBLIC*/	void getFileName(uint8 numf, char ** n);


int32 intToString(int32 numero, char* numAlfa);
		/*
		 * convierte el entero en string de manera eficiente sin usar division de biblioteca; hacer &numAlfa[int32(retornado)] para imprimirlo bien.
		 * */

void typeToString( uint8 type, char *buf);

	/*
	 * pone "WAV" o "MIDI" en buf, segun corresponda el type.
	 * */




#endif /* TRACKBANK_H_ */
