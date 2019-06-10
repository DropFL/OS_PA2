#include "../global.h"
#include <string.h>
#include <stdio.h>

static Inode root_inode = {
    .link_cnt = 1,
};
DirEntry root = {
    .inode = &root_inode,
};

static void *__init (struct fuse_conn_info *info) {
    // 현재 디렉토리 명을 root에 복사합니다.

    char *cwd = (char *)fuse_get_context()->private_data;
    strcpy(root.name, cwd);

    printf("%s\n", cwd);
}

init_type my_init = __init;