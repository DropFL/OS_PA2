#include "../global.h"

static int __rmdir (const char *path)
{
    DirEntry *dir;

    int res = find_dir(path, &dir);
    if (res) return res;

    const char *leaf = strrchr(path, '/') + 1;

    for (DirEntry *child = dir->child, *p = NULL; child; p = child, child = child->sibling)
        if (!strcmp(leaf, child->name))
        {
            if (!S_ISDIR(child->inode->mode)) return -ENOTDIR;
            if (!IS_OWNER(child->inode)) return -EPERM;
            if ( !((IS_OWNER(dir->inode) && (dir->inode->mode & S_IWUSR)) ||
                   (IS_GROUP(dir->inode) && (dir->inode->mode & S_IWGRP)) ||
                   (                        (dir->inode->mode & S_IWOTH)) )) return -EACCES;
            if (!child->child) return -ENOTEMPTY;

            if (p) p->sibling = child->sibling;
            else   dir->child = child->sibling;

            child->inode->link_cnt -= 1;

            return 0;
        }

    return -ENOENT;
}

rmdir_type my_rmdir = __rmdir;