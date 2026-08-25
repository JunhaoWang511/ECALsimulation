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

# 离线数据分析工具：源码位于 analysis/，构建时拷贝到 build/analysis/
file(GLOB analysis_sources ${PROJECT_SOURCE_DIR}/analysis/*.cpp)
foreach(_file ${analysis_sources})
  get_filename_component(_name ${_file} NAME)
  add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/analysis/${_name}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_file} ${PROJECT_BINARY_DIR}/analysis/${_name}
    DEPENDS ${_file}
    COMMENT "Copying ${_name} to build/analysis/"
  )
endforeach()
add_custom_target(copy_analysis_sources ALL
  DEPENDS ${PROJECT_BINARY_DIR}/analysis/GenerateWaveform.cpp ${PROJECT_BINARY_DIR}/analysis/FitWaveform.cpp
)

# 批量数据处理流程脚本（提交/合并/分析）及宏模板：cmake配置期拷贝到 build 根目录
# 与 run.mac 等宏文件一致，用 configure_file 在 cmake 时拷贝；
# 不用 add_custom_target（make期拷贝），避免用户删除build下脚本后重新make时被源文件覆盖
file(GLOB batch_scripts RELATIVE ${PROJECT_SOURCE_DIR}/analysis
     ${PROJECT_SOURCE_DIR}/analysis/*.sh
     ${PROJECT_SOURCE_DIR}/analysis/*.mac)
foreach(_file ${batch_scripts})
  configure_file(
    analysis/${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
  )
endforeach()

add_executable(GenerateWaveform ${PROJECT_SOURCE_DIR}/analysis/GenerateWaveform.cpp)
target_link_libraries(GenerateWaveform ${ROOT_LIBRARIES})
add_executable(FitWaveform ${PROJECT_SOURCE_DIR}/analysis/FitWaveform.cpp)
target_link_libraries(FitWaveform ${ROOT_LIBRARIES})
install(TARGETS GenerateWaveform FitWaveform DESTINATION bin)
