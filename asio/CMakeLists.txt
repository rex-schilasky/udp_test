project(asio)

find_package(Threads REQUIRED)

add_library(asio INTERFACE)
add_library(asio::asio ALIAS asio)
target_include_directories(asio INTERFACE 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
)

target_link_libraries(asio INTERFACE Threads::Threads)