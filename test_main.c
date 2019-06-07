#include <stdio.h>              // NULL

#include "global.h"             // project-global header

static struct fuse_operations op_list = {
    // TODO
};

int main (int argc, char* argv[])
{
    hello_fuse();

    // DO SOMETHING

    return 0;
}