#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>


#define MSG_SIZE 200
#define MAX_CLIENTS 30


//Struct to store the client information
struct Cliente{
    int socket;
    int estado;
    char nombre[250];

};



int main ( )
{

    int st, on,ret, salida;
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
            
            if(salida > 0){
                //Looking for the socket that established the communication
                for(int i=0;i<FD_SETSIZE;i++){
                    //If we found the socket that 
                    if(FD_ISSET(i, &auxfds)){
                        
                    }
                }
            }
        }
    close(st);
}
