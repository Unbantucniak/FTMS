cmake_minimum_required(VERSION 3.21)

project(QtBackendServer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql Widgets)

if(NOT DEFINED COMMON_INCLUDE_DIR)
    set(COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common/include")
endif()

set(SOURCES
    main.cpp
    db/db_manager.cpp
    network/client_handler.cpp
    network/tcp_server.cpp
    ai/ai_manager.cpp
)

set(HEADERS
    db/db_manager.h
    network/client_handler.h
    network/tcp_server.h
    ai/ai_manager.h
    ${COMMON_INCLUDE_DIR}/data_model.h
)

add_executable(QtBackendServer
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(QtBackendServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COMMON_INCLUDE_DIR}
)

target_link_libraries(QtBackendServer PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Sql
    Qt6::Widgets
)
