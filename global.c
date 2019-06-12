#include "global.h"
#include <string.h>

#define MAX_LNK 16 

static int depth = 0;

static int advance (const char **path, DirEntry **now)
{
    for (DirEntry* child = (*now)->child; child; child = child -> sibling)
    {
        int len = strlen(child->name);
        if (!strncmp(child->name, *path, len) && (*path)[len] == '/')
        {
            // 심볼릭 링크
            if (S_ISLNK(child->inode->mode))
            {
                // 재귀 탐색 한계
                if (depth++ == MAX_LNK)
                    return -ELOOP;
                
                // 링크의 경로 읽어오기
                char path[NAME_LEN];
                int res = my_readlink(path, path, NAME_LEN);
                if (res) return res;

                // 끝에 '/' 붙이기
                int len = strlen(path);
                path[len] = '/';
                path[len+1] = 0;

                // 재귀적으로 탐색
                int res = find_dir(path, &child);
                if (res) return res;
            }

            // 디렉토리가 아니면 에러
            if (!S_ISDIR(child->inode->mode))
                return -ENOTDIR;

            // 탐색 권한 체크
            if ((IS_GROUP(child->inode) && (child->inode->mode & S_IXGRP)) ||
                (IS_OWNER(child->inode) && (child->inode->mode & S_IXGRP)) ||
                                           (child->inode->mode & S_IXOTH)    )
            {
                *path += len + 1;
                *now = child;
                return 0;
            }

            return -EACCES;
        }
    }

    return -ENOENT;
}

int find_dir (const char *path, DirEntry **sav)
{
    DirEntry *result = &root;
    const char *cur = path + 1;

    printf("path: %s\n", path);

    while (*cur) {
        int res = advance(&cur, &result);
        if (res) return res;
    }

    *sav = result;
    depth = 0;
    return 0;
}