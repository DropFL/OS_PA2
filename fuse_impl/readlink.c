#include "../global.h"
#include <stdlib.h>

static int __readlink (const char *path, char *buffer, size_t size)
{
    if (!path[1])
        return -EEXIST;

    DirEntry *dir;

    int res = find_dir(path, &dir);
    if (res) return res;

    const char *leaf = strrchr(path, '/') + 1;

    for (DirEntry* child = dir->child; child; child = child->sibling)
        if (!strcmp(child->name, leaf))
            return read_node(child->inode, buffer, size, 0);
    
    return -EEXIST;
}

readlink_type my_readlink = __readlink;