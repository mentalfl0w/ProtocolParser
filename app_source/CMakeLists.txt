cmake_minimum_required(VERSION 3.21)

project(ProtocolParser VERSION ${CMAKE_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(QT_VERSION VERSION_GREATER_EQUAL "6.3")
    qt_standard_project_setup()
    qt_policy(SET QTP0001 NEW)
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
endif()

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/app_source CACHE STRING "Qt Creator extra QML import paths" FORCE)

string(TIMESTAMP TIME_YEAR %Y)
set(PROJECT_COMPANY "Mentalflow's Lab")
set(PROJECT_COPYRIGHT "Copyright (c) ${TIME_YEAR} Mentalflow's Lab. All rights reserved.")
set(PROJECT_DOMAIN "dev.ourdocs.cn.protocolparser")
set(PROJECT_BUNDLE_NAME ${PROJECT_NAME})
set(version_str "${CMAKE_PROJECT_VERSION_MAJOR},${CMAKE_PROJECT_VERSION_MINOR},${CMAKE_PROJECT_VERSION_PATCH}")
add_definitions(-DPROTOCOLPARSER_VERSION=${version_str})

find_package(Qt6 COMPONENTS Quick SerialPort LinguistTools WebView Core5Compat REQUIRED)

set(
    sources_files source/main.cpp include/serialportmanager.h source/serialportmanager.cpp include/eventsbus.h source/eventsbus.cpp
    dlln3x/DLLN3X.h dlln3x/DLLN3X.cpp dlln3x/zigbeeframe.h
    include/zigbeeparser.h source/zigbeeparser.cpp source/crypto.cpp include/crypto.h
    include/protocol.h source/protocol.cpp include/protocol_data.h
    include/config.h source/config.cpp
    include/serialdataresolver.h source/serialdataresolver.cpp
    include/zigbeedataresolver.h source/zigbeedataresolver.cpp
    include/eventbusresolver.h source/eventbusresolver.cpp
    include/tools.h source/tools.cpp
    languages/zh_CN.ts languages/en_US.ts
)

set(
    qml_files qml/Main.qml qml/components/ZigBeeFrame.qml
    qml/components/ZigBeeMessage.qml qml/components/SerialPortAssistant.qml
    qml/components/ZigBeeDataView.qml qml/components/TabBar.qml qml/components/CenterView.qml
    qml/components/ListTable.qml qml/components/DeviceList.qml qml/components/KeysList.qml
    qml/components/EventsHistoryList.qml qml/components/FrameChooser.qml qml/components/RibbonMarkDownViewer.qml
    qml/HelperWindow.qml
)

set(js_files js/markdown-it.js js/markdown-it-deflist.js js/markdown-it-emoji.js
    js/markdown-it-sub.js js/markdown-it-abbr.js js/markdown-it-container.js js/markdown-it-footnote.js
    js/markdown-it-ins.js js/markdown-it-mark.js js/markdown-it-sup.js
    js/jquery.js js/mermaid.js_packaged js/highlight.js
)

set(doc_files ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_SOURCE_DIR}/documents/protocol.md
    ${CMAKE_SOURCE_DIR}/documents/serialport.md ${CMAKE_SOURCE_DIR}/documents/zigbee.md
    ${CMAKE_SOURCE_DIR}/documents/others.md ${CMAKE_SOURCE_DIR}/documents/pictures/home-dark.png
    ${CMAKE_SOURCE_DIR}/documents/pictures/home-light.png ${CMAKE_SOURCE_DIR}/app_source/resources/imgs/icon.png
)

include_directories(dlln3x include)
add_subdirectory(sm_crypto)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/app/debug)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/app/release)
endif()

if (WIN32)
    configure_file(.cmake/win_app.rc.in ${CMAKE_BINARY_DIR}/win_app.rc)
    set(app_icon_resource_windows ${CMAKE_BINARY_DIR}/win_app.rc)
    qt_add_executable(${PROJECT_NAME}
        ${sources_files}
        ${app_icon_resource_windows}
    )
    file(TO_CMAKE_PATH "/" PATH_SEPARATOR)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(DLLPATH ${CMAKE_SOURCE_DIR}/3rdparty/msvc/*.dll)
    else()
        set(DLLPATH ${CMAKE_SOURCE_DIR}/3rdparty/mingw/*.dll)
    endif()
    string(REPLACE "/" ${PATH_SEPARATOR} DLLPATH "${DLLPATH}")
    file(GLOB DLL_FILES ${DLLPATH})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${DLL_FILES}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
elseif(APPLE)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${PROJECT_DOMAIN})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_BUNDLE_NAME})
    set(MACOSX_BUNDLE_COPYRIGHT ${PROJECT_COPYRIGHT})
    set(MACOSX_BUNDLE_DEVELOPMENT_REGION "Chinese")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version" FORCE)
    set(MACOSX_BUNDLE_EXECUTABLE_NAME ${PROJECT_BUNDLE_NAME})
    set(MACOSX_BUNDLE_ICON_FILE AppIcon)
    configure_file(.cmake/MacOSXBundleInfo.plist.in ${CMAKE_BINARY_DIR}/Info.plist)
    set(App_ICON "${PROJECT_SOURCE_DIR}/resources/imgs/AppIcon.icns")
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

foreach(qmlfile ${qml_files})
    string(REPLACE "qml/" "" fixedfile ${qmlfile})
    set_source_files_properties(${qmlfile} PROPERTIES QT_RESOURCE_ALIAS ${fixedfile})
endforeach(qmlfile)

foreach(jsfile ${js_files})
    string(REPLACE "js/" "" fixedfile ${jsfile})
    set_source_files_properties(${jsfile} PROPERTIES QT_RESOURCE_ALIAS ${fixedfile})
endforeach(jsfile)

foreach(docfile ${doc_files})
    string(REPLACE "${CMAKE_SOURCE_DIR}/" "" fixedfile ${docfile})
    set_source_files_properties(${docfile} PROPERTIES QT_RESOURCE_ALIAS ${fixedfile})
endforeach(docfile)

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    QML_FILES ${qml_files}
    RESOURCE_PREFIX "/qt/qml/"
    RESOURCES ${js_files} ${doc_files} resources/theme.css resources/default.css resources/github-light.css resources/github-dark.css
    VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME ${PROJECT_BUNDLE_NAME}
)

qt_add_translations(${PROJECT_NAME}
    TS_FILES languages/zh_CN.ts languages/en_US.ts
    RESOURCE_PREFIX "/translations"
)

if(RIBBONUI_BUILD_STATIC_LIB)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        RibbonUIplugin
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        RIBBONUI_BUILD_STATIC_LIB
    )
else()
    if(WIN32)
        if (MINGW)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${RIBBONUI_QML_PLUGIN_DIRECTORY}/RibbonUI.dll"
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        else()
            if (NOT RIBBONUI_BUILD_STATIC_LIB)
                if (CMAKE_BUILD_TYPE MATCHES "Debug")
                    set(DEBUG_POSTFIX "d")
                endif()
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${RIBBONUI_QML_PLUGIN_DIRECTORY}/RibbonUI${DEBUG_POSTFIX}.dll"
                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            endif()
        endif()
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Quick
    Qt::SerialPort
    Qt::WebView
    Qt::Core5Compat
    RibbonUI
    sm_crypto
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_lupdate)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_lrelease)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
