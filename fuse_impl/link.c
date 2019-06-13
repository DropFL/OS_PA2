#include "../global.h"
#include <stdlib.h>

static int __link (const char *target, const char *path)
{
    if (!path[1])
        return -EEXIST;

    DirEntry *dir1, *dir2, *targ;

    int res = find_dir(target, &dir1);
    if (res) return res;

    const char *leaf1 = strrchr(target, '/') + 1;

    for (targ = dir1->child; targ; targ = targ->sibling)
        if (!strcmp(targ->name, leaf1)) break;

    if (!targ) return -ENOENT;
    
    res = find_dir(path, &dir2);
    if (res) return res;

    const char *leaf2 = strrchr(path, '/') + 1;

    for (DirEntry *child = dir1->child; child; child = child->sibling)
        if (!strcmp(child->name, leaf2)) return -EEXIST;
    
    DirEntry *entry = (DirEntry*) calloc(1, sizeof(DirEntry));

    entry->inode = targ->inode;
    strcpy(entry->name, leaf2);
    entry->parent = dir2;

    if (dir2->child)
    {
        DirEntry *last;
        for (last = dir2->child; last->sibling; last = last->sibling);
        last->sibling = entry;
    }
    else dir2->child = entry;

    entry->inode->uid = getuid();
    entry->inode->gid = getgid();
    entry->inode->mode = targ->inode->mode;
    targ->inode->link_cnt += entry->inode->link_cnt = 1;

    return 0;
}

link_type my_link = __link;