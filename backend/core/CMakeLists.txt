# 依赖处理
find_package(Threads REQUIRED)
# find_package(SQLite3 REQUIRED)

# find_package(protobuf CONFIG QUIET)
# if(NOT protobuf_FOUND)
# 	find_package(Protobuf CONFIG REQUIRED)
# endif()
# find_package(gRPC CONFIG REQUIRED)




file(GLOB_RECURSE BACKEND_SOURCES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/registrar/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/repository/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/route/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/task_manager/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/transport/*.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.cc
)

add_library(
    core STATIC
    ${BACKEND_SOURCES}
)

target_include_directories(
    # backend PRIVATE
    core PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/registrar
	${CMAKE_CURRENT_SOURCE_DIR}/repository
	${CMAKE_CURRENT_SOURCE_DIR}/route
	${CMAKE_CURRENT_SOURCE_DIR}/task_manager
	${CMAKE_CURRENT_SOURCE_DIR}/transport
)

target_link_libraries(
    # backend PRIVATE
    core 
    PRIVATE
	Threads::Threads
	httplib
    nlohmann_json
    # gRPC::grpc++
	# protobuf::libprotobuf
	# SQLite3::SQLite3

)

target_link_libraries(backend PRIVATE core)

# 编译选项设置
set(COMPILE_OPTIONS_TARGETS backend core)

function(set_compile_options target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /utf-8)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()


foreach(target_name IN LISTS COMPILE_OPTIONS_TARGETS)
    set_compile_options(${target_name})
endforeach()