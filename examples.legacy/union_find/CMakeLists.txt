# add_executable              (shared_memory_parallel     shared_memory_parallel.cpp)
# # target_link_libraries       (shared_memory_parallel     ${libraries})
# target_link_libraries       (shared_memory_parallel     ftk)

# add_executable              (distributed_memory_parallel     distributed_memory_parallel.cpp)
# # target_link_libraries       (distributed_memory_parallel     ${libraries})
# target_link_libraries       (distributed_memory_parallel     ftk)

# ============================================

set (sources critical_point_tracking_2d_union_find.cpp)

if (FTK_HAVE_QT5)
  set (CMAKE_AUTOMOC ON)
  list (APPEND sources widget.cpp)
endif ()

add_executable              (critical_point_tracking_2d_union_find     ${sources})
# target_link_libraries       (critical_point_tracking_2d_union_find     ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries       (critical_point_tracking_2d_union_find     ftk)
  
if (FTK_HAVE_QT5)
  qt5_use_modules (critical_point_tracking_2d_union_find Widgets OpenGL)
  target_link_libraries (critical_point_tracking_2d_union_find trackball ${OPENGL_LIBRARIES})
endif ()

# ============================================

set (sources distributed_critical_point_tracking_2d.cpp)

if (FTK_HAVE_QT5)
  set (CMAKE_AUTOMOC ON)
  list (APPEND sources widget.cpp)
endif ()

add_executable              (distributed_critical_point_tracking_2d     ${sources})
# target_link_libraries       (distributed_critical_point_tracking_2d     ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries       (distributed_critical_point_tracking_2d     ftk)
  
if (FTK_HAVE_QT5)
  qt5_use_modules (distributed_critical_point_tracking_2d Widgets OpenGL)
  target_link_libraries (distributed_critical_point_tracking_2d trackball ${OPENGL_LIBRARIES})
endif ()

# ============================================

set (sources distributed_critical_point_tracking_3d.cpp)

if (FTK_HAVE_QT5)
  set (CMAKE_AUTOMOC ON)
  list (APPEND sources widget.cpp)
endif ()

add_executable              (distributed_critical_point_tracking_3d     ${sources})
# target_link_libraries       (distributed_critical_point_tracking_3d     ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries       (distributed_critical_point_tracking_3d     ftk)
  
if (FTK_HAVE_QT5)
  qt5_use_modules (distributed_critical_point_tracking_3d Widgets OpenGL)
  target_link_libraries (distributed_critical_point_tracking_3d trackball ${OPENGL_LIBRARIES})
endif ()

# ============================================

set (sources distributed_level_set_tracking_2d.cpp)

if (FTK_HAVE_QT5)
  set (CMAKE_AUTOMOC ON)
  list (APPEND sources widget.cpp)
endif ()

add_executable              (distributed_level_set_tracking_2d     ${sources})
# target_link_libraries       (distributed_level_set_tracking_2d     ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries       (distributed_level_set_tracking_2d     ftk)
  
if (FTK_HAVE_QT5)
  qt5_use_modules (distributed_level_set_tracking_2d Widgets OpenGL)
  target_link_libraries (distributed_level_set_tracking_2d trackball ${OPENGL_LIBRARIES})
endif ()

# ============================================

set (sources distributed_level_set_tracking_3d.cpp)

if (FTK_HAVE_QT5)
  set (CMAKE_AUTOMOC ON)
  list (APPEND sources widget.cpp)
endif ()

add_executable              (distributed_level_set_tracking_3d     ${sources})
# target_link_libraries       (distributed_level_set_tracking_3d     ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries       (distributed_level_set_tracking_3d     ftk)
  
if (FTK_HAVE_QT5)
  qt5_use_modules (distributed_level_set_tracking_3d Widgets OpenGL)
  target_link_libraries (distributed_level_set_tracking_3d trackball ${OPENGL_LIBRARIES})
endif ()