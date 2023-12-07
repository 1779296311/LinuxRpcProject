

add_library(
    config
    config.cpp
    # config_develop.cpp
)

target_include_directories(
    config
    INTERFACE 
    ./
)

target_link_libraries(
    config
    yaml-cpp
    pthread
)