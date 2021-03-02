#include <stdio.h>
#include <stdlib.h>
#define MAX_IO 3
#define MAX_BURST 4
#define QUANTUM 3

	typedef struct {

		int PID;
		int t_chegada;
		int burstTime[MAX_BURST];
		int waitTime[MAX_IO];
		int num_burst;
		int num_wait;
		int tamanho_burst;
		int tamanho_wait;

	}Processo;

////////////////////////////////////////////////////////////////////////////////////////

	typedef struct{

		int tamanho;
		int primeiro;
		int ultimo;
		int capacidade;
		Processo *processos;

	}Fila;

//////////////////////////////////////////////////////////////////////////////////////////

	void criarFila(Fila *f, int c) {

		f->capacidade = c; //fila com capacidade c
		f->processos = (Processo*) malloc (f->capacidade * sizeof(Processo)); //aloca memoria para processos
		f->primeiro = 0;
		f->ultimo = -1;
		f->tamanho = 0; //inicializa fila com tamanho 0 pois esta vazia

	}

////////////////////////////////////////////////////////////////////////////////////////

	void inserir(Fila *f, Processo *processo) {

		f->tamanho++; //aumenta o tamanho da  fila pois vai ser inserido um elemento
		f->ultimo=(f->ultimo+1)%f->capacidade;
		f->processos[f->ultimo]=*processo; //o processo é inserido na ultima posiçao da fila

	}

////////////////////////////////////////////////////////////////////////////////////////

	void remover(Fila *f) { // pega o item do comeÃ§o da fila

		f->tamanho--; //decrementa um num tmanho pois é retirado um elemneto
		f->primeiro=(f->primeiro+1)%f->capacidade;

	}

