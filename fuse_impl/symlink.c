#include "../global.h"
#include <stdlib.h>

static int __symlink (const char *target, const char *path)
{
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
    node->mode = __S_IFLNK | 0755;
    node->link_cnt = 1;
    
    write_node(node, target, strlen(target), 0);

    return 0;
}

symlink_type my_symlink = __symlink;