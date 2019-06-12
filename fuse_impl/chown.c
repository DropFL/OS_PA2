#include "../global.h"

static int __chown (const char *path,
                    uid_t owner, gid_t group)
{
    char new_path[NAME_LEN];
    strcpy(new_path, path);

    char* piv = strrchr(new_path, '/');
    DirEntry *dir;
    Inode *node;

    char c = piv[1];
    piv[1] = 0;

    int res = find_dir(new_path, &dir);
    if (res) return res;
    
    piv[1] = c;

    for (DirEntry* child = dir->child; child; child = child->sibling)
        if (!strcmp(child->name, piv + 1))
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