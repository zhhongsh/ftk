set (sources main.cpp)

file (GLOB_RECURSE tdgl_public_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "public/*")
# message (${tdgl_public_files})
cmrc_add_resource_library (tdgl_public ${tdgl_public_files} NAMESPACE tdgl_public)

if (FTK_HAVE_QT5)
  set (CMAKE_AUTOMOC ON)
  list (APPEND sources widget.cpp)
endif ()

add_executable (ex_vortex_extraction_tdgl ${sources})
target_link_libraries (ex_vortex_extraction_tdgl ftk tdgl_io tdgl_public)

if (FTK_HAVE_QT5)
  qt5_use_modules (ex_vortex_extraction_tdgl Widgets OpenGL)
  target_link_libraries (ex_vortex_extraction_tdgl trackball ${OPENGL_LIBRARIES})
endif ()
