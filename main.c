#define FUSE_USE_VERSION 26     // FUSE version specification

#include <fuse.h>               // FUSE library; install "libfuse-dev" with apt
#include <stdio.h>              // NULL

#include "global.h"             // project-global header

static struct fuse_operations op_list = {
    // TODO
};

int main (int argc, char* argv[])
{
    return fuse_main(argc, argv, &op_list, NULL);
}