#include "../global.h"

static int __chmod (const char *path,
                    mode_t mode)
{
    DirEntry *dir;

    int res = find_dir(path, &dir);
    if (res) return res;

    const char *leaf = strrchr(path, '/') + 1;

    for (DirEntry* child = dir->child; child; child = child->sibling)
        if (!strcmp(child->name, leaf))
        {
            if (!IS_OWNER(child->inode)) return -EPERM;
            
            child->inode->mode = mode;
            child->inode->c_time = time(NULL);

            return 0;
        }

    return -ENOENT;
}

chmod_type my_chmod = __chmod;