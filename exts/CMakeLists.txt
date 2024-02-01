ADD_LIBRARY(exts INTERFACE)

ADD_SUBDIRECTORY(glew)
SET_TARGET_PROPERTIES(glew PROPERTIES FOLDER "exts/glew")
TARGET_INCLUDE_DIRECTORIES(exts INTERFACE glew/include)

ADD_SUBDIRECTORY(glfw)
SET_TARGET_PROPERTIES(glfw PROPERTIES FOLDER "exts/glfw")
TARGET_INCLUDE_DIRECTORIES(exts INTERFACE glfw/include)

ADD_LIBRARY(imgui STATIC 
			imgui/imconfig.h
			imgui/imgui.cpp
			imgui/imgui.h
			#imgui/imgui_demo.cpp
			imgui/imgui_draw.cpp 
			imgui/imgui_impl_glfw.h
			imgui/imgui_impl_glfw.cpp
			imgui/imgui_impl_opengl3.h
			imgui/imgui_impl_opengl3.cpp
			imgui/imgui_impl_opengl3_loader.h
			imgui/imgui_internal.h
			imgui/imgui_tables.cpp
			imgui/imgui_widgets.cpp
			imgui/imstb_rectpack.h
			imgui/imstb_textedit.h
			imgui/imstb_truetype.h
			)
			

TARGET_LINK_LIBRARIES(imgui PUBLIC exts)
TARGET_INCLUDE_DIRECTORIES(imgui PUBLIC imgui)
SET_TARGET_PROPERTIES(imgui PROPERTIES FOLDER "exts/imgui")

# Set the path to SFML directory
set(SFML_DIR SFML/lib/cmake/SFML/)

# Set SFML to use static libraries
set(SFML_STATIC_LIBRARIES TRUE)

# Find and include SFML
find_package(SFML 2.6.1 COMPONENTS graphics audio REQUIRED)

TARGET_LINK_LIBRARIES(exts INTERFACE sfml-graphics sfml-audio imgui)