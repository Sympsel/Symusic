cmake_minimum_required(VERSION 3.20)  # 建议用 3.20，不需要 3.31
project(SymMusic_2)

set(CMAKE_CXX_STANDARD 17)  # Qt6 建议用 C++17
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Software/Qt/6.9.3/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
)

include_directories(include)

add_executable(SymMusic_2
        Main.cpp
        resources.qrc
        src/MainWindow.cpp
        include/MainWindow.h
        src/NavigationWidget.cpp
        include/NavigationWidget.h
        src/HeadWidget.cpp
        include/HeadWidget.h
        src/NavigationButton.cpp
        include/NavigationButton.h
        src/RecommendWidget.cpp
        include/RecommendWidget.h
        src/PlaylistItem.cpp
        include/PlaylistItem.h
        src/CommonPageWidget.cpp
        include/CommonPageWidget.h
        src/ListItem.cpp
        include/ListItem.h
        src/Song.cpp
        include/Song.h
        src/SongManager.cpp
        include/SongManager.h
        include/PathMaganger.hpp
)

target_link_libraries(SymMusic_2
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

# Windows 下自动复制 DLL 文件
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()

    # 复制 platforms 插件
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()

    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach ()
endif ()