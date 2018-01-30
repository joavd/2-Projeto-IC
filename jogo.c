/*+++
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
 * This file is an application of the API defined in the
 * showworld.h header that is mandatory for the project; and is our view in how
 * the game should be like
 *
 * @author Nuno Fachada
 * @Co-Authors Ines Goncalves, Ines Nunes, Joao Duarte
 * @date 2018
 * @copyright [GNU General Public License version 3 (GPLv3)](http://www.gnu.org/licenses/gpl.html)
 * */

/** Standard libraries used by C programming */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
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
void toroidal(int *x, int *y, int *toro, int *na, configuration config, 
    int *na1);

/** Function that receives the calculated moves of the specific agent and
* follows the previous function to properly place it */
void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein,
	int *movedir, configuration config, int *na1);

/** Receives the information about the agent, calculates it with
* the closest agent and will decide if it'll move away or closer to
* said agent, also checking for collision, infecting it */
void AI(int *x, int *y, int xNovo, int yNovo, int typeA,
        AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar,
        struct agentID *agents, int nagents, configuration config, int *na1);

/** This function is an implementation of the definition provided by the
 * ::get_agent_info_at() function pointer. It only works for AGENT and WORLD
 * example structures defined in this file. */
unsigned int example_get_ag_info(void *w, unsigned int x, unsigned int y);

