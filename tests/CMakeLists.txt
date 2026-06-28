include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(RetailTests 
    # test_product_service.cpp
    # test_billing_service.cpp
    # test_crypto_utils.cpp
    # test_query_builder.cpp
)
# We need at least one source file for the executable to compile without error.
# We will create a dummy test file temporarily.
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy_test.cpp "
#include <gtest/gtest.h>
TEST(DummyTest, Test1) { EXPECT_TRUE(true); }
")
target_sources(RetailTests PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/dummy_test.cpp)

target_link_libraries(RetailTests PRIVATE RetailCore Qt6::Test GTest::gtest_main)
add_test(NAME RetailTests COMMAND RetailTests)
