cmake_minimum_required(VERSION 2.8)
 
PROJECT(UnstructuredToStructured)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(UnstructuredToStructured MACOSX_BUNDLE UnstructuredToStructured.cxx)
 
if(VTK_LIBRARIES)
  target_link_libraries(UnstructuredToStructured ${VTK_LIBRARIES})
else()
  target_link_libraries(UnstructuredToStructured vtkHybrid vtkWidgets)
endif()
