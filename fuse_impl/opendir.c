#include "../global.h"

static int __opendir (  const char *path,
                        struct fuse_file_info *info)
{
    DirEntry *dir;

    if (path[1])
    {
        char new_path[NAME_LEN];
        strcpy(new_path, path);
        strcat(new_path, "/");

        int res = find_dir(new_path, &dir);
        if (res) return res;
    }
    else dir = &root;

    int fd = get_fd();
    if (fd == -1) return -ENFILE;

    info->fh = fd;
    table[fd].mode = FILE_READ;
    table[fd].entry = dir;

    return 0;
}

opendir_type my_opendir = __opendir;