/** Function that was received by ini.h that will read from the ini and will
* transform the information into variables */
static int handler(void* user, const char* section, const char* name,
        const char* value);


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

    /** Variable to aid the agent_number */
    int na1;

    /** Variables to count how many playable agents are created*/
    int zplay = 0;
    int hplay = 0;
    
    /** It'll run over the number between 0 and the number of agents, to
    * guarantee all are created */
    for (int i = 0; i < nagents; i++) {
        int x, y;
        AGENT_TYPE at;
        int playable = 0;

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

            /** If the agent type is Human and the playable agents weren't all
             *  created it will create one */
            if (at == Human && hplay < config.nhplayers) {
                playable = 1;
                hplay++;
            }

            if (at == Zombie && zplay < config.nzplayers) {
                playable = 1;
                zplay++;
            }

            /** It'll create the agent using the information previously
            * given, it'll place it on the grid using the generated data, and
            * it'll save it on the agent's array at the same time */
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
    printf("Press ENTER to start the game");
    getchar();
    printf("\n\n");

    /** This function will be responsible to manage the turns in an efficient
    * and correct way */
    for (int turn = 0; turn < config.maxturns; turn++) {

        /** Prints the current turn */
        printf("Current Turn: %d\n", turn);

    	/** Modern adaptation of the Fisher-Yater algorithm
    	* It'll start at the end of the array, and it'll shuffle its position
    	* with another one, decrementing them, and it will repeat until all of
    	* them are shuffled */
        for (int i1 = nagents - 1; i1 > 0; i1--) {
            int index = rand() % i1;
      
            struct agentID temp = agents[index];
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

            /** Prints the current moving agent*/
            printf("The current agent moving is: #%x - at x: %d, y: %d\n", 
            agents[na].id, xPrincipal, yPrincipal);

            /** This will check if the agent in the current turn is playable */
            if (agents[na].ply == 1) {

            	/** Waits for an input that corresponds to one of the numbers
            	* previously defined */
                while (seta != 1 || seta != 2 || seta != 3 || seta != 4 ||
                    seta != 5 || seta != 6 || seta != 7 || seta != 8 || 
                    seta != 9) {

                	/** It'll print the instructions for the input */
                    printf("Use the numpad to move\n");
                    scanf("%d", &seta);
                    /** It'll save in xMexe and yMexe the value of the original
                    * x and y of the agent */
                    xMexe = agents[na].x;
                    yMexe = agents[na].y;
                    /** These are the keys to make the agent move */
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
                        case 5:
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

                    /** This will check if we move outside of the grid */
                    toroidal(&xMexe, &yMexe, &toro, &na, config, &na1);

                    /** Verifies if the agent can move to its desired place,
                    * erasing its previous place, because we use a new agent,
                    * so we have to erase the old one */
                    if (world_get(w, xMexe, yMexe) == NULL) {
                        agents[na].x = xMexe;
                        agents[na].y = yMexe;
                        apagar = 1;

                    /** This will only run if there is something in the position
                    * it wants to go */
                    } else {
                        /** It will run through all agents, until it reaches the
                        * total */
                        for (aNovo = 0; aNovo < nagents; aNovo++) {
                            /** This will look through the agents and see which
                            * kind of agent is present within x and y */
                            if (xMexe == agents[aNovo].x &&
                            	yMexe == agents[aNovo].y && agents[na].type !=
                            	agents[aNovo].type) {
                                /** This will check if the agent is a human
                                * and if the other is a zombie */
                                if (agents[aNovo].type == Human &&
                                	agents[na].type == Zombie) {
                                    /** If it's a human, it'll transform into
                                    * a zombie */
                                    agents[aNovo].type = Zombie;
                                    /** It'll get the coordinates of the
                                    infected agent and store it in *a2 */
                                    AGENT *a2 = (AGENT *)world_get(w, agents[aNovo].x,
                                    	agents[aNovo].y);
                                    /** This will change the type of agent */
                                    mudar_agent_type(agents[aNovo].type,
                                    	(AGENT *) a2);
                                    /** This will put the newly transformed
                                    * agent into the world */
                                    world_put(w, agents[aNovo].x,
                                    	agents[aNovo].y, (ITEM *) a2);
                                    /** This will release the memory saved */
                                    free(a2);

                                }
                            }
                        }
                    }
                    /** This is a last resort to make it work without
                    * having to use so many ifs and elses */
                    goto moveIt;

                }
            /** It'll only enter this is the agent isn't playable, it works
            * based on a highly advanced artifical intelligence */
            } else {

                /** Starting at 1, it'll search around the original agent,
                * incrementing the radius by 1 until it finds it */
                for (int i = 1; i <= ((config.xdim + config.ydim)/4); i++) {
                    /** It'll always move 2 steps, following i's size, around
                    * the agent, using the switch (move)
                    * We chose this method because it would be more practical,
                    * making it lighter for the processor */
                    for (int j = 1; j <= 2; j++) {
                        /** We used if and else to run counter clock-wise,
                        * and when it runs all the way, it'll restart */
                        if (move == 4) {
                            move = 1;
                        } else move += 1;
                        /** This will move in (move) i times */
                        for (int block = 1; block <= i; block++) {
                            switch (move) {
                                case 1:
                                    /** Moves to the right i times */
                                    xNovo += 1;
                                    break;
                                case 2:
                                    /** Moves down i times */
                                    yNovo += 1;
                                    break;
                                case 3:
                                    /** Moves to the left i times */
                                    xNovo -= 1;
                                    break;
                                case 4:
                                    /** Moves upwards i times */
                                    yNovo -= 1;
                                    break;
                            }
                            /** By searching for the agent, it'll apply the
                            * toroidal to our searching function if necessary */
                            toroidal(&xNovo, &yNovo, &toro, &na, config, &na1);

                            /** Check if it finds anything */
                            if (w->grid[yNovo * w->xdim + xNovo] != NULL) {
                                /** After it finds something, it'll compare if
                                * it's from the same type, if it is, it'll
                                * ignore, if it's not, it'll go after it */
                                for (aNovo = 0; aNovo < nagents; aNovo++) {
                                    /** Makes the comparison between the x and y
                                    * that was found, and the want that wants
                                    * to move */
                                    if (xNovo == agents[aNovo].x && yNovo == 
                                    	agents[aNovo].y && agents[na].type 
                                    	!= agents[aNovo].type) {
                                        /** If our type is zombie, then we'll
                                        * change typeA to 1 (zombie) */
                                        if (agents[na].type == Zombie) {
                                            typeA = 1;
                                        }
                                        /** By default, it will change TypeA
                                        * to 0(human) */
                                        
                                        /** This is a last resort to make it
                                        * work without having to use so many
                                        * ifs and elses */
                                        goto moveIt;
                                    }
                                }
                            }
                        }
                    }
                }
            }
/** A small shortcut to make it work */
moveIt:
            /** If the agent isn't playable, it'll call the AI function */
            if (agents[na].ply == 0) {
                AI(&agents[na].x, &agents[na].y, agents[aNovo].x, 
                	agents[aNovo].y, typeA, &agents[na].type, toro, w, na, 
                	&apagar, agents, nagents, config, &na1);
            }
            /** It'll create a new agent with the na's type, playability,
            * and id */
            AGENT *a1 = agent_new(agents[na].type, agents[na].ply, 
            	agents[na].id);

            /** If the variable is 1(true), it'll delete the agent, meaning it
            * moved */
            if (apagar == 1) {
                world_apagar(w, xPrincipal, yPrincipal);
            }
            /** This will place the new agent in the respective coordinates */
            world_put(w, agents[na].x, agents[na].y, (ITEM *) a1);

            /** It'll wait for the player's input, otherwise you won't see
            * anything */
            printf("Press ENTER for the next agent to move");
            getchar();

            /** It'll take w(world) and turn it into a sw(showworld), making it
            * slightly prettier to look at */
            showworld_update(sw, w);
        }
    }
    /** After the for has finished running, it'll destroy the world */
    showworld_destroy(sw);
    world_destroy_full(w, (void (*)(ITEM *))agent_destroy);
    return 0;
}

