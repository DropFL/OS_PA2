#include "../global.h"

static int __truncate (const char *path, off_t length)
{
    DirEntry *dir;

    int res = find_dir(path, &dir);
    if (res) return res;

    const char *leaf = strrchr(path, '/') + 1;

    for (DirEntry *child = dir->child; child; child = child->sibling)
        if (!strcmp(leaf, child->name))
        {
            if (S_ISDIR(child->inode->mode)) return -EISDIR;
            if (child->inode->size == length) return 0;
            
            int res;
            if (child->inode->size < length)
            {
                res = write_node(child->inode, "", 1, length ? length - 1 : 0);
                if (res < 0) return res;
            }
            
            res = clear_node(child->inode, length);
            if (res < 0) return res;
            child->inode->size = length;
            child->inode->block_cnt = length / BLOCK_SIZE + 1;
            return 0;
        }

    return -ENOENT;
}

truncate_type my_truncate = __truncate;