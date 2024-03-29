#include "../global.h"

static void move_info (const Inode *node, struct stat *st)
{
    st->st_blocks = node->block_cnt;
    st->st_atime = node->a_time;
    st->st_ctime = node->c_time;
    st->st_mtime = node->m_time;
    st->st_uid = node->uid;
    st->st_gid = node->gid;
    st->st_mode = node->mode;
    st->st_nlink = node->link_cnt;
    st->st_size = node->size;
    st->st_dev = node->dev;
}

static int __getattr (const char *path, struct stat *st)
{
    if (!path[1])
    {
        move_info(root.inode, st);
        return 0;
    }

    DirEntry *dir;

    int res = find_dir(path, &dir);
    if (res) return res;

    const char *leaf = strrchr(path, '/') + 1;

    for (DirEntry* child = dir->child; child; child = child->sibling)
        if (!strcmp(child->name, leaf))
        {
            move_info(child->inode, st);
            return 0;
        }

    return -ENOENT;
}

getattr_type my_getattr = __getattr;