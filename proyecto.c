//  Created by Alberto Penhos and Eduardo Rubinstein on 20/11/2014.
//  Copyright (c) 2014 Alberto Penhos and Eduardo Rubinstein. All rights reserved.
//

/*
 * Referencias:
 * http://www.cs.hofstra.edu/~cscccl/csc145/imul.c
 * http://www.arc.vt.edu/resources/software/openmp/docs/openmp_mmult.c
 * http://www.eecis.udel.edu/~cavazos/cisc879/Lecture-03.pdf
 * http://biogrid.engr.uconn.edu/REU/Reports_10/Final_Reports/Rifat.pdf
 * http://www.dcc.fc.up.pt/~fds/aulas/PPD/1112/mpi_openmp.pdf
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

/*Tamaño de la matriz para evitar la sobrecarga de memoria*/
#define MATRIX_SIZE_LIMIT 100

/*Valores dentro de la matriz menores al definido */
#define MAX_VALUE 100

/*Constantes para la distribucion de Poisson ... NO REDUCIR BETA YA QUE GENERA FALSOS POSITIVOS DE FALLA*/
#define alpha 30.
#define beta .25
#define gamma 2.5
#define detla 500.
#define c 0.95

/*Nodo de lista ligada para la cola*/
typedef struct _matrixNode{
  int m,n;
  int *M;
  struct _matrixNode *next;
} matrixNode;

/*inicializacion de la lista*/
matrixNode *matListStart = NULL;

/*variable para cuando el master quiere hacer restart*/
int RB;

/*El rank inicial por si llega a fallar un proceso ej si el proceso 2 falla 
el proceso 3 se convierte en el 2 en el nuevo grupo*/
int initialRank;

float Poisson_interval( float x ){//funcion para calcular el intervalo de Poisson
  
  float R = -(float)rand( ) / (float)RAND_MAX;
  float X = 1./x*log(1-R);
  return X;
}

void printMatrix( int * M, int d1, int d2){//funcion para imprimir las matrices
  int i;
  
  fprintf(stderr,"==================\n");//stderr es ocupado para imprimir en la computadora maestra
  for(i = 0; i < d1*d2; i++){
    if( i > 0 && i%d2 == 0 )
      fprintf(stderr,"\n");
    fprintf(stderr,"%d ", M[i]);
  }
  fprintf(stderr,"\n==================\n");
  
}

int * generate_random_matrix( int val1, int val2){
  
  int i, *tmp = (int *)malloc(val1*val2*sizeof(int));
  
  /*Inicializamos en un valor random */
  for( i = 0; i < val1*val2; i++)
    tmp[i] = rand( ) % MAX_VALUE;
  
  return tmp;
}

int * allocate_matrix( int val1, int val2){
  //usamos este para guardar los resultados the la matriz
  int *tmp = (int *)malloc(val1*val2 * sizeof(int));
  int i;
  /*Inicializamos en  0*/
  for(i = 0; i < val1*val2; i++)
    tmp[i] = 0;
  
  if( tmp == NULL){
    fprintf(stderr,"error de Malloc.\n");
    exit(1);
  }
  
  return tmp;
  
}

void master_process(int slaves){
    /*es la rutina del master*/
    int *A, *B, i, d[3];
    int msg[4];
  
    /* toma un pedazo de la funcion de tiempo aleatroio */
    srand(time(NULL));
  
    d[0] = (rand( ) ) % MATRIX_SIZE_LIMIT ; 
    d[1] = (rand( ) ) % MATRIX_SIZE_LIMIT ;
    d[2] = (rand( ) ) % MATRIX_SIZE_LIMIT ;
    
    A = generate_random_matrix( d[0],d[1] );//dimensiones de la primera matriz
    B = generate_random_matrix( d[1],d[2] );//dimensiones de B
    
    
    fprintf(stderr,"Matriz A:\n");
    printMatrix(A, d[0], d[1]);//impresion matriz A
    fprintf(stderr,"Matriz B:\n");
    printMatrix(B, d[1], d[2]);
    
    /* Abdul: C es para la matriz resultado, en vez de este allocate, mandar a memoria
     * compartida 
     */
    int *C = allocate_matrix( d[0], d[2] );
    
    
    /*Divide matriz A en [sizeof A]/#numero de esclavos y le da su porcion a cada uno*/
    int nelems = d[0]*d[2]/(size - 1);//cuantos elementos le toca a cada esclavo
    int i;

    
    for(i = 1; i < size; i++){
        /* Abdul: aquí para cada subprocesso, pedirle que multiplique una de
         * las row*column disponibles, los subprocesos solo hacen producto punto
         * de vectores
         */
        
        /* Abdul: Varias maneras de implementarlo, haciendo el fork aquí, lo más obvio,
         * pero se podría hacer un pool, que siempre consulte una cola de mensajes:
         * el master manda a la cola el row column y los subprocesos que iniciaron
         * quizá en el main solo esperan trabajo de ahí
         */
      
    }
    
    
    fprintf(stderr, "========\nCálculo de la matriz calculada %d, con dimensiones %d x %d.\n", completed, d[0], d[2] );
    fprintf(stderr,"Resultado:\n");
    printMatrix(C2, d[0], d[2] );
    free(A); free(B);
    free(C); free(C2);
  }
  
}

void slave_process(int row, int column){
    /* Abdul: Cada hijo haría un producto punto y sumaría en la posición final */

    /* Requiere una cola de mensajes para ir leyendo el trabajo que deja el master,
     * la memoria compartida donde van los resultados y un semáforo para bloquear el
     * acceso a las casillas de la memoria compartida, ejemplo:
     *
     * una matriz de 3x4 x 4x2
     *
     * 1 1 1 1  X 2  =  X X
     * X X X X  X 2     X X
     * X X X X  X 2     X X
     *          X 2
     *
     * El hijo recibe en la cola de mensajes por ejemplo el mensaje 1,2 
     * entonces debiera calcular el producto punto del row1 x col2 y depositarlo en
     * posición del resultado
     *
     * De hecho, se requiere el semáforo?
     */
}

int main( int argc, char * argv[] ){

    int n = 10;
  
    master_process(10);

    /* aquí posiblemente los fork con n esclavos */
    /* for(i=0; i<n; i++) if (!fork()) slave_process();
  
  
  
  return 0;
}
