#include "global.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LNK 16
#define INDIR_BOUND 12
#define INDIR_BLOCK_N 512
#define D_INDIR_BLOCK_N (INDIR_BLOCK_N * INDIR_BLOCK_N)
#define T_INDIR_BLOCK_N (D_INDIR_BLOCK_N * INDIR_BLOCK_N)

#define DATA_AS(buf, idx, type) (*(type*)((buf) + (idx * sizeof(type))))

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
                (                           child->inode->mode & S_IXOTH)    )
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

static Block* new_block ()
{
    return calloc(1, sizeof(Block));
}

static void create_indir (Block* indir, int from, int until)
{
    for (int i = from; i <= until; i ++)
        DATA_AS(indir->data, i, long) = (long)new_block();
}

static void create_block (Inode *node, int until)
{
    int prev = node->block_cnt;
    node->block_cnt = until;

    if (until <= INDIR_BOUND) return;
    
    // 단일 간접 블럭
    until -= INDIR_BOUND;
    prev -= INDIR_BOUND;
    if (prev < 0) {
        prev = 0;
        node->indir = new_block();
    }

    int u_major = until / INDIR_BLOCK_N;
    int p_major = prev / INDIR_BLOCK_N;
    
    u_major = (u_major < INDIR_BLOCK_N) ? u_major : (INDIR_BLOCK_N - 1);
    p_major = (p_major < INDIR_BLOCK_N) ? p_major : (INDIR_BLOCK_N - 1);

    create_indir(node->indir, p_major + 1, u_major);

    if (until <= INDIR_BLOCK_N) return;
    
    // 이중 간접 블럭
    until -= INDIR_BLOCK_N;
    prev -= INDIR_BLOCK_N;
    if (prev < 0) {
        prev = 0;
        node->d_indir = new_block();
    }
    
    u_major = until / INDIR_BLOCK_N;
    p_major = prev / INDIR_BLOCK_N;
    int u_minor = until % INDIR_BLOCK_N;
    int p_minor = prev % INDIR_BLOCK_N;
    
    if (u_major >= INDIR_BLOCK_N)
        u_major = u_minor = INDIR_BLOCK_N - 1;
    
    if (p_major >= INDIR_BLOCK_N)
        p_major = p_minor = INDIR_BLOCK_N - 1;
    
    create_indir(node->d_indir, p_major + 1, u_major);

    for (int i = p_major; i <= u_major; i ++) {
        int u_bound = (i == u_major) ? u_minor : (INDIR_BLOCK_N - 1);
        int p_bound = (i == p_major) ? p_minor : -1;

        create_indir(DATA_AS(node->d_indir->data, i, Block*), p_bound + 1, u_bound);
    }
    
    if (until <= D_INDIR_BLOCK_N) return;

    // 삼중 간접 블럭
    until -= D_INDIR_BLOCK_N;
    prev -= D_INDIR_BLOCK_N;

    if (prev < 0) {
        prev = 0;
        node->t_indir = new_block();
    }

    u_major = until / D_INDIR_BLOCK_N;
    p_major = prev / D_INDIR_BLOCK_N;
    u_minor = until % INDIR_BLOCK_N;
    p_minor = prev % INDIR_BLOCK_N;
    int u_middle = until % D_INDIR_BLOCK_N / INDIR_BLOCK_N;
    int p_middle = prev % D_INDIR_BLOCK_N / INDIR_BLOCK_N;
    
    create_indir(node->t_indir, p_major + 1, u_major);

    for (int i = p_major; i <= u_major; i ++)
    {
        int u_bound = (i == u_major) ? u_middle : (INDIR_BLOCK_N - 1);
        int p_bound = (i == p_major) ? p_middle : 0;

        create_indir(DATA_AS(node->t_indir->data, i, Block*), p_bound + (i == p_major), u_bound);

        for (int j = p_bound; j < u_bound; j ++)
        {
            int u_bound = (i == u_major && j == u_middle) ? u_minor : (INDIR_BLOCK_N - 1);
            int p_bound = (i == p_major && j == p_middle) ? p_minor : -1;

            create_indir(DATA_AS(DATA_AS(
                        node->t_indir->data, i, Block*)
                        ->data, j, Block*), p_bound + 1, u_bound);
        }
    }
}

