#include "../global.h"

static int __releasedir (const char *path,
                        struct fuse_file_info *info)
{
    table[info->fh].entry = NULL;
    table[info->fh].mode = 0;

    return 0;
}

releasedir_type my_releasedir = __releasedir;