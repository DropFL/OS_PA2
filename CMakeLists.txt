cmake_minimum_required(VERSION 2.8)

# define macros
add_definitions(
    -D_FILE_OFFSET_BITS=64  # define file offset bits as 64 (used in fuse)
)

# libraries to link
link_libraries(
    fuse
)

# add all *.c files under fuse_impl
file(GLOB fuse_impls "fuse_impl/*.c")

add_executable(
    main            # target 1: main
    #################
    main.c
    global.c
    ${fuse_impls}
)