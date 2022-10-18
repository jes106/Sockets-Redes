#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

bool user = false;
bool password = false;
bool playing = false;


int main (){
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	char buffer[250];
	socklen_t len_sockname;
	fd_set readfds, auxfds;
	int salida;
	int fin = 0;
	
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1){
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}

   
    
	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2060);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1){
		perror ("Error de conexión");
		exit(1);
	}
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

	printf("Bienvenido al juego '4 en linea'.\n");
	printf("Una vez se haya conectado al servidor introduzca:\n");
	printf("\tPara registrarse en la aplicación por primera vez: \n");
	printf("\t\tREGISTRO -u suNombre -p suContraseña\n");
	printf("\tPara iniciar sesión: \n");
	printf("\t\tUSUARIO usuario\n");
		

	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do{


        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);
        
        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)){
            
            bzero(buffer,sizeof(buffer));
            recv(sd,buffer,sizeof(buffer),0);
            
            printf("\n%s\n",buffer);
            
            if(strcmp(buffer,"-ERR. Demasiados clientes conectados\n") == 0)
                fin =1;
            
            if(strcmp(buffer,"-EE. Desconectado por el servidor\n") == 0)
                fin =1;

			if(strcmp(buffer, "+Ok. Usuario correcto\n") == 0){
				user = true;
				printf("Introduzca ahora su contraseña asi:\n\tPASSWORD suContraseña\n");
			}

			if(strcmp(buffer, "+Ok. Usuario validado\n") == 0){
				password = true;
				clear();
				printf("Para empezar a jugar introduzca:\n");
				printf("\tINICIAR-PARTIDA\n");
				printf("Para salir introduzca:\n");
				printf("\tSALIR\n");
			}

			if(strcmp(buffer, "+Ok. Empieza la partida.", strlen("+Ok. Empieza la partida.")) == 0){
				playing = true;
				/********************************************************/
				/*						Completar						*/
				/********************************************************/
				if(myTurn == true){
					printf("+Ok. Turno de partida.\n");
				}
				else if(myTurn == false){
					printf("Ok. Turno del otro jugador.\n");
				}
			}
            
        }
        else{
            
            //He introducido información por teclado
           	if(FD_ISSET(0,&auxfds)){

            	bzero(buffer,sizeof(buffer));
                
            	fgets(buffer,sizeof(buffer),stdin);
                
        		if(strcmp(buffer,"SALIR\n") == 0){
                	fin = 1;
        		}
                
            	send(sd,buffer,sizeof(buffer),0);
                
            }
            
            
        }
        
        
				
    }while(fin == 0);
		
    close(sd);

    return 0;
		
}