cmake_minimum_required(VERSION 3.16)
project(HTTPServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Add executable
add_executable(http_server 
    src/main.cpp
    src/http_server.cpp
    src/http_request.cpp
    src/http_response.cpp
    src/route_handler.cpp
)

# Include directories
target_include_directories(http_server PRIVATE include)

# Link libraries
target_link_libraries(http_server PRIVATE Threads::Threads)

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(http_server PRIVATE -Wall -Wextra -O2)
endif()

# Install target
install(TARGETS http_server DESTINATION bin) 