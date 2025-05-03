enable_testing()
find_package(GTest QUIET)

if (GTest_FOUND)
    message(STATUS "Tests enabled")
else()
    message(NOTICE "GTest package not found, continuing with tests disabled")
    message(STATUS "Tests disabled")
    return()
endif()

add_executable(gtest
  gtest.cc
  TSThelpers.cc
  structs/TSTnode_t.cc
  structs/TSTopen_t.cc
  ../domains/n-pancake/npancake_t.cc
  domains/TSTnpancake.cc
  # ../domains/n-puzzle/npuzzle_t.cc
  # domains/TSTnpuzzle.cc
  # structs/TSTclosed_t.cc
)

target_link_libraries(gtest LINK_PUBLIC pdb GTest::gtest GTest::gtest_main)

add_test(AllTests gtest)
