#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_LENGTH 80               ///< Longitud del buffer
static char recieve[BUFFER_LENGTH];     ///< Buffer modificado por el modulo a llamar

/*
	Se ingresa en un buffer (stringE_D) un mensaje encriptado por el módulo encriptador,
	el módulo desencriptador toma ese mensaje del buffer y lo desencripta.
*/
int main(int argc, char** argv){

	int Id_Enc, Id_Des;
	char stringE_D[BUFFER_LENGTH];

	printf("\nIngrese mensaje a encriptar. \n");

	Id_Enc = open("/dev/encriptador", O_RDWR);
	Id_Des = open("/dev/desencriptador", O_RDWR);
	
	while(1){

        strcpy(stringE_D,"\0");
		if( scanf("%[^\n]%*c", stringE_D) <= 0 ){
			printf("Error. No se ha ingresado nigun mensaje\n");
		}

		if( strcmp(stringE_D,"exit") == 0){ break; }

		if( write(Id_Enc, stringE_D, strlen(stringE_D)) < 0 ){ return errno;}

		if( read(Id_Enc, recieve, strlen(stringE_D)) < 0 ){ return errno;}

		printf("\nEl mensaje encriptado es: %s \n" , recieve);
        strcpy(stringE_D, recieve);

        if( write(Id_Des, stringE_D, strlen(stringE_D) )  < 0 ) { return errno;}

        printf("\nPresione ENTER Desencriptar.\n");
      	getchar();

      	if (read(Id_Des, recieve, strlen(stringE_D)) < 0){ return errno; }


      	printf("El mensaje recuperado por el Desencriptador es: %s \n" , recieve);
        printf("\nPor favor ingrese otro mensaje:\n");

	    }

	    close(Id_Enc);
        close(Id_Des); 
        printf("Adios Mundo Cruel!!\n");

	return 0;
}
