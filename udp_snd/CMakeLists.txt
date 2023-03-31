project(udp_snd)

add_executable(udp_snd)
target_sources(udp_snd
  PRIVATE 
    src/udp_init.cpp
    src/udp_init.h
    src/udp_sender.cpp
    src/udp_sender.h
    src/udp_snd.cpp
    src/ecal_sender.h
    src/ecal_win_socket.h
    src/hrtimer.h
)

target_link_libraries(udp_snd PRIVATE asio::asio)
target_compile_definitions(udp_snd PRIVATE _WIN32_WINNT=0x0601)