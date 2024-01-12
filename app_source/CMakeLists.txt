cmake_minimum_required(VERSION 3.21)

project(ProtocolParser VERSION 1.0.0.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

set(PROJECT_COMPANY "Mentalflow's Lab")
set(PROJECT_COPYRIGHT "Copyright (c) 2023 Mentalflow's Lab. All rights reserved.")
set(PROJECT_DOMAIN "dev.ourdocs.cn.protocolparser")
set(PROJECT_BUNDLE_NAME ${PROJECT_NAME})

add_definitions(-DPROTOCOLPARSER_VERSION=1,0,0,0)

find_package(Qt6 COMPONENTS Quick SerialPort LinguistTools REQUIRED)

set(
    sources_files source/main.cpp include/serialportmanager.h source/serialportmanager.cpp include/eventsbus.h source/eventsbus.cpp
    dlln3x/DLLN3X.h dlln3x/DLLN3X.cpp dlln3x/zigbeeframe.h
    include/zigbeeparser.h source/zigbeeparser.cpp source/crypto.cpp include/crypto.h
    include/protocol.h source/protocol.cpp include/protocol_data.h
    include/config.h source/config.cpp
    languages/zh_CN.ts languages/en_US.ts
#    dlln3x_handler.h dlln3x_handler.cpp
)

INCLUDE_DIRECTORIES(dlln3x include)
add_subdirectory(sm_crypto)

if (WIN32)
    set(app_icon_resource_windows resources/icon.rc)
    qt_add_executable(${PROJECT_NAME}
        ${sources_files}
        resources/icon.rc
    )
    file(TO_CMAKE_PATH "/" PATH_SEPARATOR)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(DLLPATH ../3rdparty/msvc/*.dll)
    else()
        set(DLLPATH ../3rdparty/mingw/*.dll)
    endif()
    string(REPLACE "/" ${PATH_SEPARATOR} DLLPATH "${DLLPATH}")
    file(GLOB DLL_FILES ${DLLPATH})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${DLL_FILES}
        "${CMAKE_BINARY_DIR}${PATH_SEPARATOR}app_source"
    )
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE AppIcon)
    set(App_ICON "resources/imgs/AppIcon.icns")
    set_source_files_properties(${App_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    qt_add_executable(${PROJECT_NAME}
        ${sources_files}
        ${App_ICON}
    )
else ()
    set(PROJECT_BUNDLE_NAME Protocol-Parser)
    qt_add_executable(${PROJECT_NAME}
        ${sources_files}
    )
endif ()

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    VERSION 1.0
    QML_FILES qml/Main.qml qml/components/RibbonWindow.qml qml/components/ZigBeeFrame.qml
            qml/components/RibbonMessageListView.qml qml/components/RibbonMessage.qml
            qml/components/ZigBeeMessage.qml qml/components/SerialPortAssistant.qml
            qml/components/ZigBeeDataView.qml qml/components/TabBar.qml qml/components/CenterView.qml
            qml/components/ListTable.qml qml/components/DeviceList.qml qml/components/KeysList.qml
            qml/components/EventsHistoryList.qml
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER ${PROJECT_DOMAIN}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_BUNDLE_NAME}
    MACOSX_BUNDLE_COPYRIGHT ${PROJECT_COPYRIGHT}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME ${PROJECT_BUNDLE_NAME}
)

qt_add_translations(${PROJECT_NAME}
                    TS_FILES languages/zh_CN.ts languages/en_US.ts
                    RESOURCE_PREFIX "/translations"
                    )

if(RIBBONUI_BUILD_STATIC_LIB)
    add_definitions(-DRIBBONUI_BUILD_STATIC_LIB)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt::Quick
        Qt::SerialPort
        RibbonUIplugin
        sm_crypto
        FramelessHelper::Core
        FramelessHelper::Quick
    )

else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt::Quick
        Qt::SerialPort
        RibbonUI
        sm_crypto
        FramelessHelper::Core
        FramelessHelper::Quick
    )

endif()
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_lupdate)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_lrelease)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
