#include <stdio.h>              // NULL

#include "global.h"             // project-global header

static struct fuse_operations op_list = {
    .getattr    = my_getattr = NULL,
    .readlink   = my_readlink = NULL,
    .mknod      = my_mknod = NULL,
    .mkdir      = my_mkdir = NULL,
    .unlink     = my_unlink = NULL,
    .rmdir      = my_rmdir = NULL,
    .symlink    = my_symlink = NULL,
    .rename     = my_rename = NULL,
    .link       = my_link = NULL,
    .chmod      = my_chmod = NULL,
    .chown      = my_chown = NULL,
    .truncate   = my_truncate = NULL,
    .open       = my_open = NULL,
    .read       = my_read = NULL,
    .write      = my_write = NULL,
    .release    = my_release = NULL,
    .opendir    = my_opendir = NULL,
    .readdir    = my_readdir = NULL,
    .releasedir = my_releasedir = NULL,
    .init       = my_init,
    .destroy    = my_destroy = NULL,
};

int main (int argc, char* argv[])
{
    return fuse_main(argc, argv, &op_list, NULL);
}