/**
 * @file global.h
 * @author DropFL
 * @brief   이 파일 시스템의 전역헤더입니다.
 *          파일 시스템 전반적으로 사용될 항목들이 선언되어 있습니다.
 * 
 * @date 2019-06-07
 * 
 * @copyright Copyright (c) 2019 DropFL, bjh9750
 */

#define FUSE_USE_VERSION 26     // FUSE 버전 명시

#include <fuse.h>               // apt로 libfuse-dev을 설치하십시오.

/**
 * TODO: define required structures
 */

/* ==================== Functions Types ==================== */

typedef int (*getattr_type) (const char *path,
                             struct stat *st);

typedef int (*readlink_type)(const char *path,
                             char *buffer,
                             size_t size);

typedef int (*mknod_type)   (const char *path,
                             mode_t mode,
                             dev_t dev);

typedef int (*mkdir_type)   (const char *path,
                             mode_t mode);

typedef int (*unlink_type)  (const char *path);

typedef int (*rmdir_type)   (const char *path);

typedef int (*symlink_type) (const char *target,
                             const char *path);

typedef int (*rename_type)  (const char *oldpath,
                             const char *newpath);

typedef int (*link_type)    (const char *target,
                             const char *path);

typedef int (*chmod_type)   (const char *path,
                             mode_t mode);

typedef int (*chown_type)   (const char *path,
                             uid_t owner,
                             gid_t group);

typedef int (*truncate_type)(const char *path,
                             off_t length);

typedef int (*open_type)    (const char *path,
                             struct fuse_file_info *info);

typedef int (*read_type)    (const char *path,
                             char *buffer,
                             size_t size,
                             off_t offset,
                             struct fuse_file_info *info);

typedef int (*write_type)   (const char *path,
                             const char *buffer,
                             size_t size,
                             off_t offset,
                             struct fuse_file_info *info);

typedef int (*release_type) (const char *path,
                             struct fuse_file_info *info);

typedef int (*opendir_type) (const char *path,
                             struct fuse_file_info *info);

typedef int (*readdir_type) (const char *path,
                             void* buffer,
                             fuse_fill_dir_t filler,
                             off_t offset,
                             struct fuse_file_info *info);

typedef int (*releasedir_type)(const char *path,
                             struct fuse_file_info *info);

typedef void* (*init_type)  (struct fuse_conn_info *info);

typedef void (*destroy_type)(void *data);

/* ==================== Functions Defined by FUSE ==================== */

/**
 * @brief   파일의 정보를 불러오는 함수입니다.
 *          @c stat() 시스템 콜과 차이점은 다음과 같습니다.
 * 
 *          * @c st_dev 와 @c st_blksize 속성은 무시됩니다.
 *          * @c st_ino 속성은 마운트 시 @c use_ino 설정이 주어진 경우에만 사용됩니다.
 * 
 * @param   path    읽고자 하는 파일의 경로입니다.
 * @param   st      파일의 정보를 담을 @c stat 구조체입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern getattr_type     my_getattr;

/**
 * @brief   심볼릭 링크가 가리키는 파일에 대한 경로를 읽어옵니다.
 * 
 * @param   path    해당 심볼릭 링크의 경로입니다.
 * @param   buffer  링크된 파일의 경로를 담을 버퍼입니다.
 * @param   size    버퍼에 들어갈 데이터의 최대 길이입니다.
 *                  이는 마지막 널 문자까지 포함하므로 주의하십시오.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern readlink_type    my_readlink;

/**
 * @brief   파일 노드를 생성하는 함수입니다.
 *          디렉토리와 심볼릭 링크를 제외한 모든 파일을 생성하는 데 호출됩니다.
 * 
 * @param   path    생성할 파일의 경로입니다. 파일의 위치 및 이름을 정의합니다.
 * @param   mode    생성할 파일에 대한 옵션입니다.
 *                  자세한 정보는 @c inode(7) 을 참조하십시오.
 *                  http://man7.org/linux/man-pages/man7/inode.7.html
 * @param   dev     캐릭터 장치 또는 블럭 파일을 생성하는 경우에 사용됩니다.
 *                  장치의 메이저/마이너 넘버를 조합한 값입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern mknod_type       my_mknod;

/**
 * @brief   디렉토리를 생성하는 함수입니다.
 * 
 * @param   path    만들고자 하는 디렉토리의 경로입니다.
 * @param   mode    디렉토리의 권한을 지정하는 플래그입니다.
 *                  일반적인 9비트의 권한 코드와 sticky bit를 포함합니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */

