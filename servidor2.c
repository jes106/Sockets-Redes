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
#include <stdbool.h>
#include <arpa/inet.h>

#include "struct4.h"
#include "AuxiliaryFunctions.c"
#include "Tablero.c"


#define MSG_SIZE 250
#define MAX_CLIENTS 30

bool sobreescribe=false;
int numClientes = 0;

// Matriz de 6 filas, 7 colimnas y 10 tableros.
char matriz[6][7][10];
struct cliente clients[MAX_CLIENTS];


/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes);
void comprobarEstado(char cadena[MSG_SIZE], struct cliente *clienteComp);



int main ( ){
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
    char sendmatriz[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
    int salida;
    int numPartidas = 0;
    //contadores
    int i,j,k;
	int recibidos;
    char identificador[MSG_SIZE];

    int on, ret;

    /* --------------------------------------------------
        Inicializamos las matrices
    ---------------------------------------------------*/
    IniciaMatrices(matriz);
    for(int x = 0; x < MAX_CLIENTS; x++){
        clients[x].socket = -1;
        clients[x].estado = -1;
        strcpy(clients[x].username, " ");
        clients[x].socket_cont = -1;
        clients[x].tablero = -1;
        clients[x].turno = false;
    }


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
    //signal(SIGINT,manejador);
    
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
                            if(strcmp(buffer,"IMPRIMIR\n") == 0){
                             
                                for (j = 0; j < MAX_CLIENTS; j++){
                                    if(clients[j].estado == 2 || clients[j].estado == 3 || clients[j].estado == 4){
                                        printf("Cliente %d\n", j);
                                        printf("\tSocket -> %d\n", clients[j].socket);
                                        printf("\tEstado -> %d\n", clients[j].estado);
                                        printf("\tUser -> %s\n", clients[j].username);
                                        printf("\tSocket_con -> %d\n", clients[j].socket_cont);
                                        printf("\tTablero -> %d\n\n", clients[j].tablero);
                                    }
                                }
                             
                            }
                        } 
                        

                        else{
                            bzero(buffer,sizeof(buffer));
                            
                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            
                            if(recibidos > 0){
                                
                                if(strcmp(buffer,"SALIR\n") == 0){
                                    
                                    for(int x = 0; x < numClientes; x++){
                                        if(clients[x].socket == i && clients[x].socket_cont != -1){
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Err. Su contrincante ha abandonado la partida.\n");
                                            send(clients[x].socket_cont, buffer, sizeof(buffer), 0);
                                            for(int z = 0; z < numClientes; z++){
                                                if(clients[z].socket == clients[x].socket_cont){
                                                    clients[z].estado = 3;
                                                    clients[z].socket_cont = -1;
                                                    clients[z].tablero = -1;
                                                    clients[z].turno = false;
                                                    strcpy(buffer,"+Inf. Su contricante ha salido de la partida");
                                                    send(clients[x].socket_cont, buffer,sizeof(buffer),0);

                                                }
                                                
                                            }
                                        }
                                    }
                                    salirCliente(i,&readfds,&numClientes);
                                }
                                
                                else if(strncmp(buffer,"USUARIO\n", strlen("USUARIO\n")) == 0){
                                    strcpy(buffer, "-Err. Debe introducir el nombre de usuario\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }

                                else if(strncmp(buffer,"USUARIO ", strlen("USUARIO ")) == 0){
                                    int cliente = -1;
                                    for(int x = 0; x < numClientes; x++){
                                        if(clients[x].socket == i){ cliente = x; }
                                    }
                                    if(strncmp(buffer, "USUARIO \n", strlen("USUARIO \n")) == 0){
                                        strcpy(buffer, "-Err. Usuario incorecto\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else if(clients[cliente].estado == 0){
                                        
                                        char *usu;
                                        usu = strtok(buffer, " ");
                                        usu = strtok(NULL, "\n");   //Hasta aqui hemos extraido el nombre de usuario de la cadena

                                        //Comprobamos que el nombre existe en la base de datos
                                        if(UserCheck(usu) == true){
                                            clients[cliente].estado = 1;
                                            strcpy(clients[cliente].username,usu);
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

                                    else if(clients[i].estado == 1){
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Ya ha introducido su usuario.\n");
                                        send(i, buffer, sizeof(buffer), 0);  
                                    }

                                    else if(clients[i].estado == 2){
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Ya ha introducido su contraseña.\n");
                                        send(i, buffer, sizeof(buffer), 0);  
                                    }
                                    
                                    else if(clients[i].estado == 3){
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Su usuario ya ha sido validado.\n");
                                        send(i, buffer, sizeof(buffer), 0);  
                                    }

                                    else if(clients[i].estado == 4){
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Se encuentra jugando una partida.\n");
                                        send(i, buffer, sizeof(buffer), 0);  
                                    }
                                }
                                
                                else if(strncmp(buffer,"PASSWORD\n", strlen("PASSWORD\n")) == 0){
                                    strcpy(buffer, "-Err. Debe introducir el password\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }

                                else if(strncmp(buffer,"PASSWORD ", strlen("PASSWORD ")) == 0){
                                    int cliente = -1;
                                    for(int x = 0; x < numClientes; x++){
                                        if(clients[x].socket == i){ cliente = x; }
                                    }

                                    if(strncmp(buffer, "PASSWORD \n", strlen("PASSWORD \n"))== 0){
                                        strcpy(buffer, "-Err. Error en la validacion\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else{
                                        if(clients[cliente].estado == 0){
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Debe introducir primero su usuario.\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }

                                        else if(clients[cliente].estado == 1){
                                            char *pass;
                                            pass = strtok(buffer, " ");
                                            pass = strtok(NULL, "\n");   //Hasta aqui hemos extraido la constraseña de la cadena

                                            //Comprobamos que el nombre existe en la base de datos
                                            if(PasswordCheck(pass, clients[cliente].username) == true){
                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "+Ok. Usuario validado\n");
                                                clients[cliente].estado = 2;
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                            else{   //PasswordCheck(pass) == false
                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "-Err. Error en la validacion\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }  
                                        }

                                        else if(clients[cliente].estado == 2){
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Ya ha introducido su contraseña.\n");
                                            send(i, buffer, sizeof(buffer), 0);  
                                        }
                                        
                                        else if(clients[cliente].estado == 3){
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Su usuario ya ha sido validado.\n");
                                            send(i, buffer, sizeof(buffer), 0);  
                                        }

                                        else if(clients[cliente].estado == 4){
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Se encuentra jugando una partida.\n");
                                            send(i, buffer, sizeof(buffer), 0);  
                                        }
                                    }
                                }

                                else if(strncmp(buffer, "REGISTRO ", strlen("REGISTRO"))==0){
                                    char *comando=strtok(buffer, " ");
                                    comando=strtok(NULL, " ");
                                    char* user;
                                    if(comando!=NULL){
                                    if(strncmp(comando, "-u ", strlen("-u"))==0){
                                        printf("Llega hasta la comprobación de usuario\n");
                                        comando=strtok(NULL," ");
                                        user=comando;
                                    }
                                    else{
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"+Err. No se ha introducido el nombre de usuario");
                                        send(new_sd, buffer,sizeof(buffer),0);
                                        break;
                                    }
                                    }

                                    else{
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer, "+Err. Introduzca el nombre de usuario");
                                        send(new_sd, buffer, sizeof(buffer),0);
                                        break;
                                    }

                                    comando=strtok(NULL, " ");
                                    char* pass;
                                    if(comando!=NULL){
                                    if(strncmp(comando, "-p ", strlen("-p"))==0){
                                        comando=strtok(NULL, "\n");
                                        pass=comando;
                                    }
                                    }
                                    else{
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer, "+Err. Introduzca la contraseña del usuario");
                                        send(new_sd, buffer, sizeof(buffer),0);
                                        break;
                                    }
                                    printf("Pasa a la función\n");
                                    creaUsuario(user,pass);
                                }

                                else if(strncmp(buffer, "INICIAR-PARTIDA ", strlen("INICIAR-PARTIDA")) == 0){

                                    int cliente = -1;
                                    for(int x = 0; x < numClientes; x++){
                                        if(clients[x].socket == i){ cliente = x; }
                                    }

                                    // Cambiamos el estado del cliente
                                    clients[cliente].estado = 3;
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Esperando jugadores.\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                    
                                    //Primero comprobamos que hay tableros vacios
                                    if(numPartidas < 10){
                                        int tablero = BuscarTablero(matriz);

                                        //Buscamos que ocupante esta libre para jugar
                                        for(int x = 0; x < MAX_CLIENTS; x++){
                                            if(clients[x].estado == 3 && i != clients[x].socket){
                                                clients[cliente].socket_cont = clients[x].socket;
                                                clients[x].socket_cont = clients[cliente].socket;
                                                clients[x].jugador=1;
                                                clients[cliente].tablero = tablero;
                                                clients[x].tablero = clients[cliente].tablero;
                                                clients[cliente].estado = 4;
                                                clients[x].estado = 4;
                                                clients[cliente].jugador=2;

                                                clients[cliente].turno = false;
                                                clients[x].turno = true;

                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "+Ok. Empieza la partida.\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                                fflush(stdin);
                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "+Ok. Empieza la partida.\n");
                                                send(clients[x].socket, buffer, sizeof(buffer), 0);


                                                bzero(buffer, sizeof(buffer));
                                                char smatriz[255];
                                                ImprimeMatriz(matriz, i, smatriz);
                                                //strcpy(buffer, smatriz);
                                                send(i, smatriz, sizeof(smatriz), 0);
                                                bzero(buffer, sizeof(buffer));
                                                ImprimeMatriz(matriz, i, smatriz);
                                                //strcpy(matriz, smatriz);
                                                send(clients[x].socket, smatriz, sizeof(smatriz), 0);

                                                // Ahora enviamos a cada jugador de quien es el turno
                                                fflush(stdin);
                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "+Inf. Es turno del contrincante.\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                                fflush(stdin);
                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "+Inf. Es su turno.\n");
                                                send(clients[x].socket, buffer, sizeof(buffer), 0);
                                            }
                                        }
                                    }
                                    else{
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Maximo numero de partidas en juego.\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }


                                }
                                
                                else if(strncmp(buffer, "COLOCAR-FICHA ", strlen("COLOCAR-FICHA "))==0){
                                    int cliente = -1;
                                    for(int x = 0; x < numClientes; x++){
                                        if(clients[x].socket == i){ cliente = x; }
                                    }

                                    char* aux=strtok(buffer, " ");
                                    char* aux2=strtok(NULL, "\n");
                                    

                                    int columna=(int) strtol(aux2, NULL, 10);
                                    columna--;
                                    

                                    if(clients[cliente].turno == true){
                                        for(int j = 6; j >= 0; j--){
                                            if(matriz[j][columna][clients[cliente].tablero] == '-'){
                                              if(clients[cliente].jugador == 1){
                                                matriz[j][columna][clients[cliente].tablero]= 'o';
                                                break;
                                              }  
                                              if(clients[cliente].jugador == 2){
                                                matriz[j][columna][clients[cliente].tablero]= 'x';
                                                break;
                                              }
                                            }

                                            else{
                                                if(j==0){
                                                    strcpy(buffer, "+Inf. La columna está llena, elija otra");
                                                    send(clients[cliente].socket, buffer, sizeof(buffer),0);
                                                    sobreescribe=true;
                                                    break;
                                                }
                                            }
                                        }
                                        for(int k=0;k<numClientes;k++){
                                            if(clients[k].socket==clients[cliente].socket_cont && sobreescribe==false){
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"+Inf. Ahora es su turno");
                                                send(clients[k].socket,buffer,sizeof(buffer),0);
                                                clients[k].turno=true;
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"+Inf. Ahora es el turno de su contrincante");
                                                send(clients[cliente].socket,buffer,sizeof(buffer),0);
                                                clients[cliente].turno=false;
                                            }
                                            else if(clients[k].socket==clients[cliente].socket_cont && sobreescribe==true){
                                                sobreescribe=false;
                                            }
                                        }
                                        ImprimeMatriz(matriz, clients[cliente].tablero, sendmatriz);
                                        send(clients[cliente].socket, sendmatriz, sizeof(sendmatriz),0);
                                        send(clients[cliente].socket_cont, sendmatriz, sizeof(sendmatriz),0);
                                        
                                    }
                                    else{
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Debe esperar su turno.\n");
                                        send(i, buffer, sizeof(buffer), 0);
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
                                salirCliente(i,&readfds,&numClientes);
                            }
                        }
                    }
                }
            }
		}

	close(sd);
	return 0;
	
}

