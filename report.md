# PA2 구현 보고서

## 들어가기에 앞서

본 과제에서 구현하고자 하는 것은 In-memory **FUSE** (**F**ilesystem in **USE**rspace)입니다.

`FUSE`는 커널이 아닌 유저 레벨에서 파일 시스템을 구현할 수 있게 만드는 하나의 커널 모듈이자 라이브러리입니다. 사용자는 `FUSE`를 이용하여 특정 위치에서 기존의 파일 시스템이 아닌 사용자가 직접 정의한 파일 시스템이 동작하도록 할 수 있습니다.

![how fuse work](https://upload.wikimedia.org/wikipedia/commons/0/08/FUSE_structure.svg)

위 그림과 같이 각각의 파일 시스템들은 **VFS** (**V**irtual **F**ile **S**ystem)를 통해 Userspace와 상호작용합니다. `VFS`를 통해 다양한 파일 시스템들은 일관된 형태로 동작하게 됩니다. 

사용자가 `VFS` 스펙에 맞추어 파일 구조와 함수를 정의하고 `VFS`에 등록하면 사용자가 원하는 지점에서 `FUSE`를 기존의 파일 시스템 대신 사용할 수 있습니다.

`In-memory`라 함은 실제 물리적 저장장치를 기반으로 동작하는 것이 아니라 메모리 상에서 가상의 형태로 존재하고 동작하는 것을 의미합니다. 즉 하드디스크에 대한 I/O 없이 메모리에서만 파일에 대한 데이터를 읽고 쓰며 파일 시스템의 모든 동작을 시뮬레이팅합니다. 

파일을 저장하는 HDD등의 기억장치는 블럭으로 나뉘어 관리되고 이 블럭들에 파일의 데이터들이 저장됩니다. Unix / Linux 시스템에서 파일들은 블럭에 나뉘어 저장되고 파일이 저장된 블럭의 위치, 파일의 크기등을 포함한 다른 여러 메타데이터들은 inode라 불리우는 객체에 저장되어 이용됩니다. 본 과제는 In-memory system이지만 가상으로 메모리상에 블럭을 나누고 inode와 비슷한 역할을 하는 객체를 통해 Unix / Linux 시스템과 유사하게 동작하도록 하였습니다.

본 과제의 inode 객체는 12개의 direct block pointer와 1개의 single, double, triple indirect block을 갖고 있습니다. 블록 사이즈는 일반적으로 4KB이므로 저희도 그에 맞춰 4KB로 정하였습니다. 다만 블럭이 메모리 상에 있으므로 블럭에 대한 포인터는 메모리에 대한 포인터(메모리 주소)가 됩니다. 64-bit 시스템 기준으로 메모리 주소는 8바이트이므로 한 블럭에 저장 할 수 있는 포인터의 주소는 총 4KB / 8B = 512 개가 됩니다.

그러므로 본 과제에서 한 파일에 저장 할 수 있는 데이터의 최대 크기는 12 * 4KB + 512 * 4KB + 512<sup>2</sup> * 4KB + 512<sup>3</sup> * 4KB = 48 KB + 2MB + 1GB + 0.5TB 가 됩니다. 


## Implementation

다음은 `In-memory FUSE` 구현을 위한 구조체와 함수에 대한 설명입니다.

### Structure

다음은 본 과제 구현에 사용돤 구조체에 대한 설명입니다.

#### inode structure

inode가 갖고 있는 속성에 대한 설명입니다.

- `mode` inode에 대한 접근 및 실행 관련 설정이 담긴 16비트 정수입니다.
- `uid` inode의 소유자에 대한 식별자입니다.
- `gid` inode의 소유 그룹에 대한 식별자입니다.
- `size` inode가 표현하는 파일의 총 크기입니다.
- `link_cnt` inode를 참조하는 file 의 개수입니다.
- `block_cnt` inode에서 사용하는 블럭의 개수입니다.
- `a_time` inode에 마지막으로 접근한 시간입니다. open()에 의해서만 갱신됩니다.
- `m_time` inode가 참조하는 블럭을 마지막으로 수정한 시간입니다. 파일의 메타데이터를 수정한 경우 이 속성은 갱신되지 않습니다.
- `c_time` 이 inode의 내용을 마지막으로 수정한 시간입니다. 파일의 메타데이터를 수정한 경우 이 속성도 업데이트됩니다.
- `block[N_BLOCKS]` inode에 속한 블럭의 목록입니다.
- `indir` 단일 간접 블럭입니다.
- `d_indir` 이중 간접 블럭입니다.
- `t_indir` 삼중 간접 블럭입니다.

#### file structure
  
- `mode` 파일에 대한 읽기/쓰기 권한입니다.
- `ref_cnt` 파일을 참조하고 있는 File Descriptor의 개수입니다.
- `inode` 파일의 정보를 담는 Inode 객체입니다.
- `r_cur, w_cur` 파일을 읽는/쓰는 위치입니다. 여기서는 읽는 위치와 쓰는 위치를 별개로 관리합니다.

#### d_entry structure

디렉토리에 대한 정보를 갖고 있는 구조체입니다. 트리 형태를 하고 있으며 내부에 연결 리스트를 위한 포인터를 갖고 있습니다.

- `inode` d_entry가 가리키는 파일에 대한 Inode 객체입니다.
- `name[NAME_LEN+1]` 사용자에게 보여지고 경로로 취급되는 파일의 이름입니다.
- `parent` 파일(또는 디렉토리)의 상위 디렉토리에 대한 참조입니다.       
- `child` 디렉토리의 하위 첫번째 항목에 대한 참조입니다. 파일의 경우 NULL 로 지정됩니다.        
- `sibling` 같은 디렉토리에 속한 다른 항목에 대한 참조입니다. 단일 연결 리스트이며, 모든 항목을 탐색한 경우 이 값은 NULL 입니다.

### Function

다음은 본 과제에서 구현한 함수들에 대한 설명입니다.

```c
int open(const char *path, struct fuse_file_info *info)

- path에 해당하는 파일을 엽니다.
- 성공할 경우 0을 반환하며 실패할 경우 상황에 맞는 에러코드를 반환합니다.
- 파일이 없는경우 ENOENT, 파일을 열 수 없는 경우 ENFILE을 반환합니다.
- info의 값으로 file table에 file의 mode를 설정합니다.
```

```c
int opendir (const char *path, struct fuse_file_info *info)

- path에 해당하는 디렉토리를 엽니다.
- open과 유사하게 동작합니다.
```

```c
int read (const char *path, char *buffer, 
            size_t size, off_t off, struct fuse_file_info *info)

- path의 파일을 buffer에 읽어옵니다.
- 파일이 읽기 모드가 아닐경우 EINVAL을 반환합니다.
- 파일에 대한 권한이 없을경우 EACCES를 반환합니다.
- 파일 읽기에 성공했을 경우 읽은 크기를 반환합니다.
- read_node 함수를 이용해 데이터를 읽어옵니다.
```

```c
int readdir (const char *path, void *buffer, fuse_fill_dir_t filler, 
                off_t offset, struct fuse_file_info *info)

- path의 디렉토리의 정보를 buffer에 읽어옵니다.
- 성공한 경우 0을, 실패한 경우 EBADFD를 반환합니다.
```

```c
int write (const char *path, const char *buffer, size_t size, 
            off_t off, struct fuse_file_info *info)

- buffer의 내용을 path에 있는 파일에 씁니다.
- 파일이 쓰기 모드가 아닐경우 EINVAL을 반환합니다.
- 파일 쓰기에 성공할 경우 쓴 데이터의 크기를 반환합니다.
- write_node 함수를 이용해 블럭에 데이터를 씁니다.
```

```c
int mkdir (const char *path, mode_t mode)

- 디렉토리를 생성합니다.
- 이미 디렉토리가 존재할 경우 EEXIST를 반환합니다.
- inode와 d_entry를 새로 생성합니다.
```

```c
int mknod (const char *path, mode_t mode, dev_t dev)

- 파일을 생성합니다. 
- 이미 파일이 존재할 경우 EEXIST를 반환합니다.
- inode와 d_entry를 새로 생성합니다.
```

```c
int chmod (const char *path, mode_t mode)

- path의 항목의 권한을 바꿉니다.
- 해당 파일의 소유자가 아닐 경우 EPERM를 반환합니다.
- inode에 기록된 mode 값을 수정하고 c_time을 갱신합니다.
```

```c
int chown (const char *path, uid_t owner, gid_t group)

- path의 항목의 소유자를 바꿉니다.
- 해당 파일의 소유자가 아닐 경우 EPERM를 반환합니다.
- inode에 기록된 uid, gid 값을 수정하고 c_time을 갱신합니다.
```


```c
int find_dir (const char *path, DirEntry **sav)

- sav에 path의 DirEntry(d_entry) 값을 저장합니다.
```


```c
void create_block (Inode *node, int until)

- indirect block을 inode에 등록합니다.
```

```c
Block* get_block (Inode *node, int idx, char create)

- inode에서 idx번째 참조 블럭이 가리키는 블럭을 가져옵니다.
- create일 경우 블럭을 생성하고 idx 번째에 링크합니다. 
```


```c
int read_node (Inode *node, char *buffer, off_t len, off_t from)

- inode가 참조하는 블럭의 데이터를 buffer에 읽어옵니다.
- 사용자의 권한이 없을 경우 EACCES를 반환합니다.
- 데이터를 읽어오는데 성공하면 읽어온 데이터의 크기를 반환합니다.

```

```c
int write_node (Inode *node, const char *buffer, off_t len, off_t from)

- inode가 참조하는 블럭에 buffer의 데이터를 씁니다.
- 사용자의 권한이 없을 경우 EACCES를 반환합니다.
- 데이터를 쓰는데 성공하면 쓴 데이터의 크기를 반환합니다.
```

```c
int get_fd ()

- 사용 가능한 fd 값을 반환합니다.
- 더이상 fd를 할당 할 수 없을 경우 -1을 반환합니다.
```