extern mkdir_type       my_mkdir;

/**
 * @brief   지정된 파일을 삭제합니다.
 * 
 * @param   path    지우고자 하는 파일의 경로입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern unlink_type      my_unlink;

/**
 * @brief   지정된 디렉토리를 삭제합니다.
 * 
 * @param   path    지우고자 하는 디렉토리의 경로입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern rmdir_type       my_rmdir;

/**
 * @brief   심볼릭 링크를 생성합니다.
 * 
 * @param   target  심볼릭 링크가 가리킬 파일에 대한 경로입니다.
 * @param   path    생성할 심볼릭 링크의 경로입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern symlink_type     my_symlink;

/**
 * @brief   지정된 파일의 이름을 변경합니다.
 * 
 * @param   oldpath 이름 변경 대상 파일의 경로입니다.
 * @param   newpath 새로 지정할 파일의 이름입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern rename_type      my_rename;

/**
 * @brief   특정 파일에 대한 하드 링크를 생성합니다.
 * 
 * @param   target  하드 링크가 가리킬 대상 파일의 경로입니다.
 * @param   path    하드 링크를 생성할 위치입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern link_type        my_link;

/**
 * @brief   특정 파일의 권한 비트를 수정합니다.
 * 
 * @param   path    권한을 수정할 대상 파일입니다.
 *                  ? 심볼릭 링크의 경우 가리키는 파일이 해당됩니다.
 * @param   mode    덮어씌울 권한 비트입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern chmod_type       my_chmod;

/**
 * @brief   특정 파일의 소유주 및 소유 그룹을 수정합니다.
 * 
 * @param   path    소유권을 수정할 대상 파일입니다.
 *                  ? 심볼릭 링크의 경우 가리키는 파일이 해당됩니다.
 * @param   owner   소유주의 uid 값입니다.
 * @param   group   소유 그룹의 gid 값입니다.
 * 
 * @note    id 값이 -1로 채워진 항목에 대해서는 수정하지 않습니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern chown_type       my_chown;

/**
 * @brief   특정 파일의 내용을 지정한 길이로 수정합니다.
 * 
 * @param   path    대상 파일의 경로입니다.
 * @param   length  수정할 파일의 길이입니다.
 *                  파일 길이가 length보다 긴 경우, 내용이 손실됩니다.
 *                  그 반대의 경우, 파일 뒤에 널 문자를 추가하여 길이를 맞춥니다.
 * 
 * @note    길이가 변경된 경우, 파일의 @c ctime 과 @c mtime 이 수정됩니다. 
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0이 반환됩니다.
 */
extern truncate_type    my_truncate;

/**
 * @brief   특정 파일을 여는 작업을 수행합니다.
 * 
 * @param   path    대상 파일의 경로입니다.
 * @param   info    FUSE가 사용할 파일의 정보입니다.
 *                  커널의 @c open() 에 사용된 플래그를 전달하며,
 *                  FUSE에서 파일을 관리하기 위한 속성을 지정할 수도 있습니다.
 * 
 * @note    FUSE에서 다음의 플래그는 전달하지 않습니다.
 *          * O_CREAT, O_EXCL 등 생성 관련 플래그
 *          * O_TRUNC (특정 조건에서만 전달되나, 무시해도 될 듯 합니다.)
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 열린 파일에 해당되는 File Descriptor를 반환합니다.
 *          이 값은 @c info->fh 값으로 지정되어 참조될 수 있습니다.
 */
extern open_type        my_open;

/**
 * @brief   특정 열린 파일을 읽는 작업을 수행합니다.
 * 
 * @param   path    대상 파일의 경로입니다.
 * @param   buffer  파일의 내용을 저장할 버퍼입니다.
 * @param   size    버퍼에 들어갈 데이터의 최대 길이입니다.
 * @param   offset  파일의 처음으로부터 몇 바이트 떨어진 위치부터
 *                  읽어오기 시작할지 지정하는 숫자입니다.
 * @param   info    FUSE가 제공하는 파일의 정보입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 읽어온 데이터의 총 바이트 수를 반환합니다.
 *          이 때, EOF가 포함된 경우 제외합니다.
 */
extern read_type        my_read;

/**
 * @brief   특정 열린 파일에 데이터를 쓰는 작업을 수행합니다.
 * 
 * @param   path    대상 파일의 경로입니다.
 * @param   buffer  파일에 쓸 내용이 담긴 버퍼입니다.
 * @param   size    파일에 쓸 데이터의 총 길이입니다.
 * @param   offset  파일의 처음으로부터 몇 바이트 떨어진 위치부터
 *                  쓰기 시작할지 지정하는 숫자입니다.
 * @param   info    FUSE가 제공하는 파일의 정보입니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 쓰여진 데이터의 총 바이트 수를 반환합니다.
 */
