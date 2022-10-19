#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include "struct4.h"
#include "AuxiliaryFunctions.c"
#include "Tablero.c"


#define MSG_SIZE 200
#define MAX_CLIENTS 30

// Matriz de 6 filas, 7 colimnas y 10 tableros.
char matriz[6][7][10];


/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, struct cliente clients[]);
void comprobarEstado(char cadena[MSG_SIZE], struct cliente *clienteComp);



int main ( ){
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
    int salida;
    struct cliente clients[MAX_CLIENTS];
    int numClientes = 0;
    int numPartidas = 0;
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
                
                // Buscamos el socket por el que se ha establecido la comunicación
                for(i=0; i<FD_SETSIZE; i++){
                    
                    // Si encontramos el socket
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
                                
                                    strcpy(buffer, "+Ok. Bienvenido al sistema.\n");
                                
                                    send(new_sd,buffer,sizeof(buffer),0);
                                }
                                else{
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"-Err. Demasiados clientes conectados\n");
                                    send(new_sd,buffer,sizeof(buffer),0);
                                    close(new_sd);
                                }
                                
                            }
                            
                            
                        }
                        
                        // Si se ha introducido informacion por teclado
                        else if (i == 0){

                            bzero(buffer, sizeof(buffer));
                            fgets(buffer, sizeof(buffer),stdin);
                            
                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                            if(strcmp(buffer,"SALIR\n") == 0){
                             
                                for (j = 0; j < numClientes; j++){
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer,"-Err. Desconectado por el servidor\n"); 
                                    send(clients[j].socket, buffer, sizeof(buffer),0);
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
<<<<<<< HEAD
                                
                                if(strcmp(buffer,"SALIR\n") == 0){
                                    salirCliente(i,&readfds,&numClientes, clients);
=======
                                for(int j=0;j<numClientes;j++){
                                    if(clients[j].socket==i){
                                        comprobarEstado(buffer, &clients[j]);
                                    }
                                }
                                //A función comprobar estado
                                /*if(strcmp(buffer,"SALIR\n") == 0){
                                    salirCliente(i,&readfds,&numClientes,clients);  
>>>>>>> 5cfdbfbe865b99885de72b42e331201e6721b57c
                                }
                                
                                else if(strncmp(buffer,"USUARIO\n", strlen("USUARIO\n")) == 0){
                                    strcpy(buffer, "-Err. Debe introducir el nombre de usuario\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }

                                else if(strncmp(buffer,"USUARIO ", strlen("USUARIO ")) == 0){

                                    if(strncmp(buffer, "USUARIO \n", strlen("USUARIO \n")) == 0){
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
                                            clients[i].estado = 1;
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                        else{   //UserCheck(usu) == false
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Usuario incorecto\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                    }
                                }
                                
                                else if(strncmp(buffer,"PASSWORD\n", strlen("PASSWORD\n")) == 0){
                                    strcpy(buffer, "-Err. Debe introducir el password\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }

                                else if(strncmp(buffer, "PASSWORD ", strlen("PASSWORD "))== 0){

                                    if(strncmp(buffer, "PASSWORD \n", strlen("PASSWORD \n"))== 0){
                                        strcpy(buffer, "-Err. Error en la validacion\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else{
                                        char *pass;
                                        pass = strtok(buffer, " ");
                                        pass = strtok(NULL, "\n");   //Hasta aqui hemos extraido la constraseña de la cadena

                                        printf("Password -> [%s]\n", pass);

                                        //Comprobamos que el nombre existe en la base de datos
                                        if(PasswordCheck(pass) == true){
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Usuario validado\n");
                                            clients[i].estado = 1;
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                        else{   //PasswordCheck(pass) == false
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Error en la validacion\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                    }
                                }

                                else if(strncmp(buffer, "INICIAR-PARTIDA ", strlen("INICIAR-PARTIDA")) == 0){
                                    
                                    //Primero comprobamos que hay tableros vacios
                                    if(numPartidas < 10){
                                        int tablero = BuscarTablero();
                                    }
                                }
                                // else{
                                    
                                //     sprintf(identificador,"<%d>: %s",i,buffer);
                                //     bzero(buffer,sizeof(buffer));

                                //     strcpy(buffer,identificador);

                                //     printf("%s\n", buffer);

                                //     for(j=0; j<numClientes; j++)
                                //         if(clients[j].socket != i)
                                //             send(clients[j].socket,buffer,sizeof(buffer),0);

                                    
                                // }
                                                                
                                
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos== 0)
                            {
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes, clients);
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

void comprobarEstado(char cadena[MSG_SIZE], struct cliente *clienteComp){
    printf("Cadena %s", cadena);
    char *comando=strtok(cadena, " ");

    if(strcmp("USUARIO", comando)==0){
        printf("%i", clienteComp->estado);
        if(clienteComp->estado==0){
            comando=strtok(NULL,"\n");
            printf("%s", comando);
        }
    }
    


}