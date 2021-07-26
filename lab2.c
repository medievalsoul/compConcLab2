#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"timer.h"

//variáveis globais do programa:
//dimensão da matriz, número de threads, matrizA, matrizB, matriz resultado
int dim;
int nthreads;
float *matrizA;
float *matrizB;
float *matrizC;

//função executada pelas threads
void *multiplica(void * arg){
  int *inicio = (int *) arg;
  for(int i=*inicio; i<dim ; i=i+nthreads){
    for(int j=0; j<dim ; j++){
      for(int k=0; k<dim ; k++){
        matrizC[(dim*i)+j] += matrizA[(dim*i)+k] * matrizB[(dim*k)+j];
      }
    }
  }
  pthread_exit(NULL);
}

//Função que realiza a multiplicação de forma sequencial
void multSequencial(float * matrizTeste){

  //inicialização dos valores da matriz
  for(int i=0 ; i<dim ; i++){
    for(int j=0 ; j<dim ; j++){
      matrizTeste[(dim*i)+j]=0;
    }
  }
  //Multiplicação de matrizes
  for(int i=0 ; i<dim ; i++){
    for(int j=0 ; j<dim ; j++){
      for(int k=0 ; k<dim ; k++){
        matrizTeste[(dim*i)+j] += matrizA[(dim*i)+k] * matrizB[(dim*k)+j];
      }
    }
  }
}

//Função que compara as operações concorrente e sequencial
int compara(float * matrizTeste){
  int resultado = 1;
  for(int i=0 ; i<dim ; i++){
    for(int j=0 ; j<dim ; j++){
      if(matrizC[(dim*i)+j] != matrizTeste[(dim*i)+j])
        resultado = 0;
    }
  }
  return resultado;
}


int main(int argc, char* argv[]){

  //variáveis para calcular tempo de execução
  double inicioEstruturas, fimEstruturas, deltaEstruturas;
  double inicioThreads, fimThreads, deltaThreads;
  double inicioSequencial, fimSequencial, deltaSequencial;
  double inicioFinal, fimFinal, deltaFinal;

  //leitura e avaliação dos parâmetros de entrada
  if(argc<3){
    printf("Digite %s <dimensão da matriz> <numero de threads>\n",argv[0]);
    return 1;
  }

  //atribuindo os valores de dimensão e número de threads
  dim = atoi(argv[1]);
  nthreads = atoi(argv[2]);

  GET_TIME(inicioEstruturas);
  //alocação de memória e inicialização das matrizes
  matrizA = malloc(sizeof(float)*dim*dim);
  if (matrizA == NULL) {printf("--ERRO: malloc()\n"); return 2;}
  matrizB = malloc(sizeof(float)*dim*dim);
  if (matrizB == NULL) {printf("--ERRO: malloc()\n"); return 2;}
  matrizC = malloc(sizeof(float)*dim*dim);
  if (matrizC == NULL) {printf("--ERRO: malloc()\n"); return 2;}
  for(int i=0 ; i<dim ; i++){
    for(int j=0 ; j<dim ; j++){
      matrizA[(dim*i)+j]=2*i;
      matrizB[(dim*i)+j]=3*j;
      matrizC[(dim*i)+j]=0;
    }
  }
  GET_TIME(fimEstruturas);

  GET_TIME(inicioThreads);
  //criação das threads
  pthread_t tid[nthreads];
  int thread[nthreads]; //variável que identifica a thread que será sendo executada
  for(int i=0 ; i<nthreads ; i++){
    thread[i] = i;
    if (pthread_create(&tid[i], NULL, multiplica, (void*) &thread[i])) {
      printf("--ERRO: pthread_create()\n");
      return 3;
    }
  }

  //Esperando todas as threads acabarem
  for (int i=0; i<nthreads; i++) {
    if (pthread_join(tid[i], NULL)) {
       printf("--ERRO: pthread_join() \n"); return 4;}
  }
  GET_TIME(fimThreads);

  GET_TIME(inicioSequencial);
  //Versão sequencial
  //alocação de memória para a matriz de resultado sequencial
  float *matrizTeste = malloc(sizeof(float)*dim*dim);
  if (matrizTeste == NULL) {printf("--ERRO: malloc()\n"); return 2;}

  multSequencial(matrizTeste);

  GET_TIME(fimSequencial);


  GET_TIME(inicioFinal);
  //Testa se a multiplicação concorrente foi feita corretamente
  if(compara(matrizTeste)) puts("multiplicação bem sucedida");
  else puts("erro na multiplicação");

  //liberar memória
  free(matrizA);
  free(matrizB);
  free(matrizC);

  GET_TIME(fimFinal);

  deltaEstruturas = fimEstruturas - inicioEstruturas;
  deltaThreads = fimThreads - inicioThreads;
  deltaSequencial = fimSequencial - inicioSequencial;
  deltaFinal = fimFinal - inicioFinal;
  printf("tempo1: %f\n", deltaEstruturas);
  printf("tempo2: %f\n", deltaThreads);
  printf("tempo3: %f\n", deltaSequencial);
  printf("tempo4: %f\n", deltaFinal);


  return 0;
}
