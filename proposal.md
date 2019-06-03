# PA2 구현 제안서

## Implementation Goal

본 과제에서 목표로 하는 구현 함수 목록은 다음과 같습니다.

- getattr
- (open / read / mk / rm) dir
- mknod
- rename
- symlink (soft) / link (hard)
- unlink / truncate
- chmod / chown
- open / close
- read / write

## File Descriptor & Inode

![file-related structures](https://t1.daumcdn.net/cfile/tistory/2011DC0F49C907714B)

### File Descriptor

open 함수가 반환하는 파일 지시자입니다. 파일 테이블의 인덱스 번호에 대응되며, 배정 정책은 Linux와 마찬가지로 사용 가능한 번호 순서로 차례대로 배정할 예정입니다.

### File Structure

파일 지시자가 참조할 파일 테이블이 포함하는, 파일 자체를 기술하는 구조체입니다.

- flag
  열린 파일의 모드를 표시합니다 (READ. WRITE 등)
- reference counter
  해당 파일이 얼마나 많은 프로세스에서 참조되고 있는지 나타냅니다.
- inode pointer
  해당 파일의 데이터 및 메타데이터를 갖고 있는 Inode에 대한 포인터입니다.
- file cursor (read/write)
  해당 파일에서 다음에 읽거나 쓸 위치를 가리키는 포인터 입니다.

### Pseudo-Inode Structure

본 과제의 Inode Structure는 본 강의 PDF에 기술된 Unix 파일 시스템의 Inode Structure를 따를 것이다.

- flag
  owner, group, world에 대한 읽기/쓰기/실행 권한을 나타내는 속성입니다.
- reference counter
  해당 Inode가 얼마나 많은 파일에서 참조되고 있는지 나타냅니다.
- size
  해당 Inode가 갖고 있는 데이터의 총량입니다.
- uid (user id)
  소유주를 나타내는 ID값입니다.
- gid (group id)
  소유 그룹을 나타내는 ID값입니다.
- mode (directory, file, link, ...)
  해당 Inode 데이터가 기술하는 파일의 종류를 나타냅니다. `mknod`에 의해 지정됩니다.
- block pointer
  데이터 조각인 Block에 대한 포인터입니다.
- block count
  Inode가 참조하고 있는 Block의 개수입니다.

## D-Entry Specification

D-Entry는 파일 및 디렉토리를 감싸는 객체로, 파일 시스템의 구조를 정의합니다. 이 구조체에 의해 다층 디렉토리 구조를 구현할 수 있습니다.

Linux 커널의 [dcache.h](https://elixir.bootlin.com/linux/latest/source/include/linux/dcache.h)에 정의된 `dentry` 구조체를 참고하여 만들 수 있으나, 커널에서는 최적화를 위한 속성도 다수 존재합니다. 따라서 본 과제에서는 최소한의 요소 만을 포함하여 구현하려 합니다.

다음은 필수적으로 포함하고자 하는 일부 속성입니다.

- `d_inode`: 자기 자신을 기술하는 Inode에 대한 참조입니다.
- `d_parent`: 부모 디렉토리에 대한 참조입니다.
- `d_child`: 같은 디렉토리에 속하고 같은 레벨에 있는 항목들에 대한 리스트입니다.
- `d_subdirs`: 해당 디렉토리의 하위 항목들에 대한 리스트입니다. 파일에서는 의미가 없습니다.
- `d_name`: 유저에게 보여지는 해당 디렉토리 또는 파일의 이름입니다. 커널 상에서는 `d_iname`과 구분되어 있으나, 본 과제에서도 구분할지는 아직 결정되지 않았습니다.
