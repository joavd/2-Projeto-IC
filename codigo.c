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

int main() {
	unsigned int xdim = 20;
	unsigned int ydim = 20;
	unsigned int contador = 0;
	unsigned int contador2 = 0;


	// N. de personagens
	unsigned int nagentes = 0;

	// Inicializa o n. aleatorio
	srand(time(NULL));

	for (unsigned int x = 0; x < xdim; x++) {

		for (unsigned int y = 0; y < ydim; y++) {

			unsigned int tipo = 0;
			unsigned int jogavel = 0;
			unsigned int id = 0;
			contador++;

			// Define a probabilidade
			unsigned char probabilidade = rand() % 100;

			if (probabilidade < 5 && nagentes < 20) {
				tipo = (rand() % 2 == 0) ? 1 : 2;

				id = nagentes++;
			}

			agente[contador].tipo = tipo;
			agente[contador].jogavel = jogavel;
			agente[contador].id = id;
		}

	}

	/*o_meu_mundo.grelha = (AGENTE *) grelha_agente;

	o_meu_mundo.xtamanho = xdim;
	o_meu_mundo.ytamanho = ydim;*/

	for (unsigned int xi = 0; xi < xdim; xi++) {
		
		for (unsigned int yi = 0; yi < ydim; yi++) {

			contador2++;
			unsigned int celula = agente[contador2].tipo;


			switch (celula) {

				case 0:
					printf(" .   ");
					break;

				case 1:
					printf(" H");
					printf("%02x ", agente[contador2].id);
					break;

				case 2:
					printf(" Z");
					printf("%02x ", agente[contador2].id);
					break;

				default:
					printf(" ? ");
			}
		}

		printf("\n\n");
	}

	return 0;
}