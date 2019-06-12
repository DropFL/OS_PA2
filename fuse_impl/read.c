#include "../global.h"

static int __read (const char *path, char *buffer,
                    size_t size, off_t off,
                    struct fuse_file_info *info)
{
    if (table[info->fh].entry == NULL)
        return -EBADFD;

    if (!(table[info->fh].mode & FILE_READ))
        return -EINVAL;

    return read_node(table[info->fh].entry->inode, buffer, size, off);
}

read_type my_read = __read;