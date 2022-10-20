void InicializarTableros(char **** matriz){
    for(int i = 0; i<2; i++){
        for(int j = 0; j < 2; j++){
            for (int k = 0; k < 2; k++){
                //matriz[j][k][i] = '-';
            }
        }
    }
}

void InicializarTablero(char *** matriz, int i){
    for(int j = 0; j < 2; j++){
        for (int k = 0; k < 2; k++){
            //matriz[j][k][i] = '-';
        }
    }
}

int BuscarTablero(){

}

// int main(){
//     char matriz[6][7][10];
//     char paran***;
    
//     InicializarTableros(&matriz);
// }



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