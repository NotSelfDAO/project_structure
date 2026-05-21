cmake_minimum_required(VERSION 3.25.0)

set(project_name backend)

project(${project_name} VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

# 构建程序
add_executable(${project_name} main.cpp)

target_include_directories(${project_name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 加载第三方库处理函数
include(cmake/ThirdPartyHandler.cmake)
include(FetchContent)

string(MAKE_C_IDENTIFIER "${CMAKE_GENERATOR}" _gen)
set(_deps_root "${CMAKE_SOURCE_DIR}/third_party/")
set(_deps_key "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${_gen}")

set(FETCHCONTENT_BASE_DIR "${_deps_root}/${_deps_key}")

# 自定义封装函数添加
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# 加Lua支持
# Lua generator
find_program(LUA_EXECUTABLE NAMES lua lua54 REQUIRED)

if(NOT LUA_EXECUTABLE)
    message(FATAL_ERROR "Lua interpreter is required but not found in your system!")
endif()
message(STATUS "[CMake] Triggering Lua to generate build infrastructure...")

# 设置生成的临时文件存放路径（推荐放在二进制编译目录，保持源码目录干净）
set(GENERATED_CMAKE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generated_third_party_config.cmake")

execute_process(
    COMMAND "${LUA_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/running_generate/generate_third_party_config.lua" "${GENERATED_CMAKE_FILE}"
    RESULT_VARIABLE LUA_RETURN_CODE
)

if(NOT LUA_RETURN_CODE EQUAL 0)
    message(FATAL_ERROR
        "[CMake] Lua code generation failed!"
    )
endif()

# 无缝引入 Lua 刚刚为我们写好的高级原生目标
if(EXISTS "${GENERATED_CMAKE_FILE}")
    message(STATUS "[CMake] Including dynamically generated targets...")
    include(${GENERATED_CMAKE_FILE})
else()
    message(FATAL_ERROR "[CMake] Expected generated file missing: ${GENERATED_CMAKE_FILE}")
endif()

message(STATUS "[CMake] Infrastructure configured successfully. Ready to build.")

add_subdirectory(core)

# 你要添加的自己实现的其他装有子模块的子目录
# add_subdirectory()