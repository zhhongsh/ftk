include_directories (common)
include_directories (common/websocketpp)
include (CMakeRC.cmake)

add_subdirectory (common)
add_subdirectory (tracking_graph)
add_subdirectory (critical_point_extraction_2d)
add_subdirectory (critical_point_tracking_2d)
add_subdirectory (critical_point_extraction_3d)
add_subdirectory (critical_point_tracking_3d)
add_subdirectory (vortex_extraction_tdgl)
# add_subdirectory (union_find)
