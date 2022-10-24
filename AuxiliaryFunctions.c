#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>




bool UserCheck(char * user){

    FILE *f;
    if( (f = fopen("Users.txt", "r")) == NULL ){
        printf("Error al abrir el fichero Users.txt\n");
    }

    bool var = false;
    char cadena[100];
    char *aux;

    while(fgets(cadena, 100, f) != NULL){
        aux = strtok(cadena, ",");
        if(strcmp(aux, user) == 0){
            var = true;
        }
    }

    fclose(f);
    return var;
}

bool PasswordCheck(char * pass, char user[225]){

    FILE *f;
    if( (f = fopen("Users.txt", "r")) == NULL ){
        printf("Error al abrir el fichero Users.txt\n");
    }

    bool var = false;
    char cadena[100];
    char usu[255];
    char *aux;

    while(fgets(cadena, 100, f) != NULL){

        aux = strtok(cadena, ",");
        strcpy(usu, aux);
        aux = strtok(NULL, "\n");
        aux[strlen(aux)-1] = '\0';

        if(strcmp(aux, pass) == 0 && strcmp(usu, user) == 0){
            var = true;
        }
    }

    fclose(f);
    return var;
}

bool CheckStruct(struct cliente aux, struct cliente aux1){
    if(aux.socket != aux1.socket){ return false; }
    return true;
}
/*
int main(){
    char buffer[] = "i02dipea,123\n";
    
    char * aux;
    
    aux = strtok(buffer, ",");
    //aux = strtok(NULL, "\n");
    
    printf("%s", aux);

    return 0;
}*/