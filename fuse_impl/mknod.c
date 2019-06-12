#include "../global.h"
#include <stdlib.h>

static int __mknod (const char *path, mode_t mode, dev_t dev)
{
    if (S_ISDIR(mode) || S_ISLNK(mode))
        return -EINVAL;

    if (!path[1])
        return -EEXIST;

    char new_path[NAME_LEN];
    strcpy(new_path, path);

    char* piv = strrchr(new_path, '/');
    DirEntry *dir;

    char c = piv[1];
    piv[1] = 0;

    int res = find_dir(new_path, &dir);
    if (res) return res;
    
    piv[1] = c;
    
    Inode *node = (Inode*) calloc(1, sizeof(Inode));
    DirEntry *entry = (DirEntry*) calloc(1, sizeof(DirEntry));

    entry->inode = node;
    strcpy(entry->name, piv + 1);
    entry->parent = dir;

    if (dir->child)
    {
        DirEntry *last;
        for (last = dir->child; last->sibling; last = last->sibling);
        last->sibling = entry;
    }
    else dir->child = entry;

    write_node(dir->inode, entry->name, NAME_LEN, dir->inode->size);
    write_node(dir->inode, (char*)&node, sizeof(Inode*), dir->inode->size);

    node->dev = dev;
    node->uid = geteuid();
    node->gid = getegid();
    node->mode = mode;
    node->link_cnt = 1;

    return 0;
}

mknod_type my_mknod = __mknod;