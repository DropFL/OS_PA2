#include "../global.h"
#include <string.h>
#include <stdio.h>

static Inode root_inode =
{
};

DirEntry root =
{
    .child  = NULL,
    .parent = NULL,
    .sibling= NULL,
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
    root_inode.m_time = st->st_mtime;
    root_inode.c_time = st->st_ctime;
}

init_type my_init = __init;