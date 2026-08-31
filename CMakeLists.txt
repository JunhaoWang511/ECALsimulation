cmake_minimum_required(VERSION 3.16...3.21)

project(ECAL)

# 默认 Release 构建（-O3），否则无优化编译会显著拖慢数据分析工具
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")
find_package(ROOT REQUIRED)
include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include
  ${Geant4_INCLUDE_DIR}
  # $ENV{ROOTSYS}/include
  ${ROOT_INCLUDE_DIRS}
)
link_libraries(
  ${ROOT_LIBRARIES}
)
# We presume the existence of three directories
file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB macros RELATIVE ${PROJECT_SOURCE_DIR} *.mac)

# Enable macros for out-of-source build
foreach(_file ${macros})
  configure_file(
    ${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
  )
endforeach()

add_executable(ECAL main.cc ${sources} ${headers})
target_link_libraries(ECAL ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
install(TARGETS ECAL DESTINATION bin)

# analysis/ 目录文件拷贝（cmake 配置期）
file(GLOB analysis_files RELATIVE ${PROJECT_SOURCE_DIR}/analysis
     ${PROJECT_SOURCE_DIR}/analysis/*.cpp
     ${PROJECT_SOURCE_DIR}/analysis/*.h
     ${PROJECT_SOURCE_DIR}/analysis/*.sh
     ${PROJECT_SOURCE_DIR}/analysis/*.mac)
foreach(_file ${analysis_files})
  get_filename_component(_ext ${_file} EXT)
  if(_ext MATCHES "\\.(cpp|h)$")
    set(_dest ${PROJECT_BINARY_DIR}/analysis/${_file})
  else()
    set(_dest ${PROJECT_BINARY_DIR}/${_file})
  endif()
  configure_file(
    analysis/${_file}
    ${_dest}
    COPYONLY
  )
endforeach()

add_executable(GenerateWaveform ${PROJECT_BINARY_DIR}/analysis/GenerateWaveform.cpp)
target_link_libraries(GenerateWaveform ${ROOT_LIBRARIES})
add_executable(FitWaveform ${PROJECT_BINARY_DIR}/analysis/FitWaveform.cpp)
target_link_libraries(FitWaveform ${ROOT_LIBRARIES})
install(TARGETS GenerateWaveform FitWaveform DESTINATION bin)
