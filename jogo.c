/*
 * This file is part of "2º Projeto de Introdução à Computação 2017/2018"
 * (2oPIC1718).
 *
 * 2oPIC1718 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 2oPIC1718 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 2oPIC1718. If not, see <http://www.gnu.org/licenses/>.
 * */

/**
 * @file
 * This file is an example of: a) how to use the API defined in the
 * showworld.h header (mandatory for the project); and, b) how to use the
 * concrete simple implementation of the API (provided by the
 * showworld_simple.c file).
 *
 * @author Nuno Fachada
 * @date 2018
 * @copyright [GNU General Public License version 3 (GPLv3)](http://www.gnu.org/licenses/gpl.html)
 * */

#include "showworld.h"
#include "agent.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/** Horizontal world size. */
#define WORLD_X 20

/** Vertical world size. */
#define WORLD_Y 20
#define HUMANS 20
#define ZOMBIES 20
#define PZ 1
#define PH 1
#define MAX_TURN 1000

struct agentID {
    int x[40];
    int y[40];
    unsigned int id[40];
    AGENT_TYPE type[40];
    unsigned int ply[40];
};

void toroidal(int *x, int *y, int *toro, int *na);

void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein, int *movedir);

void distancia(int *x, int *y, int xNovo, int yNovo, int typeA, AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int * apagar, struct agentID *agi);

unsigned int example_get_ag_info(void *w, unsigned int x, unsigned int y);

int na1;


/* This function is an implementation of the definition provided by the
 * ::get_agent_info_at() function pointer. It only works for AGENT and WORLD
 * example structures defined in this file. */
unsigned int example_get_ag_info(void *world, unsigned int x, unsigned int y);

/**
 * This `main` function is only an example of: a) how to use the API defined in
 * the showworld.h header (mandatory for the project); and, b) how to use the
 * concrete simple implementation of the API (provided in the
 * showworld_simple.c file).
 *
 * @return Always zero.
 * */
