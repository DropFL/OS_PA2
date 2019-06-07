# add all files under fuse_impl
file(GLOB fuse_impls "fuse_impl/*.c")

add_executable(main main.c global.c ${fuse_impls})
add_executable(test test_main.c global.c ${fuse_impls})