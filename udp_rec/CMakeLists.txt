project(udp_rec)

add_executable(udp_rec)
target_sources(udp_rec
  PRIVATE 
   src/udp_init.h
   src/udp_rec.cpp
   src/udp_receiver.cpp
   src/udp_receiver.h
   src/ecal_receiver.h
   src/ecal_win_socket.h
   src/hrtimer.h
   src/udp_init.cpp
   src/io/udp_receiver_asio.cpp
)

target_include_directories(udp_rec PRIVATE 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/io>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(udp_rec PRIVATE asio::asio)
target_compile_definitions(udp_rec PRIVATE _WIN32_WINNT=0x0601)