# CMakeList.txt: myping 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(proxy_name "version")
set(def_file "${PROJECT_SOURCE_DIR}/third/dll_proxy/${proxy_name}.def")

# 文件写出位置
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/BinOutput/${CMAKE_BUILD_TYPE}/${ABI}")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/BinOutput/${CMAKE_BUILD_TYPE}/${ABI}")

# 添加头文件
include_directories("${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}")
include_directories("${PROJECT_SOURCE_DIR}/third/minhook/include")

# 将源代码添加到此项目的可执行文件。
aux_source_directory(. program_src)
aux_source_directory(utils utils_src)
aux_source_directory("${PROJECT_SOURCE_DIR}/third/minhook/src" hook_src)
aux_source_directory("${PROJECT_SOURCE_DIR}/third/minhook/src/hde" hde_src)

add_library(ChromiumCtxMenuMod SHARED
    ${program_src}
    ${utils_src}
    ${hook_src}
    ${hde_src}
    ${def_file}
)
