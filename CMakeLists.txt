cmake_minimum_required(VERSION 3.20)
project(SymMusic_2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
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
        include/entity/PlayManager.hpp
        include/entity/StatusManager.hpp
        src/ui/MarqueeLabel.cpp
        include/ui/MarqueeLabel.h
)

target_link_libraries(SymMusic_2
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
)

set(CMAKE_PREFIX_PATH "D:/Software/Qt/6.9.3/mingw_64")
set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")

find_program(WINDEPLOYQT_EXECUTABLE windeployqt
        HINTS "${QT_INSTALL_PATH}/bin")

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            --no-translations
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.exe"
            COMMENT "Deploying Qt dependencies with windeployqt..."
    )
    message(STATUS "Using windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
else()
    message(FATAL_ERROR "windeployqt not found!")
endif()