int main() {
    WORLD *w = NULL;

    SHOWWORLD *sw = showworld_new(WORLD_X, WORLD_Y, example_get_ag_info);

    w = world_new(WORLD_X, WORLD_Y);

    /** Funcao para fazer numeros aleatorios que nao correm predeterminadamente. */
    srand(time(NULL));

    struct agentID *agi;
    agi = calloc(1, sizeof (struct agentID));

    int nagents = HUMANS + ZOMBIES;

    /*
        for (int i = 0; i < HUMANS; i++) {
            int x = rand() % WORLD_X;
            int y = rand() % WORLD_Y;


            if (world_get(w, x, y) == NULL) {
                AGENT *a = agent_new(Human, i, i < PH);
                world_put(w, x, y, (ITEM *) a);
            } else {
                i--;
            }

        }
        for (int i = 0; i < ZOMBIES; i++) {
            int x = rand() % WORLD_X;
            int y = rand() % WORLD_Y;


            if (world_get(w, x, y) == NULL) {
                AGENT *a = agent_new(Zombie, i, i < PZ);
                world_put(w, x, y, (ITEM *) a);
            } else {
                i--;
            }

        }
     */

    for (int turn = 0; turn < MAX_TURN; turn++) {
        if (turn == 0) {
            for (int i = 0; i < nagents; i++) {

                int x, y;


                /*
                loop:
                 */
                /**Damos um valor aleatorio a x e y para metelos na grelha*/
                x = (rand() % WORLD_X);
                y = (rand() % WORLD_Y);

                /* Neste caso evitamos "gravar" por cima, senão perdemos a referência a
                 * agentes criados e depois não podemos libertar a respetiva memória. */
                if (world_get(w, x, y) == NULL) {
                    AGENT_TYPE at = (rand() % 2 == 0) ? Human : Zombie;
                    unsigned int playable = (rand() % 10 == 0);
                    AGENT *a = agent_new(at, i, playable);
                    world_put(w, x, y, (ITEM *) a);
                    agi->id[i] = i;
                    agi->x[i] = x;
                    agi->y[i] = y;
                    agi->type[i] = at;
                    agi->ply[i] = playable;
                    printf("id = %d ", agi->id[i]);
                    //printf(" ola %d %d %d %d\n", agi->x[i], agi->y[i], agi->id[i], agi->ply[i]);
                } else {
                    i--;
                }


                if (i == nagents - 1) {
                    showworld_update(sw, w);
                }

                printf("\n");

            }
        } else {

            //dá shuffle
            for (int i1 = 39; i1 > 0; i1--) {
                int index = rand() % i1;
                //troca
                int temp1 = agi->x[index];
                agi->x[index] = agi->x[i1];
                agi->x[i1] = temp1;

                int temp2 = agi->y[index];
                agi->y[index] = agi->y[i1];
                agi->y[i1] = temp2;

                int temp3 = agi->id[index];
                agi->id[index] = agi->id[i1];
                agi->id[i1] = temp3;

                AGENT_TYPE temp4 = agi->type[index];
                agi->type[index] = agi->type[i1];
                agi->type[i1] = temp4;

                int temp5 = agi->ply[index];
                agi->ply[index] = agi->ply[i1];
                agi->ply[i1] = temp5;
            }

            //Imprime o shuffle
            printf("Shuffle: ");
            for (int i2 = 0; i2 < 40 - 1; i2++) {
                printf("id = %d ", agi->id[i2]);
            }
            printf("\n\n");


            int move;
            int xPrincipal = 0;
            int yPrincipal = 0;
            int xNovo = 0;
            int yNovo = 0;
            int aNovo = 0;
            int na;
            for (na = 0; na < nagents - 1; na++) {
                xPrincipal = agi->x[na];
                yPrincipal = agi->y[na];
                xNovo = agi->x[na];
                yNovo = agi->y[na];
                int typeA = 0;
                int toro = 0;
                int apagar = 0;
                move = 3;



                /* TEST ---- TODO */
                for (int i = 1; i <= 10; i++) {
                    for (int j = 1; j <= 2; j++) {
                        if (move == 4) {
                            move = 1;
                        } else move += 1;
                        for (int block = 1; block <= i; block++) {
                            switch (move) {
                                case 1:
                                    // Mexe para a direita i vezes
                                    xNovo += 1;
                                    break;
                                case 2:
                                    // Mexe para baixo i vezes
                                    yNovo += 1;
                                    break;
                                case 3:
                                    xNovo -= 1;
                                    break;
                                case 4:
                                    // mexe para cima i vezes
                                    yNovo -= 1;
                                    break;
                            }
                            toroidal(&xNovo, &yNovo, &toro, &na);

                            if (w->grid[yNovo * w->xdim + xNovo] != NULL) {
                                for (aNovo = 0; aNovo < nagents - 1; aNovo++) {
                                    if (xNovo == agi->x[aNovo] && yNovo == agi->y[aNovo] && agi->type[na] != agi->type[aNovo]) {
                                        if (agi->type[na] == Zombie) {
                                            typeA = 1;
                                        }

                                        goto movela;


                                    }

                                }

                            }
                            /*
                                                                    world_update(w);
                                                                    printf("Pressione ENTER para o seguinte turno...");
                                                                    getchar();
                             */
                        }
                    }
                }

movela:
                printf("Envontreeeeeeeeiiiiiiiii %d %d %d %d\n", agi->x[na], agi->y[na], agi->x[aNovo], agi->y[aNovo]);


                distancia(&agi->x[na], &agi->y[na], agi->x[aNovo], agi->y[aNovo], typeA, &agi->type[na], toro, w, na, &apagar, agi);

                printf("Envontreeeeeeeeiiiiiiiii %d %d %d %d %d\n", agi->x[na], agi->y[na], agi->x[aNovo], agi->y[aNovo], toro);


                AGENT *a1 = agent_new(agi->type[na], agi->ply[na], agi->id[na]);
/*
                AGENT *a1 = world_get(w, agi->x[xPrincipal], agi->y[yPrincipal]);
*/
                if (apagar == 1) {
                    world_apagar(w, xPrincipal, yPrincipal);
                }

                /*
                                mudar_agent_type(agi->type[aNovo], agi->id[aNovo], (AGENT *) a2);
                                world_put(w, agi->x[aNovo], agi->y[aNovo], (ITEM *) a2);
                 * 
                 *             AGENT aNull = {None, 0, 0, xPrincipal, yPrincipal};

            AGENT a1 = agent_grid[xPrincipal][yPrincipal];
            if (apagar == 1) {

                agent_grid[xPrincipal][yPrincipal] = aNull;
            }

            agent_grid[agi->x[na]][agi->y[na]] = a1;
            printf("agente toca colhoinsssssssssssssssssssss%d %d\n", agi->x[na], agi->y[na]);

            wrld.grid = (AGENT *) agent_grid;
                 */
/*
                mudar_agent_type(agi->type[aNovo], agi->id[aNovo], (AGENT *) a1);
*/

                world_put(w, agi->x[na], agi->y[na], (ITEM *) a1);

                showworld_update(sw, w);


                printf("Pressione ENTER para o seguinte turno...");
                getchar();


            }

        }

    }
    showworld_update(sw, w);
    free(agi);
    showworld_destroy(sw);
    world_destroy_full(w, (void (*)(ITEM *))agent_destroy);
    return 0;
}

