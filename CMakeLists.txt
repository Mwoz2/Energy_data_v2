cmake_minimum_required(VERSION 3.10)
project(EnergyData)

set(CMAKE_C_STANDARD 99)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(energy_data
    main.c
    scraping.c
    client_node.c
    open62541.c
    csv_logger.c
)

target_link_libraries(energy_data pthread curl cjson)
