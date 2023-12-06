add_library(
    log 
    log.cpp
)

target_include_directories(
    log 
    INTERFACE
    ./
)

target_link_libraries(
    log 
    singleton
)