void salirCliente(int socket, fd_set * readfds, int * numClientes){

    char buffer[MSG_SIZE];
    int i = 0;
    
    close(socket);
    FD_CLR(socket,readfds);

    struct cliente aux_;
    aux_.socket = -1;
    aux_.estado = -1;
    strcpy(aux_.username, " ");
    aux_.socket_cont = -1;
    aux_.tablero = -1;
    aux_.turno = false;
    
    //Re-estructurar el array de clientes
    for (int j = 0; j < MAX_CLIENTS; j++){
        if(clients[j].socket == socket){
            clients[j] = aux_;
        }
        if (CheckStruct(clients[j], aux_) == true){
            for(int x = j; x < MAX_CLIENTS; x++){
                if(CheckStruct(clients[x], aux_) == false){
                    clients[j] = clients[x];
                    clients[x] = aux_;
                    x = MAX_CLIENTS - 1;
                }
            }
        }
    }
    
    (*numClientes)--;
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente: %d\n",socket);
    
    for(int j=0; j<(*numClientes); j++)
        if(clients[j].socket != socket)
            send(clients[j].socket,buffer,sizeof(buffer),0);


}


void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
    char buffer[MSG_SIZE];
    strcpy(buffer,"El servidor se va a cerrar, hasta luego");
    for(int i=0;i<numClientes;i++){
        send(clients[i].socket,buffer,sizeof(buffer),0);
        close(clients[i].socket);
    }
    exit(0);
}