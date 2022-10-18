#ifndef STRUCT
#define STRUCT

struct cliente{
    int socket;
    int estado; //Leyenda de estados => 0: acaba de iniciar
                    //                    1: esperando para inicio de sesión
                    //                    2: esperando para conexión
                    //                    3: conectado con su adversaio
    char username [255];
};

#endif