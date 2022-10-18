#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>

#include "struct4.h"


#define MSG_SIZE 200
#define MAX_CLIENTS 30

void comprobarEstado(int i, char cadena[MSG_SIZE]);

struct cliente clients[MAX_CLIENTS];
char buffer[MSG_SIZE];
int numClientes=0;

int main ( )
{

    int st, on,ret, salida, new_st, recibidos;
    struct sockaddr_in server;
    fd_set readfds, auxfds;
    

        //Open the socket
        st=socket(AF_INET, SOCK_STREAM,0);
        if(st==-1){
            perror("No se puede abrir el socket");
        }
        
  

    	// Activaremos una propiedad del socket para permitir· que otros
    	// sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    	// Esto permite· en protocolos como el TCP, poder ejecutar un
    	// mismo programa varias veces seguidas y enlazarlo siempre al
   	    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    	// quedase disponible (TIME_WAIT en el caso de TCP)

        //Va entre antes del bind
    	on=1;
    	ret = setsockopt( st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        server.sin_family=AF_INET;
        server.sin_port=htons(2060);
        server.sin_addr.s_addr=INADDR_ANY;
        
        //Link the port
        if (bind(st,(struct sockaddr *)&server,sizeof(server))){
            perror("Error al asociar el puerto");
            exit(-1);
        }

        //Listen operation
        if(listen(st, 30)){
            perror("No se pudo hacer el listen");
            exit(-1);
        }
        	
        //Inicializar los conjuntos fd_set
    	FD_ZERO(&readfds);
    	FD_ZERO(&auxfds);
    	FD_SET(st,&readfds);
    	FD_SET(0,&readfds);

        while(1){

            auxfds = readfds;
            
            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
            //One socket receives a signal
            if(salida > 0){
                //Looking for the socket that established the communication
                for(int i=0;i<FD_SETSIZE;i++){
                    //If we found the socket that 
                    if(FD_ISSET(i, &auxfds)){
                        socklen_t tamanio=sizeof(server);
                        if(i==st){
                            //If the connection is new
                            if((new_st=accept(st, (struct sockaddr *)&server, &tamanio))==-1){
                                perror("Error al aceptar peticiones");
                            }
                            else{
                                if(numClientes<MAX_CLIENTS){
                                    clients[numClientes].socket= new_st;
                                    clients[numClientes].estado=0;
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer, "Identifiquese con USUARIO + username registrado: ");
                                    send(new_st, buffer, sizeof(buffer),0);

                                }

                                else{
                                    strcpy(buffer,"Demasiados clientes conectados\n");
                                    send(new_st,buffer,sizeof(buffer),0);
                                    close(new_st);
                                }

                            }
                        }
    
                        //If the information is received in the buffer of the server
                        else if(i==0){
                            fgets(buffer, sizeof(buffer),stdin);

                            if(strcmp(buffer, "SALIR\n")==0){
                                for(int j=0; i<numClientes;i++){
                                    strcpy(buffer, "Desconexión servidor\n");
                                    send(clients[j].socket, buffer, sizeof(buffer),0);
                                    close(clients[j].socket);
                                    FD_CLR(clients[j].socket,&readfds);

                                close(st);
                                exit(0);
                                }
                            }

                            else{
                                printf("Aquí está tu buffer: %s", buffer);
                            }

                        }

                        else{
                            recibidos=recv(i, buffer, sizeof(buffer),0);
                            printf("Identificador %i", i);
                            printf("%s", buffer);
                        }


                    }
                        
                    }
                    
        }

        }

  close(st);
  return 0;
 
}
  

void comprobarEstado (int i, char cadena[MSG_SIZE]){
    char delimitador[]=" ";
    char *token;
    for(int i=0;i<numClientes;i++){
        if(clients[i].socket==i){
            if(clients[i].estado==0){
                clients[i].estado=1;
                token=strtok(cadena,delimitador);
                printf("Este es el token %s", token);

            }
        }
    }
    printf("Aquí tamos");
}