////////////////////////////////////////////////////////////////////////////////////////
/*
	int estaVazia(Fila *f){ // retorna verdadeiro se a fila estiver vazia

		return (f->tamanho==0);
	}
*/
/////////////////////////////////////////////////////////////////////////////////////////

	void mostrarFila(Fila *f){ //para mostra o conteudo da fila, neste caso a lista de processos
		int cont, i, espacos = 0;

		for(cont=0, i = f->primeiro; cont < f->tamanho; cont++){

			printf("%d ", f->processos[i++]);

			if(i == f->capacidade){
				i=0;
			}
				espacos++;
		}

		while(espacos < 5){
			printf("    ");
			espacos++;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////

	int dgtlen(int n){//retorna o numeor de digitos do valor introduzido, neeste caso para identificar processos (PID com 3 digitos)

	  int c;

	  if(n == 0){

	    return 1;

	  }else{

	    for(c = 0; n; n /= 10){
	      c++;
	    }
	  }
	  return c;
	}

//////////////////////////////////////////////////////////////////////////////////////


Processo *novo_processo(){ //inicializa um novo processo

	Processo *p =  (Processo*) malloc(sizeof(Processo));
	p->num_burst = 0;
	p->num_wait = 0;

	return p; //retorna o processo
}

 //////////////////////////////////////////////////////////////////////////////////////


 Processo *arruma_processo(int lista[], int indice_inicial, int indice_final){ //arruma os processos por estrutura atribuindo valores às variaveis da estrutura processo

   Processo *processo = novo_processo();

   int indice_max_burst = 0;
   int indice_max_wait = 0;
   int count = 0;

   for(int i = indice_inicial; i <= indice_final; i++){

     if(dgtlen(lista[i]) == 3){

       count = 1;
       processo->PID = lista[i];

     }else if(count == 1){

       processo->t_chegada = lista[i];
       count++;

     }else if(count % 2 == 0){ //se for indice par

       processo->burstTime[indice_max_burst] = lista[i];
       count++;
       indice_max_burst++;
       processo->tamanho_burst = indice_max_burst;

     }else if(count % 2 != 0){ //se for indice impar

       processo->waitTime[indice_max_wait] = lista[i];
       count++;
       indice_max_wait++;
       processo->tamanho_wait = indice_max_wait;

     }
   }
   return processo;
 }

 /////////////////////////////////////////////////////////////////////////////////////

 void ready_check(Fila *f, Processo *lista[], int instante){ //função para inserir no ready um processo quando um instante corresponde ao seu tempo de chegada
	 int i;
	 for(i=0; i < f->capacidade; i++){
		 if(instante==lista[i]->t_chegada){
			 inserir(f, lista[i]);
		 }
	 }
 }

/////////////////////////////////////////////////////////////////////////////////////

int to_run(Fila *f, Processo *processo){
	int run_controlo;
	run_controlo = processo->PID;
	printf("| RUN-> %d", processo->PID);
	processo->burstTime[processo->num_burst]--;
	if(processo->burstTime[processo->num_burst] == 0){
		processo->num_burst++;
		remover(f);
		run_controlo = 0;
		}
	return run_controlo;
}

///////////////////////////////////////////////////////////////////////////////////////

int to_blocked(Fila *f){

	printf("| BLOCKED-> ");

	int blocked_controlo = 0;
	int blocked_prim = f->primeiro;

	for(int count = 0; count < f->tamanho; count++){

		if(dgtlen(f->processos[blocked_prim].PID)==3){
			printf("%d ", f->processos[blocked_prim].PID);
		}

		f->processos[blocked_prim].waitTime[f->processos[blocked_prim].num_wait]--;

		if(f->processos[blocked_prim].waitTime[f->processos[blocked_prim].num_wait] == 0){
			blocked_controlo = 1;
		}
		blocked_prim++;
	}
	return blocked_controlo;
}
/////////////////////////////////////////////////////////////////////////////////////////

int menu(void)
{
 int op;

 printf("\n");
 printf("1. ROUND ROBIN\n");
 printf("2. FCFS\n");
 printf("3. SAIR\n");
 printf("\n");
 printf("Opcao: "); scanf("%d", &op);

 return op;
}
/////////////////////////////////////////////////////////////////////////////////////

void rr(Processo *processo[], int count_processos, int totalBurstTime){

	Fila ready;
	Fila fila;
	Fila blocked;
	int run_c = 0;
	int confirma = 0;
	int quantum = 0;

	criarFila(&fila, count_processos);
	criarFila(&ready, count_processos);
	criarFila(&blocked, count_processos);


		for(int instante = 0; instante < totalBurstTime; instante++){

			ready_check(&ready, processo, instante);

			printf("\n%3d|", instante);

			if(run_c == 0 && ready.tamanho > 0){
				fila = ready;
				remover(&ready);
			}

			int tam_fila = fila.tamanho;

			printf(" READY-> "); mostrarFila(&ready);

			Processo *aux = &fila.processos[fila.primeiro];


			if(fila.tamanho > 0){
				quantum++;
				run_c = to_run(&fila, &fila.processos[fila.primeiro]); //run current process

				if(quantum==QUANTUM && aux == &fila.processos[fila.primeiro]){
					inserir(&ready, &fila.processos[fila.primeiro]);
					remover(&fila);
				}else if(aux != &fila.processos[fila.primeiro]){
					quantum = 0;
				}
				printf("        ");
			}else{
				printf("| RUN-> ");	printf("           "); //caso nao haja nada para mostrar não desconfigura
			}


			if(blocked.tamanho > 0){
				confirma = to_blocked(&blocked);
			}else{
				printf("| BLOCKED-> ");
			}

			int primeiro = blocked.primeiro;
			Processo *blocked_temp;

			if(confirma == 1){
				for(int i = 0; i < blocked.tamanho; i++){
					blocked_temp = &blocked.processos[primeiro];
					if((blocked_temp->waitTime[blocked_temp->num_wait]) == 0){
						blocked_temp->num_wait++;
						inserir(&ready, blocked_temp);
						remover(&blocked);
					}
					primeiro++;
				}
			}


			if(tam_fila > fila.tamanho){
				if(aux->num_burst != aux->tamanho_burst){
					inserir(&blocked, aux);
				}
			}
		}
	}

void fcfs(Processo *processo[], int count_processos, int totalBurstTime){

		Fila ready;
		Fila fila;
		Fila blocked;
		int run_c = 0;
		int confirma = 0;

		criarFila(&fila, count_processos);
		criarFila(&ready, count_processos);
	  criarFila(&blocked, count_processos);


			for(int instante = 0; instante < totalBurstTime; instante++){

			 	ready_check(&ready, processo, instante);

				printf("\n%3d|", instante);

				if(run_c == 0 && ready.tamanho > 0){
					fila = ready;
					remover(&ready);
				}

				int tam_fila = fila.tamanho;

				printf(" READY-> "); mostrarFila(&ready);

				Processo *aux = &fila.processos[fila.primeiro];

				if(fila.tamanho > 0){
					run_c = to_run(&fila, &fila.processos[fila.primeiro]); //run current process
					printf("        ");
				}else{
					printf("| RUN-> ");	printf("           "); //caso nao haja nada para mostrar não desconfigura
				}

				if(blocked.tamanho > 0){
					confirma = to_blocked(&blocked);
				}else{
					printf("| BLOCKED-> ");
				}

				int primeiro = blocked.primeiro;
				Processo *blocked_temp;

				if(confirma == 1){
					for(int i = 0; i < blocked.tamanho; i++){
						blocked_temp = &blocked.processos[primeiro];
						if((blocked_temp->waitTime[blocked_temp->num_wait]) == 0){
						  blocked_temp->num_wait++;
							inserir(&ready, blocked_temp);
							remover(&blocked);
						}
						primeiro++;
					}
				}


				if(tam_fila > fila.tamanho){
					if(aux->num_burst != aux->tamanho_burst){
						inserir(&blocked, aux);
					}
				}
			}
		}

void main()
{

  int totalBurstTime = 0;
  FILE *file;
  int count_processos = 0;
  int *Vetor = NULL;
  int percorre_Linhas = 0;
	int op;
	int capacidade = 0;
	int indice_inicial = 0;
	int indice_final;



  file = fopen("input1.txt", "r");

  if(file == NULL)
  {
    printf("Ficheiro nao encontrado.\n");
  }

  while(feof(file) == 0)
  {
    Vetor = (int *) realloc(Vetor, (percorre_Linhas + 1) * sizeof(int));
    fscanf(file, "%d", &Vetor[percorre_Linhas]);
    percorre_Linhas++;
  }
	fclose(file);

//Ficheiro foi lido e fechado,os valores estão agora dentro de um vetor;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("\n");
  printf("                                        SIMULADOR DE ESCALONAMENTO FCFS | RR                                            \n");
  printf("_____________________________________________________________________________________________________________________\n");

  printf("\n");

  if(dgtlen(Vetor[0])!=3){

    printf("Nao foram introduzidos processos / Insira os processos corretamente com a identificacao do procsessos corretamente.");

  }else{

  	for(int i = 0 ; i < percorre_Linhas; i++){
    	printf("%d ", Vetor[i]);
      if(dgtlen(Vetor[i+1]) == 3){
        printf("\n");
    	}
    }
  }

////////////////////////////////////////////////////////////////////////////////
//contagem do numero de processos

	for(int i = 0 ; i < percorre_Linhas; i++){
		if(dgtlen(Vetor[i])==3){
			count_processos++;
		}
	}

//calculo do burst time total/////////////////////////////////////////////////////

	for(int i = percorre_Linhas-1; i > 0; i = i-2){
		if(dgtlen(Vetor[i]) != 3){
			totalBurstTime = totalBurstTime + Vetor[i];
		}else{
			i--;
			totalBurstTime = totalBurstTime + Vetor[i];
		}
	}

	Processo *processo[count_processos];

	//////////////////////////////////////////////////////////////////////////////////

	for(int i = 0; i < percorre_Linhas; i++){
		if(dgtlen(Vetor[i+1]) == 3 || Vetor[i+1] < 0 || Vetor[i+1] > 999){

			indice_final = i;
			Processo *proc = arruma_processo(Vetor, indice_inicial, indice_final);
			processo[capacidade] = proc;
			capacidade++;
			indice_inicial = indice_final + 1;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////////

  printf("\n");
  printf("_____________________________________________________________________________________________________________________\n");
  printf("\n");
  printf("Foram inseridos %d processos\n", count_processos);
  printf("\n");
  printf("O ficheiro foi lido e os dados apresentados e guardados com sucesso.\n");
  printf("\n");


	do{
		op = menu();

		switch(op){
			case 1:
				rr(processo, count_processos, totalBurstTime);
				break;

			case 2:
				fcfs(processo, count_processos, totalBurstTime);
				break;

			case 3:
				exit(1);
				break;

			default:
				printf("Opcao invalida");
			}
		printf("\n");

	}while(op);

///////////////////////////////////////////////////////////////////////////////////////

///2ªforma de implementação sem vetores dinâmicos (gerou problemas na alocaçao de memoria)

 //calculo do numero de instrucoes por processo
/*
	int n_instrucoes = -1; //nº de instrucoes por processo eccepto PID e t_chegada;

	for(int i = Linhas-1; i >= 0; i--){
		if(dgtlen(Vetor[i])==3){

			processo[count_processos]->tamanho = n_instrucoes;
			processo[count_processos]->tamanho_burst = (int)(n_instrucoes + 1)/2;
			processo[count_processos]->tamanho_wait = n_instrucoes - processo[count_processos]->tamanho_burst;
			n_instrucoes = -1;
			count_processos--;

		}else{

			n_instrucoes++;
		}
	}

////////////////////////////////////////////////////////////////////////////////////

//atribuir pid e t.chegada a cada processo
	int aleat = 0;

	for(int i = 0; i < Linhas; i++){
		if(dgtlen(Vetor[i])==3){
			processo[aleat]->PID = Vetor[i];
			processo[aleat]->t_chegada = Vetor[i + 1];
			processo[aleat]->num_burst = 0;
			processo[aleat]->num_wait = 0;
			aleat++;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////

//atribuir burst
int indice = (processo[n_processos]->tamanho) - 3;

	for(int i = Linhas-1; i > 0; i = i-2){
    if(dgtlen(Vetor[i]) != 3){
			processo[n_processos]->burstTime[indice] = Vetor[i];
			indice--;
		}else{
			i--;
			n_processos--;
			indice = processo[n_processos]->tamanho - 3;
			processo[n_processos]->burstTime[indice] = Vetor[i];
			indice--;
		}
	}

////////////////////////////////////////////////////////////////////////////////////

//atribuir wait
 n_processos = 0;
 indice = 0;

	for(int i = 3; i < Linhas-1; i = i+2){
		if(dgtlen(Vetor[i])!=3){
			processo[n_processos]->waitTime[indice] = Vetor[i];
			indice++;
		}else{
			n_processos++;
			i = i + 3;
			indice = 0;
			processo[n_processos]->waitTime[indice] = Vetor[i];
			indice++;
		}
	}
*/
}
