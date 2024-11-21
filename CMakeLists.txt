project(AlsaCPPTest)

add_executable(alsa_test save_wav.cpp ALSADevices.cpp)

target_include_directories(alsa_test PRIVATE /usr/include)

target_link_libraries(alsa_test PRIVATE asound)