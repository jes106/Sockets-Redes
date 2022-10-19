#ifndef STRUCT
#define STRUCT

struct cliente{
    int socket;
    int estado; 
        //Leyenda de estados => 0: Entra en el sistema
        //                      1: Ha introducido su usuario correctamente
        //                      2: Ha introducido su password correctamente
        //                      3: Se encuentra a la espera de jugar
        //                      4: Dentro de una partida
        //                      5: 
    char username [255];
    int socket_cont;
    
};

#endif