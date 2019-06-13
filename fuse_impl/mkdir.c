#include "../global.h"
#include <stdlib.h>

static int __mkdir (const char *path, mode_t mode)
{
    if (!S_ISDIR(mode)) mode |= __S_IFDIR;

    if (!path[1])
        return -EEXIST;

    DirEntry *dir;

    int res = find_dir(path, &dir);
    if (res) return res;

    const char *leaf = strrchr(path, '/') + 1;

    for (DirEntry* child = dir->child; child; child = child->sibling)
        if (!strcmp(child->name, leaf))
            return -EEXIST;
    
    Inode *node = (Inode*) calloc(1, sizeof(Inode));
    DirEntry *entry = (DirEntry*) calloc(1, sizeof(DirEntry));

    entry->inode = node;
    strcpy(entry->name, leaf);
    entry->parent = dir;

    if (dir->child)
    {
        DirEntry *last;
        for (last = dir->child; last->sibling; last = last->sibling);
        last->sibling = entry;
    }
    else dir->child = entry;

    node->uid = getuid();
    node->gid = getgid();
    node->mode = mode;
    node->link_cnt = 1;
    node->block_cnt = 0;

    return 0;
}

mkdir_type my_mkdir = __mkdir;