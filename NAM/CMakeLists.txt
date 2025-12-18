file(GLOB_RECURSE NAM_SOURCES *.cpp)

add_library(nam_core STATIC ${NAM_SOURCES})
target_include_directories(nam_core PRIVATE "${NAM_DEPS_PATH}/nlohmann")
target_link_libraries(nam_core PRIVATE eigen)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_include_directories(nam_core SYSTEM PRIVATE /usr/local/include)
    target_compile_options(nam_core PRIVATE -stdlib=libc++)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(nam_core PRIVATE stdc++fs)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_compile_definitions(nam_core PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()
