#include "../global.h"

static int __chown (const char *path,
                    uid_t owner, gid_t group)
{
    DirEntry *dir;

    int res = find_dir(path, &dir);
    if (res) return res;

    const char *leaf = strrchr(path, '/') + 1;

    for (DirEntry* child = dir->child; child; child = child->sibling)
        if (!strcmp(child->name, leaf))
        {
            if (!IS_OWNER(child->inode)) return -EPERM;
            
            if (owner != -1)
            {
                child->inode->uid = owner;
                child->inode->c_time = time(NULL);
            }

            if (group != -1)
            {
                child->inode->gid = group;
                child->inode->c_time = time(NULL);
            }

            return 0;
        }

    return -ENOENT;
}

chown_type my_chown = __chown;