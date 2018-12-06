#OBJECTS := encriptador.o ioctl.o

obj-m += encriptador.o 

#fifo-objs := $(OBJECTS)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean