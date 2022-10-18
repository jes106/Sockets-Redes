#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include<signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include "struct4.h"

#include "AuxiliaryFunctions.c"


#define MSG_SIZE 200
#define MAX_CLIENTS 30


/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, struct cliente clients[]);



int main ( ){
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
    struct cliente clients[MAX_CLIENTS];
    int salida;
    int numClientes = 0;
    //contadores
    int i,j,k;
	int recibidos;
    char identificador[MSG_SIZE];

    int on, ret;

    
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1){
		perror("No se puede abrir el socket cliente\n");
        exit (1);	
	}
    
    // Activaremos una propiedad del socket para permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permite· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on=1;
    ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2060);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1){
		perror("Error en la operación bind");
		exit(1);
	}
	

   	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,30) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}
    
	//Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);


    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);
    
	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
		while(1){
            
            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
            
            auxfds = readfds;
            
            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
            
            if(salida > 0){
                
                //Buscando el socket que estableció la comunicación
                for(i=0; i<FD_SETSIZE; i++){
                    
                    // Si encontramos el Socket
                    if(FD_ISSET(i, &auxfds)) {
                        
                        // El usuario es nuevo
                        if( i == sd){
                            
                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("Error aceptando peticiones");
                            }
                            else{

                                if(numClientes < MAX_CLIENTS){
                                    clients[numClientes].socket = new_sd;
                                    clients[numClientes].estado = 0;
                                    numClientes++;

                                    FD_SET(new_sd,&readfds);

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Usuario conectado.\n");
                                
                                    send(new_sd,buffer,sizeof(buffer),0);
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"-Err. Demasiados clientes conectados\n");
                                    send(new_sd,buffer,sizeof(buffer),0);
                                    close(new_sd);
                                }
                                
                            }
                            
                            
                        }
                        
                        
                        // Si se ha introducido información de teclado
                        else if (i == 0){

                            bzero(buffer, sizeof(buffer));
                            fgets(buffer, sizeof(buffer),stdin);
                            
                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                            if(strcmp(buffer,"SALIR\n") == 0){
                             
                                for (j = 0; j < numClientes; j++){
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer,"Desconexión servidor\n"); 
                                    send(clients[j].socket, buffer, sizeof(buffer), 0);
                                    close(clients[j].socket);
                                    FD_CLR(clients[j].socket,&readfds);
                                }

                                close(sd);
                                exit(-1);                              
                            }
                            //Mensajes que se quieran mandar a los clientes (implementar)
                            
                        } 
                        
                        
                        else{

                            bzero(buffer,sizeof(buffer));
                            
                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            
                            if(recibidos > 0){
                                
                                if(strcmp(buffer,"SALIR\n") == 0){
                                    salirCliente(i,&readfds,&numClientes,clients);  
                                }
                                
                                else if(strcmp(buffer, "USUARIO ", strlen("USUARIO "))== 0){

                                    if(strcmp(buffer, "USUARIO \n", strlen("USUARIO \n"))== 0){
                                        strcpy(buffer, "-Err. Usuario incorecto\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else{
                                        char *usu;
                                        usu = strtok(buffer, " ");
                                        usu = strtok(NULL, "\n");   //Hasta aqui hemos extraido el nombre de usuario de la cadena

                                        //Comprobamos que el nombre existe en la base de datos
                                        if(UserCheck(usu) == true){
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Usuario correcto\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                        else{   //UserCheck(usu) == false
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Usuario incorecto\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                    }


                                }                            
                                
                                else if(strcmp(buffer, "PASSWORD ", strlen("PASSWORD "))== 0){

                                    if(strcmp(buffer, "PASSWORD \n", strlen("PASSWORD \n"))== 0){
                                        strcpy(buffer, "-Err. Error en la validacion\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else{
                                        char *pass;
                                        pass = strtok(buffer, " ");
                                        pass = strtok(NULL, "\n");   //Hasta aqui hemos extraido la constraseña de la cadena

                                        //Comprobamos que el nombre existe en la base de datos
                                        if(PasswordCheck(pass) == true){
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Usuario validado\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                        else{   //PasswordCheck(pass) == false
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Error en la validacion\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                    }


                                }

                                else{
                                    
                                    sprintf(identificador,"<%d>: %s",i,buffer);

                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,identificador);

                                    printf("%s\n", buffer);

                                    for(j=0; j<numClientes; j++)
                                        if(clients[j].socket != i)
                                            send(clients[j].socket,buffer,sizeof(buffer),0);

                                    
                                }
                                                                
                                
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos== 0){
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,clients);
                            }
                        }
                    }
                }
            }
		}

	close(sd);
	return 0;
	
}

void salirCliente(int socket, fd_set * readfds, int * numClientes, struct cliente clients[]){
  
    char buffer[250];
    int j;
    
    close(socket);
    FD_CLR(socket,readfds);
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (clients[j].socket == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (clients[j].socket = clients[j+1].socket);
    
    (*numClientes)--;
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente: %d\n",socket);
    
    for(j=0; j<(*numClientes); j++)
        if(clients[j].socket != socket)
            send(clients[j].socket,buffer,sizeof(buffer),0);


}


void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}
