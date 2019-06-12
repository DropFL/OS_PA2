#include "../global.h"

static int __release    (const char *path,
                         struct fuse_file_info *info)
{
    table[info->fh].entry = NULL;
    table[info->fh].mode = 0;

    return 0;
}

release_type my_release = __release;