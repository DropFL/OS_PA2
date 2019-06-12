#include "../global.h"
#include <string.h>
#include <stdio.h>

static Inode root_inode =
{
};

DirEntry root =
{
    .inode  = &root_inode,
    .name   = "",
};

static void *__init (struct fuse_conn_info *info)
{
    struct stat *st = (struct stat*)fuse_get_context()->private_data;

    root_inode.uid = st->st_uid;
    root_inode.gid = st->st_gid;
    root_inode.mode = st->st_mode;
    root_inode.link_cnt = st->st_nlink;
    root_inode.a_time = st->st_atime;
    root.parent = &root;

    char self[NAME_LEN] = ".";
    Inode *node = &root_inode;
    
    write_node(&root_inode, self, NAME_LEN, 0);
    write_node(&root_inode, (char*)&node, sizeof(Inode*), NAME_LEN);
}

init_type my_init = __init;