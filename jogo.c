/*+++
 * This file is part of "2Âº Projeto de IntroduÃ§Ã£o Ã  ComputaÃ§Ã£o 2017/2018"
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
 * This file is an application of the API defined in the
 * showworld.h header that is mandatory for the project; and is our view in how
 * the game should be like
 *
 * @author Nuno Fachada
 * @Co-Authors Ines Goncalves, Ines Nunes, Joao Duarte
 * @date 2018
 * @copyright [GNU General Public License version 3 (GPLv3)]
 * (http://www.gnu.org/licenses/gpl.html)
 * */

/** Standard libraries used by C programming */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/** The graphic library chosen by us */
#include "raylib.h"
/** Library we got to read the files */
#include "ini.h"
/** Library provived by the teacher */
#include "showworld.h"
/** Our own libraries */
#include "agent.h"
#include "world.h"

/** Agent's structure that will receive all information needed for the game
* so it aids the research and data management  */
struct agentID {
    int x;
    int y;
    unsigned int id;
    AGENT_TYPE type;
    unsigned int ply;
};

/** Structure that easily saves the configuration read by the ini files */
typedef struct {
    int xdim;
    int ydim;
    int nzombies;
    int nhumans;
    int nzplayers;
    int nhplayers;
    int maxturns;
} configuration;

/** Function that will check if the agent surpasses the map's limits, changing
* to its correct place if it does */
void toroidal(int *x, int *y, int *toro, int *na);

/** Function that receives the calculated moves of the specific agent and
* follows the previous function to properly place it */
void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein,
	int *movedir);

/** Receives the information about the agent, calculates it with
* the closest agent and will decide if it'll move away or closer to
* said agent, also checking for collision, infecting it */
void distancia(int *x, int *y, int xNovo, int yNovo, int typeA,
        AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar,
        struct agentID *agents, int nagents);

/** This function is an implementation of the definition provided by the
 * ::get_agent_info_at() function pointer. It only works for AGENT and WORLD
 * example structures defined in this file. */
unsigned int example_get_ag_info(void *w, unsigned int x, unsigned int y);

/** Function that was received by ini.h that will read from the ini and will
* transform the information into variables */
static int handler(void* user, const char* section, const char* name,
        const char* value);

/** to remove */
int na1;




/** This main function is our way to use the API defined in showworld.h,
* mandatory to this project 
* @return Always zero */


