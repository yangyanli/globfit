cmake_minimum_required (VERSION 2.8)

project (globfit)

if(MSVC)
  add_definitions ("-DBOOST_ALL_NO_LIB -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /EHsc /fp:fast /wd4800 /wd4521 /wd4251 /wd4275 /wd4305 /wd4355 /wd4819 /wd4250 /wd4267")
  # comment the following 3 lines to disable global code optimization to speed up compile
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /wd4800")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")

  # /MANIFEST:NO") # please, don't disable manifest generation, otherwise crash at start for vs2008
  if(MSVC90 OR MSVC10 OR MSVC11 AND NOT ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.8 AND NOT ${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} LESS 8.6)
    include(ProcessorCount)
    ProcessorCount(N)
    if(NOT N EQUAL 0)
      SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP${N} ")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N} ")
    endif()
  endif()
endif()

set(GlobFit_OUTPUT_LIB_DIR ${PROJECT_BINARY_DIR}/lib)
set(GlobFit_OUTPUT_BIN_DIR ${PROJECT_BINARY_DIR}/bin)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/matlab DESTINATION ${GlobFit_OUTPUT_BIN_DIR}/)

make_directory(${GlobFit_OUTPUT_LIB_DIR})
make_directory(${GlobFit_OUTPUT_BIN_DIR})
if(WIN32)
  foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} CONFIG)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${GlobFit_OUTPUT_LIB_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${GlobFit_OUTPUT_BIN_DIR}")
    # ---[ Windows requires DLLs (shared libraries) to be installed in the same directory as executables
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${GlobFit_OUTPUT_BIN_DIR}")
  endforeach(config)
else(WIN32)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${GlobFit_OUTPUT_LIB_DIR}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${GlobFit_OUTPUT_BIN_DIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${GlobFit_OUTPUT_LIB_DIR}")
endif(WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(Boost_USE_STATIC_LIBS  ON)
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS program_options filesystem system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(CGAL REQUIRED)
include_directories(${CGAL_INCLUDE_DIRS})
set(CMAKE_MODULE_PATH ${CGAL_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CGAL_DIR})
include(CGAL_Macros)
find_package(GMP REQUIRED)
if(GMP_FOUND)
  include_directories(${GMP_INCLUDE_DIR})
endif(GMP_FOUND)
link_directories(${CGAL_LIBRARIES_DIR})
 
find_package(OpenSceneGraph COMPONENTS osgViewer osgText osgDB osgGA osgQt osgManipulator osgUtil REQUIRED)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

find_package(Matlab  REQUIRED)
include_directories(${MATLAB_INCLUDE_DIR})

set(lib_incs  include/CoreExports.h
              include/ColorMap.h
              include/Cone.h
              include/Cylinder.h
              include/GlobFit.h
              include/Plane.h
              include/Primitive.h
              include/RelationEdge.h
              include/RelationGraph.h
              include/RelationVertex.h
              include/Sphere.h
              include/Types.h
              )

set(lib_srcs  src/ColorMap.cpp
              src/Cone.cpp
              src/Cylinder.cpp
              src/EqualityAlignment.cpp
              src/GlobFit.cpp
              src/OrientationAlignment.cpp
              src/PlacementAlignment.cpp
              src/Plane.cpp
              src/Primitive.cpp
              src/Relation.cpp
              src/RelationEdge.cpp
              src/RelationGraph.cpp
              src/RelationVertex.cpp
              src/Sphere.cpp
              src/Solver.cpp
              src/Render.cpp
              )

set(lib_name core)
add_library(${lib_name} SHARED ${lib_srcs} ${lib_incs})
target_link_libraries(${lib_name} ${CGAL_LIBRARIES} ${GMP_LIBRARIES} ${OPENSCENEGRAPH_LIBRARIES} ${Boost_SYSTEM_LIBRARY}  ${Boost_FILESYSTEM_LIBRARY} ${MATLAB_ENG_LIBRARY} ${MATLAB_MX_LIBRARY})

if(WIN32 AND MSVC)
  set_target_properties(${lib_name} PROPERTIES LINK_FLAGS_RELEASE /OPT:REF)
elseif(CMAKE_SYSTEMname STREQUAL "Darwin")
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set_target_properties(${lib_name} PROPERTIES LINK_FLAGS -Wl)
  endif()
elseif(__COMPILER_PATHSCALE)
  set_target_properties(${lib_name} PROPERTIES LINK_FLAGS -mp)
else()
  set_target_properties(${lib_name} PROPERTIES LINK_FLAGS -Wl,--as-needed)
endif()

set_target_properties(${lib_name} PROPERTIES DEFINE_SYMBOL "CORE_API_EXPORTS")

set_target_properties(${lib_name} PROPERTIES DEBUG_POSTFIX _debug)
set_target_properties(${lib_name} PROPERTIES RELEASE_POSTFIX _release)


set(exe_incs  include/Viewer.h)

set(exe_srcs  src/Viewer.cpp
              src/main.cpp
              )

set(exe_name globfit)
add_executable(${exe_name} ${exe_srcs} ${exe_incs})
target_link_libraries(${exe_name} ${lib_name} ${Boost_THREAD_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})

if(WIN32 AND MSVC)
  set_target_properties(${exe_name} PROPERTIES LINK_FLAGS_RELEASE /OPT:REF)
elseif(CMAKE_SYSTEMname STREQUAL "Darwin")
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set_target_properties(${exe_name} PROPERTIES LINK_FLAGS -Wl)
  endif()
elseif(__COMPILER_PATHSCALE)
  set_target_properties(${exe_name} PROPERTIES LINK_FLAGS -mp)
else()
  set_target_properties(${exe_name} PROPERTIES LINK_FLAGS -Wl,--as-needed)
endif()

set_target_properties(${exe_name} PROPERTIES DEBUG_POSTFIX _debug)
set_target_properties(${exe_name} PROPERTIES RELEASE_POSTFIX _release)
