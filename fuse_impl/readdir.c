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

static int __readdir (const char *path, void *buffer,
                        fuse_fill_dir_t filler, off_t offset,
                        struct fuse_file_info *info)
{
    DirEntry *dir = table[info->fh].entry;
    if (dir == NULL) return -EBADFD;

    struct stat st;
    
    move_info(dir->inode, &st);
    filler(buffer, ".", &st, 0);
    
    move_info(dir->parent->inode, &st);
    filler(buffer, "..", &st, 0);

    for (DirEntry *child = dir->child; child; child = child->sibling)
    {
        move_info(child->inode, &st);
        filler(buffer, child->name, &st, 0);
    }

    return 0;
}

readdir_type my_readdir = __readdir;