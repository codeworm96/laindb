#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//include these file to use laindb
#include "../lib/laindb-c.h"

#define BUFSIZE 1024

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printf("database name expected\n");
        return 1;
    }

    struct LainDB *db = laindb_new(argv[1], CREATE);

    while (1)
    {
        printf("> ");
        char command[BUFSIZE];
        scanf("%s", command);
        if (strcmp(command, "GET") == 0)
        {
            char key[BUFSIZE];
            scanf("%s", key);
            struct Slice *slice = laindb_get(db, key);
            if (slice)
            { //check result
                size_t len = laindb_slice_len(slice);
                char *value = (char *)malloc(len + 1);
                memcpy(value, laindb_slice_raw(slice), len);
                value[len] = '\0';
                laindb_slice_drop(slice);
                printf("-> %s\n", value);
                free(value);
            }
            else
            {
                printf("-> (Nothing)\n");
            }
        }
        else if (strcmp(command, "DEL") == 0)
        {
            char key[BUFSIZE];
            scanf("%s", key);
            laindb_erase(db, key); //erase a key value pair
        }
        else if (strcmp(command, "PUT") == 0)
        {
            char key[BUFSIZE];
            char val[BUFSIZE];
            scanf("%s", key);
            scanf("%s", val);
            laindb_put(db, key, val, strlen(val)); //insert a key-value pair
        }
        else if (strcmp(command, "EXIT") == 0)
        {
            break;
        }
        else
        {
            printf("Unknow operation\n");
        }
    }

    laindb_drop(db);
    return 0;
}
