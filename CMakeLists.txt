cmake_minimum_required(VERSION 3.25)
project(2dPCGDesign)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Set the path to SFML directory
set(SFML_DIR SFML/lib/cmake/SFML/)

# Set SFML to use static libraries
set(SFML_STATIC_LIBRARIES TRUE)

# Find and include SFML
find_package(SFML 2.6.1 COMPONENTS graphics audio REQUIRED)

set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/imgui)
file(GLOB IMGUI_GLOB
			${IMGUI_PATH}/imconfig.h
			${IMGUI_PATH}/imgui.cpp
			${IMGUI_PATH}/imgui.h
			#imgui/imgui_demo.cpp
			${IMGUI_PATH}/imgui_draw.cpp 
			${IMGUI_PATH}/imgui_impl_glfw.h
			${IMGUI_PATH}/imgui_impl_glfw.cpp
			${IMGUI_PATH}/imgui_impl_opengl3.h
			${IMGUI_PATH}/imgui_impl_opengl3.cpp
			${IMGUI_PATH}/imgui_impl_opengl3_loader.h
			${IMGUI_PATH}/imgui_internal.h
			${IMGUI_PATH}/imgui_tables.cpp
			${IMGUI_PATH}/imgui_widgets.cpp
			${IMGUI_PATH}/imstb_rectpack.h
			${IMGUI_PATH}/imstb_textedit.h
			${IMGUI_PATH}/imstb_truetype.h
			)
# Create the ImGui library
add_library(imgui STATIC ${IMGUI_GLOB})
target_include_directories(imgui PUBLIC ${IMGUI_PATH})
target_link_libraries(imgui PRIVATE glfw)
add_executable(2DLevel Generator/2DLevel.cpp)

target_link_libraries(2DLevel PRIVATE sfml-graphics sfml-audio imgui)
target_include_directories(2DLevel PUBLIC imgui)
set_property(TARGET 2DLevel PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/images")