#include <stdio.h>              // NULL

#include "global.h"             // project-global header
#include "fuse_impl/example.h"  // example!

static struct fuse_operations op_list = {
    // TODO
};

int main (int argc, char* argv[])
{
    hello_fuse();
    return fuse_main(argc, argv, &op_list, NULL);
}