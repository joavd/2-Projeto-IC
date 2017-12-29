#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define a personagem
struct agente_inf {
	unsigned int tipo;
	unsigned int jogavel;
	unsigned int id;
} agente[400];

// Define o mundo
typedef struct {
	unsigned int xtamanho;
	unsigned int ytamanho;
} MUNDO;

/* Gerar o local onde os agentes vão fazer spawn */
void geraLocal(int xdim, int ydim);

void desenhaAgente(int xdim, int ydim);

int main() {
	// Inicializa o n. aleatorio
	srand(time(NULL));

	int xdim = 20;
	int ydim = 20;

	// Chama a funcao geraLocal
	geraLocal(xdim, ydim);

	// Chama a funcao desenhaAgente
	desenhaAgente(xdim, ydim);

	/*o_meu_mundo.grelha = (AGENTE *) grelha_agente;

	o_meu_mundo.xtamanho = xdim;
	o_meu_mundo.ytamanho = ydim;*/

	return 0;
}

void geraLocal(int xdim, int ydim) {
	int contador = 0;

	// N. de personagens
	unsigned int nagentes = 0;

	for (int x = 0; x < xdim; x++) {

		for (int y = 0; y < ydim; y++) {

			unsigned int tipo = 0;
			unsigned int jogavel = 0;
			unsigned int id = 0;
			contador++;

			// Define a probabilidade
			unsigned char probabilidade = rand() % 100;

			if (probabilidade < 10 && nagentes < 40) {
				tipo = (rand() % 2 == 0) ? 1 : 2;

				id = nagentes++;
			}

			agente[contador].tipo = tipo;
			agente[contador].jogavel = jogavel;
			agente[contador].id = id;
		}

	}
}

void desenhaAgente(int xdim, int ydim) {
	int contador = 0;
	for (int xi = 0; xi < xdim; xi++) {
		
		for (int yi = 0; yi < ydim; yi++) {

			contador++;
			unsigned int celula = agente[contador].tipo;


			switch (celula) {

				case 0:
					printf(" .   ");
					break;

				case 1:
					printf(" H");
					printf("%02x ", agente[contador].id);
					break;

				case 2:
					printf(" Z");
					printf("%02x ", agente[contador].id);
					break;

				default:
					printf(" ? ");
			}
		}

		printf("\n\n");
	}
}