extern write_type       my_write;

/**
 * @brief   더 이상 참조가 존재하지 않는 열린 파일의 후처리를 담당합니다.
 * 
 * @param   path    대상 파일의 경로입니다.
 * @param   info    FUSE가 제공하는 파일의 정보입니다.
 * 
 * @retval  의미가 없습니다. FUSE에서 반환값을 무시합니다.
 */
extern release_type     my_release;

/**
 * @brief   지정한 디렉토리를 여는 함수입니다.
 * 
 * @param   path    열고자 하는 디렉토리의 경로입니다.
 * @param   info    FUSE가 사용할 파일의 정보입니다.
 *                  FUSE에서 파일을 관리하기 위한 속성을 지정할 수도 있습니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우, FUSE에서 반환값을 직접 사용하진 않으나
 *          File Descriptor와 유사한 정수를 반환할 수 있으며,
 *          그 값은 @c open() 의 반환값과 동일하게 취급됩니다.
 */
extern opendir_type     my_opendir;

/**
 * @brief   지정한 디렉토리의 정보를 읽는 함수입니다.
 * 
 * @param   path    읽고자 하는 디렉토리의 경로입니다.
 * @param   buffer  디렉토리의 정보를 담을 버퍼입니다.
 * @param   filler  버퍼에 디렉토리의 정보를 담는 함수입니다.
 *                  자세한 사항은 Note를 참고하십시오.
 * @param   offset  이 함수에서 무시됩니다.
 *                  원래 용도는 @c my_read() 의 인자와 동일합니다.
 * @param   info    FUSE가 제공하는 디렉토리의 정보입니다.
 * 
 * @note    @c filler 함수는 원형이 다음과 같습니다.
 *          @code
 *          filler(buffer, name, stat, offset)
 *          @endcode
 *          이 함수는 @c buffer 에 디렉토리 내 요소의 정보를
 *          @c name 과 @c stat 으로 전달받아 기록합니다.
 *          (@c offset 은 여기에서도 무시합니다.)
 *          
 *          이 함수는 FUSE 내에 구현된, 커널이 요구하는 형태로
 *          디렉토리의 내용을 출력하는 데에 사용합니다.
 * 
 * @retval  오류가 발생한 경우 그에 해당되는 에러 코드가 반환됩니다.
 *          성공한 경우 0을 반환힙니다.
 */
extern readdir_type     my_readdir;

/**
 * @brief   참조가 존재하지 않는 열린 디렉토리의 후처리를 담당합니다.
 * 
 * @param   path    대상 디렉토리의 경로입니다.
 * @param   info    FUSE가 제공하는 디렉토리의 정보입니다.
 * 
 * @retval  의미가 없습니다. FUSE에서 반환값을 무시합니다.
 */
extern releasedir_type  my_releasedir;

/**
 * @brief   초기화 함수입니다. 마운트 직후 실행됩니다.
 * 
 * @param   conn    현재 파일 시스템에 대한 메타 데이터입니다.
 *                  일부 속성은 이 함수에서 설정할 수 있으며, 그 예시는 다음과 같습니다.
 * 
 *                  * 한 번에 읽을/쓸 수 있는 바이트 수
 *                  * 최대 백그라운드 I/O 요청 횟수
 *                  * 커널에 의해 결정된 / 커널에 요청하려는 플래그
 * 
 *                  위 항목을 설정하지 않아도 FUSE의 기본 설정 값으로 지정됩니다.
 * 
 * @retval  @c fuse_context의 @c private_data에 대입될 void형 포인터입니다.
 *          이 값은 @c fuse_get_context()->private_data 로 참조할 수 있습니다.
 *          필수적인 속성은 아니므로, 필요치 않다면 @c NULL을 반환하십시오.
 * 
 * @note    이 함수 안에서도 @c fuse_get_context->private_data 에 접근할 수 있고,
 *          그 값은 main에서 fuse_main의 4번째 인자인 @c user_data 입니다.
 *          즉, @c main 함수에서 @c my_init 으로 값을 전달할 수 있습니다.
 */
extern init_type        my_init;

/**
 * @brief   후처리 함수입니다. 언마운트 직전 실행됩니다.
 * 
 * @param   data    @c my_init() 에서 반환된 값입니다.
 */
extern destroy_type     my_destroy;