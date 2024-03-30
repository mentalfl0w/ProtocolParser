cmake_minimum_required(VERSION 3.20)

project(ProtocolParser_APP VERSION 1.0.4 LANGUAGES CXX)

set(RIBBONUI_BUILD_EXAMPLES OFF)
add_subdirectory(3rdparty/RibbonUI)
add_subdirectory(app_source)
