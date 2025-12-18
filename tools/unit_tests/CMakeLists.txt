add_executable(unit_tests activation_tests.cpp)
target_link_libraries(unit_tests PRIVATE nam_core Catch2::Catch2WithMain)
