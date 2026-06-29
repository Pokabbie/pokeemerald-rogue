cmake_minimum_required(VERSION 3.18)

include(FetchContent)

FetchContent_Declare(stb DOWNLOAD_EXTRACT_TIMESTAMP ON
    GIT_REPOSITORY "https://github.com/nothings/stb.git"
    GIT_TAG "master"
    UPDATE_COMMAND "${CMAKE_COMMAND}" -E copy
        <SOURCE_DIR>/stb_image.h
        <SOURCE_DIR>/stb_image_resize.h
        <SOURCE_DIR>/stb_image_write.h
        <BINARY_DIR>
)
FetchContent_Declare(ctopt DOWNLOAD_EXTRACT_TIMESTAMP ON
    GIT_REPOSITORY "https://github.com/felixjones/ctopt.git"
    GIT_TAG "main"
    UPDATE_COMMAND "${CMAKE_COMMAND}" -E copy <SOURCE_DIR>/ctopt.hpp <BINARY_DIR>
)
FetchContent_Declare(fmt DOWNLOAD_EXTRACT_TIMESTAMP ON
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG "9.1.0"
)
FetchContent_Declare(exprtk DOWNLOAD_EXTRACT_TIMESTAMP ON
    GIT_REPOSITORY "https://github.com/ArashPartow/exprtk.git"
    GIT_TAG "master"
    UPDATE_COMMAND "${CMAKE_COMMAND}" -E copy <SOURCE_DIR>/exprtk.hpp <BINARY_DIR>
)

project(gfx2agb LANGUAGES CXX VERSION 0.0.0)

FetchContent_MakeAvailable(stb ctopt exprtk)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()

add_executable(gfx2agb
    source/main.cpp
    source/bitmap.cpp
    source/color_format.cpp
    source/image_io.cpp
    source/palette.cpp
    source/util.cpp
)
set_target_properties(gfx2agb PROPERTIES CXX_STANDARD 20)

target_include_directories(gfx2agb PRIVATE include
    ${ctopt_BINARY_DIR}
    ${exprtk_BINARY_DIR}
    ${stb_BINARY_DIR}
)

target_link_libraries(gfx2agb PRIVATE fmt)
target_compile_definitions(gfx2agb PRIVATE GFX2AGB_VERSION_MAJOR=${PROJECT_VERSION_MAJOR} GFX2AGB_VERSION_MINOR=${PROJECT_VERSION_MINOR} GFX2AGB_VERSION_PATCH=${PROJECT_VERSION_PATCH})

if(MSVC)
    set_source_files_properties(source/util.cpp PROPERTIES COMPILE_OPTIONS "/bigobj")
else()
    set_source_files_properties(source/util.cpp PROPERTIES COMPILE_OPTIONS "-Wa,-mbig-obj")
endif()

install(TARGETS gfx2agb DESTINATION bin)
