#include <stdio.h>              // NULL

#include "global.h"             // project-global header

/* placeholders; remove if implemented */
getattr_type my_getattr = NULL;
readlink_type my_readlink = NULL;
mknod_type my_mknod = NULL;
mkdir_type my_mkdir = NULL;
unlink_type my_unlink = NULL;
rmdir_type my_rmdir = NULL;
symlink_type my_symlink = NULL;
rename_type my_rename = NULL;
link_type my_link = NULL;
chmod_type my_chmod = NULL;
chown_type my_chown = NULL;
truncate_type my_truncate = NULL;
open_type my_open = NULL;
read_type my_read = NULL;
write_type my_write = NULL;
release_type my_release = NULL;
opendir_type my_opendir = NULL;
readdir_type my_readdir = NULL;
releasedir_type my_releasedir = NULL;
destroy_type my_destroy = NULL;

/* fuse operations */
static struct fuse_operations op_list = {};

int main (int argc, char* argv[])
{
    op_list.getattr    = my_getattr;
    op_list.readlink   = my_readlink;
    op_list.mknod      = my_mknod;
    op_list.mkdir      = my_mkdir;
    op_list.unlink     = my_unlink;
    op_list.rmdir      = my_rmdir;
    op_list.symlink    = my_symlink;
    op_list.rename     = my_rename;
    op_list.link       = my_link;
    op_list.chmod      = my_chmod;
    op_list.chown      = my_chown;
    op_list.truncate   = my_truncate;
    op_list.open       = my_open;
    op_list.read       = my_read;
    op_list.write      = my_write;
    op_list.release    = my_release;
    op_list.opendir    = my_opendir;
    op_list.readdir    = my_readdir;
    op_list.releasedir = my_releasedir;
    op_list.init       = my_init;
    op_list.destroy    = my_destroy;
    
    return fuse_main(argc, argv, &op_list, NULL);
}