cmake_minimum_required(VERSION 3.21)

project(QtDesktopClient LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets)

# 引用根目录定义的公共头文件路径
if(NOT DEFINED COMMON_INCLUDE_DIR)
    set(COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common/include")
endif()

set(SOURCES
    main.cpp
    network/tcp_client.cpp
    ui/login_page.cpp
    ui/main_window.cpp
    ui/orders_page.cpp
    ui/profile_page.cpp
    ui/register_page.cpp
    ui/flight_card.cpp
    ui/order_card.cpp
    ui/seat_selection_dialog.cpp
)

set(HEADERS
    network/tcp_client.h
    ui/login_page.h
    ui/main_window.h
    ui/orders_page.h
    ui/profile_page.h
    ui/register_page.h
    ui/flight_card.h
    ui/order_card.h
    ui/seat_selection_dialog.h
    ui/theme_manager.h
    ${COMMON_INCLUDE_DIR}/data_model.h
)

# 已移除 .ui 文件依赖，所有 UI 均使用纯代码构建

add_executable(QtDesktopClient
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(QtDesktopClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COMMON_INCLUDE_DIR}
)

target_link_libraries(QtDesktopClient PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Widgets
)