/** This is a function that will read from ini.c and ini.h and it'll save in 
* the structure pconfig */
static int handler(void* user, const char* section, const char* name,
        const char* value) {
    configuration* pconfig = (configuration*) user;

/** This will read from the config.ini the section and name, and will save
* in the structure pconfig the corresponding variables */
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

    /** The agent information to return. */
    unsigned int ag_info = 0;

    /** Convert generic pointer to world to a WORLD object. */
    WORLD *my_world = (WORLD *) w;

    /** Check if the given (x,y) coordinates are within bounds of the world. */
    if ((x >= my_world->xdim) || (y >= my_world->ydim)) {

        /** If we got here, then the coordinates are off bounds. As such we will
        * report that the requested agent is of unknown type. No need to
        * specify agent ID or playable status, since the agent is unknown. */
        ag_info = Unknown;

    } else {

        /** Given coordinates are within bounds, let's get and pack the request
        * agent information. */

        /** Obtain agent at specified coordinates. */
        AGENT *ag = (AGENT *) world_get(my_world, x, y);

        /** Is there an agent at (x,y)? */
        if (ag == NULL) {

            /** If there is no agent at the (x,y) coordinates, set agent type to
            * None. No need to specify agent ID or playable status, since
            * there is no agent here. */
            ag_info = None;

        } else {

            /** If we get here it's because there is an agent at (x,y). Bit-pack
            * all the agent information as specified by the get_agent_info_at
            * function pointer definition. */
            ag_info = (ag->id << 3) | (ag->playable << 2) | ag->type;
        }
    }
    /* Return the requested agent information. */
    return ag_info;
}

