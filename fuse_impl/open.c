#include "../global.h"

static int __open (const char *path, struct fuse_file_info *info)
{
    if (!path[1])
        return -EISDIR;

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

    DirEntry *child;
    for (child = dir->child; child; child = child->sibling)
        if (!strcmp(child->name, piv + 1)) break;

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

    return 0;
}

open_type my_open = __open;