# Encriptador-Desencriptador

﻿# Trabajo Practico Numero 3 - Sistemas Operativos I  

# Grupo:
- Gatica Rodriguez, Carolina
- Miglierini, Carlos

### Guia de Instalación y Uso.


* Dentro de la carpeta Encriptador ejecutar:
    ~$ make

* Situarse como super usuario dentro de la carpeta Encriptador y ejecutar: 
    ~# insmod desencriptor.ko 
    ~# mknod -m 0666 /dev/encriptador c 100 0

* Dentro de la carpeta Desencriptador ejecutar:
    ~$ make

* Situarse como super usuario dentro de la carpeta Encriptador y ejecutar: 
    ~# insmod desencriptor.ko 
    ~# mknod -m 0666 /dev/desencriptador c 101 0

* Para probar los modulos, dentro de la carpeta principal Encripador-Desencriptador ejecutar
    ~$ make
    ~$ ./main

* Al salir del programa, desmontar los modulos de la siguiente manera (como superusuario):
    ~# rmmod encriptador
    ~# rmmod desencriptador


