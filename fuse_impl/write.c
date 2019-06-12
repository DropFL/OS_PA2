#include "../global.h"

static int __write (const char *path, const char *buffer,
                    size_t size, off_t off,
                    struct fuse_file_info *info)
{
    if (table[info->fh].entry == NULL)
        return -EBADFD;

    if (!(table[info->fh].mode & FILE_WRITE))
        return -EINVAL;

    return write_node(table[info->fh].entry->inode, buffer, size, off);
}

write_type my_write = __write;