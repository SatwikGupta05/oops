cmake_minimum_required(VERSION 3.10)
project(stock_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenSSL REQUIRED)

# Add cpp-httplib and nlohmann-json as external dependencies
include(FetchContent)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.11.3
)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(httplib json)

# Source files
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/httpserver.cpp")

# Create executable
add_executable(stock_server ${SOURCES})

# Include directories
target_include_directories(stock_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${httplib_SOURCE_DIR}
)

# Link libraries
target_link_libraries(stock_server PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
)

# Copy data directory to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})