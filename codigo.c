#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define a personagem
struct agente_inf {
	unsigned int tipo;
	unsigned int jogavel;
	unsigned int id;
} agente[100];

// Define o mundo
typedef struct {
	unsigned int xtamanho;
	unsigned int ytamanho;
} MUNDO;

int main() {
	unsigned int xdim = 10;
	unsigned int ydim = 10;


	// N. de personagens
	unsigned int nagentes = 0;

	// Inicializa o n. aleatorio
	srand(time(NULL));

	for (unsigned int x = 0; x < xdim; x++) {

		for (unsigned int y = 0; y < ydim; y++) {

			unsigned int tipo = 0;
			unsigned int jogavel = 0;
			unsigned int id = 0;

			// Define a probabilidade
			unsigned char probabilidade = rand() % 100;

			if (probabilidade < 10 && nagentes < 20) {
				tipo = (rand() % 2 == 0) ? 1 : 2;

				id = nagentes++;
			}

			agente[nagentes].tipo = tipo;
			agente[nagentes].jogavel = jogavel;
			agente[nagentes].id = id;
		}

	}

	/*o_meu_mundo.grelha = (AGENTE *) grelha_agente;

	o_meu_mundo.xtamanho = xdim;
	o_meu_mundo.ytamanho = ydim;*/

	for (unsigned int xi = 0; xi < xdim; xi++) {
		
		for (unsigned int yi = 0; yi < ydim; yi++) {

			// unsigned int celula = grelha_agente[xi][yi];

			// switch (celula) {

			// 	case 0:
			// 		printf(" . ");
			// 		break;

			// 	case 1:
			// 		printf("H");
			// 		break;

			// 	case 2:
			// 		printf("Z");
			// 		break;

			// 	default:
			// 		printf(" ? ");
			// }
		}

		printf("\n\n");
	}

	return 0;
}