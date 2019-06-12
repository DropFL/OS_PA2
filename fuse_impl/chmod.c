#include "../global.h"

static int __chmod (const char *path,
                    mode_t mode)
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
            
            child->inode->mode = mode;
            child->inode->c_time = time(NULL);

            return 0;
        }

    return -ENOENT;
}

chmod_type my_chmod = __chmod;