/** Function that will check if the agent surpasses the map's limits, changing
* to its correct place if it does
* x and y are the places it will want to pass
* toro is too see if it surpasses the limit or not 
* na is the agent's number */
void toroidal(int *x, int *y, int *toro, int *na, configuration config,
    int *na1) {
    int ax = *x;
    int ay = *y;

    /** If ax(agent's x) is bigger than the grid, it'll go back to 0 */
    if (ax > (config.xdim - 1)) {
        ax = 0;
        if (*na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        /** Ask male Leia later */
        *na1 = *na;

        /** If ax is lower than 0, it'll go back to config.xdim - 1 */
    } else if (ax < 0) {
        ax = (config.xdim - 1);
        if (*na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        *na1 = *na;
    }
    /** If ax(agent's x) is bigger than the grid, it'll go back to 0 */
    if (ay < 0) {
        ay = (config.ydim - 1);
        if (*na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        *na1 = *na;

    /** If ax is lower than 0, it'll go back to config.ydim - 1 */
    } else if (ay > (config.ydim - 1)) {
        ay = 0;
        if (*na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        *na1 = *na;
    }
    /** If the agent is inside of the grid, the toroidal will not
    * be applied and the boolean variables will reset */
    if (ay < 0 && ay > (config.ydim - 1) && ax < 0 && ax > (config.xdim - 1)) {
        *(toro) = 0;
        *na1 -= 1;
    }

    /** It will change the initial values of the main function according with
    * the calculations */
    *x = ax;
    *y = ay;
}

/** Function that receives the calculated moves of the specific agent and
* follows the previous function to properly place it */
void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein,
    int *movedir, configuration config, int *na1) {

    /** Creates a variable so it can perform changes without changing the
    * value */
    int dir = *movedir;

    /** Verifies if the player's current position is more or less than agent
    * found. Depending on the result we got, it'll move on the toroidal in
    * the correct way */
    if ((xN - xNovo > (config.xdim/2)) || (xN - xNovo < (-config.xdim/2))) {
        *movein = 1;
        dir = 0;
    } else if ((yN - yNovo > (config.ydim/2)) || (yN - yNovo <
        (-config.ydim/2))) {
        *movein = 1;
        dir = 1;
    } else if (((xN - xNovo > (config.xdim/2)) || (xN - xNovo <
        (-config.xdim/2)))
     && ((yN - yNovo > (config.ydim/2)) || (yN - yNovo < (-config.ydim/2)))) {
        *movein = 1;
        dir = 2;
    }

    /** Saves the result in the original variable */
    *movedir = dir;
}

/** Function of the artificial intelligence that calculates the place the agent
* will move to and if there's any obstacle in the way */
void AI(int *x, int *y, int xNovo, int yNovo, int typeA,
        AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar,
        struct agentID *agents, int nagents, configuration config, int *na1) {
    /** Saves the value of the position of the new agent  */
    int xN = *x;
    int yN = *y;
    /** Variables that save the direction and place in which the agent will
    * move to */
    int movein = 1;
    int movedir = 4;


    /** Calls the function MoveToroidal to calculate the best place to where the
    * agent should move to */
    MoveToroidal(xN, yN, xNovo, yNovo, &movein, &na, config, &na1);


    /** Variable that will attack or run from either the zombie or human
    * according to the previously calculated option */
    if (movedir == 4) {
        movein = -1;
    }

    // 0 = x; 1 = y; 2 = x && y;

    /** Switch that moves the direction of the agent depending on the place
    * it'll pass on the toroidal */
    switch (movedir) {
        /** If the value is 0, the agent will move according to its x position
        * in the toroidal */
        case 0:
            /** If the agent is a zombie */
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
            /** If the agent is a human */
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
        // 0 = x; 1 = y; 2 = x && y;
        /** If the value is 1, the agent will move according to its y position
        * in the toroidal */
        case 1:
            /** If the agent is a zombie */
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
                /** If the agent is a human */
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
        /** If the value is 2, the agent will move according to its x + y
        * position in the toroidal */
        case 2:
            /** If the agent is a zombie */
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
                /** If the agent is a human */
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
        /** If the value is 4, the agent will move in case it's not inside the
        * toroidal */
        case 4:
            /** If the agent is a zombie */
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
                /** If the agent is a human */
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

    /** Calls the toroidal to check if x and y are outside of the toroidal */
    toroidal(&xN, &yN, &toro, &na, config, &na1);

    /** If x new and y new have no agent in them, it'll send the boolean apagar
    * so that the agent can move without any duplicated ones */
    if (world_get(w, xN, yN) == NULL) {

        *x = xN;
        *y = yN;
        *apagar = 1;

    /** In case there's an agent in them, it'll detect if the agent is from the
    * same type and if it's the same, it'll stay put, otherwise, if it's a
    * zombie trying to move towards a human, it'll transform it into a zombie */
    } else {

        /** Initializes the aNovo to run through all the available agents to
        * check if there's any on that specific x and y */
        int aNovo;
        for (aNovo = 0; aNovo < nagents; aNovo++) {

            /** In case agent is the same as x and y, and if the type of the new
            * agent is different from the original x and y */
            if (xN == agents[aNovo].x && yN == agents[aNovo].y && *agTypeAnt 
            	!= agents[aNovo].type) {
                /** In case the type of the new agent is human and the original
                * agent is a zombie, it'll transform it into a zombie */
                if (agents[aNovo].type == Human && *agTypeAnt == 2) {
                    agents[aNovo].type = 2;
                    /** It'll take the agent that's in the new position
                    * (human)*/
                    AGENT *a2 = world_get(w, agents[aNovo].x, agents[aNovo].y);
                    /** It'll call the function mudar_agent_type to the new
                    * type */
                    mudar_agent_type(agents[aNovo].type, (AGENT *) a2);
                    /** Places the new modified agent in the new x and y */
                    world_put(w, agents[aNovo].x, agents[aNovo].y, (ITEM *) a2);
                    /** Releases the alocated memory */
                    free(a2);
                }
            }
        }
    }
}