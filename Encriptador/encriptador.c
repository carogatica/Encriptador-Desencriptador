#include <linux/module.h>
#include <linux/kernel.h>		
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/printk.h>
#include "encriptador.h"

MODULE_LICENSE("GPL");//licencia

#define SUCCESS 0
#define DEVICE_NAME "encriptador"
#define BUF_LEN 80

/*
*Variable para prevenir acceso concurrente 
*al mismo dispositivo
*/
static int Device_Open=0;

/* 
*Mensaje que el dispositivo va a dar cuando sea consultado
*/
static char Message[BUF_LEN];

/*
*Util si el mensaje es mas largo que el 
*tamaño del buffer
*/
static char *Message_Ptr;

/*
*Llamado cuando un proceso intenta abrir el 
*archivo del dispositivo
*/
static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "Dispositivo abierto: device_open(%p)\n",file);
#endif

	if (Device_Open)
		return -EBUSY;

	Device_Open++;

	/*
	*Inicializamos el mensaje
	*/
	Message_Ptr=Message;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "Dispositivo descargado: device_release(%p,%p)\n", inode, file);
#endif

	/*
	*Disponible para el proximo dispositivo 
	*/
	Device_Open--;

	module_put(THIS_MODULE);
	return SUCCESS;
}

/*
*Se llama a esta funcion cuando un proceso que abrio
*el archivo del dispositvo intente leerlo
*/
static ssize_t device_read(struct file *file,char __user *buffer,size_t length,loff_t * offset)
{
	/*
	*Numero de bytes escritos en el buffer
	*/
	int bytes_read=0;
#ifdef DEBUG
	printk(KERN_INFO "Leyendo archivo del Dispositivo: device_read(%p,%p,&d)\n",file,buffer,length);
#endif

	/*
	* Si se encuentra al final del archivo, retorna 0
	*/
	if(*Message_Ptr==0)  
		return 0;

	/*
	*Mientras haya caracteres que leer y no se haya
	*llegado al final del archivo, se utiliza la funcion 
	*put_user para copiar datos desde el segmento del 
	*kernel al segmento de dato del usuario.
	*Como el buffer es en el segmento de usuario y no en el
	*del kernel, no se puede usar la asignacion directamente.
	*/
	while(length && *Message_Ptr){
		put_user(*(Message_Ptr++),buffer++);
		length--;
		bytes_read++;
	}

#ifdef DEBUG
	printk(KERN_INFO "Leyendo %d bytes, %d restantes\n", bytes_read,length);
#endif

	/*
	*La funcion retorna el numbero de bytes insetados en el
	*buffer
	*/
	return bytes_read;
}

/*
*Funcion usada cuando alguien intenta escribir en el 
*archivo del dispositivo
*/
static ssize_t device_write(struct file *file, const char __user *buffer,size_t length,loff_t * offset)
{
	int i;
#ifdef DEBUG
	printk(KERN_INFO "Escritura en archivo del dispositivo device_write(%p,%s,%d)",
			file, buffer, length);
#endif

	/*
	*Guardamos en Message cada caracter del buffer
	*/
	for(i=0; i<length && i<BUF_LEN; i++)
		get_user(Message[i],buffer+i);

	/*
	*Encriptamos el mensaje sumando 1 a cada caracter
	*/
	for(i=0; i<length && i<BUF_LEN; i++)
		Message[i]=Message[i]+1;

	Message_Ptr=Message;

	/*
	*Retornamos el numero de caracteres de entrada usados
	*/
	return i;
}

/*
*Funcion llamada cuando un proceso intenta hacer un ioctl en
*el archivo del dispositivo. 
*Si el ioctl es escrito o leido/escrito, la llamada a ioctl
*retorna la salida de esta funcion
*/
static long device_ioctl(struct file *file,
				 unsigned int ioctl_num, unsigned long ioctl_param)
{
	int i;
	char *temp;
	char ch;

	switch(ioctl_num){
	case IOCTL_SET_MSG:
		/*
		*Recibe un puntero al mensaje y lo setea
		*para ser el mensaje del dispositivo.
		*Toma el parametro dado a ioctl por el proceso
		*/
		temp=(char *) ioctl_param;
		get_user(ch,temp);
		for(i=0; ch && i<BUF_LEN; i++, temp++)
			get_user(ch,temp);
		device_write(file,(char *)ioctl_param,i,0);
		break;
	case IOCTL_GET_MSG:
		/*
		*Devuelve el mensaje actual al proceso que llama.
		*/
		i=device_read(file,(char*)ioctl_param,99,0);

		/*
		*Pune cero al final del buffer para que este correctamente
		*terminado
		*/
		put_user('\0',(char *)ioctl_param+i);
		break;

		case IOCTL_GET_NTH_BYTE:
        /*
        *Retorna el mensaje 
        */        
          	return Message[ioctl_param];
            break;
	}

	return SUCCESS;
}

/*Declaraciones del Modulo*/
/*
*La estructura contiene la funcion a llamar cuando
*un proceso hace algo en el dispositivo que creamos
*/
struct file_operations Fops={
	.read=device_read,
	.write=device_write,
	.unlocked_ioctl=device_ioctl,
	.open=device_open,
	.release=device_release
};

/*
*Inicializa el modulo.
*Registra el dispositivo de entrada de caracteres
*/
int init_module()
{
	int ret_val;
	
	//Intenta registrar el dispositivo
	ret_val=register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

	//Si ret_val es negativo, hubo un error
	if(ret_val<0){
		printk(KERN_ALERT "%s fallo con %d\n",
			   "No se pudo registrar el dispositivo de caracter ",
			   ret_val);
		return ret_val;
	}

	printk(KERN_INFO "%s El mayor numero de dispositivo es %d.\n",
		   "Registracion exitosa", MAJOR_NUM);
	printk(KERN_INFO "Si quiere hablar con el driver del dispositivo,\n");
	printk(KERN_INFO "tiene que crear un archivo de dispositivo.\n");
	printk(KERN_INFO "Le sugerimos que use:\n");
	printk(KERN_INFO "mknod %s c %d 0\n", DEVICE_FILE_NAME,MAJOR_NUM);
	printk(KERN_INFO "El nobre de archivo del dispositivo es importante, porque\n");
	printk(KERN_INFO "el programa ioctl asume que es el\n");
	printk(KERN_INFO "archivo que usted usara.\n");
	printk(KERN_INFO "Para poder escribir sobre el archivo se sugiere utilizar:\n");
	printk(KERN_INFO "chmod 666 /dev/%s\n", DEVICE_FILE_NAME);
	return 0;
}

/*
*Funcion para quitar apropiadamente el archivo del /proc
*/
void cleanup_module()
{
//	int ret;

	//ret=unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

	//if(ret<0)
	//	printk(KERN_ALERT "Error al quitar el dispositivo: unregister_chrdev: %d\n",ret);

	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

	printk(KERN_INFO "El modulo de dispositivo de caracter se \n");
	printk(KERN_INFO "ha quitado exitosamente del kernel.\n");
}


