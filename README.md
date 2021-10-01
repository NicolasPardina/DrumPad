# DrumPad
Proyecto de la asignatura Programación de Sistemas y Dispositivos.


Durante el curso se desarrolló el firmware para la placa de prototipado Embest S3CEV40 contenido en el directorio Drumpad/src/. Para el proyecto final de la asignatura se proporcionaba una biblioteca precompilada DrumPad/libBSP.a que implementa todas las funciones de acceso a los distintos dispositivos de la placa (incluyendo las opcionales), así como un proyecto básico que implementa un drumpad elemental de 16 pistas con secuenciamiento fijo, y una colección de ficheros wav.


El sistema tiene una pantalla de carga mientras se inicializan los dispositivos que se van a usar. Al cargarse aparece un menú con las siguientes opciones:

Menu.c

	- Mezcla: proyecto basico + Posibilidad de grabar teclas pulsadas (MIDI).
	
	- Grabar micrófono: Graba por micrófono nuevos ficheros wav para su posterior reproducción o para asociarlo a un botón.
	
	- Configurar botones: Configurar teclas; si el wav se reproduce en bucle después de pulsar la tecla o sólo una vez, el archivo de sonido asociado a una tecla de tracks, o el slot en el que se reproduce.
	
	- Volumen: Cambia el volumen general utilizando la pantalla táctil.
	
	- Abrir ficheros: Reproducir ficheros grabados (tanto mezclas como grabaciones de micrófono). Si se reproduce una mezcla, te permite mezclar sobre ella. Indica tipo de archivo y tamaño.
  
  
Trackbank.c

Contiene un sistema de ficheros basado en listas enlazadas. Los bloques libres están enlazados y se usan y sueltan por los ficheros. Tamaño de word 4 bytes, tamaño de bloque 4 kilobytes, tamaño máximo de fichero 4 megabytes. Funciones open, close, write, read. Read  mantiene el puntero donde lo dejó antes de cerrarse.  Hasta 30 segundos de grabación wav, y numerosos minutos de grabación de mezcla (en vez de guardar el sonido, se guarda cuando se pulsa cada tecla.