int main(int argc, char **argv) {
	/** Initialized WORLD *w and starts it at NULL */
    WORLD *w = NULL;

    /** Initializes a variable structure that will save the data read by the
    * files in the configuration's structure */
    configuration config;

    /** Calls the function handler, saves the data needed in config, and if it's
    * below 0, it didn't read anything, and the prinft message will appear */
    if (ini_parse("config.ini", handler, &config) < 0) {
        printf("Nao foi possivel encontrar o ficheiro 'config.ini'\n");
        return 1;
    }
    /** Creates the total number of agents, adding the humans and zombies
    * together */
    int nagents = config.nhumans + config.nzombies;
    
    /** Creates an array of structures to save the different agents */
    struct agentID agents[nagents];
    
    /** Initializes all the structures of arrays at 0 */
    for (int i = 0; i < nagents; i++) {
        agents[i].x = 0; 
        agents[i].y = 0; 
        agents[i].id = 0; 
        agents[i].type = 0; 
        agents[i].ply = 0; 
    }

    /** Initializes the world that will be shown with the dimensions read and
    * the fuction that can interpret the world */
    SHOWWORLD *sw = showworld_new(config.xdim, config.ydim,
    	example_get_ag_info);

    /** Creates the world with the configurations read */
    w = world_new(config.xdim, config.ydim);

    /** Function to generate random numbers */
    srand(time(NULL));
    
    /** It'll run over the number between 0 and the number of agents, to
    * guarantee all are created */
    for (int i = 0; i < nagents; i++) {
        int x, y;
        AGENT_TYPE at;

        /** We give x and y a random number, to place them in the grid */
        x = (rand() % config.xdim);
        y = (rand() % config.ydim);

        /** In this case we avoid "saving" on top, otherwise we'll lose the
        * reference to * agents created and then we cannot lose said memory */
        if (world_get(w, x, y) == NULL) {
            if (i >= 0 && i < config.nhumans) {
                at = Human;
            } else if (i >= config.nhumans && i < nagents) {
                at = Zombie;
            }

            /** It'll create the agent using the information previously
            * given, it'll place it on the grid using the generated data, and
            * it'll save it on the agent's array at the same time */
            unsigned int playable = (rand() % 10 == 0);
            AGENT *a = agent_new(at, i, playable);
            world_put(w, x, y, (ITEM *) a);
            agents[i].id = i;
            agents[i].x = x;
            agents[i].y = y;
            agents[i].type = at;
            agents[i].ply = playable;
        } else {
        	/** If there's already an agent in a certain place, it will create
        	* another one, so that there's an equal amount of agents asked to
        	* generate */
            i--;
        }
    }
    /** It'll update the world, which was was previously saved by a structure, 
    * and when received by the function, it'll transform it into a 
    * graphic library */
    showworld_update(sw, w);
    printf("Carregue no ENTER para mudar de turno");
    getchar();
    printf("\n");

    /** This function will be responsible to manage the turns in an efficient
    * and correct way */
    for (int turn = 0; turn < config.maxturns; turn++) {

    	/** Modern adaptation of the Fisher-Yater algorithm
    	* It'll start at the end of the array, and it'll shuffle its position
    	* with another one, decrementing them, and it will repeat until all of
    	* them are shuffled */
        for (int i1 = nagents - 1; i1 > 0; i1--) {
            int index = rand() % i1;
      
            int temp = agents[index];
            agents[index] = agents[i1];
            agents[i1] = temp;
        }

        /** Declaration of the needed values to 0 */
        int move = 0;
        int xPrincipal = 0;
        int yPrincipal = 0;
        int xNovo = 0;
        int yNovo = 0;
        int aNovo = 0;

        /** na will run over all the agents, in the same order they were
        * shuffled in */
        for (int na = 0; na < nagents - 1; na++) {
        	/** Variable that will save the current position of the agent */
            xPrincipal = agents[na].x;
            yPrincipal = agents[na].y;
            /** Variable that'll search for the closest agent */
            xNovo = agents[na].x;
            yNovo = agents[na].y;
            /** Variable that'll save the type of agent */
            int typeA = 0;
            /**Boolean variable that will save if the toroidal was used
            * or not */
            int toro = 0;
            /** Boolean variable that will save if the agent moved or not */
            int apagar = 0;
            /** Direction in which the agent will move to */
            move = 3;
            /** Variable that'll save the player's input */
            int seta;
            /** Variable that moves the player when its playable */
            int xMexe = 0;
            int yMexe = 0;

            /** This will check if the agent in the current turn is playable */
            if (agents[na].ply == 1) {

            	/** Waits for an input that corresponds to one of the numbers
            	* previously defined */
                while (seta != 1 || seta != 2 || seta != 3 || seta != 4 || 
                	seta != 6 || seta != 7 || seta != 8 || seta != 9) {

                	/** It'll print the instructions for the input */
                    printf("Use as setas para se mexer\n");
                    scanf("%d", &seta);
                    /** It'll save in xMexe and yMexe the value of the original
                    * x and y */
                    xMexe = agents[na].x;
                    yMexe = agents[na].y;
                    switch (seta) {
                        case 1:
                            xMexe--;
                            yMexe++;

                            break;
                        case 2:
                            yMexe++;
                            break;
                        case 3:
                            xMexe++;
                            yMexe++;
                            break;
                        case 4:
                            xMexe--;
                            break;
                        case 6:
                            xMexe++;
                            break;
                        case 7:
                            xMexe--;
                            yMexe--;
                            break;
                        case 8:
                            yMexe--;
                            break;
                        case 9:
                            xMexe++;
                            yMexe--;
                            break;
                    }
                    toroidal(&xMexe, &yMexe, &toro, &na);

                    if (world_get(w, xMexe, yMexe) == NULL) {
                        agents[na].x = xMexe;
                        agents[na].y = yMexe;
                        apagar = 1;

                    } else {
                        for (aNovo = 0; aNovo < nagents; aNovo++) {
                            if (xMexe == agents[aNovo].x &&
                            	yMexe == agents[aNovo].y && agents[na].type !=
                            	agents[aNovo].type) {
                                if (agents[aNovo].type == Human &&
                                	agents[na].type == 2) {
                                    agents[aNovo].type = 2;
                                    AGENT *a2 = world_get(w, agents[aNovo].x,
                                    	agents[aNovo].y);
                                    mudar_agent_type(agents[aNovo].type,
                                    	(AGENT *) a2);
                                    world_put(w, agents[aNovo].x,
                                    	agents[aNovo].y, (ITEM *) a2);
                                    free(a2);

                                }
                            }

                        }

                    }



                    goto movela;

                }
            } else {

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
                                for (aNovo = 0; aNovo < nagents; aNovo++) {
                                    if (xNovo == agents[aNovo].x && yNovo == 
                                    	agents[aNovo].y && agents[na].type 
                                    	!= agents[aNovo].type) {
                                        if (agents[na].type == Zombie) {
                                            typeA = 1;
                                        }

                                        goto movela;

                                    }

                                }
                            }
                        }
                    }
                }
            }

