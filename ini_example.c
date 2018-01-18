/* Example: parse a simple configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"

typedef struct
{
    int xdim;
    int ydim;
    int nzombies;
    int nhumans;
    int nzplayers;
    int nhplayers;
    int maxturns;
} configuration;

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;

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
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

int main(int argc, char* argv[])
{
    configuration config;

    if (ini_parse("config.ini", handler, &config) < 0) {
        printf("Can't load 'config.ini'\n");
        return 1;
    }
    printf("Config loaded from 'config.ini': xdim=%d, ydim=%d, maxturns=%d, \n",
        config.xdim, config.ydim, config.maxturns);
    return 0;
}
