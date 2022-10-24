#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void IniciaMatrices(char matriz[6][7][10]){
  for(int i = 0; i < 10; i++){
    for(int j = 0; j < 6; j++){
        for(int k = 0 ; k < 7; k++){
            matriz[j][k][i]='-';
        }
    }
  }
}

void IniciaMatriz(char matriz[6][7][10], int i){
    for(int j = 0; j < 6; j++){
        for(int k = 0; k < 7; k++){
            matriz[j][k][i]= '-';
        }
    }
}

char ImprimeMatriz(char matriz[6][7][10], int i, char *smatriz){

    bzero(smatriz, sizeof(smatriz));

    strcat(smatriz, "|1|2|3|4|5|6|7|\n");
    
    char var[2];
    for(int j = 0; j < 6; j++){
        strcat(smatriz,"|");
        for(int k = 0; k < 7; k++){
            if(matriz[j][k][i] == '-'){ strcat(smatriz, "-|"); }
            else if(matriz[j][k][i] == 'x'){ strcat(smatriz, "x|"); }
            else if(matriz[j][k][i] == 'o'){ strcat(smatriz, "o|"); }
        }
        strcat(smatriz, "\n");
    }
    
    return *smatriz;
}

int BuscarTablero(char matriz[6][7][10]){
    int con = 0;

    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 6; j++){
            for(int k = 0 ; k < 7; k++){
                if(matriz[j][k][i] == '-'){ con++;};
            }
        }
        if(con == 42){ return i; }
    }
}


//----------------------------------------------------------------------------------------------
// void CreaMatriz(int matriz){
//     matriz = (int ***)malloc(6*sizeof(int **));

//     if(matriz == NULL){
//         fprintf(stderr, "Out of memory");
//         exit(0);
//     }

//     for(int i = 0; i < 6; i++){
//         matriz[i] = (int **)malloc(7 * sizeof(int *));

//         if(matriz[i] == NULL){
//             fprintf(stderr, "Out of memory");
//             exit(0);
//         }

//         for(int j = 0; j < 7; j++){
//             matriz[i][j] = (int *)malloc(10 * sizeof(int));

//             if(matriz[i][j] == NULL){
//                 fprintf(stderr, "Out of memory");
//                 exit(0);
//             }
//         }
//     }

// }

// void InicializaMatrices(int matriz[][][]){
//     for (int i = 0; i < 10; i++){
//         for (int j = 0; j < 6; j++){
//             for (int k = 0; k < 7; k++){
//                 matriz[j][k][i] = rand() % 100;
//             }
//         }
//     }
// }

// void ImprimeMatrices(int *** matriz){
//     for (int i = 0; i < 10; i++){
//         for (int j = 0; j < 6; j++){
//             printf("(");
//             for (int k = 0; k < 7; k++){
//                 printf("%d, ", matriz[j][k][i]);
//             }
//             printf("\n");
//         }
//         printf("\n\n");
//     }
// }

// int main(){
   
//     //Declaramos la matriz
//     int *** matriz;

//     //Reservamos memoria para la matriz
//     CreaMatriz(matriz);

//     ImprimeMatrices(matriz);

// }
//----------------------------------------------------------------------------------------------


// // Online C compiler to run C program online
// #include <stdio.h>
// #include <string.h>

// int main() {
   
//    char matriz[2][2][2];
   
//    for(int i = 0; i<2; i++){
//        for(int j = 0; j < 2; j++){
//            for (int k = 0; k < 2; k++){
//                scanf("%c", &matriz[j][k][i]);
//                getchar();
//            }
//        }
//    }
   
//    for(int i = 0; i<2; i++){
//        for(int j = 0; j < 2; j++){
//            printf("( ");
//            for (int k = 0; k < 2; k++){
//                //strcpy(matriz[j][k][i], matriz[j][k][i], strlen("-"));
//                printf("%c, ", matriz[j][k][i]);
//            }
//            printf(")\n");
//        }
//        printf("\n\n");
//    }
// }