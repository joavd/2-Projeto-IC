# Zombie_Ineses

* Inês Gonçalves - a21702076

* Inês Nunes     - a21702520

* João Duarte    - a21702097

## Our solution

In order to find our solution it was necessary a lot of effort and dedication, after several tries we reunited with
other classmates to make a game that works according to the teacher's instructions.
We starded by taking the exemple given by the teacher and modifided in order to test what we could do. After having an
idea of what was done, we started creating the data structure to be able to comunicate with the world, afterwards tests
were made to play with the agents' positions.
After we had an idea of the behaviour of the "engine" we did the artificial inteligence of the agents.
All the possible outcomes of the movement in the toroidal grid were calculated for a realistic movement, where it'll
find the nearest agent regardless of the side of the grid.
After everything was done we fixed some bugs.

### Architecture

Yada

### Data scructures

Our data scructures are organized by arrays of structs, which means that we have an array that saves all the information
about the agents (type, id, x and y coordenates, etc.) and then passes the information needed to the world struct (x and
y coordenates). All the information that is saved in the struct AGENT is the same as the one in the struct WORLD.

### Algorithms

Our algorithms are the following:

* Search function : Searches the grid in counter-clockwise never passing through the same position twice, 
making this an extremely efficient algorithm for finding a specific agent of the opposite type.

* Move (AI)  function : Calculates the position to where it'll move into, by checking where both agents are on the map,
and evaluating if the agent that moves will need to trangress the grid to the other side, and returning the 
correspondent value to the agent.

* Toroidal function : Checks if the x and y coordinates will transgress the grid and gives them a real value.

* MoveToroidal function : Checks which side it will move when passing the toroidal grid.

* Shuffle function : Shuffles the array of structures so that each turn there's a random agent moving.

* Find agent function : Runs through the array of structures to find the wanted agent.

## User guide

Our game is very simple; if you're using Ubuntu OS you have to open the folder in the terminal and run `make`. 
If you are using Windows, you have to install `*MinGW*`, and open the folder in the command line and run 
`mingw32-make.exe`.
Afterwards, the terminal/command line will open with a call to the graphic library where it'll be possible to visualize
the world. The game will run until it shows a playable agent and, in that case, it'll wait for the player's input.

## Conclusions

Yada

## References

* <a name="ref1">\[1\]</a> Pereira, A. (2017). C e Algoritmos, 2ª edição. Sílabo.
* <a name="ref2">\[2\]</a> "benhoyt" . [Simple .INI file parser in C](https://github.com/benhoyt/inih), GitHub.
* <a name="ref3">\[3\]</a> Formal conversations with the classmates Alejandro, Frederico and André Cosme about the all
project.
* <a name="ref4">\[4\]</a> Formal conversations with the classmates Leandro, Diana and Hugo about the all project.