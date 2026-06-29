cmake_minimum_required(VERSION 3.25)

project(RetailManagementSystem VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt settings
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 COMPONENTS Core Gui Widgets Sql PrintSupport Charts Test REQUIRED)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

include(FetchContent)

# SQLite3
FetchContent_Declare(
    sqlite3
    URL https://sqlite.org/2023/sqlite-amalgamation-3430200.zip
)
FetchContent_GetProperties(sqlite3)
if(NOT sqlite3_POPULATED)
    FetchContent_Populate(sqlite3)
    add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
    target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
endif()

# ZXing-C++
FetchContent_Declare(
    ZXing
    GIT_REPOSITORY https://github.com/zxing-cpp/zxing-cpp.git
    GIT_TAG        v2.2.1
)
FetchContent_MakeAvailable(ZXing)

# Collect source files
file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "src/*.h"
    "src/*.ui"
)

# Remove main.cpp from SRC_FILES to create a library for testing if needed
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Core Library
add_library(RetailCore STATIC ${SRC_FILES})
target_include_directories(RetailCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(RetailCore PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
    Qt6::Charts
    OpenSSL::SSL
    OpenSSL::Crypto
    sqlite3
    ZXing::ZXing
)

# Executable
add_executable(RetailManagementSystem src/main.cpp)
target_link_libraries(RetailManagementSystem PRIVATE RetailCore)

# Resources
# qt6_add_resources(RetailManagementSystem "resources/resources.qrc") # Un-comment when resources are added

# Tests
enable_testing()
add_subdirectory(tests)
# Target sources block removed as it was redundant and pointed to non-existent target RetailMS

