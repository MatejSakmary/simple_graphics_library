cmake_minimum_required(VERSION 3.8)

project(sgl)

add_library(${PROJECT_NAME} STATIC 
	  "src/sgl_impl.cpp"
    "src/sgl_context.cpp"
    "src/sgl_core.cpp"
    "src/sgl_framebuffer.cpp"
    "src/sgl_matrix.cpp"
    "src/sgl_vec.cpp"
    "src/sgl_renderer.cpp"
    "src/sgl_scene.cpp"
)

if(SGL_LOG_DEBUG_OUTPUT)
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC
	 	SGL_LOG_OUTPUT=true
	)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)