/**
 * This function is an implementation of the ::get_agent_info_at() function
 * definition. It only works for ::AGENT and ::WORLD structures defined in this
 * example.
 *
 * It basically receives a pointer to a ::WORLD structure, obtains the AGENT
 * structure in the given coordinates, and returns the agent information in a
 * bit-packed `unsigned int`.
 *
 * @note This is an example which will probably not work in a fully functional
 * game. Students should develop their own implementation of
 * ::get_agent_info_at() and agent/world data structures.
 *
 * @param w Generic pointer to object representing the simulation world.
 * @param x Horizontal coordinate of the simulation world from where to fetch
 * the agent information.
 * @param y Vertical coordinate of the simulation world from where to fetch
 * the agent information.
 * @return An integer containing bit-packed information about an agent, as
 * follows: bits 0-1 (agent type), bit 2 (is agent playable), bits 3-18 (agent
 * ID). Bits 19-31 are available for student-defined agent extensions.
 * */
unsigned int example_get_ag_info(void *w, unsigned int x, unsigned int y) {

    /* The agent information to return. */
    unsigned int ag_info = 0;

    /* Convert generic pointer to world to a WORLD object. */
    WORLD *my_world = (WORLD *) w;

    /* Check if the given (x,y) coordinates are within bounds of the world. */
    if ((x >= my_world->xdim) || (y >= my_world->ydim)) {

        /* If we got here, then the coordinates are off bounds. As such we will
           report that the requested agent is of unknown type. No need to
           specify agent ID or playable status, since the agent is unknown. */
        ag_info = Unknown;

    } else {

        /* Given coordinates are within bounds, let's get and pack the request
           agent information. */

        /* Obtain agent at specified coordinates. */
        AGENT *ag = (AGENT *) world_get(my_world, x, y);

        /* Is there an agent at (x,y)? */
        if (ag == NULL) {

            /* If there is no agent at the (x,y) coordinates, set agent type to
               None. No need to specify agent ID or playable status, since
               there is no agent here. */
            ag_info = None;

        } else {

            /* If we get here it's because there is an agent at (x,y). Bit-pack
               all the agent information as specified by the get_agent_info_at
               function pointer definition. */
            ag_info = (ag->id << 3) | (ag->playable << 2) | ag->type;

        }

    }

    /* Return the requested agent information. */
    return ag_info;

}

void toroidal(int *x, int *y, int *toro, int *na) {
    int ax = *x;
    int ay = *y;

    if (ax > 19) {
        ax = 0;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    } else if (ax < 0) {
        ax = 19;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    }
    if (ay < 0) {
        ay = 19;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    } else if (ay > 19) {
        ay = 0;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    }
    if (ay < 0 && ay > 19 && ax < 0 && ax > 19) {
        *(toro) = 0;
        na1 -= 1;
    }

    *x = ax;
    *y = ay;
}

void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein, int *movedir) {

    int dir = *movedir;

    if ((xN - xNovo > 10) || (xN - xNovo < -10)) {
        *movein = 1;
        dir = 0;
    } else if ((yN - yNovo > 10) || (yN - yNovo < -10)) {
        *movein = 1;
        dir = 1;
    } else if (((xN - xNovo > 10) || (xN - xNovo < -10)) && ((yN - yNovo > 10) || (yN - yNovo < -10))) {
        *movein = 1;
        dir = 2;
    }

    *movedir = dir;
}

void distancia(int *x, int *y, int xNovo, int yNovo, int typeA, AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar, struct agentID *agi) {
    int xN = *x;
    int yN = *y;
    int movein = 1;
    int movedir = 4;

    // 0 = x; 1 = y; 2 = x && y;

    MoveToroidal(xN, yN, xNovo, yNovo, &movein, &movedir);


    if (movedir == 4) {
        movein = -1;
    }

    switch (movedir) {
        case 0:
            if (typeA == 1) {
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
        case 1:
            if (typeA == 1) {
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
        case 2:
            if (typeA == 1) {
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += 1;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += 1;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
        case 4:
            if (typeA == 1) {
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
    }

    toroidal(&xN, &yN, &toro, &na);
    printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa %d %d\n", xN, yN);


    if (world_get(w, xN, yN) == NULL) {

        printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa %d %d\n", xN, yN);

        *x = xN;
        *y = yN;
        *apagar = 1;

    } else {

        int nagents = 40;
        int aNovo;
        for (aNovo = 0; aNovo < nagents; aNovo++) {

            if (xN == agi->x[aNovo] && yN == agi->y[aNovo] && agTypeAnt != agi->type[aNovo]) {
                if (agi->type[aNovo] == Human && *agTypeAnt == 2) {
                    agi->type[aNovo] = 2;
                    AGENT *a2 = world_get(w, agi->x[aNovo], agi->y[aNovo]);
                    mudar_agent_type(agi->type[aNovo], agi->id[aNovo], (AGENT *) a2);
                    world_put(w, agi->x[aNovo], agi->y[aNovo], (ITEM *) a2);


                }

            }

        }

    }

}


