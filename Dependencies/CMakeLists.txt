set(BUILD_DOC OFF)
set(BUILD_TESTING OFF)
set(BUILD_EXAMPLES OFF)
add_subdirectory(eigen)

set(CATCH_BUILD_BENCHMARKS ON)
add_subdirectory(Catch2)
