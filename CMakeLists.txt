cmake_minimum_required(VERSION 3.20)
project(SymMusic_2)

set(CMAKE_CXX_STANDARD 23)
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
        src/ui/MainWindow.cpp
        include/ui/MainWindow.h
        src/ui/NavigationWidget.cpp
        include/ui/NavigationWidget.h
        src/ui/HeadWidget.cpp
        include/ui/HeadWidget.h
        src/ui/NavigationButton.cpp
        include/ui/NavigationButton.h
        src/ui/RecommendWidget.cpp
        include/ui/RecommendWidget.h
        src/ui/PlaylistItem.cpp
        include/ui/PlaylistItem.h
        src/ui/CommonPageWidget.cpp
        include/ui/CommonPageWidget.h
        src/ui/ListItem.cpp
        include/ui/ListItem.h
        src/entity/Song.cpp
        include/entity/Song.h
        src/entity/SongManager.cpp
        include/entity/SongManager.h
        include/ui/SongInfoPage.h
        src/ui/SongInfoPage.cpp
        include/utils/FrameStyleSheet.hpp
        src/ui/PlaySlider.cpp
        include/ui/PlaySlider.h
        src/ui/VolumeSlider.cpp
        include/ui/VolumeSlider.h
        include/entity/Common.hpp
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