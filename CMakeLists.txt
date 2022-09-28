cmake_minimum_required(VERSION 3.8)

project(test_sgl)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Set general compiler flags.
if (WIN32)
  	add_definitions(-D__WINDOWS__ -D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif (WIN32)

# Do not use GUI by default.
add_definitions(-DUSE_GUI=1)

# Add all files to excutable.
add_executable(${PROJECT_NAME}
  	"src/util/hdrloader.cpp"
  	"src/util/nffread.cpp"
  	"src/util/timer.cpp"
  	"src/testapp.cpp"
)

target_include_directories(${PROJECT_NAME}
  	PRIVATE
	"src"
)

add_subdirectory("sgl")

# Set GLFW variables so that we don't build GLFW test etc
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("src/dep/glfw")

# Link libraries.
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} sgl)
target_link_libraries(${PROJECT_NAME} glfw)