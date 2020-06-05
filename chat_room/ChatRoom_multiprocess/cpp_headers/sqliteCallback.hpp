#ifndef _sqliteCallback_hpp
#define _sqliteCallback_hpp

#include "headers.hpp"


static int checkRepeatedCallback(void *data, int argc, char **argv, char **colName)
{
        int *ifFound = (int *)data;
        *ifFound = 0;
        return 0;
}

static int insertCallback(void *data, int argc, char **argv, char **colName)
{
        fprintf(stdout, "Insert TABLE:\n");
        for (int i = 0; i < argc; i++)
                fprintf(stdout, "%s = %s\n", colName[i], argv[i]);
        return 0;
}

static int checkExistCallback(void *data, int argc, char **argv, char **colName)
{
        int *ifFound = (int *)data;
        *ifFound = 1;
        return 0;
}

static int deleteCallback(void *data, int argc, char **argv, char **colName)
{
        return 0;
}


#endif