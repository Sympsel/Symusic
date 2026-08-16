cmake_minimum_required(VERSION 3.28)
project(SymMusic_2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (MSVC)
    # 确保使用正确的库路径
    add_compile_options(/EHsc /W4)
endif()

file(COPY
        "C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/Tools/MSVC/14.51.36231/modules/std.ixx"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/modules"
)
file(COPY
        "C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/Tools/MSVC/14.51.36231/modules/std.compat.ixx"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/modules"
)

# 路径设置
set(TagLib_DIR "D:/vcpkg/installed/x64-windows/share/taglib")
set(QT_INSTALL_PATH "D:/Software/Qt/6.9.3/msvc2022_64")
set(CMAKE_PREFIX_PATH ${QT_INSTALL_PATH})
set(Qt6_DIR "${QT_INSTALL_PATH}/lib/cmake/Qt6")

set(SOURCES
        src/entity/Song.cpp
)

add_library(std_modules)
target_sources(std_modules
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/modules"
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/modules/std.ixx"
        "${CMAKE_CURRENT_BINARY_DIR}/modules/std.compat.ixx"
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)
find_package(TagLib REQUIRED)

include_directories(include)

add_executable(SymMusic_2 Main.cpp ${SOURCES})
find_package()

target_link_libraries(SymMusic_2
        PRIVATE Qt6::Core
        PRIVATE Qt6::Gui
        PRIVATE Qt6::Widgets
        PRIVATE Qt6::Multimedia
        PRIVATE TagLib::tag
)

target_sources(SymMusic_2
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        FILES
        src/common/Color.ixx
        src/common/Path.ixx
        src/common/Common.ixx

        src/utils/Create.ixx
        src/utils/FrameStyleSheet.ixx
        src/utils/Log.ixx
        src/utils/LyricsParser.ixx
        src/utils/Sync.ixx
)

# 复制 TagLib DLL 到输出目录（使用 vcpkg 的路径）
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,D:/vcpkg/installed/x64-windows/debug/bin/tag.dll,D:/vcpkg/installed/x64-windows/bin/tag.dll>"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
        COMMENT "Copying TagLib DLL to output directory..."
)

find_program(WINDEPLOYQT_EXECUTABLE windeployqt
        HINTS "${QT_INSTALL_PATH}/bin"
        NO_DEFAULT_PATH)

if (WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            --no-translations
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.exe"
            COMMENT "Deploying Qt dependencies with windeployqt..."
    )
    message(STATUS "Using windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
else ()
    message(FATAL_ERROR "windeployqt not found!")
endif ()