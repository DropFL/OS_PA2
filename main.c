#define FUSE_USE_VERSION 26

#include <fuse.h>

#include "global.h"

static struct fuse_operations op_list = {
    // TODO
};

int main (int argc, char* argv[])
{
    return fuse_main(argc, argv, &op_list, NULL);
}