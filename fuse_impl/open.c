#include "../global.h"

static int __open (const char *path, struct fuse_file_info *info)
{
    if (!path[1])
        return -EISDIR;

    DirEntry *dir, *child;

    int res = find_dir(path, &dir);
    if (res) return res;

    const char *leaf = strrchr(path, '/') + 1;

    for (child = dir->child; child; child = child->sibling)
        if (!strcmp(child->name, leaf)) break;

    if (!child) return -ENOENT;

    int fd = get_fd();
    if (fd < 0) return -ENFILE;

    if (info->flags & O_RDWR)
        table[fd].mode = FILE_READ & FILE_WRITE;
    else if (info->flags & O_WRONLY)
        table[fd].mode = FILE_WRITE;
    else
        table[fd].mode = FILE_READ;
    
    table[fd].entry = child;
    child->inode->a_time = time(NULL);

    return 0;
}

open_type my_open = __open;