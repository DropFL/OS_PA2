# PA2 구현 제안서

## Function Description
- getattr
- (open / read / mk / rm) dir
- mknod
- rename 
- symlink (soft) / link (hard) / unlink
- chmod / chown
- open / close 
- read / write / truncate 

## File Descriptor & Inode
File Descriptor
- index of file table 

File Structure
- flag
- reference counter
- inode pointer 
- file cursor 

Inode Structure
- flag
- reference counter 
- size 
- uid (user id)
- gid (group id)
- mode (directory, file, link, ...)
- block pointer
- block count

## Miscellaneous

multi-level directory implementation <br>
appropriate error code