static Block* get_block (Inode *node, int idx, char create)
{
    if (idx > INDIR_BOUND + INDIR_BLOCK_N + D_INDIR_BLOCK_N + T_INDIR_BLOCK_N)
        return NULL;

    if (node->block_cnt < idx) {
        if (create) create_block(node, idx);
        else return NULL;
    }
    
    // 직접 참조되는 블럭들
    if (idx <= INDIR_BOUND)
        return node->block + idx;

    // 단일 간접 블럭
    idx -= INDIR_BOUND;
    
    if (idx <= INDIR_BLOCK_N)
        return DATA_AS(node->indir->data, idx, Block*);
    
    // 이중 간접 블럭
    idx -= INDIR_BLOCK_N;
    
    if (idx <= D_INDIR_BLOCK_N) {
        int major = idx / INDIR_BLOCK_N;
        int minor = idx % INDIR_BLOCK_N;
        
        return DATA_AS(DATA_AS(
                        node->d_indir->data, major, Block*)
                        ->data, minor, Block*);
    }
    

    // 삼중 간접 블럭
    idx -= D_INDIR_BLOCK_N;

    int major  = idx / D_INDIR_BLOCK_N;
    int middle = idx % D_INDIR_BLOCK_N / INDIR_BLOCK_N;
    int minor  = idx % INDIR_BLOCK_N;
    
    return DATA_AS(DATA_AS(DATA_AS(
                    node->t_indir->data, major, Block*)
                    ->data, middle, Block*)
                    ->data, minor, Block*);
    
}

int read_node (Inode *node, char *buffer, off_t len, off_t from)
{
    // 권한 확인
    if ((IS_OWNER(node) && node->mode & S_IRUSR) ||
        (IS_GROUP(node) && node->mode & S_IRGRP) ||
        (                  node->mode & S_IROTH)   ) return -EACCES;

    if (from >= node->size) return 0;

    // 마지막 데이터 위치
    off_t end = from + len - 1;
    if (end >= node->size)
    {
        end = node->size - 1;
        len = end - from + 1;
    }

    // 읽을 데이터가 있는 첫/마지막 블럭
    int blk_begin = from / BLOCK_SIZE,
        blk_end   = end / BLOCK_SIZE;
    
    from %= BLOCK_SIZE;
    end %= BLOCK_SIZE;
    
    // 한 블럭에 데이터가 있는 경우
    if (blk_begin == blk_end)
    {
        Block* blk = get_block(node, blk_begin, 0);
        if (blk == NULL) return 0;

        memmove(buffer, blk->data + from, len);
        return len;
    }

    // 지금까지 읽은 데이터 양
    int offset = 0;

    // 첫 블럭에서 읽기
    Block* blk = get_block(node, blk_begin, 0);
    if (blk == NULL) return 0;
    
    memmove(buffer, blk->data + from, BLOCK_SIZE - from);
    offset += BLOCK_SIZE - from;

    // 중간 블럭에서 읽기
    for (int blk_n = blk_begin + 1; blk_n < blk_end; blk_n ++)
    {
        blk = get_block(node, blk_n, 0);
        if (blk == NULL) return offset;
        memmove(buffer + offset, blk->data, BLOCK_SIZE);
        offset += BLOCK_SIZE;
    }

    // 마지막 블럭에서 읽기
    blk = get_block(node, blk_end, 0);
    if (blk == NULL) return offset;
    
    memmove(buffer + offset, blk->data + from, end + 1);
    offset += end + 1;

    return offset;
}

int write_node (Inode *node, char *buffer, off_t len, off_t from)
{
    // 권한 확인
    if ((IS_OWNER(node) && node->mode & S_IWUSR) ||
        (IS_GROUP(node) && node->mode & S_IWGRP) ||
        (                  node->mode & S_IWOTH)   ) return -EACCES;

    // 마지막 데이터 위치
    off_t end = from + len - 1;

    // 데이터를 쓸 첫/마지막 블럭
    int blk_begin = from / BLOCK_SIZE,
        blk_end   = end / BLOCK_SIZE;
    
    from %= BLOCK_SIZE;
    end %= BLOCK_SIZE;
    
    // 한 블럭에만 써도 되는 경우
    if (blk_begin == blk_end)
    {
        Block* blk = get_block(node, blk_begin, 1);
        if (blk == NULL) return -EFBIG;

        memmove(blk->data + from, buffer, len);
        node->c_time = node->m_time = time(NULL);
        return len;
    }

    // 지금까지 쓴 데이터 양
    int offset = 0;

    // 첫 블럭에 쓰기
    Block* blk = get_block(node, blk_begin, 0);
    if (blk == NULL) return -EFBIG;
    
    memmove(blk->data + from, buffer, BLOCK_SIZE - from);
    node->c_time = node->m_time = time(NULL);
    offset += BLOCK_SIZE - from;

    // 중간 블럭에 쓰기
    for (int blk_n = blk_begin + 1; blk_n < blk_end; blk_n ++)
    {
        blk = get_block(node, blk_n, 0);
        if (blk == NULL) return -EFBIG;
        memmove(blk->data, buffer + offset, BLOCK_SIZE);
        offset += BLOCK_SIZE;
    }

    // 마지막 블럭에 쓰기
    blk = get_block(node, blk_end, 0);
    if (blk == NULL) return -EFBIG;
    
    memmove(blk->data + from, buffer + offset, end + 1);
    offset += end + 1;

    return offset;
}