movela:
            printf("aaaaaaaaaaaaaaaaaa %d %d\n", agents[na].x, agents[na].y);


            if (agents[na].ply == 0) {
                distancia(&agents[na].x, &agents[na].y, agents[aNovo].x, 
                	agents[aNovo].y, typeA, &agents[na].type, toro, w, na, 
                	&apagar, agents, nagents);
            }

            AGENT *a1 = agent_new(agents[na].type, agents[na].ply, 
            	agents[na].id);

            if (apagar == 1) {
                world_apagar(w, xPrincipal, yPrincipal);
            }
            world_put(w, agents[na].x, agents[na].y, (ITEM *) a1);
            printf("Pressione ENTER para o seguinte turno...");

            getchar();

            showworld_update(sw, w);

        }
    }


    //showworld_update(sw, w);

    showworld_destroy(sw);
    world_destroy_full(w, (void (*)(ITEM *))agent_destroy);
    return 0;
}

static int handler(void* user, const char* section, const char* name,
        const char* value) {
    configuration* pconfig = (configuration*) user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("dim", "xdim")) {
        pconfig->xdim = atoi(value);
    } else if (MATCH("dim", "ydim")) {
        pconfig->ydim = atoi(value);
    } else if (MATCH("nInit", "nzombies")) {
        pconfig->nzombies = atoi(value);
    } else if (MATCH("nInit", "nhumans")) {
        pconfig->nhumans = atoi(value);
    } else if (MATCH("nControl", "nzplayers")) {
        pconfig->nzplayers = atoi(value);
    } else if (MATCH("nControl", "nhplayers")) {
        pconfig->nhplayers = atoi(value);
    } else if (MATCH("turns", "maxturns")) {
        pconfig->maxturns = atoi(value);
    } else {
        return 0; /* unknown section/name, error */
    }
    return 1;
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
    } else if (((xN - xNovo > 10) || (xN - xNovo < -10)) && ((yN - yNovo > 10) 
    	|| (yN - yNovo < -10))) {
        *movein = 1;
        dir = 2;
    }

    *movedir = dir;
}

void distancia(int *x, int *y, int xNovo, int yNovo, int typeA,
        AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar,
        struct agentID *agents, int nagents) {
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

    if (world_get(w, xN, yN) == NULL) {

        *x = xN;
        *y = yN;
        *apagar = 1;

    } else {

        int aNovo;
        for (aNovo = 0; aNovo < nagents; aNovo++) {

            if (xN == agents[aNovo].x && yN == agents[aNovo].y && agTypeAnt 
            	!= agents[aNovo].type) {
                if (agents[aNovo].type == Human && *agTypeAnt == 2) {
                    agents[aNovo].type = 2;
                    AGENT *a2 = world_get(w, agents[aNovo].x, agents[aNovo].y);
                    mudar_agent_type(agents[aNovo].type, (AGENT *) a2);
                    world_put(w, agents[aNovo].x, agents[aNovo].y, (ITEM *) a2);
                    free(a2);
                }
            }
        }
    }
}