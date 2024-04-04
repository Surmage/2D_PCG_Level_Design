cmake_minimum_required(VERSION 3.25)
project(2dPCGDesign)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

SET(ROOT ${CMAKE_CURRENT_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/bin>)

#set_property(TARGET 2DLevel PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/images")

ADD_EXECUTABLE(Level 2DLevel.cpp 2DLevel.h)

ADD_LIBRARY(imgui STATIC 
			exts/imgui/imconfig.h
			exts/imgui/imconfig-SFML.h
			exts/imgui/imgui.cpp
			exts/imgui/imgui.h
			exts/imgui/imgui_draw.cpp 
			exts/imgui/imgui_internal.h
			exts/imgui/imgui_tables.cpp
			exts/imgui/imgui_widgets.cpp
			exts/imgui/imgui-SFML.cpp
			exts/imgui/imgui-SFML.h
			exts/imgui/imgui-SFML_export.h
			exts/imgui/imstb_rectpack.h
			exts/imgui/imstb_textedit.h
			exts/imgui/imstb_truetype.h
			)
			
#TARGET_LINK_LIBRARIES(imgui PUBLIC Level)
TARGET_INCLUDE_DIRECTORIES(imgui PUBLIC exts/imgui)
SET_TARGET_PROPERTIES(imgui PROPERTIES FOLDER "exts/imgui")
TARGET_LINK_LIBRARIES(Level INTERFACE imgui)

# Set the path to SFML directory
set(SFML_DIR exts/SFML/lib/cmake/SFML/)

# Set SFML to use static libraries
set(SFML_STATIC_LIBRARIES TRUE)

# Find and include SFML
find_package(SFML 2.6.1 COMPONENTS graphics audio REQUIRED)
target_link_libraries(imgui PRIVATE sfml-graphics sfml-audio)
target_link_libraries(Level PRIVATE sfml-graphics sfml-audio imgui)
IF(MSVC)
    set_property(TARGET Level PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
ENDIF()